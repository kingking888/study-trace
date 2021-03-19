// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"

#include <thread>
#include <atomic>

std::atomic_bool g_run_falg_ = false;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     ) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH: {
        OutputDebugStringW(L"injector DLL_PROCESS_ATTACH");

        std::thread t([]() {
            while (g_run_falg_) {
                std::this_thread::sleep_for(std::chrono::seconds(2));

                OutputDebugStringW(L"thread run");
            }
        });
        t.detach();

        break;
    }

    case DLL_THREAD_ATTACH:
        OutputDebugStringW(L"injector DLL_THREAD_ATTACH");
        break;

    case DLL_THREAD_DETACH:
        OutputDebugStringW(L"injector DLL_THREAD_DETACH");
        break;

    case DLL_PROCESS_DETACH: {
        OutputDebugStringW(L"injector DLL_PROCESS_DETACH");
        break;
    }

    }

    return TRUE;
}

