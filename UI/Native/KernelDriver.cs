/**
 * ═══════════════════════════════════════════════════════════════════════════
 *  SPOOFLIGHT - Kernel Driver Communication
 *  
 *  📚 LEKTION: User-Mode ↔ Kernel-Mode Kommunikation
 *  
 *  Um mit dem Kernel Driver zu kommunizieren, verwenden wir:
 *  1. CreateFile() - Öffnet Handle zum Driver Device
 *  2. DeviceIoControl() - Sendet/Empfängt Daten
 *  3. CloseHandle() - Schließt die Verbindung
 *  
 *  Die IOCTL-Codes und Strukturen müssen EXAKT mit dem Driver übereinstimmen!
 * ═══════════════════════════════════════════════════════════════════════════
 */

using System;
using System.Runtime.InteropServices;
using System.Text;
using Microsoft.Win32.SafeHandles;

namespace UI.Native;

/// <summary>
/// Strukturen für Driver-Kommunikation
/// WICHTIG: Müssen exakt mit shared.h übereinstimmen!
/// </summary>

[StructLayout(LayoutKind.Sequential)]
public struct DriverVersion
{
    public uint Major;
    public uint Minor;
    public uint Build;
}

[StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
public struct SpoofDiskRequest
{
    public uint DiskIndex;
    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 64)]
    public string OriginalSerial;
    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 64)]
    public string SpoofedSerial;
    public uint Flags;
}

[StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
public struct SpoofSmbiosRequest
{
    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 48)]
    public string OriginalUuid;
    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 48)]
    public string SpoofedUuid;
    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 64)]
    public string OriginalSerial;
    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 64)]
    public string SpoofedSerial;
    public uint Flags;
}

[StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
public struct DriverStatus
{
    public uint IsLoaded;
    public uint DiskSpoofActive;
    public uint SmbiosSpoofActive;
    public uint HookedDisks;
    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 256)]
    public string LastError;
}

/// <summary>
/// Kernel Driver Interface
/// </summary>
public static class KernelDriver
{
    // Device Path - muss mit shared.h übereinstimmen
    private const string DevicePath = "\\\\.\\SpooflightDriver";
    
    // IOCTL Codes - müssen mit shared.h übereinstimmen
    private const uint FILE_DEVICE_SPOOFLIGHT = 0x8000;
    
    private static uint CTL_CODE(uint deviceType, uint function, uint method, uint access)
    {
        return (deviceType << 16) | (access << 14) | (function << 2) | method;
    }
    
    private const uint METHOD_BUFFERED = 0;
    private const uint FILE_ANY_ACCESS = 0;
    
    private static readonly uint IOCTL_GET_VERSION = CTL_CODE(FILE_DEVICE_SPOOFLIGHT, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS);
    private static readonly uint IOCTL_SPOOF_DISK = CTL_CODE(FILE_DEVICE_SPOOFLIGHT, 0x801, METHOD_BUFFERED, FILE_ANY_ACCESS);
    private static readonly uint IOCTL_SPOOF_SMBIOS = CTL_CODE(FILE_DEVICE_SPOOFLIGHT, 0x802, METHOD_BUFFERED, FILE_ANY_ACCESS);
    private static readonly uint IOCTL_GET_STATUS = CTL_CODE(FILE_DEVICE_SPOOFLIGHT, 0x803, METHOD_BUFFERED, FILE_ANY_ACCESS);
    private static readonly uint IOCTL_ENABLE = CTL_CODE(FILE_DEVICE_SPOOFLIGHT, 0x804, METHOD_BUFFERED, FILE_ANY_ACCESS);
    private static readonly uint IOCTL_DISABLE = CTL_CODE(FILE_DEVICE_SPOOFLIGHT, 0x805, METHOD_BUFFERED, FILE_ANY_ACCESS);
    
    // Win32 API Imports
    [DllImport("kernel32.dll", SetLastError = true, CharSet = CharSet.Auto)]
    private static extern SafeFileHandle CreateFile(
        string lpFileName,
        uint dwDesiredAccess,
        uint dwShareMode,
        IntPtr lpSecurityAttributes,
        uint dwCreationDisposition,
        uint dwFlagsAndAttributes,
        IntPtr hTemplateFile
    );
    
    [DllImport("kernel32.dll", SetLastError = true)]
    private static extern bool DeviceIoControl(
        SafeFileHandle hDevice,
        uint dwIoControlCode,
        IntPtr lpInBuffer,
        uint nInBufferSize,
        IntPtr lpOutBuffer,
        uint nOutBufferSize,
        out uint lpBytesReturned,
        IntPtr lpOverlapped
    );
    
    private const uint GENERIC_READ = 0x80000000;
    private const uint GENERIC_WRITE = 0x40000000;
    private const uint OPEN_EXISTING = 3;
    
    // ═══════════════════════════════════════════════════════════════════════
    // Public API
    // ═══════════════════════════════════════════════════════════════════════
    
