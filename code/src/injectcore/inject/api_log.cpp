#include "pch.h"
#include "api_log.h"
#include "inject_api.h"

namespace inject {
    dword_t g_wechat_log_origin_call_ = 0;
    dword_t g_wechat_log_ret_addr_ = 0;
    dword_t g_wechat_log_eax_ = 0;
    byte_t g_wechat_log_back_code_[kHookLen] = {};

    void printWeChatLog(dword_t eax) {
        if (eax == 0) {
            return;
        }

        std::string content = GetMsgByAddress2(eax);
        Infof(content);
    }

    void __declspec(naked) onWeChatAddLog() {
        // ���䱻���ǵĴ���
        __asm call g_wechat_log_origin_call_;

        // �������мĴ���
        __asm {
            pushad
            pushfd
        }

        __asm mov g_wechat_log_eax_, eax;
        printWeChatLog(g_wechat_log_eax_);

        // �ָ��Ĵ���
        __asm {
            popfd
            popad
        }

        // ���ر�HOOKָ�����һ��ָ��
        __asm   jmp g_wechat_log_ret_addr_;
    }

    void hookWeChatLog() {
        dword_t baseAddr = getWeChatBaseAddr();
        dword_t hookAddr = baseAddr + static_cast<dword_t>(inject::WeChatOffset::LogHookOffset);

        g_wechat_log_ret_addr_ = hookAddr + kHookLen;
        g_wechat_log_origin_call_ = baseAddr + static_cast<dword_t>(inject::WeChatOffset::LogHookOriginCallOffset);

        byte_t jmpCode[kHookLen] = {};
        jmpCode[0] = 0xE9; // jmp

        *(reinterpret_cast<dword_t*>(&jmpCode[1])) = (dword_t)onWeChatAddLog - hookAddr - kHookLen;

        HANDLE handle = ::OpenProcess(PROCESS_ALL_ACCESS, 0, GetCurrentProcessId());

        if (handle == 0) {
            Warnf("OpenProcess error");
            return;
        }

        int ret = ::ReadProcessMemory(handle, reinterpret_cast<void*>(hookAddr), g_wechat_log_back_code_, kHookLen, nullptr);

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

        Infof("hookWeChatLog success");

        ::CloseHandle(handle);
    }

    void unhookWeChatLog() {
        dword_t baseAddr = getWeChatBaseAddr();
        dword_t hookAddr = baseAddr + static_cast<dword_t>(inject::WeChatOffset::LogHookOffset);

        HANDLE handle = ::OpenProcess(PROCESS_ALL_ACCESS, 0, GetCurrentProcessId());

        if (handle == 0) {
            Warnf("OpenProcess error");
            return;
        }

        int ret = ::WriteProcessMemory(handle, reinterpret_cast<void*>(hookAddr), g_wechat_log_back_code_, kHookLen, nullptr);

        if (ret == 0) {
            ::CloseHandle(handle);
            Warnf("WriteProcessMemory error");
            return;
        }

        Infof("unhookWeChatLog success");
        ::CloseHandle(handle);
    }
}

