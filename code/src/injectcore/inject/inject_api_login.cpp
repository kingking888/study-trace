#include "pch.h"
#include "inject_api.h"
#include "log.hpp"

#include "base/file/file_path.h"

namespace inject {
    dword_t g_p_esi = 0;
    byte_t g_back_code[kHookLen] = {0};
    byte_t jmpCode[kHookLen] = { 0 };
    LoginQrCodeCb g_login_qr_code_cb = nullptr;

    const int kMaxQrCodeImageLen = 0xFFFF;

    // ���ص�ַ��֮ǰhook��ַ����5���ֽڵĵ�ַ
    dword_t returnAddr = getWeChatBaseAddr() + (dword_t)WeChatOffset::QrCodeHookOffset + kHookLen;

    void onLoginQrCodeCallback(dword_t pic) {
        // ��ȡͼƬ����
        DWORD picLenAddr = pic + 0x4;
        size_t imageLen = (size_t) * ((LPVOID*)picLenAddr);

        if (imageLen >= kMaxQrCodeImageLen) {
            Warnf("get qr code, address:%X,len:%d", pic, imageLen);
            return;
        }

        Infof("get qr code, address:%X,len:%d", pic, imageLen);

        // ����ͼƬ������
        char data[kMaxQrCodeImageLen] {};

        memcpy(data, *((LPVOID*)pic), imageLen);

        if (g_login_qr_code_cb != nullptr) {
            g_login_qr_code_cb(data, imageLen);
        }
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

        // E9��jmpָ��
        jmpCode[0] = 0xE9;
        // jmpCode = E9 onShowQrCode��ַ
        // 1. &jmpCode[1]��ȡ��������1��ʼ�ĵ�ַ
        // 2. (dword_t*)�� ת����unsinage long����ָ�룬ָ���СΪ4�ֽ�
        // 3. *�������ã���ָ���ַָ��hook�ĵ�ַ���൱�ڶ�һ��������ֵ���и�ֵ
        // jmp 0x11 11 11 11 ��4�ֽڵ�ַ
        *(dword_t*)(&jmpCode[1]) = (dword_t)onShowQrCode - hookAddr - kHookLen;

        Debugf("%x %x %x %x %x", jmpCode[0], jmpCode[1], jmpCode[2], jmpCode[3], jmpCode[4]);
        Debugf("wechat base address: %X, hook addr:%X,%X", baseAddr, hookAddr, (dword_t)onShowQrCode);

        HANDLE hwnd = ::OpenProcess(PROCESS_ALL_ACCESS, NULL, GetCurrentProcessId());

        // ����ԭ���Ĵ��룬�û�ж��hook��ʱ��ָ�����
        if (::ReadProcessMemory(hwnd, (LPVOID)hookAddr, g_back_code, kHookLen, NULL) == 0) {
            ::CloseHandle(hwnd);
            Warnf("ReadProcessMemory error");
            return;
        }

        // д�����ǵĴ��룬Ҳ��5���ֽ�
        if (::WriteProcessMemory(hwnd, (LPVOID)hookAddr, jmpCode, kHookLen, NULL) == 0) {
            ::CloseHandle(hwnd);
            Warnf("WriteProcessMemory error");
            return;
        }

        ::CloseHandle(hwnd);
    }

    //void saveQrCodeToImage(const byte_t* image, const int len, const std::wstring& saveFilePath) {
    //    wchar_t szTempPath[MAX_PATH] = { 0 };
    //    wchar_t szPicturePath[MAX_PATH] = { 0 };
    //    GetTempPathW(MAX_PATH, szTempPath);

    //    wsprintf(szPicturePath, L"%s%s", szTempPath, "qrcode.png");
    //    DeleteFileW(szTempPath);

    //    if (nbase::FilePathIsExist(saveFilePath)) {

    //    }

    //    //���ļ�д��TempĿ¼��
    //    HANDLE hFile = CreateFileW(szPicturePath, GENERIC_ALL, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    //    if (hFile == NULL) {
    //        LogWarn("CreateFileW err,path={}", nbase::UTF16ToUTF8(szPicturePath));
    //        return;
    //    }

    //    DWORD dwRead = 0;

    //    if (WriteFile(hFile, PicData, cpyLen, &dwRead, NULL) == 0) {
    //        LogWarn("WriteFile err,path={}", nbase::UTF16ToUTF8(szPicturePath));
    //        return;
    //    }

    //    CloseHandle(hFile);
    //    LogInfof("success save to %s", nbase::UTF16ToUTF8(szPicturePath));
    //}

    void unHookLoginQrCode() {
        dword_t hookAddr = getWeChatBaseAddr() + (dword_t)WeChatOffset::QrCodeHookOffset;
        HANDLE hwnd = ::OpenProcess(PROCESS_ALL_ACCESS, NULL, GetCurrentProcessId());
        // д��ԭ���ı��ݴ���
        ::WriteProcessMemory(hwnd, (LPVOID)hookAddr, g_back_code, kHookLen, NULL);
        ::CloseHandle(hwnd);
    }

    dword_t getWeChatBaseAddr() {
        return (dword_t)GetModuleHandle(kWeChatDll.c_str());
    }
}