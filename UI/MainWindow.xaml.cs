/**
 * ═══════════════════════════════════════════════════════════════════════════
 *  SPOOFLIGHT - Main Window Code-Behind
 *  Hier ist die Logik für unsere UI
 * ═══════════════════════════════════════════════════════════════════════════
 * 
 * 📚 LEKTION: Code-Behind in WPF
 * 
 * In WPF gibt es zwei Dateien pro Fenster:
 *   - MainWindow.xaml = Das Aussehen (wie HTML/CSS)
 *   - MainWindow.xaml.cs = Die Logik (wie JavaScript)
 * 
 * Du kennst das von Web-Entwicklung!
 */

using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;
using System.Windows.Shapes;
using UI.Native;

namespace UI;

public partial class MainWindow : Window
{
    // ═══════════════════════════════════════════════════════════════════════
    // 📚 LEKTION: Felder (Fields)
    // 
    // Wie "let" Variablen in JavaScript, aber auf Klassen-Ebene.
    // Sie behalten ihren Wert solange das Fenster existiert.
    // ═══════════════════════════════════════════════════════════════════════
    
    private bool _isInitialized = false;
    private string? _currentGuid = null;
    
    // MAC Spoofing
    private List<NetworkAdapter> _adapters = new();
    private int _selectedAdapterIndex = -1;
    
    public MainWindow()
    {
        InitializeComponent();
        
        // 📚 LEKTION: Event Handler in C#
        // "Loaded" wird aufgerufen sobald das Fenster fertig geladen ist.
        // Wie "DOMContentLoaded" in JavaScript!
        Loaded += OnWindowLoaded;
    }
    
    // ═══════════════════════════════════════════════════════════════════════
    // Event Handlers
    // ═══════════════════════════════════════════════════════════════════════
    
    private void OnWindowLoaded(object sender, RoutedEventArgs e)
    {
        Log("Starting Spooflight...");
        
        // DLL initialisieren
        try
        {
            _isInitialized = SpooflightCore.Initialize();
            
            if (_isInitialized)
            {
                string version = SpooflightCore.GetVersion();
                VersionText.Text = $"v{version}";
                
                StatusText.Text = "Online";
                StatusDot.Fill = new SolidColorBrush(Color.FromRgb(34, 197, 94)); // Green
                
                Log($"SpooflightCore v{version} loaded");
                
                // Netzwerk-Adapter laden
                LoadNetworkAdapters();
                
                // Hardware-IDs laden
                LoadHardwareInfo();
            }
            else
            {
                SetError("Failed to initialize SpooflightCore.dll");
            }
        }
        catch (DllNotFoundException ex)
        {
            SetError($"DLL not found! Make sure SpooflightCore.dll is in the app folder.\n{ex.Message}");
        }
        catch (Exception ex)
        {
            SetError($"Error: {ex.Message}");
        }
    }
    
    /// <summary>
    /// 📖 Read Button - Liest die aktuelle MachineGuid
    /// </summary>
    private void OnReadGuidClick(object sender, RoutedEventArgs e)
    {
        if (!CheckInitialized()) return;
        
        Log("Reading MachineGuid from registry...");
        
        try
        {
            _currentGuid = SpooflightCore.ReadMachineGuid();
            
            if (_currentGuid != null)
            {
                CurrentGuidText.Text = _currentGuid;
                CurrentGuidText.Foreground = new SolidColorBrush(Color.FromRgb(232, 232, 232));
                SpoofGuidButton.IsEnabled = true;
                Log($"Current GUID: {_currentGuid}");
            }
            else
            {
                Log("ERROR: Could not read MachineGuid");
                CurrentGuidText.Text = "Error reading GUID";
                CurrentGuidText.Foreground = new SolidColorBrush(Color.FromRgb(239, 68, 68));
            }
        }
        catch (Exception ex)
        {
            Log($"ERROR: {ex.Message}");
        }
    }
    
    /// <summary>
    /// 🎲 Generate Button - Generiert eine zufällige GUID
    /// </summary>
    private void OnGenerateGuidClick(object sender, RoutedEventArgs e)
    {
        // 📚 LEKTION: Guid.NewGuid()
        // .NET hat eine eingebaute Funktion um GUIDs zu generieren!
        string newGuid = Guid.NewGuid().ToString();
        NewGuidInput.Text = newGuid;
        Log($"Generated new GUID: {newGuid}");
    }
    
