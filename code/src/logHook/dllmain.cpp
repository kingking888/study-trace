// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"

#include "inject/inject_api.h"

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     ) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        inject::hookWeChatLog();
        break;

    case DLL_THREAD_ATTACH:
        break;

    case DLL_THREAD_DETACH:
        break;

    case DLL_PROCESS_DETACH:
        inject::unhookWeChatLog();
        break;
    }

    return TRUE;
}

