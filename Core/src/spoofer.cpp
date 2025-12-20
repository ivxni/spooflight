/**
 * ═══════════════════════════════════════════════════════════════════════════
 *  SPOOFLIGHT - HAUPTIMPLEMENTIERUNG
 *  Hier ist der echte Code, der die Arbeit macht!
 * ═══════════════════════════════════════════════════════════════════════════
 */

#include "../include/spoofer.h"

// Windows-spezifische Header
#include <windows.h>     // Basis Windows API
#include <cstring>       // Für strcpy, strlen etc.
#include <iphlpapi.h>    // Netzwerk-Adapter API (für MAC Spoofing)
#include <cstdio>        // Für sprintf
#include <cstdlib>       // Für rand, srand
#include <ctime>         // Für time (random seed)
#include <intrin.h>      // Für __cpuid

// ═══════════════════════════════════════════════════════════════════════════
//  GLOBALE VARIABLEN
// ═══════════════════════════════════════════════════════════════════════════

/**
 * 📚 LEKTION: static Variablen
 * 
 * "static" bedeutet: Diese Variable ist nur in DIESER Datei sichtbar.
 * Andere .cpp Dateien können sie nicht sehen oder ändern.
 * 
 * Das ist wie "private" in JavaScript Klassen.
 */
static bool g_initialized = false;
static const char* VERSION = "1.0.0";

// ═══════════════════════════════════════════════════════════════════════════
//  HILFSFUNKTIONEN (intern, nicht exportiert)
// ═══════════════════════════════════════════════════════════════════════════

/**
 * 📚 LEKTION: Funktionen ohne SPOOF_API sind "privat"
 * 
 * Diese Funktion hat kein SPOOF_API davor, also wird sie NICHT exportiert.
 * C# kann sie nicht aufrufen - nur unser eigener C++ Code.
 */
static void CopyString(char* dest, const char* src, int maxLen) {
    // strncpy kopiert maximal maxLen-1 Zeichen und fügt '\0' am Ende hinzu
    strncpy(dest, src, maxLen - 1);
    dest[maxLen - 1] = '\0';  // Sicherheit: immer null-terminieren
}

// ═══════════════════════════════════════════════════════════════════════════
//  EXPORTIERTE FUNKTIONEN - Implementation
// ═══════════════════════════════════════════════════════════════════════════

/**
 * Initialize - Initialisiert die Library
 */
SPOOF_API bool Initialize() {
    if (g_initialized) {
        return true;  // Bereits initialisiert
    }
    
    // ╔═══════════════════════════════════════════════════════════════════╗
    // ║  🎯 AUFGABE 1: Setze g_initialized auf true                       ║
    // ║                                                                   ║
    // ║  Hinweis: In C++ schreibst du einfach: variable = wert;           ║
    // ║  Genau wie in JavaScript!                                         ║
    // ╚═══════════════════════════════════════════════════════════════════╝
    
    // TODO: Fülle die Lücke aus
    g_initialized = true;
    
    return g_initialized;
}

/**
 * Shutdown - Räumt auf
 */
SPOOF_API void Shutdown() {
    // ╔═══════════════════════════════════════════════════════════════════╗
    // ║  🎯 AUFGABE 2: Setze g_initialized zurück auf false               ║
    // ║                                                                   ║
    // ║  Wenn wir "herunterfahren", ist die Library nicht mehr bereit.    ║
    // ╚═══════════════════════════════════════════════════════════════════╝
    
    // TODO: Fülle die Lücke aus
    g_initialized = false;
}

/**
 * GetSpooflightVersion - Gibt Versionsnummer zurück
 * 
 * (Umbenannt von GetVersion wegen Konflikt mit Windows API!)
 */
SPOOF_API const char* GetSpooflightVersion() {
    return VERSION;
}

// ═══════════════════════════════════════════════════════════════════════════
//  REGISTRY FUNKTIONEN (Phase 2 - schon vorbereitet)
// ═══════════════════════════════════════════════════════════════════════════

/**
 * GetMachineGuid - Liest die MachineGuid aus der Windows Registry
 * 
 * 📚 LEKTION: Die Windows Registry
 * 
 * Die Registry ist wie eine große Datenbank mit Einstellungen.
 * Struktur: HKEY\Pfad\Schlüssel = Wert
 * 
 * MachineGuid liegt hier:
 *   HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Cryptography\MachineGuid
 * 
 * Diese GUID identifiziert deinen PC eindeutig!
 */
SPOOF_API bool GetMachineGuid(char* buffer, int bufferSize) {
    if (!g_initialized) {
        return false;  // Library nicht initialisiert!
    }
    
    // Registry-Schlüssel öffnen
    HKEY hKey;
    LONG result = RegOpenKeyExA(
        HKEY_LOCAL_MACHINE,                           // Hauptschlüssel
        "SOFTWARE\\Microsoft\\Cryptography",          // Pfad
        0,                                            // Optionen
        KEY_READ,                                     // Nur lesen
        &hKey                                         // Ausgabe: Handle
    );
    
    if (result != ERROR_SUCCESS) {
        return false;  // Konnte nicht öffnen
    }
    
    // Wert auslesen
    DWORD dataSize = bufferSize;
    result = RegQueryValueExA(
        hKey,                    // Handle von oben
        "MachineGuid",           // Name des Wertes
        NULL,                    // Reserviert
        NULL,                    // Typ (interessiert uns nicht)
        (LPBYTE)buffer,          // Wohin schreiben
        &dataSize                // Größe
    );
    
    // Aufräumen: Handle schließen
    RegCloseKey(hKey);
    
    return (result == ERROR_SUCCESS);
}

/**
 * SpoofMachineGuid - Ändert die MachineGuid
 * 
 * ⚠️ ACHTUNG: Braucht Administrator-Rechte!
 */
