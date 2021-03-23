#ifndef _LOG_F771DCA4_8D04_4B28_8A27_589FE58A717D_
#define _LOG_F771DCA4_8D04_4B28_8A27_589FE58A717D_

#include "pch.h"
#include <cstdio>
#include <string>

namespace inject {
    /** @fn Debug
      * @brief ���������־
      * @param
      * @return
      */
    void Debugf(std::string format, ...);

    /** @fn Debug
      * @brief ���������־
      * @param
      * @return
      */
    void Debugf(std::wstring format, ...);

    /** @fn Info
    * @brief ���һ����־
    * @param
    * @return
    */
    void Infof(std::string format, ...);

    /** @fn Info
    * @brief ���һ����־
    * @param
    * @return
    */
    void Infof(std::wstring format, ...);

    /** @fn Warn
      * @brief ���������־
      * @param
      * @return
      */
    void Warnf(std::string format, ...);

    /** @fn Warn
      * @brief ���������־
      * @param
      * @return
      */
    void Warnf(std::wstring format, ...);
}

#endif//_LOG_F771DCA4_8D04_4B28_8A27_589FE58A717D_