/**
 * ═══════════════════════════════════════════════════════════════════════════
 *  SPOOFLIGHT KERNEL DRIVER
 *  
 *  📚 LEKTION: Windows Kernel Driver (WDM)
 *  
 *  Ein Kernel Driver läuft im Ring 0 - dem höchsten Privileg-Level.
 *  Er kann ALLES auf dem System machen, aber:
 *  - Ein Fehler = Blue Screen (BSOD)
 *  - Kein Exception Handling wie in User-Mode
 *  - Speicher muss manuell verwaltet werden
 *  
 *  Dieser Driver:
 *  1. Erstellt ein Device für User-Mode Kommunikation
 *  2. Fängt Storage IOCTLs ab (für Disk Serial Spoofing)
 *  3. Kann SMBIOS Tabellen modifizieren
 * ═══════════════════════════════════════════════════════════════════════════
 */

#include <ntddk.h>
#include <wdm.h>
#include <ntstrsafe.h>
#include "shared.h"

//
// 📚 LEKTION: Driver Tags
// Jede Speicher-Allokation braucht einen Tag für Debugging
//
#define DRIVER_TAG 'fopS'  // "Spof" rückwärts

//
// Globale Variablen
//
PDEVICE_OBJECT g_DeviceObject = NULL;
UNICODE_STRING g_DeviceName;
UNICODE_STRING g_SymbolicLink;

// Spoof-Konfiguration
typedef struct _SPOOF_CONFIG {
    BOOLEAN DiskSpoofEnabled;
    BOOLEAN SmbiosSpoofEnabled;
    CHAR SpoofedDiskSerial[64];
    CHAR SpoofedSmbiosUuid[48];
} SPOOF_CONFIG, *PSPOOF_CONFIG;

SPOOF_CONFIG g_Config = { 0 };

//
// Forward Declarations
//
DRIVER_INITIALIZE DriverEntry;
DRIVER_UNLOAD DriverUnload;

_Dispatch_type_(IRP_MJ_CREATE)
_Dispatch_type_(IRP_MJ_CLOSE)
DRIVER_DISPATCH DispatchCreateClose;

_Dispatch_type_(IRP_MJ_DEVICE_CONTROL)
DRIVER_DISPATCH DispatchDeviceControl;

//
// 📚 LEKTION: DriverEntry
// 
// Das ist die "main()" Funktion für Kernel Drivers.
// Wird aufgerufen wenn der Driver geladen wird.
//
NTSTATUS DriverEntry(
    _In_ PDRIVER_OBJECT DriverObject,
    _In_ PUNICODE_STRING RegistryPath
)
{
    UNREFERENCED_PARAMETER(RegistryPath);
    NTSTATUS status;
    
    DbgPrint("[Spooflight] DriverEntry called\n");
    
    // Device Name initialisieren
    RtlInitUnicodeString(&g_DeviceName, DEVICE_NAME);
    RtlInitUnicodeString(&g_SymbolicLink, SYMBOLIC_LINK_NAME);
    
    // Device erstellen
    // 📚 LEKTION: IoCreateDevice
    // Erstellt ein "Device Object" - das ist unser Kommunikationsendpunkt
    status = IoCreateDevice(
        DriverObject,
        0,                          // DeviceExtension Größe
        &g_DeviceName,
        FILE_DEVICE_SPOOFLIGHT,     // Device Type
        FILE_DEVICE_SECURE_OPEN,    // Characteristics
        FALSE,                      // Exclusive
        &g_DeviceObject
    );
    
    if (!NT_SUCCESS(status)) {
        DbgPrint("[Spooflight] IoCreateDevice failed: 0x%08X\n", status);
        return status;
    }
    
    // Symbolic Link erstellen
    // 📚 LEKTION: Symbolic Link
    // User-Mode kann "\\\\.\\SpooflightDriver" öffnen
    // Das mappt auf unser Device "\\Device\\SpooflightDriver"
    status = IoCreateSymbolicLink(&g_SymbolicLink, &g_DeviceName);
    
    if (!NT_SUCCESS(status)) {
        DbgPrint("[Spooflight] IoCreateSymbolicLink failed: 0x%08X\n", status);
        IoDeleteDevice(g_DeviceObject);
        return status;
    }
    
    // Dispatch Routinen setzen
    // 📚 LEKTION: IRP Major Functions
    // Windows sendet I/O Request Packets (IRPs) an unseren Driver
    // Wir müssen Handler für verschiedene IRP-Typen registrieren
    DriverObject->MajorFunction[IRP_MJ_CREATE] = DispatchCreateClose;
    DriverObject->MajorFunction[IRP_MJ_CLOSE] = DispatchCreateClose;
    DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DispatchDeviceControl;
    DriverObject->DriverUnload = DriverUnload;
    
    // Buffered I/O für sichere Datenübertragung
    g_DeviceObject->Flags |= DO_BUFFERED_IO;
    g_DeviceObject->Flags &= ~DO_DEVICE_INITIALIZING;
    
    DbgPrint("[Spooflight] Driver loaded successfully!\n");
    
    return STATUS_SUCCESS;
}

