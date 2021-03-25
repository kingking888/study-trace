#include "pch.h"
#include "api_login.h"
#include "log.h"

#include "base/file/file_util.h"
#include "base/util/string_util.h"

namespace inject {
    dword_t g_p_esi = 0;
    byte_t g_back_code[kHookLen] = {0};
    byte_t jmpCode[kHookLen] = { 0 };
    LoginQrCodeCb g_login_qr_code_cb = nullptr;

    const int kMaxQrCodeImageLen = 0xFFFF;

    // 返回地址是之前hook地址往后5个字节的地址
    dword_t returnAddr = getWeChatBaseAddr() + (dword_t)WeChatOffset::QrCodeHookOffset + kHookLen;

    void onLoginQrCodeCallback(dword_t pic) {
        // 获取图片长度
        DWORD picLenAddr = pic + 0x4;
        size_t imageLen = (size_t) * ((LPVOID*)picLenAddr);

        if (imageLen >= kMaxQrCodeImageLen) {
            Warnf("get qr code, address:%X,len:%d", pic, imageLen);
            return;
        }

        Infof("get qr code, address:%X,len:%d", pic, imageLen);

        // 拷贝图片的数据
        byte_t data[kMaxQrCodeImageLen] {};

        memcpy(data, *((LPVOID*)pic), imageLen);

        if (g_login_qr_code_cb != nullptr) {
            QrCodeInfo info{};
            info.addr = pic;
            info.image = data;
            info.image_len = imageLen;
            g_login_qr_code_cb(info);
        }
    }

    // __declspec(naked)：声明这是一个裸函数，告诉编译器不需要任何其他操作
    void __declspec(naked) onShowQrCode() {
        /* 原始写法，这里有个漏洞，标志位寄存器没有备份。
        // 备份8个寄存器
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

        // 还原8个寄存器
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

        // 备份寄存器快捷写法：
        // pushad所有32位通用寄存器
        // pushfd所有32位标志寄存器
        __asm{
            pushad
            pushfd
        }

        // 取出esi里面保存二维码的地址
        __asm mov g_p_esi, esi;

        onLoginQrCodeCallback(g_p_esi);

        __asm {
            popfd
            popad
        }

        // 跳转到hook前call调用的下一个地址
        __asm jmp returnAddr;
    }

    void hookLoginQrCode(const LoginQrCodeCb& cb) {
        dword_t baseAddr = getWeChatBaseAddr(); // 微信基址
        dword_t hookAddr = baseAddr + (dword_t)WeChatOffset::QrCodeHookOffset;

        // E9：jmp指令
        jmpCode[0] = 0xE9;
        // jmpCode = E9 onShowQrCode地址
        // 1. &jmpCode[1]：取数组索引1开始的地址
        // 2. (dword_t*)： 转换成unsinage long类型指针，指针大小为4字节
        // 3. *：解引用，把指针地址指向hook的地址，相当于对一个整形数值进行赋值
        // jmp 0x11 11 11 11 ，4字节地址
        *(dword_t*)(&jmpCode[1]) = (dword_t)onShowQrCode - hookAddr - kHookLen;

        Debugf("%x %x %x %x %x", jmpCode[0], jmpCode[1], jmpCode[2], jmpCode[3], jmpCode[4]);
        Debugf("wechat base address: %X, hook addr:%X,%X", baseAddr, hookAddr, (dword_t)onShowQrCode);

        HANDLE hwnd = ::OpenProcess(PROCESS_ALL_ACCESS, NULL, GetCurrentProcessId());

        // 保存原来的代码，用户卸载hook的时候恢复代码
        if (::ReadProcessMemory(hwnd, (LPVOID)hookAddr, g_back_code, kHookLen, NULL) == 0) {
            ::CloseHandle(hwnd);
            Warnf("ReadProcessMemory error");
            return;
        }

        // 写入我们的代码，也是5个字节
        if (::WriteProcessMemory(hwnd, (LPVOID)hookAddr, jmpCode, kHookLen, NULL) == 0) {
            ::CloseHandle(hwnd);
            Warnf("WriteProcessMemory error");
            return;
        }

        ::CloseHandle(hwnd);

        g_login_qr_code_cb = cb;
    }

    void saveQrCodeToImage(const byte_t* image, const int len, const std::wstring& saveFilePath) noexcept {
        if (nbase::FilePathIsExist(saveFilePath, false)) {
            Warnf("image file is exist,delete");
            nbase::DeleteFileW(saveFilePath);
        }

        // create root dir
        auto pos = saveFilePath.find_last_of(L"\\");

        if (pos != std::wstring::npos) {
            std::wstring dir = saveFilePath.substr(0, pos);

            if (!nbase::FilePathIsExist(dir, true)) {
                nbase::CreateDirectoryW(dir);
            }
        }

        // write to file
        HANDLE fileHandle = CreateFileW(saveFilePath.c_str(), GENERIC_ALL, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

        if (fileHandle == NULL) {
            Warnf("CreateFileW err,path={}", nbase::UTF16ToUTF8(saveFilePath));
            return;
        }

        dword_t dwRead = 0;

        if (WriteFile(fileHandle, image, len, &dwRead, NULL) == 0) {
            Warnf("WriteFile err,path={}", nbase::UTF16ToUTF8(saveFilePath));
            return;
        }

        CloseHandle(fileHandle);
        Debugf("success save to %s", nbase::UTF16ToUTF8(saveFilePath));
    }

    void unHookLoginQrCode() {
        dword_t hookAddr = getWeChatBaseAddr() + (dword_t)WeChatOffset::QrCodeHookOffset;
        HANDLE hwnd = ::OpenProcess(PROCESS_ALL_ACCESS, NULL, GetCurrentProcessId());
        // 写入原来的备份代码
        ::WriteProcessMemory(hwnd, (LPVOID)hookAddr, g_back_code, kHookLen, NULL);
        ::CloseHandle(hwnd);

        Debugf("unHookLoginQrCode");
    }

    void refreshQrCode() {
        //Infof("refreshQrCode %X", refreshQrCode);

        //dword_t baseAddr = getWeChatBaseAddr();
        //dword_t callAddr1 = baseAddr + (dword_t)WeChatOffset::RefreshQrCode1;
        ////dword_t callAddr2 = baseAddr + (dword_t)WeChatOffset::RefreshQrCode1;

        //__asm {
        //    call callAddr1;
        //    //mov ecx, eax;
        //    //call callAddr2;
        //}
    }

    bool getLoginStatus() {
        dword_t ptr = getWeChatBaseAddr() + (dword_t)WeChatOffset::LoginFlagOffset;
        return *reinterpret_cast<dword_t*>(ptr);
    }

    dword_t getWeChatBaseAddr() {
        return (dword_t)GetModuleHandle(kWeChatDll.c_str());
    }
}