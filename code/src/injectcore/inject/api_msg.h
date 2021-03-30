#ifndef _API_MSG_1B535061_342F_4F82_B054_A9D2A4E076C9_
#define _API_MSG_1B535061_342F_4F82_B054_A9D2A4E076C9_

#include "inject_api.h"

namespace inject {

    /** @fn hookRecvMsg
      * @brief 接收消息
      * @param
      * @return
      */
    void hookRecvMsg(const RecvMsgCb& cb);
    void unHookRecvMsg();
}

#endif//_API_MSG_1B535061_342F_4F82_B054_A9D2A4E076C9_