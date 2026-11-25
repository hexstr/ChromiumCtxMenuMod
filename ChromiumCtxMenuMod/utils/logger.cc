#include "logger.hh"

#include <debugapi.h>
#include <stdio.h>

#include <atomic>
#include <cstdarg>
#include <cstdio>

namespace {

    std::atomic<bool> enable_debug_{ false };

}

void EnableLogDebug() {
    enable_debug_.store(true, std::memory_order_relaxed);
}

void DisableLogDebug() {
    enable_debug_.store(false, std::memory_order_relaxed);
}

bool IsDebugEnabled() {
    return enable_debug_.load(std::memory_order_relaxed);
}

void DebugPrintf(const char* format, ...) {
    if (!IsDebugEnabled()) {
        return;
    }
    char buffer[1024];
    va_list args;

    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    OutputDebugStringA(buffer);
}

void DebugPrintfW(const wchar_t* format, ...) {
    if (!IsDebugEnabled()) {
        return;
    }

    wchar_t buffer[1024];
    va_list args;

    va_start(args, format);
    _vsnwprintf_s(buffer, _countof(buffer), _TRUNCATE, format, args);
    va_end(args);

    OutputDebugStringW(buffer);
}

void DumpHexShort(const void* addr, int line) {
    if (!IsDebugEnabled()) {
        return;
    }

    const unsigned char* ptr = (const unsigned char*)addr;
    char buf[32];

    for (int i = 0; i < line; ++i) {
        snprintf(buf, sizeof(buf), "%02X %02X %02X %02X\n",
                 ptr[i * 4], ptr[i * 4 + 1], ptr[i * 4 + 2], ptr[i * 4 + 3]);
        OutputDebugStringA(buf);
    }
}