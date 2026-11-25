#ifndef HOOK_IMPL_HEADER
#define HOOK_IMPL_HEADER

#define HOOKMETHOD(name, ret_type, ...) \
    ret_type (*name)(__VA_ARGS__);      \
    ret_type new_##name(__VA_ARGS__)

#define MakeInfo(Func)   #Func, (void*)new_##Func, (void**)&Func
#define MakeMethod(Func) (void*)new_##Func, (void**)&Func
#define HookFunc(Func)   HookFunction((void*)Func, (void*)new_##Func, (void**)&Func);

#include <MinHook.h>

namespace HookImpl {
    static void InitHook() {
        MH_Initialize();
    }
    static int HookFunction(void* const symbol, void* const replace, void** result) {
        MH_CreateHook(symbol, replace, result);
        return MH_EnableHook(symbol);
    }
} // namespace HookImpl

#endif // hook_impl.hh