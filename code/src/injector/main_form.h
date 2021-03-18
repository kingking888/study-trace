#ifndef _MAIN_FORM_E14C8512_152C_412D_924E_6C7EA9CAD2F4_
#define _MAIN_FORM_E14C8512_152C_412D_924E_6C7EA9CAD2F4_

#include "duilib/UIlib.h"

class MainForm : public ui::WindowImplBase {
  public:
    MainForm();
    ~MainForm();

    /**
     * һ�������ӿ��Ǳ���Ҫ��д�Ľӿڣ����������������ӿ�����������
     * GetSkinFolder		�ӿ�������Ҫ���ƵĴ���Ƥ����Դ·��
     * GetSkinFile			�ӿ�������Ҫ���ƵĴ��ڵ� xml �����ļ�
     * GetWindowClassName	�ӿ����ô���Ψһ��������
     */
    virtual std::wstring GetSkinFolder() override;
    virtual std::wstring GetSkinFile() override;
    virtual std::wstring GetWindowClassName() const override;

    /**
     * �յ� WM_CREATE ��Ϣʱ�ú����ᱻ���ã�ͨ����һЩ�ؼ���ʼ���Ĳ���
     */
    virtual void InitWindow() override;

    /**
     * �յ� WM_CLOSE ��Ϣʱ�ú����ᱻ����
     */
    virtual LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

    static const std::wstring kClassName;

  protected:
    bool onClick(ui::EventArgs* e);

    void onClickInject();
    void onClickUnload();
  private:
    wchar_t load_dll_path_[2048];
    ui::RichEdit* ed_path_;
    ui::Button* btn_inject_;
    ui::Button* btn_unload_;
};

#endif//_MAIN_FORM_E14C8512_152C_412D_924E_6C7EA9CAD2F4_