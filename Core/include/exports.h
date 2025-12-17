/**
 * ═══════════════════════════════════════════════════════════════════════════
 *  SPOOFLIGHT - DLL EXPORT HEADER
 *  Diese Datei definiert das Makro für DLL-Exports
 * ═══════════════════════════════════════════════════════════════════════════
 * 
 * 📚 LEKTION: Was ist ein Makro?
 * Ein Makro ist wie eine "Suchen & Ersetzen" Regel.
 * Der Compiler ersetzt SPOOF_API überall mit dem langen __declspec(...) Text.
 * 
 * Das spart Tipparbeit und macht den Code lesbarer!
 */

#ifndef SPOOFLIGHT_EXPORTS_H
#define SPOOFLIGHT_EXPORTS_H

// ═══════════════════════════════════════════════════════════════════════════
// 📚 LEKTION: Präprozessor-Direktiven (#ifdef, #define, etc.)
// ═══════════════════════════════════════════════════════════════════════════
// 
// Diese Zeilen werden VOR dem eigentlichen Kompilieren ausgeführt.
// #ifdef = "if defined" = "falls dieses Symbol existiert"
// 
// Wenn wir die DLL BAUEN, exportieren wir Funktionen (dllexport)
// Wenn jemand die DLL BENUTZT, importiert er sie (dllimport)

#ifdef SPOOFLIGHT_EXPORTS
    // Wir bauen die DLL → Funktionen EXPORTIEREN (nach außen sichtbar machen)
    #define SPOOF_API extern "C" __declspec(dllexport)
#else
    // Jemand nutzt unsere DLL → Funktionen IMPORTIEREN
    #define SPOOF_API extern "C" __declspec(dllimport)
#endif

// ═══════════════════════════════════════════════════════════════════════════
// 📚 LEKTION: Warum "extern C"?
// ═══════════════════════════════════════════════════════════════════════════
//
// C++ "mangelt" (verändert) Funktionsnamen, z.B.:
//    SpoofMAC(string) → _Z8SpoofMACNSt7__cxx1112basic_stringE
//
// Das macht es für C# unmöglich, die Funktion zu finden!
// "extern C" sagt: "Benutze einfache C-Namen ohne Mangling"
//    SpoofMAC → SpoofMAC  ✓

#endif // SPOOFLIGHT_EXPORTS_H