SPOOF_API SpoofResult SpoofMachineGuid(const char* newGuid) {
    SpoofResult result = {};  // Alles auf 0/leer initialisieren
    
    // ╔═══════════════════════════════════════════════════════════════════╗
    // ║  🎯 AUFGABE 4: Prüfe ob die Library initialisiert ist             ║
    // ║                                                                   ║
    // ║  Hinweis: if (!variable) prüft ob variable false ist              ║
    // ║  Genau wie in JavaScript!                                         ║
    // ║                                                                   ║
    // ║  Wenn NICHT initialisiert:                                        ║
    // ║    - setze result.success auf false                               ║
    // ║    - kopiere "Not initialized" in result.message                  ║
    // ║    - return result                                                ║
    // ╚═══════════════════════════════════════════════════════════════════╝
    
    if (!g_initialized) {
        result.success = false;
        CopyString(result.message, "Not initialized", sizeof(result.message));
        return result;
    }
    
    // Alten Wert speichern
    GetMachineGuid(result.oldValue, sizeof(result.oldValue));
    
    // Registry zum Schreiben öffnen
    HKEY hKey;
    LONG regResult = RegOpenKeyExA(
        HKEY_LOCAL_MACHINE,
        "SOFTWARE\\Microsoft\\Cryptography",
        0,
        KEY_WRITE,
        &hKey
    );
    
    if (regResult != ERROR_SUCCESS) {
        result.success = false;
        CopyString(result.message, "Failed to open registry (need admin?)", sizeof(result.message));
        return result;
    }
    
    // Neuen Wert schreiben
    regResult = RegSetValueExA(
        hKey,
        "MachineGuid",
        0,
        REG_SZ,
        (const BYTE*)newGuid,
        (DWORD)(strlen(newGuid) + 1)
    );
    
    RegCloseKey(hKey);
    
    if (regResult == ERROR_SUCCESS) {
        result.success = true;
        CopyString(result.message, "MachineGuid spoofed successfully!", sizeof(result.message));
        CopyString(result.newValue, newGuid, sizeof(result.newValue));
    } else {
        result.success = false;
        CopyString(result.message, "Failed to write registry value", sizeof(result.message));
    }
    
    return result;
}

// ═══════════════════════════════════════════════════════════════════════════
//  MAC ADDRESS SPOOFING (Phase 3)
// ═══════════════════════════════════════════════════════════════════════════

static const char* NETWORK_CLASS_KEY = 
    "SYSTEM\\CurrentControlSet\\Control\\Class\\{4d36e972-e325-11ce-bfc1-08002be10318}";

static void FormatMacAddress(const BYTE* mac, char* output) {
    sprintf(output, "%02X:%02X:%02X:%02X:%02X:%02X",
        mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}

static void NormalizeMacString(const char* input, char* output) {
    int j = 0;
    for (int i = 0; input[i] != '\0' && j < 12; i++) {
        char c = input[i];
        if ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f')) {
            output[j++] = (c >= 'a' && c <= 'f') ? (c - 32) : c;
        }
    }
    output[j] = '\0';
}

SPOOF_API AdapterList GetNetworkAdapters() {
    AdapterList list = {};
    
    if (!g_initialized) {
        return list;
    }
    
    ULONG bufferSize = 0;
    GetAdaptersInfo(NULL, &bufferSize);
    
    if (bufferSize == 0) {
        return list;
    }
    
    IP_ADAPTER_INFO* adapterInfo = (IP_ADAPTER_INFO*)malloc(bufferSize);
    if (!adapterInfo) {
        return list;
    }
    
    if (GetAdaptersInfo(adapterInfo, &bufferSize) != ERROR_SUCCESS) {
        free(adapterInfo);
        return list;
    }
    
    IP_ADAPTER_INFO* current = adapterInfo;
    while (current != NULL && list.count < MAX_ADAPTERS) {
        NetworkAdapter& adapter = list.adapters[list.count];
        
        CopyString(adapter.name, current->AdapterName, sizeof(adapter.name));
        CopyString(adapter.description, current->Description, sizeof(adapter.description));
        FormatMacAddress(current->Address, adapter.currentMac);
        adapter.index = current->Index;
        adapter.registryKey[0] = '\0';
        
        list.count++;
        current = current->Next;
    }
    
    free(adapterInfo);
    return list;
}

SPOOF_API bool GetMacAddress(int adapterIndex, char* buffer, int bufferSize) {
    if (!g_initialized || bufferSize < MAC_STRING_LENGTH) {
        return false;
    }
    
    AdapterList adapters = GetNetworkAdapters();
    
    for (int i = 0; i < adapters.count; i++) {
        if (adapters.adapters[i].index == adapterIndex) {
            CopyString(buffer, adapters.adapters[i].currentMac, bufferSize);
            return true;
        }
    }
    
    return false;
}

static bool FindAdapterRegistryKey(const char* description, char* outSubKey, int maxLen) {
    HKEY hClassKey;
    
    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, NETWORK_CLASS_KEY, 0, KEY_READ, &hClassKey) != ERROR_SUCCESS) {
        return false;
    }
    
    char subKeyName[16];
    DWORD subKeyIndex = 0;
    DWORD subKeyNameLen;
    
    while (true) {
        subKeyNameLen = sizeof(subKeyName);
        
        if (RegEnumKeyExA(hClassKey, subKeyIndex, subKeyName, &subKeyNameLen, 
                         NULL, NULL, NULL, NULL) != ERROR_SUCCESS) {
            break;
        }
        
        HKEY hSubKey;
        char fullPath[512];
        sprintf(fullPath, "%s\\%s", NETWORK_CLASS_KEY, subKeyName);
        
        if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, fullPath, 0, KEY_READ, &hSubKey) == ERROR_SUCCESS) {
            char driverDesc[256] = {0};
            DWORD descSize = sizeof(driverDesc);
            
            if (RegQueryValueExA(hSubKey, "DriverDesc", NULL, NULL, 
                                (LPBYTE)driverDesc, &descSize) == ERROR_SUCCESS) {
                if (strcmp(driverDesc, description) == 0) {
                    CopyString(outSubKey, subKeyName, maxLen);
                    RegCloseKey(hSubKey);
                    RegCloseKey(hClassKey);
                    return true;
                }
            }
            RegCloseKey(hSubKey);
        }
        
        subKeyIndex++;
    }
    
    RegCloseKey(hClassKey);
    return false;
}

