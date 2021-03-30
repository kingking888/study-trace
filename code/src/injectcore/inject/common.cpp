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

    std::string GetMsgByAddress2(dword_t address) {
        char* buf =  reinterpret_cast<char*>(address);
        return std::string(buf);
    }

    void setupHook(const dword_t hookOffset, dword_t jumFuncPtr, byte_t backupCode[], dword_t& retAddr) {
        dword_t hookAddr = getWeChatBaseAddr() + hookOffset;
        retAddr = hookAddr + kHookLen;

        byte_t jmpCode[kHookLen] = {};
        jmpCode[0] = 0xE9; // jmp

        *(reinterpret_cast<dword_t*>(&jmpCode[1])) = jumFuncPtr - hookAddr - kHookLen;

        HANDLE handle = ::OpenProcess(PROCESS_ALL_ACCESS, 0, GetCurrentProcessId());

        if (handle == 0) {
            Warnf("OpenProcess error");
            return;
        }

        int ret = ::ReadProcessMemory(handle, reinterpret_cast<void*>(hookAddr), backupCode, kHookLen, nullptr);

        if (ret == 0) {
            ::CloseHandle(handle);
            Warnf("ReadProcessMemory error");
            return;
        }

        ret = ::WriteProcessMemory(handle, reinterpret_cast<void*>(hookAddr), jmpCode, kHookLen, nullptr);

        if (ret == 0) {
            ::CloseHandle(handle);
            Warnf("WriteProcessMemory error");
            return;
        }

        Infof("setupHook success");
        ::CloseHandle(handle);
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