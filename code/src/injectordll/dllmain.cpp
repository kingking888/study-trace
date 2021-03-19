// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"

#include <thread>
#include <atomic>

std::atomic_bool g_run_falg_ = true;

#define TEST_THREAD
//#define TEST_THREAD_NOT_EXIT_DLL_WILL_UNLOAD_SUCCESS

DWORD WINAPI ThreadProc(LPVOID lpThreadParameter) {
    while (g_run_falg_) {
        OutputDebugStringW(L"injector thread run");
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    OutputDebugStringW(L"injector thread exit");

    return 0;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     ) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH: {
        OutputDebugStringW(L"injector DLL_PROCESS_ATTACH");

#ifdef TEST_THREAD
        /*
        std::thread t([]() {
            while (g_run_falg_) {
                std::this_thread::sleep_for(std::chrono::seconds(1));
                OutputDebugStringW(L"injector thread run");
            }

            OutputDebugStringW(L"injector thread exit");
        });
        t.detach();
        */

        // std::thread 会导致注入器卡死，且无法正常退出微信。需使用windows api，原因不明。
        DWORD dwThreadId;
        HANDLE hThread = CreateThread(NULL, 0, ThreadProc, NULL, 0, &dwThreadId);
        std::this_thread::sleep_for(std::chrono::seconds(1));
        CloseHandle(hThread);
#endif

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

#ifndef TEST_THREAD_NOT_EXIT_DLL_WILL_UNLOAD_SUCCESS
        g_run_falg_ = false; // 最坏的情况是，1秒后线程退出。
        std::this_thread::sleep_for(std::chrono::seconds(1));
#endif

        break;
    }

    }

    return TRUE;
}