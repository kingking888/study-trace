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

    void InjectDll(const std::string& exeName, const std::wstring& targetDllFilePath) {
        //��ȡ΢��Pid
        DWORD dwPid = ProcessNameFindPID(exeName.c_str());

        if (dwPid == 0) {
            throw std::exception("not find process.");
        }

        std::wstring fileName = getFileName(targetDllFilePath);

        //���dll�Ƿ��Ѿ�ע��
        if (!CheckIsInject(dwPid, targetDllFilePath)) {
            //�򿪽���
            HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPid);

            if (hProcess == nullptr) {
                throw std::exception(nbase::UTF16ToUTF8(L"���̴�ʧ��").c_str());
            }

            // ��΢�Ž����������ڴ�
            // 1�������̾����2����ռ�λ��Ϊ�վ������3һ��·����ַ,4Ȩ�ޣ�5�ɶ���д
            LPVOID pAddress = VirtualAllocEx(hProcess, NULL, MAX_PATH, MEM_COMMIT, PAGE_EXECUTE_READWRITE);

            if (pAddress == nullptr) {
                throw std::exception(nbase::UTF16ToUTF8(L"�ڴ����ʧ��").c_str());
            }

            //д��dll·����΢�Ž���
            const auto path = nbase::UTF16ToUTF8(targetDllFilePath);

            if (WriteProcessMemory(hProcess, pAddress, path.c_str(), path.length(), NULL) == 0) {
                throw std::exception(nbase::UTF16ToUTF8(L"·��д��ʧ��").c_str());
            }

            //��ȡLoadLibraryA������ַ
            FARPROC pLoadLibraryAddress = GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA");

            if (pLoadLibraryAddress == NULL) {
                throw std::exception(nbase::UTF16ToUTF8(L"��ȡLoadLibraryA������ַʧ��").c_str());
            }

            // ע��ɹ����̲߳���
            // CreateRemoteThread���ĸ�������Ҫ���Լ��ĵ�ַ�� GetModuleHandle ��ȡKernel32��ַ
            HANDLE hRemoteThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)pLoadLibraryAddress, pAddress, 0, NULL);

            if (hRemoteThread == NULL) {
                throw std::exception(nbase::UTF16ToUTF8(L"Զ���߳�ע��ʧ��").c_str());
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
        //��ʼ��ģ����Ϣ�ṹ��
        MODULEENTRY32 me32 = { sizeof(MODULEENTRY32) };
        //����ģ����� 1 �������� 2 ����ID
        hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, processid);

        //��������Ч�ͷ���false
        if (hModuleSnap == INVALID_HANDLE_VALUE) {
            throw std::exception(nbase::UTF16ToUTF8(L"����ģ�����ʧ��").c_str());
        }

        //ͨ��ģ����վ����ȡ��һ��ģ�����Ϣ
        if (!Module32First(hModuleSnap, &me32)) {
            //��ȡʧ����رվ��
            CloseHandle(hModuleSnap);

            throw std::exception(nbase::UTF16ToUTF8(L"��ȡ��һ��ģ�����Ϣʧ��").c_str());
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
        //��ȡ΢��Pid
        DWORD dwPid = ProcessNameFindPID(exeName.c_str());

        if (dwPid == 0) {
            throw std::exception(nbase::UTF16ToUTF8(L"�Ҳ�������").c_str());
        }

        std::wstring fileName = getFileName(targetDllFilePath);

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
            throw std::exception(nbase::UTF16ToUTF8(L"�Ҳ���Ŀ��ģ��").c_str());
        }

        //��Ŀ�����
        HANDLE hPro = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPid);
        //��ȡFreeLibrary������ַ
        FARPROC pFun = GetProcAddress(GetModuleHandleA("kernel32.dll"), "FreeLibrary");

        //����Զ���߳�ִ��FreeLibrary
        HANDLE hThread = CreateRemoteThread(hPro, NULL, 0, (LPTHREAD_START_ROUTINE)pFun, ME32.modBaseAddr, 0, NULL);

        if (!hThread) {
            throw std::exception(nbase::UTF16ToUTF8(L"����Զ���߳�ʧ��").c_str());
        }

        if (WaitForSingleObject(hThread, 2 * 1000) == WAIT_TIMEOUT) {
            CloseHandle(hThread);
            CloseHandle(hPro);
            throw std::exception(nbase::UTF16ToUTF8(L"Զ���߳�ִ�г�ʱ����̬������в����߳�û�˳���").c_str());
        }

        CloseHandle(hThread);
        CloseHandle(hPro);
    }

}