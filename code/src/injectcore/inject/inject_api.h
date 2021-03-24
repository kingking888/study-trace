#ifndef _INJECT_API_897C5832_9A64_4687_9440_3E175EDAC0A7_
#define _INJECT_API_897C5832_9A64_4687_9440_3E175EDAC0A7_

#include "inject_dll.h"
#include <functional>
#include <windows.h>

const std::wstring kWeChatDll = L"WeChatWin.dll";
const int kHookLen = 5;

typedef unsigned long dword_t; // DWORD -> unsigned long
typedef unsigned char byte_t;  //

namespace inject {
    /** @class WeChatOffset
    * @brief 微信偏移
    * @author fei.xu
    * @date 2021/3/19
    */
    enum class WeChatOffset : dword_t {
        QrCodeHookOffset = 0x5726CA, // 二维码Hook地址
        //RefreshQrCode1 = 0x3FDA40,   // 刷新二维码Call地址1
        //RefreshQrCode2 = 0x3984B0,   // 刷新二维码Call地址2
        //LoginStatusChangedOffset = 0x47110F, // 登录信息改变hook偏移
        LoginFlagOffset = 0x18A39F0,   // 登陆标志
    };

    struct QrCodeInfo {
        dword_t addr;
        byte_t* image;
        int image_len;
    };

    typedef std::function<void(const QrCodeInfo&)> LoginQrCodeCb;

    /** @fn HookLoginQrCode
      * @brief Hook登录二维码
      * @param cb：二维码刷新回调
      * @return
      */
    void hookLoginQrCode(const LoginQrCodeCb& cb);
    /** @fn saveQrCodeToImage
      * @brief 保存二维码数据为png图片
      * @param image：图片数据
      *        len：图片长度
      *        saveFilePath：保存路径，可以是相对路径
      * @return
      */
    void saveQrCodeToImage(const byte_t* image, const int len, const std::wstring& saveFilePath) noexcept;
    /** @fn UnHookLoginQrCode
      * @brief 卸载登录二维码Hook
      * @param
      * @return
      */
    void unHookLoginQrCode();
    /** @fn refreshQrCode
      * @brief 刷新二维码
      * @param
      * @return
      */
    //void refreshQrCode();

    /** @fn getLoginStatus
      * @brief 获取微信是否登录
      * @param
      * @return
      */
    bool getLoginStatus();

    // helper
    /** @fn getWeChatBaseAddr
      * @brief 获取模块基址
      * @param
      * @return
      */
    dword_t getWeChatBaseAddr();
}

#endif//_INJECT_API_897C5832_9A64_4687_9440_3E175EDAC0A7_