#ifndef _API_LOGIN_006F6D10_CD7F_486F_9B08_B405CFC59D45_
#define _API_LOGIN_006F6D10_CD7F_486F_9B08_B405CFC59D45_

#include "inject_api.h"

namespace inject {
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
}

#endif//_API_LOGIN_006F6D10_CD7F_486F_9B08_B405CFC59D45_