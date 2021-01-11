#include "utils.h"

bool GetCurrentMonitorSize(HWND window, RECT* region) {
    MONITORINFO mi;
    mi.cbSize = sizeof(mi);

    if (GetMonitorInfo(MonitorFromWindow(window, MONITOR_DEFAULTTONEAREST), &mi)) {
        *region = mi.rcMonitor;

        // If not the primary display the origin for top left won't be (0,0)
        if (mi.dwFlags != MONITORINFOF_PRIMARY) {
            region->left = 0;
            region->right = mi.rcMonitor.right - mi.rcMonitor.left;
            region->top = 0;
            region->bottom = mi.rcMonitor.bottom - mi.rcMonitor.top;
        }

        return true;
    }

    return false;
}