# DebugView

## DbgView不能显示OutputDebugString的输出内容

[原文](https://blog.csdn.net/zhuimengfuyun/article/details/45309383)

win10下实测有效：

1. 打开注册表：computer--> HKEY_LOCAL_MACHINE-->SYSTEM->CurrentControlSet-->Control-->Session Manager
2. 新建一个key值，类型为DWORD，**Value Name:Debug Print Filter** ，**Value data : f**
3. 然后重启电脑,问题有可能就解决了，其他还有可能引起该问题的原因可能是版本不匹配，或者其他编辑器作为了DbgView 的输出口。