    /// <summary>
    /// ⚡ Spoof Button - Ändert die MachineGuid
    /// </summary>
    private void OnSpoofGuidClick(object sender, RoutedEventArgs e)
    {
        if (!CheckInitialized()) return;
        
        string newGuid = NewGuidInput.Text.Trim();
        
        // Validierung
        if (string.IsNullOrEmpty(newGuid))
        {
            Log("ERROR: Please enter a new GUID or click 'Generate Random'");
            return;
        }
        
        // GUID Format prüfen (einfache Prüfung)
        if (!Guid.TryParse(newGuid, out _))
        {
            Log("ERROR: Invalid GUID format! Use: xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx");
            return;
        }
        
        Log($"Spoofing MachineGuid to: {newGuid}");
        Log("⚠️  This requires Administrator privileges!");
        
        try
        {
            SpoofResult result = SpooflightCore.SpoofMachineGuid(newGuid);
            
            if (result.Success)
            {
                Log("✅ SUCCESS: " + result.Message);
                Log($"   Old: {result.OldValue}");
                Log($"   New: {result.NewValue}");
                
                // UI aktualisieren
                CurrentGuidText.Text = result.NewValue;
                _currentGuid = result.NewValue;
                
                MessageBox.Show(
                    $"MachineGuid successfully spoofed!\n\nOld: {result.OldValue}\nNew: {result.NewValue}",
                    "Success",
                    MessageBoxButton.OK,
                    MessageBoxImage.Information
                );
            }
            else
            {
                Log("❌ FAILED: " + result.Message);
                
                MessageBox.Show(
                    $"Failed to spoof MachineGuid:\n\n{result.Message}\n\nTry running as Administrator!",
                    "Error",
                    MessageBoxButton.OK,
                    MessageBoxImage.Error
                );
            }
        }
        catch (Exception ex)
        {
            Log($"ERROR: {ex.Message}");
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════
    // Hilfsmethoden
    // ═══════════════════════════════════════════════════════════════════════
    
    private bool CheckInitialized()
    {
        if (!_isInitialized)
        {
            Log("ERROR: SpooflightCore not initialized!");
            return false;
        }
        return true;
    }
    
    private void SetError(string message)
    {
        StatusText.Text = "Error";
        StatusDot.Fill = new SolidColorBrush(Color.FromRgb(239, 68, 68)); // Red
        Log($"ERROR: {message}");
    }
    
    /// <summary>
    /// Tab Navigation
    /// </summary>
    private void OnTabChanged(object sender, RoutedEventArgs e)
    {
        // Alle Pages verstecken
        if (PageGuid != null) PageGuid.Visibility = Visibility.Collapsed;
        if (PageMac != null) PageMac.Visibility = Visibility.Collapsed;
        if (PageHwid != null) PageHwid.Visibility = Visibility.Collapsed;
        if (PageAnalysis != null) PageAnalysis.Visibility = Visibility.Collapsed;
        if (PageKernel != null) PageKernel.Visibility = Visibility.Collapsed;
        
        // Aktive Page zeigen
        if (TabGuid?.IsChecked == true) PageGuid.Visibility = Visibility.Visible;
        else if (TabMac?.IsChecked == true) PageMac.Visibility = Visibility.Visible;
        else if (TabHwid?.IsChecked == true) PageHwid.Visibility = Visibility.Visible;
        else if (TabAnalysis?.IsChecked == true) PageAnalysis.Visibility = Visibility.Visible;
        else if (TabKernel?.IsChecked == true) PageKernel.Visibility = Visibility.Visible;
    }
    
    /// <summary>
    /// Schreibt eine Nachricht ins Log
    /// </summary>
    private void Log(string message)
    {
        string timestamp = DateTime.Now.ToString("HH:mm:ss");
        string newLine = $"[{timestamp}] {message}\n";
        
        // 📚 LEKTION: Dispatcher
        // WPF UI kann nur vom "UI Thread" geändert werden.
        // Dispatcher.Invoke stellt sicher, dass wir auf dem richtigen Thread sind.
        Dispatcher.Invoke(() =>
        {
            LogText.Text += newLine;
        });
    }
    
    // ═══════════════════════════════════════════════════════════════════════
    // MAC Spoofing Event Handlers
    // ═══════════════════════════════════════════════════════════════════════
    
    /// <summary>
    /// Lädt alle Netzwerk-Adapter in die ComboBox
    /// </summary>
    private void LoadNetworkAdapters()
    {
        try
        {
            Log("Loading network adapters...");
            _adapters = SpooflightCore.GetAdapters();
            
            AdapterComboBox.Items.Clear();
            
            foreach (var adapter in _adapters)
            {
                // Zeige Description und aktuelle MAC
                string displayText = $"{adapter.Description} ({adapter.CurrentMac})";
                AdapterComboBox.Items.Add(displayText);
            }
            
            Log($"Found {_adapters.Count} network adapter(s)");
            
            if (_adapters.Count > 0)
            {
                AdapterComboBox.SelectedIndex = 0;
            }
        }
        catch (Exception ex)
        {
            Log($"ERROR loading adapters: {ex.Message}");
        }
    }
    
    /// <summary>
    /// Wenn ein anderer Adapter ausgewählt wird
    /// </summary>
    private void OnAdapterSelectionChanged(object sender, System.Windows.Controls.SelectionChangedEventArgs e)
    {
        int selectedIndex = AdapterComboBox.SelectedIndex;
        
        if (selectedIndex >= 0 && selectedIndex < _adapters.Count)
        {
            var adapter = _adapters[selectedIndex];
            _selectedAdapterIndex = adapter.Index;
            
            CurrentMacText.Text = adapter.CurrentMac;
            CurrentMacText.Foreground = new SolidColorBrush(Color.FromRgb(232, 232, 232));
            SpoofMacButton.IsEnabled = true;
            
            Log($"Selected adapter: {adapter.Description}");
        }
        else
        {
            _selectedAdapterIndex = -1;
            CurrentMacText.Text = "Select an adapter";
            CurrentMacText.Foreground = new SolidColorBrush(Color.FromRgb(107, 114, 128));
            SpoofMacButton.IsEnabled = false;
        }
    }
    
    /// <summary>
    /// 🔄 Refresh - Adapter neu laden
    /// </summary>
    private void OnRefreshAdaptersClick(object sender, RoutedEventArgs e)
    {
        LoadNetworkAdapters();
    }
    
    /// <summary>
    /// 🎲 Generate Random MAC
    /// </summary>
    private void OnGenerateMacClick(object sender, RoutedEventArgs e)
    {
        string newMac = SpooflightCore.GenerateRandomMac();
        NewMacInput.Text = newMac;
        Log($"Generated random MAC: {newMac}");
    }
    
    /// <summary>
    /// ⚡ Spoof MAC Button
    /// </summary>
    private void OnSpoofMacClick(object sender, RoutedEventArgs e)
    {
        if (!CheckInitialized()) return;
        
        if (_selectedAdapterIndex < 0)
        {
            Log("ERROR: Please select a network adapter first!");
            return;
        }
        
        string newMac = NewMacInput.Text.Trim();
        
        if (string.IsNullOrEmpty(newMac))
        {
            Log("ERROR: Please enter a new MAC or click 'Generate Random'");
            return;
        }
        
        Log($"Spoofing MAC address to: {newMac}");
        Log("⚠️  This requires Administrator privileges!");
        
        try
        {
            SpoofResult result = SpooflightCore.SpoofMacAddress(_selectedAdapterIndex, newMac);
            
            if (result.Success)
            {
                Log("✅ SUCCESS: " + result.Message);
                Log($"   Old: {result.OldValue}");
                Log($"   New: {result.NewValue}");
                
                MessageBox.Show(
                    $"MAC Address spoofed!\n\nOld: {result.OldValue}\nNew: {result.NewValue}\n\n⚠️ Disable and re-enable the adapter for changes to take effect!",
                    "Success",
                    MessageBoxButton.OK,
                    MessageBoxImage.Information
                );
                
                // Adapter-Liste neu laden
                LoadNetworkAdapters();
            }
            else
            {
                Log("❌ FAILED: " + result.Message);
                
                MessageBox.Show(
                    $"Failed to spoof MAC:\n\n{result.Message}\n\nTry running as Administrator!",
                    "Error",
                    MessageBoxButton.OK,
                    MessageBoxImage.Error
                );
            }
        }
        catch (Exception ex)
        {
            Log($"ERROR: {ex.Message}");
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════
    // Hardware ID Spoofing Event Handlers (Phase 4)
    // ═══════════════════════════════════════════════════════════════════════
    
    /// <summary>
    /// Lädt alle Hardware-IDs und zeigt sie an
    /// </summary>
    private void LoadHardwareInfo()
    {
        try
        {
            Log("Loading hardware information...");
            
            // Alle Hardware-Infos auf einmal laden
            HardwareInfo info = SpooflightCore.GetHardwareInfo();
            
            // UI aktualisieren
            DiskSerialText.Text = !string.IsNullOrEmpty(info.DiskSerial) ? info.DiskSerial : "Not available";
            VolumeSerialText.Text = !string.IsNullOrEmpty(info.VolumeSerial) ? info.VolumeSerial : "Not available";
            SmbiosUuidText.Text = !string.IsNullOrEmpty(info.BiosSerial) ? info.BiosSerial : "Not available";
            CpuIdText.Text = !string.IsNullOrEmpty(info.CpuId) ? info.CpuId : "Not available";
            ProductIdText.Text = !string.IsNullOrEmpty(info.ProductId) ? info.ProductId : "Not available";
            
            Log("Hardware IDs loaded successfully!");
            Log($"   Disk: {info.DiskSerial}");
            Log($"   Volume: {info.VolumeSerial}");
            Log($"   SMBIOS: {info.BiosSerial}");
            Log($"   CPU: {info.CpuId}");
            Log($"   Product: {info.ProductId}");
        }
        catch (Exception ex)
        {
            Log($"ERROR loading hardware info: {ex.Message}");
        }
    }
    
    /// <summary>
    /// 🔄 Refresh Hardware IDs Button
    /// </summary>
    private void OnRefreshHardwareClick(object sender, RoutedEventArgs e)
    {
        LoadHardwareInfo();
    }
    
    /// <summary>
    /// 🎲 Generate Random Product ID
    /// </summary>
    private void OnGenerateProductIdClick(object sender, RoutedEventArgs e)
    {
        string newProductId = SpooflightCore.GenerateRandomProdId();
        NewProductIdInput.Text = newProductId;
        Log($"Generated random Product ID: {newProductId}");
    }
    
    /// <summary>
    /// ⚡ Spoof Product ID Button
    /// </summary>
    private void OnSpoofProductIdClick(object sender, RoutedEventArgs e)
    {
        if (!CheckInitialized()) return;
        
        string newProductId = NewProductIdInput.Text.Trim();
        
        if (string.IsNullOrEmpty(newProductId))
        {
            Log("ERROR: Please enter a new Product ID or click 'Generate Random'");
            return;
        }
        
        Log($"Spoofing Product ID to: {newProductId}");
        Log("⚠️  This requires Administrator privileges!");
        
        try
        {
            SpoofResult result = SpooflightCore.SpoofProductId(newProductId);
            
            if (result.Success)
            {
                Log("✅ SUCCESS: " + result.Message);
                Log($"   Old: {result.OldValue}");
                Log($"   New: {result.NewValue}");
                
                // UI aktualisieren
                ProductIdText.Text = result.NewValue;
                
                MessageBox.Show(
                    $"Product ID spoofed!\n\nOld: {result.OldValue}\nNew: {result.NewValue}",
                    "Success",
                    MessageBoxButton.OK,
                    MessageBoxImage.Information
                );
            }
            else
            {
                Log("❌ FAILED: " + result.Message);
                
                MessageBox.Show(
                    $"Failed to spoof Product ID:\n\n{result.Message}\n\nTry running as Administrator!",
                    "Error",
                    MessageBoxButton.OK,
                    MessageBoxImage.Error
                );
            }
        }
        catch (Exception ex)
        {
            Log($"ERROR: {ex.Message}");
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════
    // System-Analyse Event Handlers (Phase 5)
    // ═══════════════════════════════════════════════════════════════════════
    
    /// <summary>
    /// 🔬 Run Full System Analysis
    /// </summary>
    private void OnRunAnalysisClick(object sender, RoutedEventArgs e)
    {
        if (!CheckInitialized()) return;
        
        Log("Running full system analysis...");
        
        try
        {
            // Komplette Analyse ausführen
            SystemAnalysis analysis = SpooflightCore.RunSystemAnalysis();
            
            // TPM Status
            string tpmStatus = analysis.Tpm.Present 
                ? $"TPM {(analysis.Tpm.Enabled ? "ACTIVE ⚠️" : "DISABLED ✅")}\n" +
                  $"Version: {(string.IsNullOrEmpty(analysis.Tpm.Version) ? "Unknown" : analysis.Tpm.Version)}\n" +
                  $"Manufacturer: {analysis.Tpm.Manufacturer}\n" +
                  $"{analysis.Tpm.Status}"
                : "TPM not detected ✅";
            TpmStatusText.Text = tpmStatus;
            
            // TPM Farbe anpassen
            if (analysis.Tpm.Present && analysis.Tpm.Enabled)
            {
                TpmStatusText.Foreground = new SolidColorBrush(Color.FromRgb(245, 158, 11)); // Warning orange
            }
            else
            {
                TpmStatusText.Foreground = new SolidColorBrush(Color.FromRgb(16, 185, 129)); // Success green
            }
            
            // CPU Info
            string cpuInfo = $"Vendor: {analysis.Cpu.Vendor}\n" +
                            $"Brand: {analysis.Cpu.Brand}\n" +
                            $"Processor ID: {analysis.Cpu.ProcessorId}\n" +
                            $"Family: {analysis.Cpu.Family}, Model: {analysis.Cpu.Model}, Stepping: {analysis.Cpu.Stepping}\n" +
                            $"Microcode: {analysis.Cpu.Microcode}";
            CpuInfoText.Text = cpuInfo;
            
            // Disk Details
            string diskDetails = "";
            if (analysis.Disks != null)
            {
                for (int i = 0; i < analysis.DiskCount && i < analysis.Disks.Length; i++)
                {
                    var disk = analysis.Disks[i];
                    diskDetails += $"[{i}] {disk.Model}\n";
                    diskDetails += $"    Serial: {disk.Serial}\n";
                    diskDetails += $"    Interface: {disk.InterfaceType}, FW: {disk.Firmware}\n";
                    if (!string.IsNullOrEmpty(disk.SpoofingAdvice))
                    {
                        diskDetails += $"    ⚠️ {(disk.IsNvme ? "NVMe" : "SATA")}: Hardware-burned serial\n";
                    }
                }
            }
            DiskDetailsText.Text = string.IsNullOrEmpty(diskDetails) ? "No disks found" : diskDetails.TrimEnd();
            
            // VM Detection
            if (analysis.Cpu.HypervisorPresent)
            {
                VmStatusText.Text = $"✅ Running in VM: {analysis.Cpu.HypervisorVendor}\nHardware IDs are virtualized!";
                VmStatusText.Foreground = new SolidColorBrush(Color.FromRgb(16, 185, 129));
                VmDetectionBorder.Background = new SolidColorBrush(Color.FromRgb(26, 42, 26));
            }
            else
            {
                VmStatusText.Text = "⚠️ Bare Metal - Not running in VM\nConsider VM for hardware ID isolation";
                VmStatusText.Foreground = new SolidColorBrush(Color.FromRgb(245, 158, 11));
                VmDetectionBorder.Background = new SolidColorBrush(Color.FromRgb(42, 32, 26));
            }
            
            // SMBIOS Info ins Log
            Log($"BIOS: {analysis.BiosVendor} {analysis.BiosVersion}");
            Log($"System: {analysis.SystemManufacturer} {analysis.SystemProduct}");
            Log($"Baseboard Serial: {analysis.BaseboardSerial}");
            
            // Recommendations
            RecommendationsText.Text = analysis.Recommendations;
            
            Log("System analysis complete!");
            Log("═════════════════════════════════════════");
            Log($"TPM: {(analysis.Tpm.Present ? (analysis.Tpm.Enabled ? "ACTIVE ⚠️" : "Disabled ✅") : "Not present ✅")}");
            Log($"VM: {(analysis.Cpu.HypervisorPresent ? "Yes ✅" : "No (bare metal)")}");
            Log($"Disks: {analysis.DiskCount} found");
            Log("═════════════════════════════════════════");
        }
        catch (Exception ex)
        {
            Log($"ERROR during analysis: {ex.Message}");
            TpmStatusText.Text = "Error during analysis";
        }
    }
}