SPOOF_API SpoofResult SpoofMacAddress(int adapterIndex, const char* newMac) {
    SpoofResult result = {};
    
    if (!g_initialized) {
        result.success = false;
        CopyString(result.message, "Not initialized", sizeof(result.message));
        return result;
    }
    
    char normalizedMac[13];
    NormalizeMacString(newMac, normalizedMac);
    
    if (strlen(normalizedMac) != 12) {
        result.success = false;
        CopyString(result.message, "Invalid MAC format! Use AA:BB:CC:DD:EE:FF", sizeof(result.message));
        return result;
    }
    
    AdapterList adapters = GetNetworkAdapters();
    NetworkAdapter* targetAdapter = NULL;
    
    for (int i = 0; i < adapters.count; i++) {
        if (adapters.adapters[i].index == adapterIndex) {
            targetAdapter = &adapters.adapters[i];
            break;
        }
    }
    
    if (!targetAdapter) {
        result.success = false;
        CopyString(result.message, "Adapter not found", sizeof(result.message));
        return result;
    }
    
    CopyString(result.oldValue, targetAdapter->currentMac, sizeof(result.oldValue));
    
    char subKey[16];
    if (!FindAdapterRegistryKey(targetAdapter->description, subKey, sizeof(subKey))) {
        result.success = false;
        CopyString(result.message, "Could not find adapter in registry", sizeof(result.message));
        return result;
    }
    
    char fullRegPath[512];
    sprintf(fullRegPath, "%s\\%s", NETWORK_CLASS_KEY, subKey);
    
    HKEY hKey;
    LONG regResult = RegOpenKeyExA(HKEY_LOCAL_MACHINE, fullRegPath, 0, KEY_WRITE, &hKey);
    
    if (regResult != ERROR_SUCCESS) {
        result.success = false;
        CopyString(result.message, "Failed to open registry (need admin?)", sizeof(result.message));
        return result;
    }
    
    regResult = RegSetValueExA(
        hKey,
        "NetworkAddress",
        0,
        REG_SZ,
        (const BYTE*)normalizedMac,
        (DWORD)(strlen(normalizedMac) + 1)
    );
    
    RegCloseKey(hKey);
    
    if (regResult == ERROR_SUCCESS) {
        result.success = true;
        sprintf(result.newValue, "%c%c:%c%c:%c%c:%c%c:%c%c:%c%c",
            normalizedMac[0], normalizedMac[1], normalizedMac[2], normalizedMac[3],
            normalizedMac[4], normalizedMac[5], normalizedMac[6], normalizedMac[7],
            normalizedMac[8], normalizedMac[9], normalizedMac[10], normalizedMac[11]);
        CopyString(result.message, "MAC spoofed! Restart adapter to apply.", sizeof(result.message));
    } else {
        result.success = false;
        CopyString(result.message, "Failed to write NetworkAddress to registry", sizeof(result.message));
    }
    
    return result;
}

// ═══════════════════════════════════════════════════════════════════════════
//  WMI / HARDWARE ID SPOOFING (Phase 4)
// ═══════════════════════════════════════════════════════════════════════════

/**
 * 📚 LEKTION: Volume Serial Number
 * 
 * Jede formatierte Partition hat eine Volume Serial Number.
 * Diese wird bei der Formatierung zufällig generiert.
 * 
 * Format: XXXX-XXXX (z.B. "1234-ABCD")
 * 
 * Windows API: GetVolumeInformationA()
 */
SPOOF_API bool GetVolumeSerial(const char* driveLetter, char* buffer, int bufferSize) {
    if (!g_initialized || bufferSize < 10) {
        return false;
    }
    
    // Laufwerkspfad erstellen: "C:\"
    char rootPath[8];
    sprintf(rootPath, "%c:\\", driveLetter[0]);
    
    DWORD volumeSerial = 0;
    
    // 📚 LEKTION: GetVolumeInformationA
    // Diese Windows API gibt Infos über ein Laufwerk zurück:
    // - Volume Name (z.B. "Windows")
    // - Serial Number
    // - Dateisystem (NTFS, FAT32, etc.)
    BOOL success = GetVolumeInformationA(
        rootPath,           // Laufwerkspfad
        NULL,               // Volume Name (brauchen wir nicht)
        0,                  // Volume Name Buffer Size
        &volumeSerial,      // Serial Number OUTPUT
        NULL,               // Max Component Length
        NULL,               // File System Flags
        NULL,               // File System Name
        0                   // File System Name Buffer Size
    );
    
    if (success && volumeSerial != 0) {
        // Format: XXXX-XXXX
        sprintf(buffer, "%04X-%04X", 
            (volumeSerial >> 16) & 0xFFFF,  // High Word
            volumeSerial & 0xFFFF);          // Low Word
        return true;
    }
    
    return false;
}

/**
 * 📚 LEKTION: Disk Serial Number via Registry
 * 
 * Die physische Festplatten-Seriennummer ist schwerer zu lesen.
 * WMI nutzt Win32_DiskDrive, aber wir können auch die Registry nutzen:
 * HKLM\HARDWARE\DEVICEMAP\Scsi\...
 * 
 * Einfacher: Wir lesen über SetupAPI die Disk ID
 */
SPOOF_API bool GetDiskSerial(char* buffer, int bufferSize) {
    if (!g_initialized || bufferSize < 32) {
        return false;
    }
    
    // Methode 1: Über IOCTL (braucht Handle auf physisches Laufwerk)
    HANDLE hDrive = CreateFileA(
        "\\\\.\\PhysicalDrive0",    // Erste physische Festplatte
        0,                          // Keine speziellen Rechte nötig für Query
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        OPEN_EXISTING,
        0,
        NULL
    );
    
    if (hDrive == INVALID_HANDLE_VALUE) {
        CopyString(buffer, "ACCESS_DENIED", bufferSize);
        return false;
    }
    
    // STORAGE_PROPERTY_QUERY für Serial Number
    struct {
        STORAGE_PROPERTY_QUERY query;
        char buffer[1024];
    } queryBuffer = {};
    
    queryBuffer.query.PropertyId = StorageDeviceProperty;
    queryBuffer.query.QueryType = PropertyStandardQuery;
    
    STORAGE_DEVICE_DESCRIPTOR* descriptor = (STORAGE_DEVICE_DESCRIPTOR*)&queryBuffer;
    DWORD bytesReturned = 0;
    
    BOOL success = DeviceIoControl(
        hDrive,
        IOCTL_STORAGE_QUERY_PROPERTY,
        &queryBuffer.query,
        sizeof(STORAGE_PROPERTY_QUERY),
        &queryBuffer,
        sizeof(queryBuffer),
        &bytesReturned,
        NULL
    );
    
    CloseHandle(hDrive);
    
    if (success && descriptor->SerialNumberOffset != 0) {
        char* serial = (char*)descriptor + descriptor->SerialNumberOffset;
        
        // Serial trimmen (oft mit Leerzeichen gepadded)
        while (*serial == ' ') serial++;
        CopyString(buffer, serial, bufferSize);
        
        // Trailing spaces entfernen
        int len = strlen(buffer);
        while (len > 0 && buffer[len-1] == ' ') {
            buffer[--len] = '\0';
        }
        
        return true;
    }
    
    CopyString(buffer, "NOT_FOUND", bufferSize);
    return false;
}

