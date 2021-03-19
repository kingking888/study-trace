#ifndef _INJECT_TOOLS_4B2B684A_9CA6_4A9E_B7C0_9EC427D54FD7_
#define _INJECT_TOOLS_4B2B684A_9CA6_4A9E_B7C0_9EC427D54FD7_

#include <cstring>

namespace core {
    /** @fn ProcessNameFindPID
    * @brief ͨ����������ȡ����ID
    * @param ProcessName���������֣���.exe
    * @return
    */
    DWORD ProcessNameFindPID(const char* processName) noexcept;

    /** @fn getFileName
      * @brief ��·���н�ȡ�ļ���
      * @param filePath�������ļ�·����
      * @return
      */
    std::wstring getFileName(const std::wstring& filePath);

    /** @fn StartProcess
      * @brief ��������
      * @param filePath����������·��
      *        firstWindowName���жϳɹ��Ĵ�������΢��ΪWeChatLoginWndForPC
      * @return
      */
    void StartProcess(std::wstring filePath, std::wstring firstWindowName = L"WeChatLoginWndForPC");

    /** @fn InjectDll
      * @brief ע��dll
      * @param exeName��������
      *		   targetDllFilePath��Ҫע�붯̬���·��
      * @return
      */
    void InjectDll(const std::string& exeName, const std::wstring& targetDllFilePath);

    /** @fn UnloadDll
      * @brief ж��DLL
      * @param exeName��Ҫж�ض�̬���Ŀ�����
      *        targetDllFilename��Ҫж�ض�̬�������
      * @return
      */
    void UnloadDll(const std::string& exeName, const std::wstring& targetDllFilePath);

    /** @fn CheckIsInject
      * @brief ���DLL�Ƿ��Ѿ�ע��
      * @param dwProcessid������ID
      *        targetDllFilePath���ļ�·����
      * @return
      */
    bool CheckIsInject(DWORD processid, const std::wstring& targetDllFilePath);

}

#endif//_INJECT_TOOLS_4B2B684A_9CA6_4A9E_B7C0_9EC427D54FD7_