/**
 * ═══════════════════════════════════════════════════════════════════════════
 *  SPOOFLIGHT - HAUPTHEADER
 *  Hier deklarieren wir alle Funktionen, die unsere DLL anbietet
 * ═══════════════════════════════════════════════════════════════════════════
 * 
 * 📚 LEKTION: Header Guards (#ifndef ... #endif)
 * 
 * Stell dir vor, zwei Dateien inkludieren beide "spoofer.h"
 * Ohne Guards würde der Code 2x eingefügt → Fehler!
 * 
 * Mit Guards:
 *   1. Erstes #include: SPOOFLIGHT_SPOOFER_H existiert nicht → Code wird eingefügt
 *   2. Zweites #include: SPOOFLIGHT_SPOOFER_H existiert bereits → Code wird übersprungen
 */

#ifndef SPOOFLIGHT_SPOOFER_H
#define SPOOFLIGHT_SPOOFER_H

#include "exports.h"

// ═══════════════════════════════════════════════════════════════════════════
//  DATENTYPEN
// ═══════════════════════════════════════════════════════════════════════════

/**
 * 📚 LEKTION: Structs in C++
 * 
 * Ein Struct ist wie ein JavaScript Objekt, aber mit festen Typen.
 * 
 * JavaScript:  { success: true, message: "OK" }
 * C++:         struct Result { bool success; char message[256]; };
 * 
 * Wichtig: In C++ musst du die maximale Größe von Strings vorher festlegen!
 * char message[256] = "Platz für 256 Zeichen"
 */
struct SpoofResult {
    bool success;           // true = hat geklappt, false = Fehler
    char message[256];      // Nachricht (max 255 Zeichen + Nullterminator)
    char oldValue[256];     // Alter Wert vor dem Spoofing
    char newValue[256];     // Neuer Wert nach dem Spoofing
};

// ═══════════════════════════════════════════════════════════════════════════
//  EXPORTIERTE FUNKTIONEN
// ═══════════════════════════════════════════════════════════════════════════

/**
 * Initialisiert die Spoofer-Library
 * Muss als erstes aufgerufen werden!
 *
 * @return true wenn erfolgreich
 */

SPOOF_API bool Initialize();

/**
 * Räumt auf und gibt Ressourcen frei
 * Sollte am Ende aufgerufen werden
 */
SPOOF_API void Shutdown();

/**
 * Gibt die Version der Library zurück
 * 
 * 📚 LEKTION: Namenskollisionen vermeiden!
 * Windows hat schon eine GetVersion() Funktion.
 * Deshalb nennen wir unsere GetSpooflightVersion().
 * 
 * @return Version als String, z.B. "1.0.0"
 */
SPOOF_API const char* GetSpooflightVersion();

// ═══════════════════════════════════════════════════════════════════════════
//  REGISTRY SPOOFING (Phase 2)
// ═══════════════════════════════════════════════════════════════════════════

/**
 * Liest die aktuelle MachineGuid aus der Registry
 * 
 * @param buffer      - Wohin der Wert geschrieben werden soll
 * @param bufferSize  - Größe des Buffers
 * @return true wenn erfolgreich
 */
SPOOF_API bool GetMachineGuid(char* buffer, int bufferSize);

/**
 * Ändert die MachineGuid in der Registry
 * 
 * @param newGuid - Die neue GUID (Format: xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx)
 * @return SpoofResult mit Erfolg/Fehler Info
 */
SPOOF_API SpoofResult SpoofMachineGuid(const char* newGuid);

// ═══════════════════════════════════════════════════════════════════════════
//  MAC ADDRESS SPOOFING (Phase 3)
// ═══════════════════════════════════════════════════════════════════════════

/**
 * 📚 LEKTION: Structs für mehrere Rückgabewerte
 * 
 * In JavaScript würdest du ein Array von Objekten zurückgeben.
 * In C++ definieren wir ein Struct für jeden Adapter.
 * 
 * MAX_ADAPTERS begrenzt wie viele Adapter wir speichern können.
 */
#define MAX_ADAPTERS 16
#define MAC_STRING_LENGTH 18  // "AA:BB:CC:DD:EE:FF" + null

struct NetworkAdapter {
    char name[256];              // Adapter-Name (z.B. "Ethernet")
    char description[256];       // Beschreibung (z.B. "Realtek Gaming...")
    char currentMac[MAC_STRING_LENGTH];  // Aktuelle MAC
    char registryKey[512];       // Registry-Pfad für diesen Adapter
    int index;                   // Adapter-Index
};

struct AdapterList {
    NetworkAdapter adapters[MAX_ADAPTERS];
    int count;                   // Wie viele Adapter gefunden wurden
};

/**
 * Listet alle Netzwerk-Adapter auf
 * 
 * @return AdapterList mit allen gefundenen Adaptern
 */
SPOOF_API AdapterList GetNetworkAdapters();

