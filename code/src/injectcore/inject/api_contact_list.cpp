#include "pch.h"
#include "api_contact_list.h"
#include "common.h"
#include "log.h"

#include "base/util/string_util.h"

namespace inject {
    byte_t g_contact_back_code_[kHookLen] = {};
    dword_t g_contact_over_write_call_addr_ = 0;
    // hookͨѶ¼�б����صĵ�ַ
    dword_t g_contact_return_addr_ = 0;

    dword_t g_contact_ecx_ = 0;
    ContactListCb g_contact_list_cb_ = nullptr;
    std::vector<ContactInfo> g_contact_list_ = {};

    void onAddContactInfo(dword_t ecx) {
        dword_t id = ecx + (dword_t)WeChatOffset::ContactListContentWeChatIdOffset;
        dword_t userName = ecx + (dword_t)WeChatOffset::ContactListContentWeChatUserNameOffset;
        dword_t nickName = ecx + (dword_t)WeChatOffset::ContactListContentWeChatNickNameOffset;
        dword_t name = ecx + (dword_t)WeChatOffset::ContactListContentWeChatNameOffset;

        std::wstring idStr = GetMsgByAddress(id);
        std::wstring userNameStr = GetMsgByAddress(userName);
        std::wstring nickNameStr = GetMsgByAddress(nickName);
        std::wstring nameStr = GetMsgByAddress(name);

        /*ContactInfo g{ idStr, userNameStr, nickNameStr, nameStr };
        g_contact_list_.push_back(std::move(g));*/

        /*Infof(L"onQueryPersonInfoCb id=%s,userName=%s,nickName=%s,name=%s",
              nbase::UTF16ToUTF8(idStr),
              nbase::UTF16ToUTF8(userNameStr),
              nbase::UTF16ToUTF8(nickNameStr),
              nbase::UTF16ToUTF8(nameStr));*/
        Infof(L"id=" + idStr + L",name=" + userNameStr + L",nick=" + nickNameStr + L",name=" + nameStr);
    }

    void __declspec(naked) onQueryPersonInfoCb() {
        __asm mov g_contact_ecx_, ecx;

        // �������мĴ���
        __asm {
            pushad
            pushfd
        }

        onAddContactInfo(g_contact_ecx_);

        /* if (g_contact_list_cb_ != nullptr) {
             g_contact_list_cb_()
         }*/

        // �ָ��ֳ�
        __asm {
            popfd
            popad
        }

        // ���䱻���ǵĴ���
        __asm call g_contact_over_write_call_addr_;

        // ���ر�HOOKָ�����һ��ָ��
        __asm jmp g_contact_return_addr_;
    }

    void hookContactList(const ContactListCb& cb, bool autoUnhook) {
        dword_t baseAdd = getWeChatBaseAddr();
        dword_t hookAddr = baseAdd + (dword_t)WeChatOffset::ContactListHookOffset;
        g_contact_return_addr_ = hookAddr + kHookLen;

        // ԭ����call����һ��
        g_contact_over_write_call_addr_ = baseAdd + (dword_t)WeChatOffset::ContactListOriginCallOffset;

        byte_t jmpCode[kHookLen] = {0};
        jmpCode[0] = 0xE9; // jmp

        *(reinterpret_cast<dword_t*>(&jmpCode[1])) = reinterpret_cast<dword_t>(onQueryPersonInfoCb) - hookAddr - kHookLen;

        Debugf("%x %x %x %x %x", jmpCode[0], jmpCode[1], jmpCode[2], jmpCode[3], jmpCode[4]);
        Debugf("wechat base address: %X, hook addr:%X,%X", baseAdd, hookAddr, (dword_t)onQueryPersonInfoCb);

        HANDLE handle = ::OpenProcess(PROCESS_ALL_ACCESS, 0, GetCurrentProcessId());

        if (handle == nullptr) {
            Warnf("OpenProcess error");
            return;
        }

        // ����ԭ���Ĵ���
        if (::ReadProcessMemory(handle, reinterpret_cast<void*>(hookAddr), g_contact_back_code_, kHookLen, nullptr) == 0) {
            ::CloseHandle(handle);
            Warnf("ReadProcessMemory error");
            return;
        }

        // д�����ǵĴ���
        if (::WriteProcessMemory(handle, reinterpret_cast<void*>(hookAddr), jmpCode, kHookLen, nullptr) == 0) {
            ::CloseHandle(handle);
            Warnf("WriteProcessMemory error");
            return;
        }

        ::CloseHandle(handle);
        g_contact_list_cb_ = cb;
    }

    void unHookContactList() {
        Debugf("unHookContactList");
        unHook((dword_t)WeChatOffset::ContactListHookOffset, g_contact_back_code_);
    }
}