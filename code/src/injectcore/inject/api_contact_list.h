#ifndef _INJECT_API_CONTACT_LIST_9DF88A07_E81A_478A_A609_5B1DFE2E9274_
#define _INJECT_API_CONTACT_LIST_9DF88A07_E81A_478A_A609_5B1DFE2E9274_

#include "inject/inject_api.h"

namespace inject {
    /** @fn hookContactList
    * @brief 挂载通讯录列表hook，只能在登录前
    * @param cb：回调
    *        autoUnhook：是否自动卸载hook，因为会多次被调用，以防脏数据
    * @return
    */
    void hookContactList(const ContactListCb& cb, bool autoUnhook = true);
    /** @fn unHookContactList
      * @brief 卸载通讯录列表hook
      * @param
      * @return
      */
    void unHookContactList();
}

#endif//_INJECT_API_CONTACT_LIST_9DF88A07_E81A_478A_A609_5B1DFE2E9274_