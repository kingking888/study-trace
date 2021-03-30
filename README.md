# study-trace
study od,ce and trace wechat

## Depends 

开发依赖：
- vs2017
- [nim_duilib](https://github.com/netease-im/NIM_Duilib_Framework)
- [wechat: 3.1.0.67 (2020-12-24)](https://weixin.qq.com/cgi-bin/readtemplate?lang=zh_CN&t=weixin_faq_list)

调试工具：
- DebugView：windows内核日志，方便查看dll的日志，调试
- OD(Ollydbg)：汇编调试工具，可断点修改汇编代码等。逆向必备工具
- CE(Cheat Engine)：修改器，可以查找特征，通常用来找各种内存地址。
- 读微信Png图片.exe：网上找的资源，可以快速验证二维码地址是否有效。

## Quick Start

使用vs2017打开trace_all.sln，把injector设为启动项，运行。选择要注入的dll即可查看效果。

工程说明：
- injectcore：逆向核心代码，静态库可复用。
- injector：微信注入器，界面基于nim_duilib。
- injectordll：注入器测试动态库，使用debugview工具可查看日志。
- loginQRCode：登录二维码HookDemo，可以显示微信登录二维码，包括刷新操作等。
- loginStatusMonit：登录状态监控，每1秒钟打印一次微信的登录状态。
- logHook：微信日志hook，可以通过debugview工具查看微信的日志。

## 声明

本项目仅供技术研究，请勿用于任何商业用途，请勿用于非法用途，如有任何人凭此做任何非法事情，均与作者无关，后果自负，特此声明。