    /// <summary>
    /// Prüft ob der Driver geladen ist
    /// </summary>
    public static bool IsDriverLoaded()
    {
        try
        {
            using var handle = CreateFile(
                DevicePath,
                GENERIC_READ | GENERIC_WRITE,
                0,
                IntPtr.Zero,
                OPEN_EXISTING,
                0,
                IntPtr.Zero
            );
            
            return !handle.IsInvalid;
        }
        catch
        {
            return false;
        }
    }
    
    /// <summary>
    /// Holt die Driver-Version
    /// </summary>
    public static DriverVersion? GetVersion()
    {
        using var handle = OpenDriver();
        if (handle == null) return null;
        
        var version = new DriverVersion();
        int size = Marshal.SizeOf(version);
        IntPtr buffer = Marshal.AllocHGlobal(size);
        
        try
        {
            if (DeviceIoControl(handle, IOCTL_GET_VERSION, IntPtr.Zero, 0, buffer, (uint)size, out _, IntPtr.Zero))
            {
                version = Marshal.PtrToStructure<DriverVersion>(buffer);
                return version;
            }
        }
        finally
        {
            Marshal.FreeHGlobal(buffer);
        }
        
        return null;
    }
    
    /// <summary>
    /// Holt den Driver-Status
    /// </summary>
    public static DriverStatus? GetStatus()
    {
        using var handle = OpenDriver();
        if (handle == null) return null;
        
        var status = new DriverStatus();
        int size = Marshal.SizeOf(status);
        IntPtr buffer = Marshal.AllocHGlobal(size);
        
        try
        {
            if (DeviceIoControl(handle, IOCTL_GET_STATUS, IntPtr.Zero, 0, buffer, (uint)size, out _, IntPtr.Zero))
            {
                status = Marshal.PtrToStructure<DriverStatus>(buffer);
                return status;
            }
        }
        finally
        {
            Marshal.FreeHGlobal(buffer);
        }
        
        return null;
    }
    
    /// <summary>
    /// Spooft die Disk Serial
    /// </summary>
    public static bool SpoofDiskSerial(uint diskIndex, string newSerial)
    {
        using var handle = OpenDriver();
        if (handle == null) return false;
        
        var request = new SpoofDiskRequest
        {
            DiskIndex = diskIndex,
            SpoofedSerial = newSerial,
            OriginalSerial = "",
            Flags = 0
        };
        
        int size = Marshal.SizeOf(request);
        IntPtr buffer = Marshal.AllocHGlobal(size);
        
        try
        {
            Marshal.StructureToPtr(request, buffer, false);
            return DeviceIoControl(handle, IOCTL_SPOOF_DISK, buffer, (uint)size, buffer, (uint)size, out _, IntPtr.Zero);
        }
        finally
        {
            Marshal.FreeHGlobal(buffer);
        }
    }
    
    /// <summary>
    /// Spooft die SMBIOS UUID
    /// </summary>
    public static bool SpoofSmbios(string newUuid, string newSerial = "")
    {
        using var handle = OpenDriver();
        if (handle == null) return false;
        
        var request = new SpoofSmbiosRequest
        {
            SpoofedUuid = newUuid,
            SpoofedSerial = newSerial,
            OriginalUuid = "",
            OriginalSerial = "",
            Flags = 0
        };
        
        int size = Marshal.SizeOf(request);
        IntPtr buffer = Marshal.AllocHGlobal(size);
        
        try
        {
            Marshal.StructureToPtr(request, buffer, false);
            return DeviceIoControl(handle, IOCTL_SPOOF_SMBIOS, buffer, (uint)size, buffer, (uint)size, out _, IntPtr.Zero);
        }
        finally
        {
            Marshal.FreeHGlobal(buffer);
        }
    }
    
    /// <summary>
    /// Aktiviert Spoofing
    /// </summary>
    public static bool EnableSpoofing()
    {
        using var handle = OpenDriver();
        if (handle == null) return false;
        
        return DeviceIoControl(handle, IOCTL_ENABLE, IntPtr.Zero, 0, IntPtr.Zero, 0, out _, IntPtr.Zero);
    }
    
    /// <summary>
    /// Deaktiviert Spoofing
    /// </summary>
    public static bool DisableSpoofing()
    {
        using var handle = OpenDriver();
        if (handle == null) return false;
        
        return DeviceIoControl(handle, IOCTL_DISABLE, IntPtr.Zero, 0, IntPtr.Zero, 0, out _, IntPtr.Zero);
    }
    
    // ═══════════════════════════════════════════════════════════════════════
    // Helper
    // ═══════════════════════════════════════════════════════════════════════
    
    private static SafeFileHandle? OpenDriver()
    {
        var handle = CreateFile(
            DevicePath,
            GENERIC_READ | GENERIC_WRITE,
            0,
            IntPtr.Zero,
            OPEN_EXISTING,
            0,
            IntPtr.Zero
        );
        
        return handle.IsInvalid ? null : handle;
    }
}
