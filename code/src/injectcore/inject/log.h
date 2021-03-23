#ifndef _LOG_F771DCA4_8D04_4B28_8A27_589FE58A717D_
#define _LOG_F771DCA4_8D04_4B28_8A27_589FE58A717D_

#include "pch.h"
#include <cstdio>
#include <string>

namespace inject {
    /** @fn Debug
      * @brief 输出调试日志
      * @param
      * @return
      */
    void Debugf(std::string format, ...);

    /** @fn Debug
      * @brief 输出调试日志
      * @param
      * @return
      */
    void Debugf(std::wstring format, ...);

    /** @fn Info
    * @brief 输出一般日志
    * @param
    * @return
    */
    void Infof(std::string format, ...);

    /** @fn Info
    * @brief 输出一般日志
    * @param
    * @return
    */
    void Infof(std::wstring format, ...);

    /** @fn Warn
      * @brief 输出警告日志
      * @param
      * @return
      */
    void Warnf(std::string format, ...);

    /** @fn Warn
      * @brief 输出警告日志
      * @param
      * @return
      */
    void Warnf(std::wstring format, ...);
}

#endif//_LOG_F771DCA4_8D04_4B28_8A27_589FE58A717D_