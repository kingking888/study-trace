#include "stdafx.h"
#include "main_form.h"
#include "core/inject_tools.h"
#include "base/file/file_path.h"
#include "msgbox/msgbox.h"

#include "base/win32/path_util.h"

#include <COmmdlg.h>

const std::wstring MainForm::kClassName = L"InjectorForm";
const std::string kWeChatExeName = "WeChat.exe";

MainForm::MainForm() : ed_path_(nullptr), btn_inject_(nullptr), btn_unload_(nullptr) {
    ::memset(load_dll_path_, 0, sizeof(load_dll_path_));
}

MainForm::~MainForm() {
}

std::wstring MainForm::GetSkinFolder() {
    return L"injector";
}

std::wstring MainForm::GetSkinFile() {
    return L"injector.xml";
}

std::wstring MainForm::GetWindowClassName() const {
    return kClassName;
}

void MainForm::InitWindow() {
    this->m_pRoot->AttachBubbledEvent(ui::kEventClick, std::bind(&MainForm::onClick, this, std::placeholders::_1));

    ed_path_ = dynamic_cast<ui::RichEdit*>(FindControl(L"ed_path"));
    btn_inject_ = dynamic_cast<ui::Button*>(FindControl(L"btn_inject_wechat"));
    btn_unload_ = dynamic_cast<ui::Button*>(FindControl(L"btn_unload"));
    assert(ed_path_);
}

LRESULT MainForm::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
    PostQuitMessage(0L);
    return __super::OnClose(uMsg, wParam, lParam, bHandled);
}

bool OpenFileDialog(wchar_t filePath[], int length) {
    OPENFILENAME ofn;
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    // must !
    ofn.lpstrFile = filePath;
    ofn.nMaxFile = length;
    //
    ofn.lpstrFilter = TEXT("动态库(*.dll)\0*.dll\0All Files(*.*)\0*.*\0\0");
    ofn.Flags = OFN_FILEMUSTEXIST;
    ofn.lpstrInitialDir = nbase::win32::GetCurrentModuleDirectory().c_str();
    //no extention file!    ofn.lpstrFilter="Any file(*.*)\0*.*\0ddfs\0ddfs*\0";
    return (GetOpenFileName((LPOPENFILENAME)&ofn));
}

bool MainForm::onClick(ui::EventArgs* e) {
    if (e->pSender == btn_inject_) {
        onClickInject();

    } else if (e->pSender == btn_unload_) {
        onClickUnload();
    }

    return false;
}

void MainForm::onClickInject() {
    if (!nbase::FilePathIsExist(ed_path_->GetText(), false)) {
        nim_comp::ShowMsgBox(this->GetHWND(), nullptr, L"路径不存在！", false);
        return;
    }

    if (OpenFileDialog(load_dll_path_, sizeof(load_dll_path_))) {
        HANDLE pid;

        try {
            pid = (HANDLE)core::ProcessNameFindPID(kWeChatExeName.c_str());

            if (pid == 0) {
                core::StartProcess(ed_path_->GetText());
            }

            core::InjectDll(kWeChatExeName, load_dll_path_, pid);
            btn_inject_->SetEnabled(false);
            btn_unload_->SetEnabled(true);
            nim_comp::ShowMsgBox(this->GetHWND(), nullptr, L"注入成功！", false);

        } catch (const std::exception& e) {
            nim_comp::ShowMsgBox(this->GetHWND(), nullptr, nbase::UTF8ToUTF16(e.what()), false);
        }
    }
}

void MainForm::onClickUnload() {
    try {
        core::UnloadDll(kWeChatExeName, load_dll_path_);
        btn_inject_->SetEnabled(true);
        btn_unload_->SetEnabled(false);
        nim_comp::ShowMsgBox(this->GetHWND(), nullptr, L"卸载成功！", false);

    } catch (const std::exception& e) {
        nim_comp::ShowMsgBox(this->GetHWND(), nullptr, nbase::UTF8ToUTF16(e.what()), false);
    }
}
