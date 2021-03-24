// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"

#include "inject/inject_api.h"
#include "inject/log.h"

#include <windows.h>
#include <atomic>
#include <thread>

std::atomic_bool g_run_falg_ = true;

DWORD WINAPI ThreadProc(LPVOID lpThreadParameter) {
    while (g_run_falg_) {
        inject::Infof(L"wechat login status:%d", inject::getLoginStatus());
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    inject::Infof(L"injector thread exit");
    return 0;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     ) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH: {
        HANDLE hThread = CreateThread(nullptr, 0, ThreadProc, nullptr, 0, nullptr);
        CloseHandle(hThread);
        break;
    }

    case DLL_THREAD_ATTACH:
        break;

    case DLL_THREAD_DETACH:
        break;

    case DLL_PROCESS_DETACH:
        g_run_falg_ = false; // 最坏的情况是，1秒后线程退出。
        std::this_thread::sleep_for(std::chrono::seconds(1));
        break;
    }

    return TRUE;
}

