#include "pch.h"
#include "api_msg.h"

#include "base/util/string_util.h"

namespace inject {
    dword_t g_recv_origin_call_code_ = 0;
    dword_t g_recv_return_addr_ = 0;
    dword_t g_recv_msg_ecx_ = 0;
    byte_t g_recv_msg_back_code_[kHookLen] = {};

    void onRecvMsgProc(dword_t ecx) {
        Infof("onRecvMsgProc ");
        std::wstring from = GetMsgByAddress(ecx + (dword_t)inject::WeChatOffset::RecvMsgStructSenderOffset);
        std::wstring msgContent = GetMsgByAddress(ecx + (dword_t)inject::WeChatOffset::RecvMsgStructContentOffset);

        if (!msgContent.empty() && !from.empty()) {
            Infof(L"onRecvMsgProc " + from + L":" + msgContent);

        } else {
            Warnf("onRecvMsgProc from or msgContent is empty");
        }
    }

    void __declspec(naked) onRecvMsg() {
        // 调用被覆盖的代码（不能在这里调用，会把ecx清空）
        //__asm call g_recv_origin_call_code_;

        // 保存寄存器
        __asm {
            pushad
            pushfd
        }

        __asm mov g_recv_msg_ecx_, ecx;
        onRecvMsgProc(g_recv_msg_ecx_);

        __asm{
            popfd
            popad

            // 调用被覆盖的代码
            __asm call g_recv_origin_call_code_;
            // 返回到原来的地址
            jmp g_recv_return_addr_
        }
    }

    void hookRecvMsg(const RecvMsgCb& cb) {
        g_recv_origin_call_code_ = getWeChatBaseAddr() + (dword_t)inject::WeChatOffset::RecvMsgOriginCallOffset;

        setupHook((dword_t)inject::WeChatOffset::RecvMsgHookOffset, (dword_t)onRecvMsg, g_recv_msg_back_code_, g_recv_return_addr_);
        Infof("hookRecvMsg success");
    }
    void unHookRecvMsg() {
        Infof("unHookRecvMsg success");
        unHook((dword_t)inject::WeChatOffset::RecvMsgHookOffset, g_recv_msg_back_code_);
    }
}