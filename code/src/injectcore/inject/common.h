#ifndef _COMMON_F1F62149_4F25_40BB_8DEC_853C7B2CDDF1_
#define _COMMON_F1F62149_4F25_40BB_8DEC_853C7B2CDDF1_

#include "inject_api.h"

namespace inject {
    /** @fn getWeChatBaseAddr
    * @brief ��ȡ΢�Ż�ַ
    * @param
    * @return
    */
    dword_t getWeChatBaseAddr();

    /** @fn GetMsgByAddress
      * @brief ���ڴ��ж�ȡ�ַ���
      * @param address���ڴ��ַ
      * @return
      */
    std::wstring GetMsgByAddress(dword_t address);

    /** @fn GetMsgByAddress
      * @brief ���ڴ��ж�ȡ�ַ���
      * @param address���ڴ��ַ
      * @return
      */
    std::string GetMsgByAddress2(dword_t address);

    /** @fn setupHook
      * @brief ����Hook
      * @param hookOffset��Ҫhook��ƫ��
      * @param jumFuncPtr���滻���Լ�����ĺ�����ַ
      * @param backupCode�������ǵĴ��뱸��
      * @param retAddr�����ص�ַ
      * @return
      */
    void setupHook(const dword_t hookOffset, dword_t jumFuncPtr, byte_t backupCode[], dword_t& retAddr);
    /** @fn unHook
      * @brief ж��
      * @param offset��ƫ��
      *        backCode��ԭ���Ļ�����
      * @return
      */
    void unHook(dword_t offset, byte_t backCode[]);
}

#endif//_COMMON_F1F62149_4F25_40BB_8DEC_853C7B2CDDF1_