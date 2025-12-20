/**
 * ═══════════════════════════════════════════════════════════════════════════
 *  SPOOFLIGHT - P/Invoke Bindings
 *  Diese Datei verbindet C# mit unserer C++ DLL
 * ═══════════════════════════════════════════════════════════════════════════
 * 
 * 📚 LEKTION: Was ist P/Invoke?
 * 
 * P/Invoke = "Platform Invocation Services"
 * Es erlaubt C# Code, Funktionen aus nativen DLLs (C/C++) aufzurufen.
 * 
 * Das ist wie ein "Übersetzer" zwischen C# und C++:
 *   C#: string → C++: const char*
 *   C#: bool   → C++: bool
 *   C#: struct → C++: struct (mit passendem Layout!)
 */

using System.Runtime.InteropServices;
using System.Text;  // Für StringBuilder

namespace UI.Native;

/// <summary>
/// 📚 LEKTION: StructLayout
/// 
/// C++ und C# ordnen Struct-Felder unterschiedlich im Speicher an.
/// [StructLayout(LayoutKind.Sequential)] sagt: "Ordne die Felder genau so
/// an wie C++ es macht - eines nach dem anderen."
/// 
/// CharSet.Ansi bedeutet: Nutze ASCII/ANSI Strings (wie C++ char*)
/// </summary>
[StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
public struct SpoofResult
{
    [MarshalAs(UnmanagedType.I1)]  // bool in C++ ist 1 Byte
    public bool Success;
    
    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 256)]
    public string Message;
    
    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 256)]
    public string OldValue;
    
    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 256)]
    public string NewValue;
}

// ═══════════════════════════════════════════════════════════════════════════
// MAC Address Spoofing Structs (Phase 3)
// ═══════════════════════════════════════════════════════════════════════════

[StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
public struct NetworkAdapter
{
    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 256)]
    public string Name;
    
    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 256)]
    public string Description;
    
    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 18)]
    public string CurrentMac;
    
    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 512)]
    public string RegistryKey;
    
    public int Index;
}

[StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
public struct AdapterList
{
    [MarshalAs(UnmanagedType.ByValArray, SizeConst = 16)]
    public NetworkAdapter[] Adapters;
    
    public int Count;
}

// ═══════════════════════════════════════════════════════════════════════════
// Hardware ID / WMI Spoofing Structs (Phase 4)
// ═══════════════════════════════════════════════════════════════════════════

/// <summary>
/// 📚 LEKTION: Hardware-IDs
/// 
/// Diese Struktur enthält alle wichtigen Hardware-Identifikatoren:
/// - Disk Serial: Festplatten-Seriennummer (vom Hersteller)
/// - Volume Serial: Partitions-ID (wird bei Formatierung generiert)
/// - BIOS Serial: SMBIOS UUID (im BIOS/UEFI gespeichert)
/// - Product ID: Windows Produkt-ID
/// - CPU ID: Prozessor-Identifikation
/// </summary>
[StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
public struct HardwareInfo
{
    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 256)]
    public string DiskSerial;
    
    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 32)]
    public string VolumeSerial;
    
    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 256)]
    public string BiosSerial;
    
    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 256)]
    public string MotherboardSerial;
    
    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 256)]
    public string CpuId;
    
    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 256)]
    public string ProductId;
}

// ═══════════════════════════════════════════════════════════════════════════
// System-Analyse Structs (Phase 5)
// ═══════════════════════════════════════════════════════════════════════════

/// <summary>
/// TPM (Trusted Platform Module) Information
/// </summary>
[StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
public struct TpmInfo
{
    [MarshalAs(UnmanagedType.I1)]
    public bool Present;
    
    [MarshalAs(UnmanagedType.I1)]
    public bool Enabled;
    
    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 32)]
    public string Version;
    
    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 64)]
    public string Manufacturer;
    
    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 128)]
    public string EndorsementKeyHash;
    
    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 256)]
    public string Status;
    
    [MarshalAs(UnmanagedType.I1)]
    public bool CanBeDisabled;
}

