#ifndef _INJECT_DEF_87B73D6D_C942_42E7_BDB5_7AB167FB67E7_
#define _INJECT_DEF_87B73D6D_C942_42E7_BDB5_7AB167FB67E7_

#include <vector>
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

    /** @class QrCodeInfo
      * @brief 二维码信息
      * @author fei.xu
      * @date 2021/3/25
      */
    struct QrCodeInfo {
        dword_t addr;
        byte_t* image;
        int image_len;
    };

    /** @class ContactInfo
      * @brief 通讯里联系人信息
      * @author fei.xu
      * @date 2021/3/25
      */
    struct ContactInfo {
        std::wstring wxid; // 微信ID
        std::wstring name; // 名称
        std::wstring nickName; // 昵称（备注）
    };

    typedef std::function<void(const QrCodeInfo&)> LoginQrCodeCb;
    typedef std::function<void(std::vector<ContactInfo>&)> ContactListCb;

    dword_t getWeChatBaseAddr();
}
#endif//_INJECT_DEF_87B73D6D_C942_42E7_BDB5_7AB167FB67E7_