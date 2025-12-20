# Spooflight Kernel Driver

## 📚 Was ist das?

Ein Windows Kernel Driver (WDM) für tiefes Hardware-ID Spoofing:
- **Disk Serial** - Fängt IOCTL_STORAGE_QUERY_PROPERTY ab
- **SMBIOS UUID** - Modifiziert Firmware-Tabellen Zugriffe
- **CPU ID** - (geplant) Hypervisor-basiertes CPUID Spoofing

## 🔧 Voraussetzungen

1. **Visual Studio 2022** mit C++ Desktop Development
2. **Windows Driver Kit (WDK)** - Download: https://learn.microsoft.com/windows-hardware/drivers/download-the-wdk
3. **Test-Signing aktiviert**:
   ```cmd
   bcdedit /set testsigning on
   ```
   Dann Neustart.

## 🔨 Build

### Option 1: Visual Studio
1. `SpooflightDriver.vcxproj` in VS2022 öffnen
2. Configuration: `Debug | x64`
3. Build (F7)
4. Output: `bin/x64/Debug/SpooflightDriver.sys`

### Option 2: Command Line
```cmd
# In "x64 Native Tools Command Prompt for VS 2022"
msbuild SpooflightDriver.vcxproj /p:Configuration=Debug /p:Platform=x64
```

## 📦 Installation

### Test-Installation (Entwicklung)
```cmd
# Als Administrator
sc create SpooflightDriver type= kernel binPath= "C:\path\to\SpooflightDriver.sys"
sc start SpooflightDriver
```

### Deinstallation
```cmd
sc stop SpooflightDriver
sc delete SpooflightDriver
```

### INF-basiert (Production)
```cmd
# Signieren (für Test)
signtool sign /a /v /s PrivateCertStore /n "YourCertName" SpooflightDriver.sys

# Installieren
pnputil /add-driver SpooflightDriver.inf /install
```

## 🔍 Debugging

### DebugView (Sysinternals)
1. DebugView als Admin starten
2. Capture → Capture Kernel aktivieren
3. Driver laden → DbgPrint Ausgaben erscheinen

### WinDbg (Kernel Debugging)
```
# In VM:
bcdedit /debug on
bcdedit /dbgsettings net hostip:192.168.1.100 port:50000
```

## ⚠️ Sicherheitshinweise

- **NUR für Lernzwecke!** Kernel Drivers können das System destabilisieren.
- **Immer in VM testen!** Ein Fehler = Blue Screen.
- **Test-Signing** macht das System unsicherer. Nach Entwicklung wieder deaktivieren.

## 📁 Dateien

```
KernelDriver/
├── shared.h              ← IOCTL Codes (User + Kernel)
├── spoof_driver.c        ← Driver Implementation
├── SpooflightDriver.inf  ← Installation Info
├── SpooflightDriver.vcxproj ← VS Projekt
└── README.md             ← Diese Datei
```

## 🔗 Kommunikation

User-Mode App kommuniziert über IOCTLs:

```csharp
// C# Beispiel
if (KernelDriver.IsDriverLoaded())
{
    var version = KernelDriver.GetVersion();
    KernelDriver.SpoofDiskSerial(0, "SPOOFED123456");
    KernelDriver.EnableSpoofing();
}
```
