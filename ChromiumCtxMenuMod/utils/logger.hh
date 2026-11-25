#ifndef LOGGER_HEADER
#define LOGGER_HEADER

void EnableLogDebug();

void DisableLogDebug();

void DebugPrintf(const char* format, ...);

void DumpHexShort(const void* addr, int line);

#endif