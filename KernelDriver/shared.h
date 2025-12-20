/**
 * ═══════════════════════════════════════════════════════════════════════════
 *  SPOOFLIGHT KERNEL DRIVER - SHARED DEFINITIONS
 *  
 *  Diese Datei wird von BEIDEN Seiten verwendet:
 *  - Kernel Driver (spoof_driver.c)
 *  - User-Mode App (C# via P/Invoke)
 *  
 *  📚 LEKTION: IOCTL (I/O Control)
 *  
 *  IOCTLs sind der Kommunikationskanal zwischen User-Mode und Kernel-Mode.
 *  User-Mode: DeviceIoControl(handle, IOCTL_CODE, input, output)
 *  Kernel:    IRP_MJ_DEVICE_CONTROL Handler
 * ═══════════════════════════════════════════════════════════════════════════
 */

#ifndef SPOOFLIGHT_SHARED_H
#define SPOOFLIGHT_SHARED_H

//
// Device Name und Symbolic Link
// User-Mode öffnet: \\.\SpooflightDriver
//
#define DRIVER_NAME         L"SpooflightDriver"
#define DEVICE_NAME         L"\\Device\\SpooflightDriver"
#define SYMBOLIC_LINK_NAME  L"\\DosDevices\\SpooflightDriver"
#define USER_DEVICE_PATH    "\\\\.\\SpooflightDriver"

//
// 📚 LEKTION: IOCTL Code Aufbau
// 
// CTL_CODE(DeviceType, Function, Method, Access)
// 
// DeviceType: FILE_DEVICE_UNKNOWN für custom drivers
// Function:   0x800+ für custom functions (0-0x7FF reserved by Microsoft)
// Method:     METHOD_BUFFERED = sicher, Kernel kopiert Daten
// Access:     FILE_ANY_ACCESS, FILE_READ_DATA, FILE_WRITE_DATA
//

#define FILE_DEVICE_SPOOFLIGHT  0x8000

// IOCTL Codes
#define IOCTL_SPOOFLIGHT_GET_VERSION \
    CTL_CODE(FILE_DEVICE_SPOOFLIGHT, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_SPOOFLIGHT_SPOOF_DISK_SERIAL \
    CTL_CODE(FILE_DEVICE_SPOOFLIGHT, 0x801, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_SPOOFLIGHT_SPOOF_SMBIOS \
    CTL_CODE(FILE_DEVICE_SPOOFLIGHT, 0x802, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_SPOOFLIGHT_GET_STATUS \
    CTL_CODE(FILE_DEVICE_SPOOFLIGHT, 0x803, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_SPOOFLIGHT_ENABLE_SPOOF \
    CTL_CODE(FILE_DEVICE_SPOOFLIGHT, 0x804, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_SPOOFLIGHT_DISABLE_SPOOF \
    CTL_CODE(FILE_DEVICE_SPOOFLIGHT, 0x805, METHOD_BUFFERED, FILE_ANY_ACCESS)

//
// Strukturen für IOCTL Daten
//

// Version Info
typedef struct _SPOOFLIGHT_VERSION {
    unsigned long Major;
    unsigned long Minor;
    unsigned long Build;
} SPOOFLIGHT_VERSION, *PSPOOFLIGHT_VERSION;

// Disk Serial Spoof Request
typedef struct _SPOOF_DISK_REQUEST {
    unsigned long DiskIndex;           // Welche Disk (0, 1, 2...)
    char OriginalSerial[64];           // Wird vom Driver gefüllt
    char SpoofedSerial[64];            // Gewünschte neue Serial
    unsigned long Flags;               // Reserved
} SPOOF_DISK_REQUEST, *PSPOOF_DISK_REQUEST;

// SMBIOS Spoof Request
typedef struct _SPOOF_SMBIOS_REQUEST {
    char OriginalUuid[48];             // Wird vom Driver gefüllt
    char SpoofedUuid[48];              // Gewünschte neue UUID
    char OriginalSerial[64];           // Motherboard Serial
    char SpoofedSerial[64];
    unsigned long Flags;
} SPOOF_SMBIOS_REQUEST, *PSPOOF_SMBIOS_REQUEST;

// Driver Status
typedef struct _SPOOFLIGHT_STATUS {
    unsigned long IsLoaded;            // 1 = geladen
    unsigned long DiskSpoofActive;     // 1 = Disk Spoofing aktiv
    unsigned long SmbiosSpoofActive;   // 1 = SMBIOS Spoofing aktiv
    unsigned long HookedDisks;         // Anzahl gehookter Disks
    char LastError[256];               // Letzter Fehler
} SPOOFLIGHT_STATUS, *PSPOOFLIGHT_STATUS;

#endif // SPOOFLIGHT_SHARED_H
