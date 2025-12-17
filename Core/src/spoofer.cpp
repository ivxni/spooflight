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
 * GetVersion - Gibt Versionsnummer zurück
 */
SPOOF_API const char* GetVersion() {
    // ╔═══════════════════════════════════════════════════════════════════╗
    // ║  🎯 AUFGABE 3: Gib die VERSION Variable zurück                    ║
    // ║                                                                   ║
    // ║  In C++ gibt "return x;" den Wert x zurück - wie in JS.           ║
    // ║  VERSION ist oben als "1.0.0" definiert.                          ║
    // ╚═══════════════════════════════════════════════════════════════════╝
    
    // TODO: Fülle die Lücke aus
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
        result.success = _____;
        CopyString(result.message, "_____", sizeof(result.message));
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
        KEY_WRITE,              // Schreibzugriff!
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
        REG_SZ,                                    // Typ: String
        (const BYTE*)newGuid,                     // Neuer Wert
        (DWORD)(strlen(newGuid) + 1)              // Länge inkl. Null-Terminator
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

