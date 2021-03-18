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
    //������ر���
    HKEY hKey;
    CString strAppRegeditPath("");
    TCHAR szProductType[MAX_PATH];
    memset(szProductType, 0, sizeof(szProductType));

    DWORD dwBuflen = MAX_PATH;
    LONG lRet = 0;

    //�����Ǵ�ע���,ֻ�д򿪺��������������
    lRet = RegOpenKeyEx(HKEY_CURRENT_USER, //Ҫ�򿪵ĸ���
                        LPCTSTR(strAppName), //Ҫ�򿪵����Ӽ�
                        0, //���һ��Ϊ0
                        KEY_QUERY_VALUE, //ָ���򿪷�ʽ,��Ϊ��
                        &hKey); //�������ؾ��

    if (lRet != ERROR_SUCCESS) { //�ж��Ƿ�򿪳ɹ�
        return strAppRegeditPath;

    } else {
        //���濪ʼ��ѯ
        lRet = RegQueryValueEx(hKey, //��ע���ʱ���صľ��
                               TEXT("Wechat"), //Ҫ��ѯ������,��ѯ�������װĿ¼������
                               NULL, //һ��ΪNULL����0
                               NULL,
                               (LPBYTE)szProductType, //����Ҫ�Ķ�����������
                               &dwBuflen);

        if (lRet != ERROR_SUCCESS) { //�ж��Ƿ��ѯ�ɹ�
            return strAppRegeditPath;

        } else {
            RegCloseKey(hKey);

            strAppRegeditPath = szProductType;

            int nPos = strAppRegeditPath.Find('-');

            if (nPos >= 0) {
                CString sSubStr = strAppRegeditPath.Left(nPos - 1);//����$,�������ʱnPos+1
                strAppRegeditPath = sSubStr;
            }
        }
    }

    return strAppRegeditPath;
}
CString GetAppRegeditPath2(CString strAppName) {
    //������ر���
    HKEY hKey;
    CString strAppRegeditPath("");
    TCHAR szProductType[MAX_PATH];
    memset(szProductType, 0, sizeof(szProductType));

    DWORD dwBuflen = MAX_PATH;
    LONG lRet = 0;

    //�����Ǵ�ע���,ֻ�д򿪺��������������
    lRet = RegOpenKeyEx(HKEY_CURRENT_USER, //Ҫ�򿪵ĸ���
                        LPCTSTR(strAppName), //Ҫ�򿪵����Ӽ�
                        0, //���һ��Ϊ0
                        KEY_QUERY_VALUE, //ָ���򿪷�ʽ,��Ϊ��
                        &hKey); //�������ؾ��

    if (lRet != ERROR_SUCCESS) { //�ж��Ƿ�򿪳ɹ�
        return strAppRegeditPath;

    } else {
        //���濪ʼ��ѯ
        lRet = RegQueryValueEx(hKey, //��ע���ʱ���صľ��
                               TEXT("InstallPath"), //Ҫ��ѯ������,��ѯ�������װĿ¼������
                               NULL, //һ��ΪNULL����0
                               NULL,
                               (LPBYTE)szProductType, //����Ҫ�Ķ�����������
                               &dwBuflen);

        if (lRet != ERROR_SUCCESS) { //�ж��Ƿ��ѯ�ɹ�
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

    DWORD ProcessNameFindPID(const char* ProcessName) {
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

        si.dwFlags = STARTF_USESHOWWINDOW; // ָ��wShowWindow��Ա��Ч
        si.wShowWindow = TRUE;             // �˳�Ա��ΪTRUE�Ļ�����ʾ�½����̵������ڣ�ΪFALSE�Ļ�����ʾ

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

    void InjectDll(const std::string& exeName, const std::wstring& targetDllFilePath, HANDLE& outPid) {
        //��ȡ΢��Pid
        DWORD dwPid = ProcessNameFindPID(exeName.c_str());

        if (dwPid == 0) {
            throw std::exception("not find process.");
        }

        const auto index = targetDllFilePath.find_last_of(L"\\");

        if (index == std::wstring::npos) {
            throw std::exception("invalid filePath");
        }

        std::wstring fileName = targetDllFilePath.substr(index + 1, targetDllFilePath.length() - index - 1);

        //���dll�Ƿ��Ѿ�ע��
        if (!CheckIsInject(dwPid, fileName)) {
            //�򿪽���
            HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPid);

            if (hProcess == nullptr) {
                throw std::exception("���̴�ʧ��");
            }

            // ��΢�Ž����������ڴ�
            // 1�������̾����2����ռ�λ��Ϊ�վ������3һ��·����ַ,4Ȩ�ޣ�5�ɶ���д
            LPVOID pAddress = VirtualAllocEx(hProcess, NULL, MAX_PATH, MEM_COMMIT, PAGE_EXECUTE_READWRITE);

            if (pAddress == nullptr) {
                throw std::exception("�ڴ����ʧ��");
            }

            //д��dll·����΢�Ž���
            const auto path = nbase::UTF16ToUTF8(targetDllFilePath);

            if (WriteProcessMemory(hProcess, pAddress, path.c_str(), path.length(), NULL) == 0) {
                throw std::exception("·��д��ʧ��");
            }

            //��ȡLoadLibraryA������ַ
            FARPROC pLoadLibraryAddress = GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA");

            if (pLoadLibraryAddress == NULL) {
                throw std::exception("��ȡLoadLibraryA������ַʧ��");
            }

            // ע��ɹ����̲߳���
            // CreateRemoteThread���ĸ�������Ҫ���Լ��ĵ�ַ�� GetModuleHandle ��ȡKernel32��ַ
            HANDLE hRemoteThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)pLoadLibraryAddress, pAddress, 0, NULL);

            if (hRemoteThread == NULL) {
                throw std::exception("Զ���߳�ע��ʧ��");
            }

            CloseHandle(hRemoteThread);
            CloseHandle(hProcess);
        }
    }

    bool CheckIsInject(DWORD processid, const std::wstring& dllFilename) {
        HANDLE hModuleSnap = INVALID_HANDLE_VALUE;
        //��ʼ��ģ����Ϣ�ṹ��
        MODULEENTRY32 me32 = { sizeof(MODULEENTRY32) };
        //����ģ����� 1 �������� 2 ����ID
        hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, processid);

        //��������Ч�ͷ���false
        if (hModuleSnap == INVALID_HANDLE_VALUE) {
            throw std::exception("����ģ�����ʧ��");
        }

        //ͨ��ģ����վ����ȡ��һ��ģ�����Ϣ
        if (!Module32First(hModuleSnap, &me32)) {
            //��ȡʧ����رվ��
            CloseHandle(hModuleSnap);

            throw std::exception("��ȡ��һ��ģ�����Ϣʧ��");
        }

        do {
            if (std::wstring(me32.szModule) == dllFilename) {
                return true;
            }

        } while (Module32Next(hModuleSnap, &me32));

        return false;
    }

    void UnloadDll(const std::string& exeName, const std::wstring& targetDllFilePath) {
        //��ȡ΢��Pid
        DWORD dwPid = ProcessNameFindPID(exeName.c_str());

        if (dwPid == 0) {
            throw std::exception("�Ҳ�������");
        }

        const auto index = targetDllFilePath.find_last_of(L"\\");

        if (index == std::wstring::npos) {
            throw std::exception("invalid filePath");
        }

        std::wstring fileName = targetDllFilePath.substr(index + 1, targetDllFilePath.length() - index - 1);

        //����ģ��
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
            throw std::exception("�Ҳ���Ŀ��ģ��");
        }

        //��Ŀ�����
        HANDLE hPro = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPid);
        //��ȡFreeLibrary������ַ
        FARPROC pFun = GetProcAddress(GetModuleHandleA("kernel32.dll"), "FreeLibrary");

        //����Զ���߳�ִ��FreeLibrary
        HANDLE hThread = CreateRemoteThread(hPro, NULL, 0, (LPTHREAD_START_ROUTINE)pFun, ME32.modBaseAddr, 0, NULL);

        if (!hThread) {
            throw std::exception("����Զ���߳�ʧ��");
        }

        if (WaitForSingleObject(hThread, 2 * 1000) == WAIT_TIMEOUT) {
            CloseHandle(hThread);
            CloseHandle(hPro);
            throw std::exception("Զ���߳�ִ�г�ʱ");
        }

        CloseHandle(hThread);
        CloseHandle(hPro);

        /*HANDLE hProcess = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION | PROCESS_VM_WRITE,
                                      FALSE, dwPid);

        if (hProcess == NULL) {
            throw std::exception("�򿪽���ʧ��");
        }

        DWORD dwSize = 0;
        DWORD dwWritten = 0;
        DWORD dwHandle = 0;

        dwSize = fileName.size() + 1;
        LPVOID lpBuf = VirtualAllocEx(hProcess, NULL, dwSize, MEM_COMMIT, PAGE_READWRITE);

        if (!WriteProcessMemory(hProcess, lpBuf, (LPVOID)fileName.c_str(), dwSize, &dwWritten)) {
            VirtualFreeEx(hProcess, lpBuf, dwSize, MEM_DECOMMIT);
            CloseHandle(hProcess);
            throw std::exception("WriteProcessMemory ʧ��");
        }

        LPVOID pFun = GetProcAddress(GetModuleHandle(L"Kernel32"), "GetModuleHandleA");

        HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)pFun,
                                            lpBuf, 0, NULL);

        if (hThread == NULL) {
            CloseHandle(hProcess);
            throw std::exception("����Զ���߳�ʧ��");
        }

        if (WaitForSingleObject(hThread, 2 * 1000) == WAIT_TIMEOUT) {
            throw std::exception("Զ���߳�ִ�г�ʱ");
        }

        GetExitCodeThread(hThread, &dwHandle);

        VirtualFreeEx(hProcess, lpBuf, dwSize, MEM_DECOMMIT);
        CloseHandle(hThread);

        pFun = GetProcAddress(GetModuleHandle(L"Kernel32"), "FreeLibraryAndExitThread");
        hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)pFun, (LPVOID)dwHandle, 0, NULL);

        if (WaitForSingleObject(hThread, 2 * 1000) == WAIT_TIMEOUT) {
            throw std::exception("Զ���߳�ִ�г�ʱ");
        }

        CloseHandle(hThread);
        CloseHandle(hProcess);*/
    }

}