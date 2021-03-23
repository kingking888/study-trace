#include "pch.h"

#include "resource.h"
#include <windows.h>

#include "inject/inject_api.h"
#include "inject/log.h"

#include "base/util/string_util.h"

#include <atlimage.h>
#include <thread>

HWND g_dialog_hwnd_ = 0;
const std::wstring kSaveQRCodeImageName = L"qrcode.png";

#define WM_USER_SHOW_CODE (WM_USER + 100)

INT_PTR CALLBACK DialogProc(HWND hwndDialog,
                            UINT uMsg,
                            WPARAM wParam,
                            LPARAM lParam);

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     ) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH: {
        inject::Infof("DLL_PROCESS_ATTACH");

        // 模态对话框，导致无法通过注入器卸载，使用EndDialog(g_dialog_hwnd_, 0)关闭模态对话框的同时，自动卸载dll
        ::DialogBox(hModule, MAKEINTRESOURCE(IDDMain), NULL, DialogProc);
        break;
    }

    case DLL_THREAD_ATTACH:
        break;

    case DLL_THREAD_DETACH:
        break;

    case DLL_PROCESS_DETACH:
        inject::Infof("DLL_PROCESS_DETACH");
        //EndDialog(g_dialog_hwnd_,0);
        //inject::unHookLoginQrCode();
        break;
    }

    return TRUE;
}

inject::QrCodeInfo g_cp_ = {};

INT_PTR CALLBACK DialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    UNREFERENCED_PARAMETER(lParam);

    switch (message) {
    case WM_INITDIALOG:
        g_dialog_hwnd_ = hDlg;
        return (INT_PTR)TRUE;

    case WM_CLOSE:
        EndDialog(hDlg, 0);
        inject::unHookLoginQrCode();
        break;

    case WM_USER_SHOW_CODE: {
        std::wstring addrText = nbase::UTF8ToUTF16(nbase::StringPrintf("%X", g_cp_.addr));
        std::wstring imageAddrText = nbase::UTF8ToUTF16(nbase::StringPrintf("%X", *((dword_t*)g_cp_.addr)));
        std::wstring imageLenText = nbase::UTF8ToUTF16(nbase::StringPrintf("%X", g_cp_.image_len));
        SetDlgItemText(g_dialog_hwnd_, IDC_EDIT_ADDR, addrText.c_str());
        SetDlgItemText(g_dialog_hwnd_, IDC_EDIT_PIC_ADDR, imageAddrText.c_str());
        SetDlgItemText(g_dialog_hwnd_, IDC_EDIT_PIC_LEN, imageLenText.c_str());

        // 显示二维码图片
        CImage image;
        CRect rect;

        HWND picHwnd = GetDlgItem(g_dialog_hwnd_, IDC_STATIC_PIC);
        ::GetClientRect(picHwnd, &rect);
        image.Load(kSaveQRCodeImageName.c_str());
        image.Draw(GetDC(picHwnd), rect);

        break;
    }

    case WM_COMMAND: {
        switch (LOWORD(wParam)) {
        case ID_BTN_HOOK: {
            ::MessageBoxW(hDlg, L"Hook成功", L"提示", 0);
            const auto cb = [](const inject::QrCodeInfo & info) {
                g_cp_ = info;
                inject::saveQrCodeToImage(info.image, info.image_len, kSaveQRCodeImageName);
                ::SendMessage(g_dialog_hwnd_, WM_USER_SHOW_CODE, 0, 0);
            };
            inject::hookLoginQrCode(cb);
            break;
        }

        case ID_BTN_Unhook:
            inject::unHookLoginQrCode();
            ::MessageBoxW(hDlg, L"卸载Hook成功", L"提示", 0);
            break;

        default:
            break;
        }

        break;
    }
    }

    return (INT_PTR)FALSE;
}

