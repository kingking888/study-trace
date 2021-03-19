#include "pch.h"
#include "inject_api.h"

namespace inject {
    dword_t g_p_esi = 0;
    dword_t g_back_code[kHookLen] {};
    LoginQrCodeCb g_login_qr_code_cb = nullptr;
    // ���ص�ַ��֮ǰhook��ַ����5���ֽڵĵ�ַ
    DWORD returnAddr = getWeChatBaseAddr() + (dword_t)WeChatOffset::QrCodeHookOffset + kHookLen;

    void onLoginQrCodeCallback(dword_t pic) {
        // ��ȡͼƬ����
        DWORD picLenAddr = pic + 0x4;
        size_t imageLen = (size_t) * ((LPVOID*)picLenAddr);

        // ����ͼƬ������
        char data[0xFFF] {};
        memcpy(data, *((LPVOID*)pic), imageLen);

        if (g_login_qr_code_cb != nullptr) {
            g_login_qr_code_cb(data, imageLen);
        }

        char log[128] {};
        sprintf_s(log, 128, "inject get qr code, address:%X,len:%d", pic, imageLen);
        OutputDebugStringA(log);
    }

    // __declspec(naked)����������һ���㺯�������߱���������Ҫ�κ���������
    void __declspec(naked) onShowQrCode() {
        /* ԭʼд���������и�©������־λ�Ĵ���û�б��ݡ�
        // ����8���Ĵ���
        dword_t pEax;
        dword_t pEcx;
        dword_t pEdx;
        dword_t pEbx;
        dword_t pEsp;
        dword_t pEbp;
        dword_t pEsi;
        dword_t pEdi;
        __asm{
            mov pEax, eax
            mov pEcx, ecx
            mov pEdx, edx
            mov pEbx, ebx
            mov pEsp, esp
            mov pEbp, ebp
            mov pEsi, esi
            mov pEdi, edi
        }

        // ��ԭ8���Ĵ���
        __asm {
            mov eax, pEax
            mov ecx, pEcx
            mov edx, pEdx
            mov ebx, pEbx
            mov esp, pEsp
            mov ebp, pEbp
            mov esi, pEsi
            mov edi, pEdi
        }
        */

        OutputDebugStringA("[inject] onShowQrCode");

        // ���ݼĴ������д����
        // pushad����32λͨ�üĴ���
        // pushfd����32λ��־�Ĵ���
        __asm{
            pushad
            pushfd
        }

        // ȡ��esi���汣���ά��ĵ�ַ
        __asm mov g_p_esi, esi;
        onLoginQrCodeCallback(g_p_esi);

        __asm {
            popfd
            popad
        }

        // ��ת��hookǰcall���õ���һ����ַ
        __asm jmp returnAddr;
    }

    void hookLoginQrCode(const LoginQrCodeCb& cb) {
        dword_t baseAddr = getWeChatBaseAddr(); // ΢�Ż�ַ
        dword_t hookAddr = baseAddr + (dword_t)WeChatOffset::QrCodeHookOffset;
        dword_t jmpCode[kHookLen] {};

        // E9��jmpָ��
        jmpCode[0] = 0xE9;
        // jmp 0x11 11 11 11 ��4�ֽڵ�ַ
        *(DWORD*)(&jmpCode[1]) = (dword_t)onShowQrCode - hookAddr - 5;

        char temp[128] {};
        sprintf_s(temp, 128, "[inject] wechat base address: %d, hook addr:%d,%d", baseAddr, hookAddr, (dword_t)onShowQrCode);
        OutputDebugStringA(temp);

        // jmpCode = E9 onShowQrCode��ַ
        // 1. &jmpCode[1]��ȡ��������1��ʼ�ĵ�ַ
        // 2. (dword_t*)�� ת����unsinage long����ָ�룬ָ���СΪ4�ֽ�
        // 3. *�������ã���ָ���ַָ��hook�ĵ�ַ���൱�ڶ�һ��������ֵ���и�ֵ
        *(dword_t*)(&jmpCode[1]) = (dword_t)onShowQrCode - hookAddr - kHookLen;

        HANDLE hwnd = ::OpenProcess(PROCESS_ALL_ACCESS, NULL, GetCurrentProcessId());

        // ����ԭ���Ĵ��룬�û�ж��hook��ʱ��ָ�����
        if (::ReadProcessMemory(hwnd, (LPVOID)hookAddr, g_back_code, kHookLen, NULL) == 0) {
            ::CloseHandle(hwnd);
            OutputDebugStringA("ReadProcessMemory error");
            return;
        }

        // д�����ǵĴ��룬Ҳ��5���ֽ�
        if (::WriteProcessMemory(hwnd, (LPVOID)hookAddr, jmpCode, kHookLen, NULL) == 0) {
            ::CloseHandle(hwnd);
            OutputDebugStringA("WriteProcessMemory error");
            return;
        }

        ::CloseHandle(hwnd);
    }

    void unHookLoginQrCode() {
        dword_t hookAddr = getWeChatBaseAddr() + (dword_t)WeChatOffset::QrCodeHookOffset;
        HANDLE hwnd = ::OpenProcess(PROCESS_ALL_ACCESS, NULL, GetCurrentProcessId());
        // д��ԭ���ı��ݴ���
        ::WriteProcessMemory(hwnd, (LPVOID)hookAddr, g_back_code, kHookLen, NULL);
        ::CloseHandle(hwnd);
    }

    dword_t getWeChatBaseAddr() {
        return (DWORD)GetModuleHandle(kWeChatDll.c_str());
    }
}