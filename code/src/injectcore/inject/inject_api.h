#ifndef _INJECT_API_897C5832_9A64_4687_9440_3E175EDAC0A7_
#define _INJECT_API_897C5832_9A64_4687_9440_3E175EDAC0A7_

#include "inject_dll.h"
#include <functional>
#include <windows.h>

const std::wstring kWeChatDll = L"WeChatWin.dll";
const int kHookLen = 5;

namespace inject {
    typedef unsigned long dword_t; // DWORD -> unsigned long

    typedef std::function<void(const char* image, int len)> LoginQrCodeCb;

    /** @class WeChatOffset
      * @brief 微信偏移
      * @author fei.xu
      * @date 2021/3/19
      */
    enum class WeChatOffset : dword_t {
        QrCodeHookOffset = 0x572459, // 二维码Hook地址
    };

    /** @fn HookLoginQrCode
      * @brief Hook登录二维码
      * @param cb：二维码刷新回调
      * @return
      */
    void hookLoginQrCode(const LoginQrCodeCb& cb);
    /** @fn UnHookLoginQrCode
      * @brief 卸载登录二维码Hook
      * @param
      * @return
      */
    void unHookLoginQrCode();

    // helper
    /** @fn getWeChatBaseAddr
      * @brief 获取模块基址
      * @param
      * @return
      */
    dword_t getWeChatBaseAddr();
}

#endif//_INJECT_API_897C5832_9A64_4687_9440_3E175EDAC0A7_