/**
 * Liest die aktuelle MAC-Adresse eines Adapters
 * 
 * @param adapterIndex - Index des Adapters (aus GetNetworkAdapters)
 * @param buffer       - Wohin die MAC geschrieben wird
 * @param bufferSize   - Größe des Buffers
 * @return true wenn erfolgreich
 */
SPOOF_API bool GetMacAddress(int adapterIndex, char* buffer, int bufferSize);

/**
 * Ändert die MAC-Adresse eines Netzwerk-Adapters
 * 
 * ⚠️ ACHTUNG: Braucht Administrator-Rechte!
 * 
 * @param adapterIndex - Index des Adapters
 * @param newMac       - Neue MAC (Format: "AABBCCDDEEFF" oder "AA:BB:CC:DD:EE:FF")
 * @return SpoofResult mit Erfolg/Fehler Info
 */
SPOOF_API SpoofResult SpoofMacAddress(int adapterIndex, const char* newMac);

// ═══════════════════════════════════════════════════════════════════════════
//  WMI / HARDWARE ID SPOOFING (Phase 4)
// ═══════════════════════════════════════════════════════════════════════════

/**
 * 📚 LEKTION: Hardware-IDs und WMI
 * 
 * Windows speichert Hardware-Informationen an mehreren Stellen:
 * 1. Registry - Direkt änderbar (mit Admin-Rechten)
 * 2. WMI (Windows Management Instrumentation) - Liest oft aus Registry
 * 3. SMBIOS - Im BIOS gespeichert (braucht Kernel-Treiber zum Ändern)
 * 
 * Wir konzentrieren uns auf Registry-basiertes Spoofing, das für die
 * meisten Anwendungen ausreicht.
 */

/**
 * Struktur für alle Hardware-IDs auf einen Blick
 */
struct HardwareInfo {
    char diskSerial[256];        // Festplatten-Seriennummer
    char volumeSerial[32];       // Volume Serial (C:)
    char biosSerial[256];        // BIOS Serial Number
    char motherboardSerial[256]; // Motherboard Serial
    char cpuId[256];             // CPU Processor ID
    char productId[256];         // Windows Product ID
};

/**
 * Liest alle Hardware-IDs aus
 * 
 * @return HardwareInfo mit allen gefundenen IDs
 */
SPOOF_API HardwareInfo GetHardwareInfo();

/**
 * Liest die Disk Serial Number
 * 
 * 📚 LEKTION: Disk Serial
 * Die Festplatten-Seriennummer ist eine der wichtigsten Hardware-IDs.
 * Sie wird über WMI (Win32_DiskDrive) oder direkt über IOCTLs gelesen.
 * 
 * @param buffer      - Wohin geschrieben wird
 * @param bufferSize  - Größe des Buffers
 * @return true wenn erfolgreich
 */
SPOOF_API bool GetDiskSerial(char* buffer, int bufferSize);

/**
 * Liest die Volume Serial Number (z.B. von C:)
 * 
 * 📚 LEKTION: Volume vs Disk Serial
 * - Volume Serial: Wird bei Formatierung generiert, leicht änderbar
 * - Disk Serial: Vom Hersteller, schwerer zu ändern
 * 
 * @param driveLetter - z.B. "C"
 * @param buffer      - Wohin geschrieben wird
 * @param bufferSize  - Größe des Buffers
 * @return true wenn erfolgreich
 */
SPOOF_API bool GetVolumeSerial(const char* driveLetter, char* buffer, int bufferSize);

/**
 * Ändert die Volume Serial Number
 * 
 * @param driveLetter - z.B. "C"
 * @param newSerial   - Neue Seriennummer (8 Hex-Zeichen, z.B. "AABB-CCDD")
 * @return SpoofResult mit Erfolg/Fehler Info
 */
SPOOF_API SpoofResult SpoofVolumeSerial(const char* driveLetter, const char* newSerial);

/**
 * Liest die SMBIOS UUID (Motherboard/BIOS ID)
 * 
 * @param buffer      - Wohin geschrieben wird
 * @param bufferSize  - Größe des Buffers
 * @return true wenn erfolgreich
 */
SPOOF_API bool GetSmbiosUuid(char* buffer, int bufferSize);

/**
 * Liest die Windows Product ID
 * 
 * @param buffer      - Wohin geschrieben wird
 * @param bufferSize  - Größe des Buffers
 * @return true wenn erfolgreich
 */
SPOOF_API bool GetProductId(char* buffer, int bufferSize);

/**
 * Ändert die Windows Product ID in der Registry
 * 
 * @param newProductId - Neue Product ID
 * @return SpoofResult mit Erfolg/Fehler Info
 */
SPOOF_API SpoofResult SpoofProductId(const char* newProductId);

/**
 * Generiert eine zufällige Volume Serial Number
 * 
 * @param buffer      - Wohin geschrieben wird (min. 10 Zeichen)
 * @param bufferSize  - Größe des Buffers
 */