/**
 * 📚 LEKTION: SMBIOS UUID
 * 
 * Die SMBIOS (System Management BIOS) UUID ist eine eindeutige ID
 * die im BIOS/UEFI gespeichert ist. Sie identifiziert das Motherboard.
 * 
 * Windows API: GetSystemFirmwareTable()
 * 
 * Das Ändern dieser UUID erfordert einen Kernel-Treiber oder BIOS-Modifikation!
 */
SPOOF_API bool GetSmbiosUuid(char* buffer, int bufferSize) {
    if (!g_initialized || bufferSize < 40) {
        return false;
    }
    
    // Größe der SMBIOS-Tabelle ermitteln
    DWORD smbiosSize = GetSystemFirmwareTable('RSMB', 0, NULL, 0);
    
    if (smbiosSize == 0) {
        CopyString(buffer, "NOT_AVAILABLE", bufferSize);
        return false;
    }
    
    // Buffer allozieren
    BYTE* smbiosData = (BYTE*)malloc(smbiosSize);
    if (!smbiosData) {
        return false;
    }
    
    // SMBIOS-Tabelle lesen
    DWORD bytesRead = GetSystemFirmwareTable('RSMB', 0, smbiosData, smbiosSize);
    
    if (bytesRead != smbiosSize) {
        free(smbiosData);
        return false;
    }
    
    // SMBIOS Struktur parsen
    // Die UUID ist in Type 1 (System Information) Struktur
    // Offset 8-23 (16 Bytes)
    
    // Header überspringen (8 Bytes)
    BYTE* data = smbiosData + 8;
    BYTE* end = smbiosData + smbiosSize;
    
    while (data < end) {
        BYTE type = data[0];
        BYTE length = data[1];
        
        if (type == 1 && length >= 25) {  // System Information
            // UUID ist bei Offset 8 in der Struktur
            BYTE* uuid = data + 8;
            
            // UUID formatieren (RFC 4122 Format)
            sprintf(buffer, 
                "%02X%02X%02X%02X-%02X%02X-%02X%02X-%02X%02X-%02X%02X%02X%02X%02X%02X",
                uuid[3], uuid[2], uuid[1], uuid[0],  // Time-Low (little-endian)
                uuid[5], uuid[4],                     // Time-Mid
                uuid[7], uuid[6],                     // Time-High
                uuid[8], uuid[9],                     // Clock-Seq
                uuid[10], uuid[11], uuid[12], uuid[13], uuid[14], uuid[15]  // Node
            );
            
            free(smbiosData);
            return true;
        }
        
        // Zur nächsten Struktur springen
        data += length;
        
        // Strings überspringen (null-terminiert, doppel-null am Ende)
        while (data < end - 1 && !(data[0] == 0 && data[1] == 0)) {
            data++;
        }
        data += 2;  // Doppel-Null überspringen
    }
    
    free(smbiosData);
    CopyString(buffer, "NOT_FOUND", bufferSize);
    return false;
}

/**
 * 📚 LEKTION: Windows Product ID
 * 
 * Die Product ID ist in der Registry gespeichert:
 * HKLM\SOFTWARE\Microsoft\Windows NT\CurrentVersion\ProductId
 * 
 * Diese können wir leicht lesen UND ändern!
 */
SPOOF_API bool GetProductId(char* buffer, int bufferSize) {
    if (!g_initialized) {
        return false;
    }
    
    HKEY hKey;
    LONG result = RegOpenKeyExA(
        HKEY_LOCAL_MACHINE,
        "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion",
        0,
        KEY_READ,
        &hKey
    );
    
    if (result != ERROR_SUCCESS) {
        return false;
    }
    
    DWORD dataSize = bufferSize;
    result = RegQueryValueExA(
        hKey,
        "ProductId",
        NULL,
        NULL,
        (LPBYTE)buffer,
        &dataSize
    );
    
    RegCloseKey(hKey);
    return (result == ERROR_SUCCESS);
}

/**
 * Spooft die Windows Product ID
 */
SPOOF_API SpoofResult SpoofProductId(const char* newProductId) {
    SpoofResult result = {};
    
    if (!g_initialized) {
        result.success = false;
        CopyString(result.message, "Not initialized", sizeof(result.message));
        return result;
    }
    
    // Alten Wert speichern
    GetProductId(result.oldValue, sizeof(result.oldValue));
    
    HKEY hKey;
    LONG regResult = RegOpenKeyExA(
        HKEY_LOCAL_MACHINE,
        "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion",
        0,
        KEY_WRITE,
        &hKey
    );
    
    if (regResult != ERROR_SUCCESS) {
        result.success = false;
        CopyString(result.message, "Failed to open registry (need admin?)", sizeof(result.message));
        return result;
    }
    
    regResult = RegSetValueExA(
        hKey,
        "ProductId",
        0,
        REG_SZ,
        (const BYTE*)newProductId,
        (DWORD)(strlen(newProductId) + 1)
    );
    
    RegCloseKey(hKey);
    
    if (regResult == ERROR_SUCCESS) {
        result.success = true;
        CopyString(result.message, "Product ID spoofed successfully!", sizeof(result.message));
        CopyString(result.newValue, newProductId, sizeof(result.newValue));
    } else {
        result.success = false;
        CopyString(result.message, "Failed to write Product ID", sizeof(result.message));
    }
    
    return result;
}

/**
 * Liest alle Hardware-Infos auf einmal
 */