/// <summary>
/// Erweiterte CPU Information
/// </summary>
[StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
public struct CpuInfo
{
    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 16)]
    public string Vendor;
    
    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 64)]
    public string Brand;
    
    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 32)]
    public string ProcessorId;
    
    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 32)]
    public string Microcode;
    
    public int Family;
    public int Model;
    public int Stepping;
    
    [MarshalAs(UnmanagedType.I1)]
    public bool HypervisorPresent;
    
    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 16)]
    public string HypervisorVendor;
}

/// <summary>
/// NVMe/Disk erweiterte Information
/// </summary>
[StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
public struct DiskInfo
{
    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 64)]
    public string Model;
    
    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 64)]
    public string Serial;
    
    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 32)]
    public string Firmware;
    
    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 32)]
    public string InterfaceType;
    
    [MarshalAs(UnmanagedType.I1)]
    public bool IsNvme;
    
    [MarshalAs(UnmanagedType.I1)]
    public bool SerialCanBeHidden;
    
    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 256)]
    public string SpoofingAdvice;
}

/// <summary>
/// Netzwerk-Adapter erweiterte Information
/// </summary>
[StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
public struct NicInfo
{
    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 256)]
    public string Description;
    
    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 24)]
    public string PermanentMac;
    
    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 24)]
    public string CurrentMac;
    
    [MarshalAs(UnmanagedType.I1)]
    public bool IsSpoofed;
    
    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 128)]
    public string DriverName;
    
    [MarshalAs(UnmanagedType.I1)]
    public bool SupportsRegistrySpoof;
}

/// <summary>
/// Komplette System-Analyse
/// </summary>
[StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
public struct SystemAnalysis
{
    public TpmInfo Tpm;
    public CpuInfo Cpu;
    
    [MarshalAs(UnmanagedType.ByValArray, SizeConst = 8)]
    public DiskInfo[] Disks;
    public int DiskCount;
    
    [MarshalAs(UnmanagedType.ByValArray, SizeConst = 8)]
    public NicInfo[] Nics;
    public int NicCount;
    
    // SMBIOS Info
    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 48)]
    public string SmbiosUuid;
    
    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 64)]
    public string BiosVendor;
    
    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 64)]
    public string BiosVersion;
    
    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 64)]
    public string SystemManufacturer;
    
    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 64)]
    public string SystemProduct;
    
    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 64)]
    public string BaseboardSerial;
    
    // Empfehlungen
    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 1024)]
    public string Recommendations;
}

/// <summary>
/// Wrapper-Klasse für die SpooflightCore.dll
/// </summary>
public static class SpooflightCore
{
    // ═══════════════════════════════════════════════════════════════════════
    // 📚 LEKTION: DllImport
    // 
    // [DllImport("DllName")] sagt C#: "Diese Funktion kommt aus einer DLL"
    // 
    // CallingConvention.Cdecl = So wie C/C++ Funktionen standardmäßig aufgerufen werden
    // CharSet.Ansi = Strings als char* (nicht wchar_t*)
    // ═══════════════════════════════════════════════════════════════════════
    
    private const string DllName = "SpooflightCore.dll";
    
    // ─────────────────────────────────────────────────────────────────────────
    // Basis-Funktionen
    // ─────────────────────────────────────────────────────────────────────────
    
    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    [return: MarshalAs(UnmanagedType.I1)]
    public static extern bool Initialize();
    
    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    public static extern void Shutdown();
    
    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    public static extern IntPtr GetSpooflightVersion();
    
    // ─────────────────────────────────────────────────────────────────────────
    // Registry Spoofing
    // ─────────────────────────────────────────────────────────────────────────
    
    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
    [return: MarshalAs(UnmanagedType.I1)]
    public static extern bool GetMachineGuid(
        [Out] StringBuilder buffer, 
        int bufferSize
    );
    
    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
    public static extern SpoofResult SpoofMachineGuid(
        [MarshalAs(UnmanagedType.LPStr)] string newGuid
    );
    
    // ─────────────────────────────────────────────────────────────────────────
    // MAC Address Spoofing (Phase 3)
    // ─────────────────────────────────────────────────────────────────────────
    
    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    public static extern AdapterList GetNetworkAdapters();
    
    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
    [return: MarshalAs(UnmanagedType.I1)]
    public static extern bool GetMacAddress(
        int adapterIndex,
        [Out] StringBuilder buffer,
        int bufferSize
    );
    
    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
    public static extern SpoofResult SpoofMacAddress(
        int adapterIndex,
        [MarshalAs(UnmanagedType.LPStr)] string newMac
    );
    
