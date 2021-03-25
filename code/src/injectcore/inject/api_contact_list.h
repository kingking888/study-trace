#ifndef _INJECT_API_CONTACT_LIST_9DF88A07_E81A_478A_A609_5B1DFE2E9274_
#define _INJECT_API_CONTACT_LIST_9DF88A07_E81A_478A_A609_5B1DFE2E9274_

#include "inject/inject_api.h"

namespace inject {
    /** @fn hookContactList
    * @brief ����ͨѶ¼�б�hook��ֻ���ڵ�¼ǰ
    * @param cb���ص�
    *        autoUnhook���Ƿ��Զ�ж��hook����Ϊ���α����ã��Է�������
    * @return
    */
    void hookContactList(const ContactListCb& cb, bool autoUnhook = true);
    /** @fn unHookContactList
      * @brief ж��ͨѶ¼�б�hook
      * @param
      * @return
      */
    void unHookContactList();
}

#endif//_INJECT_API_CONTACT_LIST_9DF88A07_E81A_478A_A609_5B1DFE2E9274_