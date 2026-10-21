#pragma once

// Makra do eksportowania funkcji z DLL
#ifdef RESOURCEMONITOR_EXPORTS
#define RM_API __declspec(dllexport)
#else
#define RM_API __declspec(dllimport)
#endif

extern "C" {
    // Zwraca użycie CPU w procentach (0.0 - 100.0)
    RM_API double GetCpuUsage();
    
    // Zwraca użycie RAM w procentach (0.0 - 100.0)
    RM_API double GetRamUsagePercent();
    
    // Zwraca całkowitą i dostępną pamięć w MB
    RM_API unsigned long long GetTotalRamMB();
    RM_API unsigned long long GetAvailableRamMB();
}