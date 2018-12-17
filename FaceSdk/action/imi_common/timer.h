#pragma once

#ifdef _WIN32

#include <windows.h>
#pragma comment(lib, "Winmm.lib")
#define TIC(name) DWORD time_##name = timeGetTime();
#define TOC(name) DWORD time_##name##_2 = timeGetTime(); printf("time_%s = %.1f\n", #name , (time_##name##_2 - time_##name) * 1.0f);
#else
#define TIC(name)
#define TOC(name)
#endif