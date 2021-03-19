#include "stdafx.h"
#include "inject_tools.h"
#include <direct.h>
#include <stdlib.h>
#include <TlHelp32.h>
#include <stdio.h>
#include <atlstr.h>

#pragma comment(lib,"advapi32")

#if 0
CString GetAppRegeditPath(CString strAppName) {
    //定义相关变量
    HKEY hKey;
    CString strAppRegeditPath("");
    TCHAR szProductType[MAX_PATH];
    memset(szProductType, 0, sizeof(szProductType));

    DWORD dwBuflen = MAX_PATH;
    LONG lRet = 0;

    //下面是打开注册表,只有打开后才能做其他操作
    lRet = RegOpenKeyEx(HKEY_CURRENT_USER, //要打开的根键
                        LPCTSTR(strAppName), //要打开的子子键
                        0, //这个一定为0
                        KEY_QUERY_VALUE, //指定打开方式,此为读
                        &hKey); //用来返回句柄

    if (lRet != ERROR_SUCCESS) { //判断是否打开成功
        return strAppRegeditPath;

    } else {
        //下面开始查询
        lRet = RegQueryValueEx(hKey, //打开注册表时返回的句柄
                               TEXT("Wechat"), //要查询的名称,查询的软件安装目录在这里
                               NULL, //一定为NULL或者0
                               NULL,
                               (LPBYTE)szProductType, //我们要的东西放在这里
                               &dwBuflen);

        if (lRet != ERROR_SUCCESS) { //判断是否查询成功
            return strAppRegeditPath;

        } else {
            RegCloseKey(hKey);

            strAppRegeditPath = szProductType;

            int nPos = strAppRegeditPath.Find('-');

            if (nPos >= 0) {
                CString sSubStr = strAppRegeditPath.Left(nPos - 1);//包含$,不想包含时nPos+1
                strAppRegeditPath = sSubStr;
            }
        }
    }

    return strAppRegeditPath;
}
CString GetAppRegeditPath2(CString strAppName) {
    //定义相关变量
    HKEY hKey;
    CString strAppRegeditPath("");
    TCHAR szProductType[MAX_PATH];
    memset(szProductType, 0, sizeof(szProductType));

    DWORD dwBuflen = MAX_PATH;
    LONG lRet = 0;

    //下面是打开注册表,只有打开后才能做其他操作
    lRet = RegOpenKeyEx(HKEY_CURRENT_USER, //要打开的根键
                        LPCTSTR(strAppName), //要打开的子子键
                        0, //这个一定为0
                        KEY_QUERY_VALUE, //指定打开方式,此为读
                        &hKey); //用来返回句柄

    if (lRet != ERROR_SUCCESS) { //判断是否打开成功
        return strAppRegeditPath;

    } else {
        //下面开始查询
        lRet = RegQueryValueEx(hKey, //打开注册表时返回的句柄
                               TEXT("InstallPath"), //要查询的名称,查询的软件安装目录在这里
                               NULL, //一定为NULL或者0
                               NULL,
                               (LPBYTE)szProductType, //我们要的东西放在这里
                               &dwBuflen);

        if (lRet != ERROR_SUCCESS) { //判断是否查询成功
            return strAppRegeditPath;

        } else {
            RegCloseKey(hKey);
            strAppRegeditPath = szProductType;

        }
    }

    return strAppRegeditPath;
}
#endif

namespace core {

    DWORD ProcessNameFindPID(const char* ProcessName) noexcept {
        PROCESSENTRY32 pe32 = { 0 };
        pe32.dwSize = sizeof(PROCESSENTRY32);
        HANDLE hProcess = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

        if (Process32First(hProcess, &pe32) == TRUE) {
            do {
                USES_CONVERSION;

                if (strcmp(ProcessName, W2A(pe32.szExeFile)) == 0) {
                    return pe32.th32ProcessID;
                }
            } while (Process32Next(hProcess, &pe32));
        }

        return 0;
    }

