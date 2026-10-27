#include <fstream>
#include <string>
#include <sys/sysinfo.h>
#include <unistd.h>

// Zmienne do obliczania CPU
static long long prevIdle = 0;
static long long prevTotal = 0;

extern "C" {
    // Eksportujemy funkcje, aby C# mógł je zobaczyć
    __attribute__((visibility("default"))) double GetCpuUsage() {
        std::ifstream file("/proc/stat");
        std::string cpu;
        long long user, nice, system, idle, iowait, irq, softirq, steal;
        
        file >> cpu >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal;

        long long currIdle = idle + iowait;
        long long currTotal = user + nice + system + idle + iowait + irq + softirq + steal;

        long long diffIdle = currIdle - prevIdle;
        long long diffTotal = currTotal - prevTotal;

        prevIdle = currIdle;
        prevTotal = currTotal;

        if (diffTotal == 0) return 0.0;
        return (1.0 - (double)diffIdle / diffTotal) * 100.0;
    }

    __attribute__((visibility("default"))) double GetRamUsagePercent() {
        struct sysinfo memInfo;
        sysinfo(&memInfo);
        long long totalPhys = memInfo.totalram * memInfo.mem_unit;
        long long freePhys = memInfo.freeram * memInfo.mem_unit;
        long long usedPhys = totalPhys - freePhys;
        return ((double)usedPhys / totalPhys) * 100.0;
    }
    
    __attribute__((visibility("default"))) long long GetTotalRamMB() {
        struct sysinfo memInfo;
        sysinfo(&memInfo);
        return (memInfo.totalram * memInfo.mem_unit) / (1024 * 1024);
    }
}