    // ─────────────────────────────────────────────────────────────────────────
    // Hardware ID / WMI Spoofing (Phase 4)
    // ─────────────────────────────────────────────────────────────────────────
    
    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    public static extern HardwareInfo GetHardwareInfo();
    
    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
    [return: MarshalAs(UnmanagedType.I1)]
    public static extern bool GetDiskSerial(
        [Out] StringBuilder buffer,
        int bufferSize
    );
    
    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
    [return: MarshalAs(UnmanagedType.I1)]
    public static extern bool GetVolumeSerial(
        [MarshalAs(UnmanagedType.LPStr)] string driveLetter,
        [Out] StringBuilder buffer,
        int bufferSize
    );
    
    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
    [return: MarshalAs(UnmanagedType.I1)]
    public static extern bool GetSmbiosUuid(
        [Out] StringBuilder buffer,
        int bufferSize
    );
    
    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
    [return: MarshalAs(UnmanagedType.I1)]
    public static extern bool GetProductId(
        [Out] StringBuilder buffer,
        int bufferSize
    );
    
    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
    public static extern SpoofResult SpoofProductId(
        [MarshalAs(UnmanagedType.LPStr)] string newProductId
    );
    
    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
    public static extern SpoofResult SpoofVolumeSerial(
        [MarshalAs(UnmanagedType.LPStr)] string driveLetter,
        [MarshalAs(UnmanagedType.LPStr)] string newSerial
    );
    
    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
    public static extern void GenerateRandomVolumeSerial(
        [Out] StringBuilder buffer,
        int bufferSize
    );
    
    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
    public static extern void GenerateRandomProductId(
        [Out] StringBuilder buffer,
        int bufferSize
    );
    
    // ─────────────────────────────────────────────────────────────────────────
    // System-Analyse (Phase 5)
    // ─────────────────────────────────────────────────────────────────────────
    
    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    public static extern SystemAnalysis AnalyzeSystem();
    
    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    public static extern TpmInfo GetTpmInfo();
    
    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    public static extern CpuInfo GetCpuInfo();
    
    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    public static extern DiskInfo GetDiskInfo(int diskIndex);
    
    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    public static extern NicInfo GetNicInfo(int nicIndex);
    
    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    [return: MarshalAs(UnmanagedType.I1)]
    public static extern bool IsRunningInVM();
    
    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
    public static extern void GetSpoofingRecommendations(
        [Out] StringBuilder buffer,
        int bufferSize
    );
    
    // ═══════════════════════════════════════════════════════════════════════
    // Hilfsmethoden (C#-freundliche Wrapper)
    // ═══════════════════════════════════════════════════════════════════════
    
    /// <summary>
    /// Gibt die Version als C# String zurück
    /// </summary>
    public static string GetVersion()
    {
        IntPtr ptr = GetSpooflightVersion();
        return Marshal.PtrToStringAnsi(ptr) ?? "Unknown";
    }
    
    /// <summary>
    /// Liest die MachineGuid als C# String
    /// </summary>
    public static string? ReadMachineGuid()
    {
        var buffer = new StringBuilder(256);
        bool success = GetMachineGuid(buffer, buffer.Capacity);
        return success ? buffer.ToString() : null;
    }
    
    /// <summary>
    /// Gibt eine Liste aller Netzwerk-Adapter zurück
    /// </summary>
    public static List<NetworkAdapter> GetAdapters()
    {
        var result = new List<NetworkAdapter>();
        var adapterList = GetNetworkAdapters();
        
        if (adapterList.Adapters != null)
        {
            for (int i = 0; i < adapterList.Count && i < adapterList.Adapters.Length; i++)
            {
                result.Add(adapterList.Adapters[i]);
            }
        }
        
        return result;
    }
    
    /// <summary>
    /// Generiert eine zufällige MAC-Adresse
    /// </summary>
    public static string GenerateRandomMac()
    {
        var random = new Random();
        byte[] mac = new byte[6];
        random.NextBytes(mac);
        
        // Erstes Byte: Bit 0 = 0 (Unicast), Bit 1 = 1 (Locally Administered)
        // Das macht die MAC "lokal verwaltet" und nicht "universell vergeben"
        mac[0] = (byte)((mac[0] & 0xFC) | 0x02);
        
        return $"{mac[0]:X2}:{mac[1]:X2}:{mac[2]:X2}:{mac[3]:X2}:{mac[4]:X2}:{mac[5]:X2}";
    }
    
