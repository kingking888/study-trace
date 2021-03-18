#ifndef _INJECT_TOOLS_4B2B684A_9CA6_4A9E_B7C0_9EC427D54FD7_
#define _INJECT_TOOLS_4B2B684A_9CA6_4A9E_B7C0_9EC427D54FD7_

#include <cstring>

namespace core {
    /** @fn ProcessNameFindPID
    * @brief 通过进程名获取进程ID
    * @param ProcessName：进程名字，带.exe
    * @return
    */
    DWORD ProcessNameFindPID(const char* processName);

    /** @fn StartProcess
      * @brief 启动进程
      * @param filePath：进程完整路径
      *        firstWindowName：判断成功的窗口名，微信为WeChatLoginWndForPC
      * @return
      */
    void StartProcess(std::wstring filePath, std::wstring firstWindowName = L"WeChatLoginWndForPC");

    /** @fn InjectDll
      * @brief 注入dll
      * @param exeName：进程名
      *		   targetDllFilePath：要注入动态库的路径
      *        outPid：返回成功注入的进程ID
      * @return
      */
    void InjectDll(const std::string& exeName, const std::wstring& targetDllFilePath, HANDLE& outPid);

    /** @fn UnloadDll
      * @brief 卸载DLL
      * @param exeName：要卸载动态库的目标进程
      *        targetDllFilename：要卸载动态库的名字
      * @return
      */
    void UnloadDll(const std::string& exeName, const std::wstring& targetDllFilePath);

    /** @fn CheckIsInject
      * @brief 检测DLL是否已经注入
      * @param dwProcessid：进程ID
      * @return
      */
    bool CheckIsInject(DWORD processid, const std::wstring& dllFilename);

}

#endif//_INJECT_TOOLS_4B2B684A_9CA6_4A9E_B7C0_9EC427D54FD7_