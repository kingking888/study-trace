#include "pch.h"
#include "common.h"
#include "log.h"

namespace inject {
    dword_t getWeChatBaseAddr() {
        return (dword_t)GetModuleHandle(kWeChatDll.c_str());
    }

    std::wstring GetMsgByAddress(dword_t address) {
        std::wstring tmp;
        dword_t msgLength = *(dword_t*)(address + 4);

        if (msgLength > 0) {
            wchar_t* msg = new wchar_t[msgLength + 1] { 0 };
            wmemcpy_s(msg, msgLength + 1, (wchar_t*)(*(dword_t*)address), msgLength + 1);
            tmp = msg;
            delete[]msg;
        }

        return  tmp;
    }

    void unHook(dword_t offset, byte_t backCode[]) {
        dword_t hookAddr = getWeChatBaseAddr() + offset;
        HANDLE hwnd = ::OpenProcess(PROCESS_ALL_ACCESS, NULL, GetCurrentProcessId());

        // 写入原来的备份代码
        if (::WriteProcessMemory(hwnd, (LPVOID)hookAddr, backCode, kHookLen, NULL) == 0) {
            Warnf("WriteProcessMemory error");
        }

        ::CloseHandle(hwnd);
    }
}