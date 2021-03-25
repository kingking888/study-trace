#ifndef _API_LOGIN_006F6D10_CD7F_486F_9B08_B405CFC59D45_
#define _API_LOGIN_006F6D10_CD7F_486F_9B08_B405CFC59D45_

#include "inject_api.h"

namespace inject {
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
}

#endif//_API_LOGIN_006F6D10_CD7F_486F_9B08_B405CFC59D45_