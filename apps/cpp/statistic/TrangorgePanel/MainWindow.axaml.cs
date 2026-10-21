using Avalonia.Controls;
using Avalonia.Threading;
using System;

namespace TrangorgePanel
{
    public partial class MainWindow : Window
    {
        private DispatcherTimer _timer;

        public MainWindow()
        {
            InitializeComponent();

            // Inicjalizacja timera (odświeżanie co 1000 ms)
            _timer = new DispatcherTimer
            {
                Interval = TimeSpan.FromSeconds(1)
            };
            _timer.Tick += Timer_Tick;
            _timer.Start();

            // Pierwsze wywołanie, żeby ustawić całkowitą pamięć
            UpdateTotalRam();
        }

        private void Timer_Tick(object? sender, EventArgs e)
        {
            try
            {
                // Pobieramy dane z biblioteki C++
                double cpu = NativeBridge.GetCpuUsage();
                double ram = NativeBridge.GetRamUsagePercent();

                // Aktualizujemy UI
                BarCpu.Value = cpu;
                TxtCpu.Text = $"{cpu:F1}%";

                BarRam.Value = ram;
                TxtRam.Text = $"{ram:F1}%";
            }
            catch (Exception ex)
            {
                // Obsługa błędu, np. gdy C++ rzuci wyjątkiem
                TxtCpu.Text = "Error";
                System.Diagnostics.Debug.WriteLine(ex.Message);
            }
        }

        private void UpdateTotalRam()
        {
            try 
            {
                long totalRam = NativeBridge.GetTotalRamMB();
                TxtTotalRam.Text = $"Total Physical Memory: {totalRam} MB";
            }
            catch { }
        }
    }
}