    // ═══════════════════════════════════════════════════════════════════════
    // Hardware ID Helper Methods (Phase 4)
    // ═══════════════════════════════════════════════════════════════════════
    
    /// <summary>
    /// Liest die Disk Serial Number als String
    /// </summary>
    public static string? ReadDiskSerial()
    {
        var buffer = new StringBuilder(256);
        bool success = GetDiskSerial(buffer, buffer.Capacity);
        return success ? buffer.ToString() : null;
    }
    
    /// <summary>
    /// Liest die Volume Serial Number eines Laufwerks
    /// </summary>
    public static string? ReadVolumeSerial(string driveLetter = "C")
    {
        var buffer = new StringBuilder(32);
        bool success = GetVolumeSerial(driveLetter, buffer, buffer.Capacity);
        return success ? buffer.ToString() : null;
    }
    
    /// <summary>
    /// Liest die SMBIOS/BIOS UUID
    /// </summary>
    public static string? ReadSmbiosUuid()
    {
        var buffer = new StringBuilder(256);
        bool success = GetSmbiosUuid(buffer, buffer.Capacity);
        return success ? buffer.ToString() : null;
    }
    
    /// <summary>
    /// Liest die Windows Product ID
    /// </summary>
    public static string? ReadProductId()
    {
        var buffer = new StringBuilder(256);
        bool success = GetProductId(buffer, buffer.Capacity);
        return success ? buffer.ToString() : null;
    }
    
    /// <summary>
    /// Generiert eine zufällige Volume Serial
    /// </summary>
    public static string GenerateRandomVolSerial()
    {
        var buffer = new StringBuilder(16);
        GenerateRandomVolumeSerial(buffer, buffer.Capacity);
        return buffer.ToString();
    }
    
    /// <summary>
    /// Generiert eine zufällige Product ID
    /// </summary>
    public static string GenerateRandomProdId()
    {
        var buffer = new StringBuilder(32);
        GenerateRandomProductId(buffer, buffer.Capacity);
        return buffer.ToString();
    }
    
    // ═══════════════════════════════════════════════════════════════════════
    // System-Analyse Helper Methods (Phase 5)
    // ═══════════════════════════════════════════════════════════════════════
    
    /// <summary>
    /// Führt eine komplette System-Analyse durch und gibt das Ergebnis zurück
    /// </summary>
    public static SystemAnalysis RunSystemAnalysis()
    {
        return AnalyzeSystem();
    }
    
    /// <summary>
    /// Prüft ob das System in einer VM läuft
    /// </summary>
    public static bool CheckIfRunningInVM()
    {
        return IsRunningInVM();
    }
    
    /// <summary>
    /// Gibt alle Disks mit Details zurück
    /// </summary>
    public static List<DiskInfo> GetAllDisks()
    {
        var result = new List<DiskInfo>();
        for (int i = 0; i < 8; i++)
        {
            var disk = GetDiskInfo(i);
            if (!string.IsNullOrEmpty(disk.Serial) && disk.Serial != "DRIVE_NOT_FOUND")
            {
                result.Add(disk);
            }
        }
        return result;
    }
    
    /// <summary>
    /// Gibt alle NICs mit erweiterten Details zurück
    /// </summary>
    public static List<NicInfo> GetAllNicsExtended()
    {
        var result = new List<NicInfo>();
        for (int i = 0; i < 8; i++)
        {
            var nic = GetNicInfo(i);
            if (!string.IsNullOrEmpty(nic.Description) && nic.Description != "No adapters found")
            {
                result.Add(nic);
            }
            else
            {
                break;
            }
        }
        return result;
    }
    
    /// <summary>
    /// Gibt Spoofing-Empfehlungen als String zurück
    /// </summary>
    public static string GetRecommendations()
    {
        var buffer = new StringBuilder(1024);
        GetSpoofingRecommendations(buffer, buffer.Capacity);
        return buffer.ToString();
    }
}

