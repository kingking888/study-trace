#include "pch.h"

#include "resource.h"
#include <windows.h>

#include "inject/inject_api.h"

HWND g_dialog_hwnd_ = 0;

INT_PTR CALLBACK DialogProc(HWND hwndDialog,
                            UINT uMsg,
                            WPARAM wParam,
                            LPARAM lParam);

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     ) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        ::DialogBox(hModule, MAKEINTRESOURCE(IDDMain), NULL, DialogProc);

    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        //EndDialog(g_dialog_hwnd_, 0);

        break;
    }

    return TRUE;
}

INT_PTR CALLBACK DialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    UNREFERENCED_PARAMETER(lParam);

    switch (message) {
    case WM_INITDIALOG:
        g_dialog_hwnd_ = hDlg;
        return (INT_PTR)TRUE;

    case WM_CLOSE:
        EndDialog(hDlg, LOWORD(wParam));
        break;

    case WM_COMMAND: {
        switch (LOWORD(wParam)) {
        case ID_BTN_HOOK: {
            OutputDebugStringA("inject hook success");
            ::MessageBoxW(hDlg, L"Hook成功", L"提示", 0);

            const auto cb = [](const char* image, int len) {

            };
            inject::hookLoginQrCode(cb);
            break;
        }

        case ID_BTN_Unhook:
            break;

        default:
            break;
        }

        break;
    }
    }

    return (INT_PTR)FALSE;
}