SPOOF_API void GenerateRandomVolumeSerial(char* buffer, int bufferSize);

/**
 * Generiert eine zufällige Product ID im Windows-Format
 * 
 * @param buffer      - Wohin geschrieben wird (min. 30 Zeichen)
 * @param bufferSize  - Größe des Buffers
 */
SPOOF_API void GenerateRandomProductId(char* buffer, int bufferSize);

// ═══════════════════════════════════════════════════════════════════════════
//  ERWEITERTE SYSTEM-ANALYSE (Phase 5)
// ═══════════════════════════════════════════════════════════════════════════

/**
 * 📚 LEKTION: Tiefere Hardware-Analyse
 * 
 * Um zu wissen was wir spoofen können und was nicht, müssen wir
 * erst verstehen was das System alles exponiert.
 * 
 * Diese Strukturen zeigen ALLE relevanten Hardware-IDs an.
 */

/**
 * TPM (Trusted Platform Module) Information
 */
struct TpmInfo {
    bool present;                    // TPM vorhanden?
    bool enabled;                    // TPM aktiviert?
    char version[32];                // TPM Version (1.2 oder 2.0)
    char manufacturer[64];           // TPM Hersteller
    char endorsementKeyHash[128];    // Hash des EK (nicht der echte Key!)
    char status[256];                // Status-Beschreibung
    bool canBeDisabled;              // Kann deaktiviert werden?
};

/**
 * Erweiterte CPU Information
 */
struct CpuInfo {
    char vendor[16];                 // "GenuineIntel" oder "AuthenticAMD"
    char brand[64];                  // z.B. "Intel Core i7-10700K"
    char processorId[32];            // CPUID-basierte ID
    char microcode[32];              // Microcode Version
    int family;
    int model;
    int stepping;
    bool hypervisorPresent;          // Läuft in VM?
    char hypervisorVendor[16];       // VMware, VirtualBox, etc.
};

/**
 * NVMe/Disk erweiterte Information
 */
struct DiskInfo {
    char model[64];                  // Disk Model
    char serial[64];                 // Serial Number
    char firmware[32];               // Firmware Version
    char interfaceType[32];          // NVMe, SATA, etc.
    bool isNvme;                     // NVMe oder SATA?
    bool serialCanBeHidden;          // Kann versteckt werden?
    char spoofingAdvice[256];        // Empfehlung
};

/**
 * Netzwerk-Adapter erweiterte Information
 */
struct NicInfo {
    char description[256];
    char permanentMac[24];           // "Echte" MAC aus Hardware
    char currentMac[24];             // Aktuelle MAC (evtl. gespooft)
    bool isSpoofed;                  // Ist die MAC bereits gespooft?
    char driverName[128];            // Treiber-Name
    bool supportsRegistrySpoof;      // Registry-Spoofing möglich?
};

/**
 * Komplette System-Analyse
 */
struct SystemAnalysis {
    TpmInfo tpm;
    CpuInfo cpu;
    DiskInfo disks[8];
    int diskCount;
    NicInfo nics[8];
    int nicCount;
    
    // SMBIOS Info
    char smbiosUuid[48];
    char biosVendor[64];
    char biosVersion[64];
    char systemManufacturer[64];
    char systemProduct[64];
    char baseboardSerial[64];
    
    // Spoofing-Empfehlungen
    char recommendations[1024];
};

/**
 * Führt eine komplette System-Analyse durch
 * 
 * @return SystemAnalysis mit allen gefundenen Informationen
 */
SPOOF_API SystemAnalysis AnalyzeSystem();

/**
 * Prüft ob TPM vorhanden und aktiv ist
 * 
 * @return TpmInfo mit TPM-Details
 */
SPOOF_API TpmInfo GetTpmInfo();

/**
 * Liest erweiterte CPU-Informationen
 * 
 * @return CpuInfo mit CPU-Details
 */
SPOOF_API CpuInfo GetCpuInfo();

/**
 * Liest erweiterte Disk-Informationen für alle Laufwerke
 * 
 * @param diskIndex - 0 für erste Disk, 1 für zweite, etc.
 * @return DiskInfo mit Disk-Details
 */
SPOOF_API DiskInfo GetDiskInfo(int diskIndex);

/**
 * Liest erweiterte NIC-Informationen
 * 
 * @param nicIndex - 0 für ersten Adapter, etc.
 * @return NicInfo mit NIC-Details
 */
SPOOF_API NicInfo GetNicInfo(int nicIndex);

/**
 * Prüft ob wir in einer VM laufen
 * 
 * @return true wenn VM erkannt
 */
SPOOF_API bool IsRunningInVM();

/**
 * Gibt Spoofing-Empfehlungen basierend auf System-Analyse
 * 
 * @param buffer      - Wohin geschrieben wird
 * @param bufferSize  - Größe des Buffers
 */
SPOOF_API void GetSpoofingRecommendations(char* buffer, int bufferSize);

#endif // SPOOFLIGHT_SPOOFER_H

