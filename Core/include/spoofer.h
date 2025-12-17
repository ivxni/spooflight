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
 * @return Version als String, z.B. "1.0.0"
 */
SPOOF_API const char* GetVersion();

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
//  MAC ADDRESS SPOOFING (Phase 3) - Kommt später!
// ═══════════════════════════════════════════════════════════════════════════

// SPOOF_API SpoofResult SpoofMacAddress(const char* adapterName, const char* newMac);

// ═══════════════════════════════════════════════════════════════════════════
//  WMI SPOOFING (Phase 4) - Kommt später!
// ═══════════════════════════════════════════════════════════════════════════

// SPOOF_API SpoofResult SpoofDiskSerial(const char* newSerial);
// SPOOF_API SpoofResult SpoofBiosSerial(const char* newSerial);

#endif // SPOOFLIGHT_SPOOFER_H

