# SPOOFLIGHT - Projekt Status

> **Letztes Update:** 2025-12-18
> **Für:** Neuen Chat-Kontext / Weiterarbeit

---

## 🎯 Projektübersicht

**HWID Spoofer** für IT-Security Studium (Lernprojekt).

| Komponente | Technologie | Status |
|------------|-------------|--------|
| Backend | C++ (CLion, MSVC) | ✅ Funktioniert |
| Frontend | C# WPF (Rider, .NET 10) | ✅ Funktioniert |
| Kommunikation | P/Invoke (DllImport) | ✅ Funktioniert |

---

## 📁 Projektstruktur

```
SPOOFLIGHT/
├── Core/                           ← C++ Backend (CLion)
│   ├── CMakeLists.txt
│   ├── include/
│   │   ├── exports.h               ← DLL Export Makros
│   │   └── spoofer.h               ← Funktions-Deklarationen
│   ├── src/
│   │   ├── spoofer.cpp             ← Hauptimplementierung
│   │   └── test_main.cpp           ← Test-Executable
│   └── cmake-build-debug-visual-studio/
│       └── SpooflightCore.dll      ← Kompilierte DLL
│
├── UI/                             ← C# Frontend (Rider)
│   ├── UI.csproj
│   ├── MainWindow.xaml             ← UI Layout (Dark Theme)
│   ├── MainWindow.xaml.cs          ← UI Logik
│   └── Native/
│       └── SpooflightCore.cs       ← P/Invoke Bindings
│
├── docs/
│   └── cursor_hwid_changer_spoofer_projekt.md  ← Original Chat-Export
│
└── PROJECT_STATUS.md               ← Diese Datei
```

---

## ✅ Was funktioniert

### 1. Machine GUID Spoofing (Phase 2) ✅
- ✅ Lesen aus Registry (`HKLM\SOFTWARE\Microsoft\Cryptography\MachineGuid`)
- ✅ Schreiben (braucht Admin-Rechte)
- ✅ UI mit Read/Generate/Spoof Buttons

### 2. MAC Address Spoofing (Phase 3) ✅
- ✅ C++ Implementation (GetNetworkAdapters, SpoofMacAddress)
- ✅ C# UI zeigt alle Adapter an
- ✅ MAC-Spoofing über Registry funktioniert
- ✅ Random MAC Generator

### 3. Hardware ID Spoofing (Phase 4) ✅
- ✅ **Disk Serial** - Lesen über IOCTL (nicht änderbar ohne Kernel-Treiber)
- ✅ **Volume Serial** - Lesen (Ändern über Sysinternals volumeid.exe)
- ✅ **SMBIOS UUID** - Lesen aus Firmware (nicht änderbar ohne BIOS-Mod)
- ✅ **CPU ID** - Lesen über __cpuid Intrinsic
- ✅ **Windows Product ID** - Lesen UND Schreiben über Registry
- ✅ UI mit Hardware-IDs Übersicht
- ✅ Product ID Spoofing funktioniert

---

## ⏳ Was noch offen ist

### 4. System-Analyse (Phase 5) ✅
- ✅ TPM Detection
- ✅ CPU Info (CPUID, Hypervisor Detection)
- ✅ Disk Details (NVMe/SATA, Serial, Firmware)
- ✅ VM Detection
- ✅ Spoofing Recommendations

### 5. Kernel Driver (Phase 6) ✅
- ✅ WDM Driver Grundgerüst
- ✅ IOCTL Communication Interface
- ✅ User-Mode C# Bindings
- ⏳ Disk Serial Hooking (in Entwicklung)
- ⏳ SMBIOS Table Hooking (in Entwicklung)

### Nächste Phasen
- [ ] Disk Serial IOCTL Hooking implementieren
- [ ] SMBIOS GetSystemFirmwareTable Hook
- [ ] Driver signieren für Production

---

## 🔧 Wichtige Code-Stellen

### C++ DLL Exports (spoofer.h)
```cpp
// Basis
SPOOF_API bool Initialize();
SPOOF_API void Shutdown();
SPOOF_API const char* GetSpooflightVersion();

// Registry (Phase 2)
SPOOF_API bool GetMachineGuid(char* buffer, int bufferSize);
SPOOF_API SpoofResult SpoofMachineGuid(const char* newGuid);

// MAC Spoofing (Phase 3)
SPOOF_API AdapterList GetNetworkAdapters();
SPOOF_API bool GetMacAddress(int adapterIndex, char* buffer, int bufferSize);
SPOOF_API SpoofResult SpoofMacAddress(int adapterIndex, const char* newMac);

// Hardware IDs (Phase 4)
SPOOF_API HardwareInfo GetHardwareInfo();
SPOOF_API bool GetDiskSerial(char* buffer, int bufferSize);
SPOOF_API bool GetVolumeSerial(const char* driveLetter, char* buffer, int bufferSize);
SPOOF_API bool GetSmbiosUuid(char* buffer, int bufferSize);
SPOOF_API bool GetProductId(char* buffer, int bufferSize);
SPOOF_API SpoofResult SpoofProductId(const char* newProductId);
```

### C# P/Invoke (SpooflightCore.cs)
```csharp
[DllImport("SpooflightCore.dll", CallingConvention = CallingConvention.Cdecl)]
public static extern AdapterList GetNetworkAdapters();
```

### Structs müssen exakt übereinstimmen!
```cpp
// C++
struct NetworkAdapter {
    char name[256];
    char description[256];
    char currentMac[18];
    char registryKey[512];
    int index;
};
```
```csharp
// C#
[StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
public struct NetworkAdapter {
    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 256)] public string Name;
    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 256)] public string Description;
    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 18)] public string CurrentMac;
    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 512)] public string RegistryKey;
    public int Index;
}
```

---

## 🛠️ Build-Befehle

### C++ (CLion)
```
Target: Spooflight → Build (Ctrl+F9)
Output: Core/cmake-build-debug-visual-studio/SpooflightCore.dll
```

### C# (Rider)
```
Build → Rebuild Solution (Ctrl+Shift+F9)
DLL wird automatisch kopiert (siehe UI.csproj)
```

### Testen ohne UI (C++)
```
Target: SpooflightTest → Run
```

---

## 📚 Lernmodus

Der User lernt C++ während des Projekts. Code enthält:
- 📚 LEKTION: Kommentare für Erklärungen
- 🎯 AUFGABE: Lücken zum Selbst-Ausfüllen (bereits erledigt)

Schwierigkeitsniveau: Web-Fullstack-Dev der C++ lernt.

---

## 🔗 Nächste Schritte im neuen Chat

1. **Build testen** - CLion: Target "Spooflight" → Build (Ctrl+F9)
2. **UI starten** - Rider: Rebuild + Run
3. **Testen** - Hardware IDs sollten angezeigt werden, Product ID spoofbar

