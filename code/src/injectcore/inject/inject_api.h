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

    struct QrCodeInfo {
        dword_t addr;
        byte_t* image;
        int image_len;
    };

    typedef std::function<void(const QrCodeInfo&)> LoginQrCodeCb;

    /** @fn HookLoginQrCode
      * @brief Hook��¼��ά��
      * @param cb����ά��ˢ�»ص�
      * @return
      */
    void hookLoginQrCode(const LoginQrCodeCb& cb);
    /** @fn saveQrCodeToImage
      * @brief �����ά������ΪpngͼƬ
      * @param image��ͼƬ����
      *        len��ͼƬ����
      *        saveFilePath������·�������������·��
      * @return
      */
    void saveQrCodeToImage(const byte_t* image, const int len, const std::wstring& saveFilePath) noexcept;
    /** @fn UnHookLoginQrCode
      * @brief ж�ص�¼��ά��Hook
      * @param
      * @return
      */
    void unHookLoginQrCode();
    /** @fn refreshQrCode
      * @brief ˢ�¶�ά��
      * @param
      * @return
      */
    //void refreshQrCode();

    /** @fn getLoginStatus
      * @brief ��ȡ΢���Ƿ��¼
      * @param
      * @return
      */
    bool getLoginStatus();

    // helper
    /** @fn getWeChatBaseAddr
      * @brief ��ȡģ���ַ
      * @param
      * @return
      */
    dword_t getWeChatBaseAddr();
}

#endif//_INJECT_API_897C5832_9A64_4687_9440_3E175EDAC0A7_