SPOOF_API HardwareInfo GetHardwareInfo() {
    HardwareInfo info = {};
    
    if (!g_initialized) {
        return info;
    }
    
    // Disk Serial
    GetDiskSerial(info.diskSerial, sizeof(info.diskSerial));
    
    // Volume Serial (C:)
    GetVolumeSerial("C", info.volumeSerial, sizeof(info.volumeSerial));
    
    // SMBIOS UUID (wird als BIOS Serial angezeigt)
    GetSmbiosUuid(info.biosSerial, sizeof(info.biosSerial));
    
    // Product ID
    GetProductId(info.productId, sizeof(info.productId));
    
    // Motherboard Serial (auch aus SMBIOS, aber anderes Feld)
    // Für jetzt: gleich wie BIOS
    CopyString(info.motherboardSerial, info.biosSerial, sizeof(info.motherboardSerial));
    
    // CPU ID
    int cpuInfo[4] = {0};
    __cpuid(cpuInfo, 1);
    sprintf(info.cpuId, "%08X%08X", cpuInfo[3], cpuInfo[0]);
    
    return info;
}

/**
 * Generiert eine zufällige Volume Serial Number
 */
SPOOF_API void GenerateRandomVolumeSerial(char* buffer, int bufferSize) {
    if (bufferSize < 10) return;
    
    static bool seeded = false;
    if (!seeded) {
        srand((unsigned int)time(NULL));
        seeded = true;
    }
    
    DWORD serial = ((DWORD)rand() << 16) | (DWORD)rand();
    sprintf(buffer, "%04X-%04X", 
        (serial >> 16) & 0xFFFF,
        serial & 0xFFFF);
}

/**
 * Generiert eine zufällige Product ID im Windows-Format
 * Format: XXXXX-XXX-XXXXXXX-XXXXX
 */
SPOOF_API void GenerateRandomProductId(char* buffer, int bufferSize) {
    if (bufferSize < 30) return;
    
    static bool seeded = false;
    if (!seeded) {
        srand((unsigned int)time(NULL));
        seeded = true;
    }
    
    sprintf(buffer, "%05d-%03d-%07d-%05d",
        rand() % 100000,
        rand() % 1000,
        rand() % 10000000,
        rand() % 100000);
}

/**
 * 📚 LEKTION: Volume Serial Spoofing
 * 
 * Die Volume Serial steht im Boot Sector der Partition.
 * Direktes Ändern ist gefährlich! 
 * 
 * Alternative: volumeid.exe von Sysinternals
 * Oder: Wir schreiben direkt in den Boot Sector (GEFÄHRLICH!)
 * 
 * Für jetzt: Wir geben eine Warnung aus, dass es nicht direkt geht.
 */
SPOOF_API SpoofResult SpoofVolumeSerial(const char* driveLetter, const char* newSerial) {
    SpoofResult result = {};
    
    if (!g_initialized) {
        result.success = false;
        CopyString(result.message, "Not initialized", sizeof(result.message));
        return result;
    }
    
    // Alten Wert speichern
    GetVolumeSerial(driveLetter, result.oldValue, sizeof(result.oldValue));
    
    // Volume Serial kann nicht einfach über die Registry geändert werden.
    // Man muss den Boot Sector modifizieren - das ist riskant!
    
    // Für ein sicheres Spoofing empfehlen wir volumeid.exe von Sysinternals:
    // https://docs.microsoft.com/en-us/sysinternals/downloads/volumeid
    
    result.success = false;
    CopyString(result.message, 
        "Volume Serial spoofing requires boot sector modification.\n"
        "Use Sysinternals 'volumeid.exe' tool for safe spoofing.\n"
        "Download: https://docs.microsoft.com/sysinternals/downloads/volumeid",
        sizeof(result.message));
    
    return result;
}

// ═══════════════════════════════════════════════════════════════════════════
//  ERWEITERTE SYSTEM-ANALYSE (Phase 5)
// ═══════════════════════════════════════════════════════════════════════════

/**
 * 📚 LEKTION: TPM (Trusted Platform Module)
 * 
 * TPM ist ein Sicherheits-Chip der kryptographische Schlüssel speichert.
 * 
 * Typen:
 * - dTPM: Discrete TPM - separater Chip auf dem Motherboard
 * - fTPM: Firmware TPM - in der CPU integriert (Intel PTT, AMD fTPM)
 * 
 * Der Endorsement Key (EK) ist einzigartig und kann NICHT geändert werden!
 * Aber: TPM kann deaktiviert werden, dann ist es für Software unsichtbar.
 */
SPOOF_API TpmInfo GetTpmInfo() {
    TpmInfo info = {};
    
    // TPM Status über Registry prüfen
    HKEY hKey;
    LONG result = RegOpenKeyExA(
        HKEY_LOCAL_MACHINE,
        "SYSTEM\\CurrentControlSet\\Services\\TPM",
        0,
        KEY_READ,
        &hKey
    );
    
    if (result == ERROR_SUCCESS) {
        info.present = true;
        
        DWORD startType = 0;
        DWORD dataSize = sizeof(DWORD);
        
        if (RegQueryValueExA(hKey, "Start", NULL, NULL, (LPBYTE)&startType, &dataSize) == ERROR_SUCCESS) {
            info.enabled = (startType != 4);  // 4 = Disabled
        }
        
        RegCloseKey(hKey);
    }
    
    // TPM WMI-Informationen über Registry (vereinfacht)
    result = RegOpenKeyExA(
        HKEY_LOCAL_MACHINE,
        "SYSTEM\\CurrentControlSet\\Services\\TPM\\WMI",
        0,
        KEY_READ,
        &hKey
    );
    
    if (result == ERROR_SUCCESS) {
        char buffer[256] = {};
        DWORD bufferSize = sizeof(buffer);
        
        if (RegQueryValueExA(hKey, "ManufacturerId", NULL, NULL, (LPBYTE)buffer, &bufferSize) == ERROR_SUCCESS) {
            // ManufacturerId ist ein DWORD, konvertieren
            DWORD mfgId = *(DWORD*)buffer;
            sprintf(info.manufacturer, "0x%08X", mfgId);
        }
        
        RegCloseKey(hKey);
    }
    
    // Versuche TPM-Version zu lesen
    result = RegOpenKeyExA(
        HKEY_LOCAL_MACHINE,
        "SOFTWARE\\Microsoft\\Tpm",
        0,
        KEY_READ,
        &hKey
    );
    
    if (result == ERROR_SUCCESS) {
        char specVersion[64] = {};
        DWORD bufferSize = sizeof(specVersion);
        
        if (RegQueryValueExA(hKey, "SpecVersion", NULL, NULL, (LPBYTE)specVersion, &bufferSize) == ERROR_SUCCESS) {
            CopyString(info.version, specVersion, sizeof(info.version));
        }
        
        RegCloseKey(hKey);
    }
    
    // Status-Text generieren
    if (!info.present) {
        CopyString(info.status, "TPM not detected or driver not installed", sizeof(info.status));
    } else if (!info.enabled) {
        CopyString(info.status, "TPM present but DISABLED - good for privacy!", sizeof(info.status));
    } else {
        sprintf(info.status, "TPM %s ACTIVE - Consider disabling in BIOS for full spoof", 
                strlen(info.version) > 0 ? info.version : "2.0");
    }
    
    info.canBeDisabled = true;  // TPM kann immer im BIOS deaktiviert werden
    
    // EK Hash - wir können den echten EK nicht lesen (Sicherheit!), aber Status prüfen
    CopyString(info.endorsementKeyHash, "Cannot read - TPM protected", sizeof(info.endorsementKeyHash));
    
    return info;
}

