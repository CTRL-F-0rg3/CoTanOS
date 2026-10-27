using System;
using System.Runtime.InteropServices;

namespace TrangorgePanel
{
    public static class NativeBridge
    {
        // Wskazujemy naszą bibliotekę C++. 
        // Na Linuxie to libResourceMonitor.so, na Windowsie byłoby ResourceMonitor.dll
        private const string LibName = "libResourceMonitor.so"; 

        [DllImport(LibName, CallingConvention = CallingConvention.Cdecl)]
        public static extern double GetCpuUsage();

        [DllImport(LibName, CallingConvention = CallingConvention.Cdecl)]
        public static extern double GetRamUsagePercent();

        [DllImport(LibName, CallingConvention = CallingConvention.Cdecl)]
        public static extern long GetTotalRamMB();
    }
}