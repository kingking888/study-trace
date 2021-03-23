#ifndef _LOG_F771DCA4_8D04_4B28_8A27_589FE58A717D_
#define _LOG_F771DCA4_8D04_4B28_8A27_589FE58A717D_

#include "pch.h"
#include <cstdio>
#include <string>

namespace inject {
    const int kMaxLogBuffLen = 1024;
    const char* kModuleName = "inject";
    const wchar_t* kModuleNameW = L"inject";

    /** @fn Debug
      * @brief 输出调试日志
      * @param
      * @return
      */
    void Debugf(std::string format, ...) {
        char buf[kMaxLogBuffLen] {};
        int len = sprintf_s(buf, "[%s] [debug]", kModuleName);
        char* p = buf + len;

        va_list args;
        va_start(args, format);
        len = _vsnprintf(p, sizeof(buf), format.c_str(), args);
        p[len] = '\n';
        p[len + 1] = '\0';

        OutputDebugStringA(buf);
        va_end(args);
    }

    /** @fn Debug
      * @brief 输出调试日志
      * @param
      * @return
      */
    void Debugf(std::wstring format, ...) {
        wchar_t buf[kMaxLogBuffLen] {};
        int len = wsprintf(buf, L"[%s] [debug]", kModuleNameW);
        wchar_t* p = buf + (len);

        va_list args;
        va_start(args, format);
        len = _vswprintf(p, format.c_str(), args);
        p[len] = L'\n';
        p[len + 1] = L'\0';

        OutputDebugStringW(buf);
        va_end(args);
    }

    /** @fn Info
    * @brief 输出一般日志
    * @param
    * @return
    */
    void Infof(std::string format, ...) {
        char buf[kMaxLogBuffLen] {};
        int len = sprintf_s(buf, "[%s] [info]", kModuleName);
        char* p = buf + len;

        va_list args;
        va_start(args, format);
        len = _vsnprintf(p, sizeof(buf), format.c_str(), args);
        p[len] = '\n';
        p[len + 1] = '\0';

        OutputDebugStringA(buf);
        va_end(args);
    }

    /** @fn Info
    * @brief 输出一般日志
    * @param
    * @return
    */
    void Infof(std::wstring format, ...) {
        wchar_t buf[kMaxLogBuffLen] {};
        int len = wsprintf(buf, L"[%s] [info]", kModuleNameW);
        wchar_t* p = buf + len;

        va_list args;
        va_start(args, format);
        len = _vswprintf(p, format.c_str(), args);
        p[len] = '\n';
        p[len + 1] = '\0';

        OutputDebugStringW(buf);
        va_end(args);
    }

    /** @fn Warn
      * @brief 输出警告日志
      * @param
      * @return
      */
    void Warnf(std::string format, ...) {
        char buf[kMaxLogBuffLen] {};
        int len = sprintf_s(buf, "[%s] [warn]", kModuleName);
        char* p = buf + len;

        va_list args;
        va_start(args, format);
        len = _vsnprintf(p, sizeof(buf), format.c_str(), args);
        p[len] = '\n';
        p[len + 1] = '\0';

        OutputDebugStringA(buf);
        va_end(args);
    }

    /** @fn Warn
      * @brief 输出警告日志
      * @param
      * @return
      */
    void Warnf(std::wstring format, ...) {
        wchar_t buf[kMaxLogBuffLen] {};
        int len = wsprintf(buf, L"[%s] [warn]", kModuleNameW);
        wchar_t* p = buf + len;

        va_list args;
        va_start(args, format);
        len = _vswprintf(p, format.c_str(), args);
        p[len] = '\n';
        p[len + 1] = '\0';

        OutputDebugStringW(buf);
        va_end(args);
    }


#if 0
    void test() {
        Debugf("%d", 1);
        Debugf(L"你好 %d", 1);

        Infof("%d", 1);
        Infof(L"你好 %d", 1);

        Warnf("hello %d", 1);
        Warnf(L"你好 %d", 1);
        Warnf(L"你好 %d", 1);
    }
#endif
}

#endif//_LOG_F771DCA4_8D04_4B28_8A27_589FE58A717D_