#include <unistd.h>
#include <windows.h>

#include <thread>

#include <MinHook.h>

#include "utils/hook_impl.hh"
#include "utils/ini.hh"
#include "utils/logger.hh"

struct Item {
    int command_id = 0;
    int type = 0;
    std::u16string label;
};

std::string parse_u16string_to_ansi(const void* obj_addr) {
    if (!obj_addr)
        return std::string();

    struct LibcxxLongLayout {
        const wchar_t* data;
        size_t size;
        size_t cap;
    };

    const auto* long_str = reinterpret_cast<const LibcxxLongLayout*>(obj_addr);
    const wchar_t* wstr = nullptr;
    int len = 0;

    uintptr_t first_ptr = reinterpret_cast<uintptr_t>(long_str->data);
    bool is_heap_candidate = (first_ptr >= 0x10000) &&
                             (first_ptr < 0x7FFFFFFFFFFF) &&
                             ((first_ptr & 1) == 0) &&
                             (long_str->size > 0) &&
                             (long_str->size < 4096) &&
                             (long_str->cap >= long_str->size);

    if (is_heap_candidate) {
        wstr = long_str->data;
        len = static_cast<int>(long_str->size);
    }
    else {
        wstr = reinterpret_cast<const wchar_t*>(obj_addr);
        while (len < 11 && wstr[len] != L'\0') {
            len++;
        }
    }

    if (!wstr || len <= 0)
        return std::string();

    int size_needed = WideCharToMultiByte(CP_ACP, 0, wstr, len, nullptr, 0, nullptr, nullptr);
    if (size_needed <= 0)
        return std::string();

    std::string dest(size_needed, 0);
    WideCharToMultiByte(CP_ACP, 0, wstr, len, &dest[0], size_needed, nullptr, nullptr);
    return dest;
}

static std::unordered_set<int> black_list_;

HOOKMETHOD(SimpleMenuModel__AddItem, void, void* _, Item* item) {
    int command_id = item->command_id;
    std::string label = parse_u16string_to_ansi(&item->label);
    DebugPrintf("[ccmm] [AddItem] command_id: %d - %s", command_id, label.c_str());
    if (black_list_.count(command_id)) {
        return;
    }
    SimpleMenuModel__AddItem(_, item);
}

void LoadPlugin() {
    EnableLogDebug();

    ini::IniFile cfg("ccmm.ini");
    if (!cfg.IsValid()) {
        DebugPrintf("[ccmm] No ccmm.ini.");
        return;
    }

    auto offset = cfg.ReadInt("Config", "offset");
    if (offset.has_value() == false) {
        DebugPrintf("[ccmm] No offset.");
        return;
    }

    black_list_ = cfg.ReadIntSet("Filter", "blacklist_ids");
    if (black_list_.size() <= 0) {
        DebugPrintf("[ccmm] No blacklist_ids.");
        return;
    }

    const char* target_dll_name = "chrome.dll";

    auto dll_name = cfg.ReadString("Config", "dll");

    if (dll_name && !dll_name->empty()) {
        target_dll_name = dll_name->c_str();
        DebugPrintf("[ccmm] Target dll: %s.", target_dll_name);
    }

    DisableLogDebug();

    auto is_enable_debug = cfg.ReadInt("Config", "debug");
    if (is_enable_debug == 1) {
        EnableLogDebug();
    }

    DebugPrintf("[ccmm] [%d] Loading...", getpid());

    Sleep(2000);

    if (auto handle = LoadLibrary(target_dll_name)) {
        HookImpl::InitHook();
        HookImpl::HookFunction((void*)((char*)handle + offset.value()), MakeMethod(SimpleMenuModel__AddItem));
        DebugPrintf("[ccmm] [%d] Success!", getpid());
    }
    else {
        DebugPrintf("[ccmm] Cannot load %s.", target_dll_name);
    }
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hModule);

        LPCWSTR cmd_line = GetCommandLineW();
        if (wcsstr(cmd_line, L"--type=") == nullptr) {
            std::thread([=]() {
                LoadPlugin();
            }).detach();
        }
    }
    return TRUE;
}