    void StartProcess(std::wstring filePath, std::wstring firstWindowName) {
        if (!nbase::FilePathIsExist(filePath, false)) {
            throw std::exception("exe is not exist.");
        }

        STARTUPINFO si;
        PROCESS_INFORMATION pi;
        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        ZeroMemory(&pi, sizeof(pi));

        si.dwFlags = STARTF_USESHOWWINDOW; // 指定wShowWindow成员有效
        si.wShowWindow = TRUE;             // 此成员设为TRUE的话则显示新建进程的主窗口，为FALSE的话则不显示

        if (!CreateProcess(filePath.c_str(), NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
            throw std::exception("create wechat process error, regedit may be changed!");
        }

        if (!firstWindowName.empty()) {
            HWND  hwnd = nullptr;
            int i = 0;

            while (nullptr == hwnd) {
                hwnd = FindWindow(firstWindowName.c_str(), NULL);
                ++i;

                if (i >= 3)
                    break;

                Sleep(500);
            }

            if (nullptr == hwnd) {
                throw std::exception("not find the window,please check it");
            }
        }

        /*dwPid = pi.dwProcessId;
        wxPid = pi.hProcess;*/
    }

    void InjectDll(const std::string& exeName, const std::wstring& targetDllFilePath) {
        //获取微信Pid
        DWORD dwPid = ProcessNameFindPID(exeName.c_str());

        if (dwPid == 0) {
            throw std::exception("not find process.");
        }

        std::wstring fileName = getFileName(targetDllFilePath);

        //检测dll是否已经注入
        if (!CheckIsInject(dwPid, targetDllFilePath)) {
            //打开进程
            HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPid);

            if (hProcess == nullptr) {
                throw std::exception(nbase::UTF16ToUTF8(L"进程打开失败").c_str());
            }

            // 在微信进程中申请内存
            // 1参数进程句柄，2分配空间位置为空就随机，3一个路径地址,4权限，5可读可写
            LPVOID pAddress = VirtualAllocEx(hProcess, NULL, MAX_PATH, MEM_COMMIT, PAGE_EXECUTE_READWRITE);

            if (pAddress == nullptr) {
                throw std::exception(nbase::UTF16ToUTF8(L"内存分配失败").c_str());
            }

            //写入dll路径到微信进程
            const auto path = nbase::UTF16ToUTF8(targetDllFilePath);

            if (WriteProcessMemory(hProcess, pAddress, path.c_str(), path.length(), NULL) == 0) {
                throw std::exception(nbase::UTF16ToUTF8(L"路径写入失败").c_str());
            }

            //获取LoadLibraryA函数地址
            FARPROC pLoadLibraryAddress = GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA");

            if (pLoadLibraryAddress == NULL) {
                throw std::exception(nbase::UTF16ToUTF8(L"获取LoadLibraryA函数地址失败").c_str());
            }

            // 注入成功后开线程操作
            // CreateRemoteThread第四个参数需要拿自己的地址用 GetModuleHandle 获取Kernel32基址
            HANDLE hRemoteThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)pLoadLibraryAddress, pAddress, 0, NULL);

            if (hRemoteThread == NULL) {
                throw std::exception(nbase::UTF16ToUTF8(L"远程线程注入失败").c_str());
            }

            CloseHandle(hRemoteThread);
            CloseHandle(hProcess);
        }
    }

    std::wstring getFileName(const std::wstring& filePath) {
        const auto index = filePath.find_last_of(L"\\");

        if (index == std::wstring::npos) {
            throw std::exception("invalid filePath");
        }

        std::wstring fileName = filePath.substr(index + 1, filePath.length() - index - 1);
        return std::move(fileName);
    }

    bool CheckIsInject(DWORD processid, const std::wstring& targetDllFilePath) {
        HANDLE hModuleSnap = INVALID_HANDLE_VALUE;
        //初始化模块信息结构体
        MODULEENTRY32 me32 = { sizeof(MODULEENTRY32) };
        //创建模块快照 1 快照类型 2 进程ID
        hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, processid);

        //如果句柄无效就返回false
        if (hModuleSnap == INVALID_HANDLE_VALUE) {
            throw std::exception(nbase::UTF16ToUTF8(L"创建模块快照失败").c_str());
        }

        //通过模块快照句柄获取第一个模块的信息
        if (!Module32First(hModuleSnap, &me32)) {
            //获取失败则关闭句柄
            CloseHandle(hModuleSnap);

            throw std::exception(nbase::UTF16ToUTF8(L"获取第一个模块的信息失败").c_str());
        }

        std::wstring fileName = getFileName(targetDllFilePath);

        do {
            if (std::wstring(me32.szModule) == fileName) {
                return true;
            }

        } while (Module32Next(hModuleSnap, &me32));

        return false;
    }

    void UnloadDll(const std::string& exeName, const std::wstring& targetDllFilePath) {
        //获取微信Pid
        DWORD dwPid = ProcessNameFindPID(exeName.c_str());

        if (dwPid == 0) {
            throw std::exception(nbase::UTF16ToUTF8(L"找不到进程").c_str());
        }

        std::wstring fileName = getFileName(targetDllFilePath);

        //遍历模块
        HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPid);
        MODULEENTRY32 ME32 = { 0 };
        ME32.dwSize = sizeof(MODULEENTRY32);
        BOOL isNext = Module32First(hSnap, &ME32);
        BOOL flag = FALSE;

        while (isNext) {
            USES_CONVERSION;

            if (strcmp(W2A(ME32.szModule), nbase::UTF16ToUTF8(fileName).c_str()) == 0) {
                flag = TRUE;
                break;
            }

            isNext = Module32Next(hSnap, &ME32);
        }

        CloseHandle(hSnap);

        if (flag == FALSE) {
            throw std::exception(nbase::UTF16ToUTF8(L"找不到目标模块").c_str());
        }

        //打开目标进程
        HANDLE hPro = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPid);
        //获取FreeLibrary函数地址
        FARPROC pFun = GetProcAddress(GetModuleHandleA("kernel32.dll"), "FreeLibrary");

        //创建远程线程执行FreeLibrary
        HANDLE hThread = CreateRemoteThread(hPro, NULL, 0, (LPTHREAD_START_ROUTINE)pFun, ME32.modBaseAddr, 0, NULL);

        if (!hThread) {
            throw std::exception(nbase::UTF16ToUTF8(L"创建远程线程失败").c_str());
        }

        if (WaitForSingleObject(hThread, 2 * 1000) == WAIT_TIMEOUT) {
            CloseHandle(hThread);
            CloseHandle(hPro);
            throw std::exception(nbase::UTF16ToUTF8(L"远程线程执行超时，动态库或许有残留线程没退出！").c_str());
        }

        CloseHandle(hThread);
        CloseHandle(hPro);
    }

}