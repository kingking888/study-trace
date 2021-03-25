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
    * @brief ΢��ƫ��
    * @author fei.xu
    * @date 2021/3/19
    */
    enum class WeChatOffset : dword_t {
        QrCodeHookOffset = 0x5726CA, // ��ά��Hook��ַ
        //RefreshQrCode1 = 0x3FDA40,   // ˢ�¶�ά��Call��ַ1
        //RefreshQrCode2 = 0x3984B0,   // ˢ�¶�ά��Call��ַ2
        //LoginStatusChangedOffset = 0x47110F, // ��¼��Ϣ�ı�hookƫ��
        LoginFlagOffset = 0x18A39F0,   // ��½��־
    };

    /** @class QrCodeInfo
      * @brief ��ά����Ϣ
      * @author fei.xu
      * @date 2021/3/25
      */
    struct QrCodeInfo {
        dword_t addr;
        byte_t* image;
        int image_len;
    };

    /** @class ContactInfo
      * @brief ͨѶ����ϵ����Ϣ
      * @author fei.xu
      * @date 2021/3/25
      */
    struct ContactInfo {
        std::wstring wxid; // ΢��ID
        std::wstring name; // ����
        std::wstring nickName; // �ǳƣ���ע��
    };

    typedef std::function<void(const QrCodeInfo&)> LoginQrCodeCb;
    typedef std::function<void(std::vector<ContactInfo>&)> ContactListCb;

    dword_t getWeChatBaseAddr();
}
#endif//_INJECT_DEF_87B73D6D_C942_42E7_BDB5_7AB167FB67E7_