//
// 📚 LEKTION: DriverUnload
// 
// Wird aufgerufen wenn der Driver entladen wird.
// MUSS alle Ressourcen freigeben!
//
VOID DriverUnload(
    _In_ PDRIVER_OBJECT DriverObject
)
{
    UNREFERENCED_PARAMETER(DriverObject);
    
    DbgPrint("[Spooflight] DriverUnload called\n");
    
    // Symbolic Link löschen
    IoDeleteSymbolicLink(&g_SymbolicLink);
    
    // Device löschen
    if (g_DeviceObject) {
        IoDeleteDevice(g_DeviceObject);
    }
    
    DbgPrint("[Spooflight] Driver unloaded\n");
}

//
// 📚 LEKTION: Create/Close Dispatch
// 
// Wird aufgerufen wenn User-Mode CreateFile/CloseHandle macht
//
NTSTATUS DispatchCreateClose(
    _In_ PDEVICE_OBJECT DeviceObject,
    _In_ PIRP Irp
)
{
    UNREFERENCED_PARAMETER(DeviceObject);
    
    Irp->IoStatus.Status = STATUS_SUCCESS;
    Irp->IoStatus.Information = 0;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    
    return STATUS_SUCCESS;
}

//
// 📚 LEKTION: DeviceControl Dispatch
// 
// Das Herzstück! Hier kommen die IOCTL-Requests an.
// User-Mode ruft DeviceIoControl() auf → landet hier
//
NTSTATUS DispatchDeviceControl(
    _In_ PDEVICE_OBJECT DeviceObject,
    _In_ PIRP Irp
)
{
    UNREFERENCED_PARAMETER(DeviceObject);
    
    PIO_STACK_LOCATION irpSp;
    NTSTATUS status = STATUS_SUCCESS;
    ULONG bytesReturned = 0;
    
    irpSp = IoGetCurrentIrpStackLocation(Irp);
    
    ULONG ioctl = irpSp->Parameters.DeviceIoControl.IoControlCode;
    ULONG inputLen = irpSp->Parameters.DeviceIoControl.InputBufferLength;
    ULONG outputLen = irpSp->Parameters.DeviceIoControl.OutputBufferLength;
    PVOID buffer = Irp->AssociatedIrp.SystemBuffer;
    
    DbgPrint("[Spooflight] IOCTL: 0x%08X\n", ioctl);
    
    switch (ioctl) {
        
        //
        // GET_VERSION - Gibt Driver-Version zurück
        //
        case IOCTL_SPOOFLIGHT_GET_VERSION: {
            if (outputLen >= sizeof(SPOOFLIGHT_VERSION)) {
                PSPOOFLIGHT_VERSION ver = (PSPOOFLIGHT_VERSION)buffer;
                ver->Major = 1;
                ver->Minor = 0;
                ver->Build = 0;
                bytesReturned = sizeof(SPOOFLIGHT_VERSION);
                DbgPrint("[Spooflight] Version requested: %lu.%lu.%lu\n", 
                    ver->Major, ver->Minor, ver->Build);
            } else {
                status = STATUS_BUFFER_TOO_SMALL;
            }
            break;
        }
        
        //
        // GET_STATUS - Gibt aktuellen Spoof-Status zurück
        //
        case IOCTL_SPOOFLIGHT_GET_STATUS: {
            if (outputLen >= sizeof(SPOOFLIGHT_STATUS)) {
                PSPOOFLIGHT_STATUS stat = (PSPOOFLIGHT_STATUS)buffer;
                stat->IsLoaded = 1;
                stat->DiskSpoofActive = g_Config.DiskSpoofEnabled ? 1 : 0;
                stat->SmbiosSpoofActive = g_Config.SmbiosSpoofEnabled ? 1 : 0;
                stat->HookedDisks = 0;  // TODO: Implement
                RtlStringCbCopyA(stat->LastError, sizeof(stat->LastError), "OK");
                bytesReturned = sizeof(SPOOFLIGHT_STATUS);
            } else {
                status = STATUS_BUFFER_TOO_SMALL;
            }
            break;
        }
        
        //
        // SPOOF_DISK_SERIAL - Konfiguriert Disk Serial Spoofing
        //
        case IOCTL_SPOOFLIGHT_SPOOF_DISK_SERIAL: {
            if (inputLen >= sizeof(SPOOF_DISK_REQUEST)) {
                PSPOOF_DISK_REQUEST req = (PSPOOF_DISK_REQUEST)buffer;
                
                DbgPrint("[Spooflight] Disk Spoof Request - Index: %lu, New Serial: %s\n",
                    req->DiskIndex, req->SpoofedSerial);
                
                // Serial speichern
                RtlStringCbCopyA(g_Config.SpoofedDiskSerial, 
                    sizeof(g_Config.SpoofedDiskSerial), 
                    req->SpoofedSerial);
                g_Config.DiskSpoofEnabled = TRUE;
                
                status = STATUS_SUCCESS;
                bytesReturned = sizeof(SPOOF_DISK_REQUEST);
            } else {
                status = STATUS_BUFFER_TOO_SMALL;
            }
            break;
        }
        
        //
        // SPOOF_SMBIOS - Konfiguriert SMBIOS Spoofing
        //
        case IOCTL_SPOOFLIGHT_SPOOF_SMBIOS: {
            if (inputLen >= sizeof(SPOOF_SMBIOS_REQUEST)) {
                PSPOOF_SMBIOS_REQUEST req = (PSPOOF_SMBIOS_REQUEST)buffer;
                
                DbgPrint("[Spooflight] SMBIOS Spoof Request - New UUID: %s\n",
                    req->SpoofedUuid);
                
                RtlStringCbCopyA(g_Config.SpoofedSmbiosUuid,
                    sizeof(g_Config.SpoofedSmbiosUuid),
                    req->SpoofedUuid);
                g_Config.SmbiosSpoofEnabled = TRUE;
                
                status = STATUS_SUCCESS;
                bytesReturned = sizeof(SPOOF_SMBIOS_REQUEST);
            } else {
                status = STATUS_BUFFER_TOO_SMALL;
            }
            break;
        }
        
        //
        // ENABLE_SPOOF - Aktiviert Spoofing
        //
        case IOCTL_SPOOFLIGHT_ENABLE_SPOOF: {
            g_Config.DiskSpoofEnabled = TRUE;
            g_Config.SmbiosSpoofEnabled = TRUE;
            DbgPrint("[Spooflight] Spoofing ENABLED\n");
            status = STATUS_SUCCESS;
            break;
        }
        
        //
        // DISABLE_SPOOF - Deaktiviert Spoofing
        //
        case IOCTL_SPOOFLIGHT_DISABLE_SPOOF: {
            g_Config.DiskSpoofEnabled = FALSE;
            g_Config.SmbiosSpoofEnabled = FALSE;
            DbgPrint("[Spooflight] Spoofing DISABLED\n");
            status = STATUS_SUCCESS;
            break;
        }
        
        default:
            DbgPrint("[Spooflight] Unknown IOCTL: 0x%08X\n", ioctl);
            status = STATUS_INVALID_DEVICE_REQUEST;
            break;
    }
    
    Irp->IoStatus.Status = status;
    Irp->IoStatus.Information = bytesReturned;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    
    return status;
}