/**
 * 📚 LEKTION: CPUID Instruktion
 * 
 * CPUID ist eine x86-Instruktion die CPU-Informationen zurückgibt.
 * Sie kann NICHT im User-Mode gefälscht werden!
 * 
 * Möglichkeiten zum "Spoofen":
 * 1. Hypervisor (VMX) - fängt CPUID ab und gibt andere Werte zurück
 * 2. VM nutzen - VM gibt ihre eigenen CPUID-Werte
 * 3. Kernel-Treiber mit VMX - Komplexeste Lösung
 */
SPOOF_API CpuInfo GetCpuInfo() {
    CpuInfo info = {};
    int cpuInfo[4] = {0};
    
    // CPUID 0x00 - Vendor String
    __cpuid(cpuInfo, 0);
    memcpy(info.vendor, &cpuInfo[1], 4);      // EBX
    memcpy(info.vendor + 4, &cpuInfo[3], 4);  // EDX
    memcpy(info.vendor + 8, &cpuInfo[2], 4);  // ECX
    info.vendor[12] = '\0';
    
    // CPUID 0x01 - Family, Model, Stepping
    __cpuid(cpuInfo, 1);
    info.stepping = cpuInfo[0] & 0xF;
    info.model = (cpuInfo[0] >> 4) & 0xF;
    info.family = (cpuInfo[0] >> 8) & 0xF;
    
    // Extended Model/Family
    if (info.family == 0xF) {
        info.family += (cpuInfo[0] >> 20) & 0xFF;
    }
    if (info.family >= 6) {
        info.model += ((cpuInfo[0] >> 16) & 0xF) << 4;
    }
    
    // Processor ID (was viele Anti-Cheats lesen)
    sprintf(info.processorId, "%08X%08X", cpuInfo[3], cpuInfo[0]);
    
    // CPUID 0x80000002-4 - Brand String
    char brand[48] = {};
    __cpuid(cpuInfo, 0x80000002);
    memcpy(brand, cpuInfo, 16);
    __cpuid(cpuInfo, 0x80000003);
    memcpy(brand + 16, cpuInfo, 16);
    __cpuid(cpuInfo, 0x80000004);
    memcpy(brand + 32, cpuInfo, 16);
    
    // Trim leading spaces
    char* brandStart = brand;
    while (*brandStart == ' ') brandStart++;
    CopyString(info.brand, brandStart, sizeof(info.brand));
    
    // Hypervisor Detection (CPUID 0x01, ECX Bit 31)
    __cpuid(cpuInfo, 1);
    info.hypervisorPresent = (cpuInfo[2] >> 31) & 1;
    
    if (info.hypervisorPresent) {
        // CPUID 0x40000000 - Hypervisor Vendor
        __cpuid(cpuInfo, 0x40000000);
        memcpy(info.hypervisorVendor, &cpuInfo[1], 4);
        memcpy(info.hypervisorVendor + 4, &cpuInfo[2], 4);
        memcpy(info.hypervisorVendor + 8, &cpuInfo[3], 4);
        info.hypervisorVendor[12] = '\0';
    }
    
    // Microcode Version aus Registry
    HKEY hKey;
    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, 
        "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0", 
        0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        
        DWORD updateRev = 0;
        DWORD dataSize = sizeof(DWORD);
        if (RegQueryValueExA(hKey, "Update Revision", NULL, NULL, (LPBYTE)&updateRev, &dataSize) == ERROR_SUCCESS) {
            sprintf(info.microcode, "0x%08X", updateRev);
        }
        RegCloseKey(hKey);
    }
    
    return info;
}

/**
 * 📚 LEKTION: NVMe Serial Numbers
 * 
 * NVMe-Serials werden über IOCTL_STORAGE_QUERY_PROPERTY gelesen.
 * Sie sind in der Firmware der SSD gebrannt.
 * 
 * Möglichkeiten:
 * 1. Filter-Treiber: Fängt IOCTLs ab, gibt fake Serial zurück
 * 2. WMI Provider Hook: WMI-Anfragen abfangen
 * 3. Hardware: Manche SSDs erlauben Firmware-Modding (riskant!)
 */
