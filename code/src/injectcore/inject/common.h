#ifndef _COMMON_F1F62149_4F25_40BB_8DEC_853C7B2CDDF1_
#define _COMMON_F1F62149_4F25_40BB_8DEC_853C7B2CDDF1_

#include "inject_api.h"

namespace inject {
    /** @fn getWeChatBaseAddr
    * @brief 获取微信基址
    * @param
    * @return
    */
    dword_t getWeChatBaseAddr();

    /** @fn GetMsgByAddress
      * @brief 从内存中读取字符串
      * @param address：内存地址
      * @return
      */
    std::wstring GetMsgByAddress(dword_t address);

    /** @fn GetMsgByAddress
      * @brief 从内存中读取字符串
      * @param address：内存地址
      * @return
      */
    std::string GetMsgByAddress2(dword_t address);

    /** @fn unHook
      * @brief 卸载
      * @param offset：偏移
      *        backCode：原来的机器码
      * @return
      */
    void unHook(dword_t offset, byte_t backCode[]);
}

#endif//_COMMON_F1F62149_4F25_40BB_8DEC_853C7B2CDDF1_