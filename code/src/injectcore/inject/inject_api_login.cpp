#include "pch.h"
#include "inject_api.h"

namespace inject {
    dword_t g_p_esi = 0;
    dword_t g_back_code[kHookLen] {};
    LoginQrCodeCb g_login_qr_code_cb = nullptr;
    // 返回地址是之前hook地址往后5个字节的地址
    DWORD returnAddr = getWeChatBaseAddr() + (dword_t)WeChatOffset::QrCodeHookOffset + kHookLen;

    void onLoginQrCodeCallback(dword_t pic) {
        // 获取图片长度
        DWORD picLenAddr = pic + 0x4;
        size_t imageLen = (size_t) * ((LPVOID*)picLenAddr);

        // 拷贝图片的数据
        char data[0xFFF] {};
        memcpy(data, *((LPVOID*)pic), imageLen);

        if (g_login_qr_code_cb != nullptr) {
            g_login_qr_code_cb(data, imageLen);
        }

        char log[128] {};
        sprintf_s(log, 128, "inject get qr code, address:%X,len:%d", pic, imageLen);
        OutputDebugStringA(log);
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

        OutputDebugStringA("[inject] onShowQrCode");

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
        dword_t jmpCode[kHookLen] {};

        // E9：jmp指令
        jmpCode[0] = 0xE9;
        // jmp 0x11 11 11 11 ，4字节地址
        *(DWORD*)(&jmpCode[1]) = (dword_t)onShowQrCode - hookAddr - 5;

        char temp[128] {};
        sprintf_s(temp, 128, "[inject] wechat base address: %d, hook addr:%d,%d", baseAddr, hookAddr, (dword_t)onShowQrCode);
        OutputDebugStringA(temp);

        // jmpCode = E9 onShowQrCode地址
        // 1. &jmpCode[1]：取数组索引1开始的地址
        // 2. (dword_t*)： 转换成unsinage long类型指针，指针大小为4字节
        // 3. *：解引用，把指针地址指向hook的地址，相当于对一个整形数值进行赋值
        *(dword_t*)(&jmpCode[1]) = (dword_t)onShowQrCode - hookAddr - kHookLen;

        HANDLE hwnd = ::OpenProcess(PROCESS_ALL_ACCESS, NULL, GetCurrentProcessId());

        // 保存原来的代码，用户卸载hook的时候恢复代码
        if (::ReadProcessMemory(hwnd, (LPVOID)hookAddr, g_back_code, kHookLen, NULL) == 0) {
            ::CloseHandle(hwnd);
            OutputDebugStringA("ReadProcessMemory error");
            return;
        }

        // 写入我们的代码，也是5个字节
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
        // 写入原来的备份代码
        ::WriteProcessMemory(hwnd, (LPVOID)hookAddr, g_back_code, kHookLen, NULL);
        ::CloseHandle(hwnd);
    }

    dword_t getWeChatBaseAddr() {
        return (DWORD)GetModuleHandle(kWeChatDll.c_str());
    }
}