SPOOF_API DiskInfo GetDiskInfo(int diskIndex) {
    DiskInfo info = {};
    
    char drivePath[32];
    sprintf(drivePath, "\\\\.\\PhysicalDrive%d", diskIndex);
    
    HANDLE hDrive = CreateFileA(
        drivePath,
        0,  // Keine speziellen Rechte für Query
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        OPEN_EXISTING,
        0,
        NULL
    );
    
    if (hDrive == INVALID_HANDLE_VALUE) {
        CopyString(info.serial, "DRIVE_NOT_FOUND", sizeof(info.serial));
        CopyString(info.spoofingAdvice, "Drive not accessible", sizeof(info.spoofingAdvice));
        return info;
    }
    
    // Storage Property Query
    struct {
        STORAGE_PROPERTY_QUERY query;
        char buffer[1024];
    } queryBuffer = {};
    
    queryBuffer.query.PropertyId = StorageDeviceProperty;
    queryBuffer.query.QueryType = PropertyStandardQuery;
    
    STORAGE_DEVICE_DESCRIPTOR* descriptor = (STORAGE_DEVICE_DESCRIPTOR*)&queryBuffer;
    DWORD bytesReturned = 0;
    
    BOOL success = DeviceIoControl(
        hDrive,
        IOCTL_STORAGE_QUERY_PROPERTY,
        &queryBuffer.query,
        sizeof(STORAGE_PROPERTY_QUERY),
        &queryBuffer,
        sizeof(queryBuffer),
        &bytesReturned,
        NULL
    );
    
    if (success) {
        // Model
        if (descriptor->ProductIdOffset != 0) {
            char* model = (char*)descriptor + descriptor->ProductIdOffset;
            CopyString(info.model, model, sizeof(info.model));
            // Trim
            int len = strlen(info.model);
            while (len > 0 && info.model[len-1] == ' ') info.model[--len] = '\0';
        }
        
        // Serial
        if (descriptor->SerialNumberOffset != 0) {
            char* serial = (char*)descriptor + descriptor->SerialNumberOffset;
            while (*serial == ' ') serial++;
            CopyString(info.serial, serial, sizeof(info.serial));
            int len = strlen(info.serial);
            while (len > 0 && info.serial[len-1] == ' ') info.serial[--len] = '\0';
        }
        
        // Firmware
        if (descriptor->ProductRevisionOffset != 0) {
            char* fw = (char*)descriptor + descriptor->ProductRevisionOffset;
            CopyString(info.firmware, fw, sizeof(info.firmware));
        }
        
        // Interface Type
        info.isNvme = (descriptor->BusType == BusTypeNvme);
        
        switch (descriptor->BusType) {
            case BusTypeNvme: CopyString(info.interfaceType, "NVMe", sizeof(info.interfaceType)); break;
            case BusTypeSata: CopyString(info.interfaceType, "SATA", sizeof(info.interfaceType)); break;
            case BusTypeUsb: CopyString(info.interfaceType, "USB", sizeof(info.interfaceType)); break;
            default: sprintf(info.interfaceType, "Other (%d)", descriptor->BusType); break;
        }
    }
    
    CloseHandle(hDrive);
    
    // Spoofing Advice
    info.serialCanBeHidden = false;
    
    if (info.isNvme) {
        CopyString(info.spoofingAdvice, 
            "NVMe Serial is hardware-burned. Options:\n"
            "1. Filter Driver (kernel-mode, intercepts IOCTLs)\n"
            "2. Run in VM with virtual NVMe\n"
            "3. Use RAID controller that hides individual serials",
            sizeof(info.spoofingAdvice));
    } else {
        CopyString(info.spoofingAdvice,
            "SATA Serial is hardware-burned. Options:\n"
            "1. Filter Driver (kernel-mode)\n"
            "2. Some SATA controllers allow SMART data modification\n"
            "3. Virtual machine",
            sizeof(info.spoofingAdvice));
    }
    
    return info;
}

/**
 * 📚 LEKTION: Permanente vs. Aktuelle MAC-Adresse
 * 
 * Jede NIC hat zwei MACs:
 * 1. Permanent MAC: Im EEPROM gebrannt (OUI + Unique ID)
 * 2. Current MAC: Was der Treiber nutzt (kann gespooft sein!)
 * 
 * Die permanente MAC können manche Tools auslesen, auch wenn du spoofst!
 */
SPOOF_API NicInfo GetNicInfo(int nicIndex) {
    NicInfo info = {};
    
    // Adapter-Liste holen
    ULONG bufferSize = 0;
    GetAdaptersInfo(NULL, &bufferSize);
    
    if (bufferSize == 0) {
        CopyString(info.description, "No adapters found", sizeof(info.description));
        return info;
    }
    
    IP_ADAPTER_INFO* adapterInfo = (IP_ADAPTER_INFO*)malloc(bufferSize);
    if (!adapterInfo || GetAdaptersInfo(adapterInfo, &bufferSize) != ERROR_SUCCESS) {
        free(adapterInfo);
        return info;
    }
    
    IP_ADAPTER_INFO* current = adapterInfo;
    int index = 0;
    
    while (current && index < nicIndex) {
        current = current->Next;
        index++;
    }
    
    if (current) {
        CopyString(info.description, current->Description, sizeof(info.description));
        
        // Aktuelle MAC
        sprintf(info.currentMac, "%02X:%02X:%02X:%02X:%02X:%02X",
            current->Address[0], current->Address[1], current->Address[2],
            current->Address[3], current->Address[4], current->Address[5]);
        
        // Permanente MAC - über Registry lesen
        char regPath[512];
        sprintf(regPath, "SYSTEM\\CurrentControlSet\\Control\\Class\\{4d36e972-e325-11ce-bfc1-08002be10318}");
        
        HKEY hClassKey;
        if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, regPath, 0, KEY_READ, &hClassKey) == ERROR_SUCCESS) {
            char subKeyName[16];
            DWORD subKeyIndex = 0;
            DWORD subKeyNameLen;
            
            while (true) {
                subKeyNameLen = sizeof(subKeyName);
                if (RegEnumKeyExA(hClassKey, subKeyIndex, subKeyName, &subKeyNameLen, NULL, NULL, NULL, NULL) != ERROR_SUCCESS) break;
                
                HKEY hSubKey;
                char fullPath[512];
                sprintf(fullPath, "%s\\%s", regPath, subKeyName);
                
                if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, fullPath, 0, KEY_READ, &hSubKey) == ERROR_SUCCESS) {
                    char driverDesc[256] = {};
                    DWORD descSize = sizeof(driverDesc);
                    
                    if (RegQueryValueExA(hSubKey, "DriverDesc", NULL, NULL, (LPBYTE)driverDesc, &descSize) == ERROR_SUCCESS) {
                        if (strcmp(driverDesc, current->Description) == 0) {
                            // Gefunden! Driver Name lesen
                            char driverName[128] = {};
                            DWORD driverSize = sizeof(driverName);
                            RegQueryValueExA(hSubKey, "DriverDisplayName", NULL, NULL, (LPBYTE)driverName, &driverSize);
                            CopyString(info.driverName, driverName, sizeof(info.driverName));
                            
                            // Check ob NetworkAddress gesetzt ist (= gespooft)
                            char networkAddr[18] = {};
                            DWORD addrSize = sizeof(networkAddr);
                            if (RegQueryValueExA(hSubKey, "NetworkAddress", NULL, NULL, (LPBYTE)networkAddr, &addrSize) == ERROR_SUCCESS) {
                                info.isSpoofed = (strlen(networkAddr) > 0);
                            }
                            
                            RegCloseKey(hSubKey);
                            break;
                        }
                    }
                    RegCloseKey(hSubKey);
                }
                subKeyIndex++;
            }
            RegCloseKey(hClassKey);
        }
        
        // Permanente MAC = Aktuelle wenn nicht gespooft, sonst unbekannt
        if (!info.isSpoofed) {
            CopyString(info.permanentMac, info.currentMac, sizeof(info.permanentMac));
        } else {
            CopyString(info.permanentMac, "HIDDEN (spoofed)", sizeof(info.permanentMac));
        }
        
        info.supportsRegistrySpoof = true;  // Fast alle NICs unterstützen das
    }
    
    free(adapterInfo);
    return info;
}

/**
 * Prüft ob wir in einer VM laufen
 */
SPOOF_API bool IsRunningInVM() {
    CpuInfo cpu = GetCpuInfo();
    return cpu.hypervisorPresent;
}

/**
 * Liest erweiterte SMBIOS-Informationen
 */
static void ReadSmbiosStrings(BYTE* data, BYTE length, int stringIndex, char* output, int outputSize) {
    if (stringIndex == 0) {
        CopyString(output, "", outputSize);
        return;
    }
    
    BYTE* stringStart = data + length;
    int currentString = 1;
    
    while (currentString < stringIndex && *stringStart != 0) {
        while (*stringStart != 0) stringStart++;
        stringStart++;
        currentString++;
    }
    
    if (*stringStart != 0) {
        CopyString(output, (char*)stringStart, outputSize);
    }
}

/**
 * Komplette System-Analyse
 */
SPOOF_API SystemAnalysis AnalyzeSystem() {
    SystemAnalysis analysis = {};
    
    // TPM
    analysis.tpm = GetTpmInfo();
    
    // CPU
    analysis.cpu = GetCpuInfo();
    
    // Disks (bis zu 8)
    for (int i = 0; i < 8; i++) {
        DiskInfo disk = GetDiskInfo(i);
        if (strlen(disk.serial) > 0 && strcmp(disk.serial, "DRIVE_NOT_FOUND") != 0) {
            analysis.disks[analysis.diskCount++] = disk;
        }
    }
    
    // NICs (bis zu 8)
    for (int i = 0; i < 8; i++) {
        NicInfo nic = GetNicInfo(i);
        if (strlen(nic.description) > 0 && strcmp(nic.description, "No adapters found") != 0) {
            analysis.nics[analysis.nicCount++] = nic;
        } else {
            break;
        }
    }
    
    // SMBIOS Details
    GetSmbiosUuid(analysis.smbiosUuid, sizeof(analysis.smbiosUuid));
    
    // Erweiterte SMBIOS-Daten
    DWORD smbiosSize = GetSystemFirmwareTable('RSMB', 0, NULL, 0);
    if (smbiosSize > 0) {
        BYTE* smbiosData = (BYTE*)malloc(smbiosSize);
        if (smbiosData && GetSystemFirmwareTable('RSMB', 0, smbiosData, smbiosSize) == smbiosSize) {
            BYTE* data = smbiosData + 8;
            BYTE* end = smbiosData + smbiosSize;
            
            while (data < end) {
                BYTE type = data[0];
                BYTE length = data[1];
                
                if (type == 0 && length >= 18) {  // BIOS Information
                    ReadSmbiosStrings(data, length, data[4], analysis.biosVendor, sizeof(analysis.biosVendor));
                    ReadSmbiosStrings(data, length, data[5], analysis.biosVersion, sizeof(analysis.biosVersion));
                }
                else if (type == 1 && length >= 25) {  // System Information
                    ReadSmbiosStrings(data, length, data[4], analysis.systemManufacturer, sizeof(analysis.systemManufacturer));
                    ReadSmbiosStrings(data, length, data[5], analysis.systemProduct, sizeof(analysis.systemProduct));
                }
                else if (type == 2 && length >= 8) {  // Baseboard
                    ReadSmbiosStrings(data, length, data[7], analysis.baseboardSerial, sizeof(analysis.baseboardSerial));
                }
                
                // Zur nächsten Struktur
                data += length;
                while (data < end - 1 && !(data[0] == 0 && data[1] == 0)) data++;
                data += 2;
            }
        }
        free(smbiosData);
    }
    
    // Empfehlungen generieren
    GetSpoofingRecommendations(analysis.recommendations, sizeof(analysis.recommendations));
    
    return analysis;
}

/**
 * Generiert Spoofing-Empfehlungen
 */
SPOOF_API void GetSpoofingRecommendations(char* buffer, int bufferSize) {
    char recommendations[1024] = {};
    
    strcat(recommendations, "=== SPOOFING RECOMMENDATIONS ===\n\n");
    
    // TPM Check
    TpmInfo tpm = GetTpmInfo();
    if (tpm.present && tpm.enabled) {
        strcat(recommendations, "[!] TPM ACTIVE: Disable in BIOS for full spoof!\n");
    } else if (tpm.present) {
        strcat(recommendations, "[OK] TPM disabled\n");
    }
    
    // VM Check
    if (IsRunningInVM()) {
        strcat(recommendations, "[OK] Running in VM - hardware IDs are virtual\n");
    } else {
        strcat(recommendations, "[!] Bare metal: Consider VM for hardware ID isolation\n");
    }
    
    // Disk advice
    strcat(recommendations, "\n[DISK] Serials burned in firmware:\n");
    strcat(recommendations, "  -> Filter Driver or VM required\n");
    
    // MAC advice
    strcat(recommendations, "\n[MAC] Registry spoof works for most apps\n");
    strcat(recommendations, "  -> Some tools read permanent MAC via WMI\n");
    
    // Full spoof strategy
    strcat(recommendations, "\n=== FULL SPOOF STRATEGY ===\n");
    strcat(recommendations, "1. Disable fTPM in BIOS\n");
    strcat(recommendations, "2. Flash BIOS (new SMBIOS UUID)\n");
    strcat(recommendations, "3. Clean Windows install\n");
    strcat(recommendations, "4. Use this tool for remaining IDs\n");
    strcat(recommendations, "5. For disk serial: VM or Filter Driver\n");
    
    CopyString(buffer, recommendations, bufferSize);
}

