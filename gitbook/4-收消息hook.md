# 收消息hook

这次做一个改变，我计划在不看视频的情况下尝试找出收消息的Hook，并且实现收文本消息的Demo（为了简化，先只考虑能收到文本消息。通过开源项目，会有个消息类型来区分，Hook的时候要注意处理）。

## 思路

首先，微信有本地SQLite数据库，他收到一条消息肯定会写入到数据库里面（数据库可以通过解密工具解密后使用SQLite Expert查看），所以我的目标是找到这个Call，然后Hook取数据。

参考找通讯录列表的思路：
1. 我们需要使用另外一个微信给调试的微信发消息，然后使用CE搜索这一串字符串（Unicode）
2. 不停的发消息，排除一部分地址
3. 使用OD附加调试，dd 到某一个地址，下内存写入断点
4. 反复的找Call，以及在堆栈界面，找“返回到 WeChatWin.dll+xxx”之类的，右键点击“跳转到汇编窗口”然后下断点，加注释
5. 通过第4步，反复的观察

## 实战1（按照老视频，试验失败）

### 找存储消息的内存地址

![find_recv_msg_hook_1](./images/find_recv_msg_hook_1.png) 
![find_recv_msg_hook_2](./images/find_recv_msg_hook_2.png) 

### 下内存写入断点  

堆栈界面往下滚，看见“返回到 WeChatWi.78C38E95 来自 WeChatWi.78C168D0”之类的，就右击“反汇编窗口中跟随”，下断点加注释。

然后再往下滚，看见了一个sql插入语句，说明这个地址应该是对的，接下来就是排除上面“返回到 xx”的Call哪一个附近有数据了。

> PS：找到这些Call下好断点之后，那个地址的内存写入断点就可以取消了

![find_recv_msg_hook_3](./images/find_recv_msg_hook_3.png)  


最新消息：05AAEF44 

写入：
55169FF5    66:8907         mov word ptr ds:[edi],ax

调用：
5484C5EE    E8 E9D99100     call WeChatWi.55169FDC                   ; 堆栈1

### 其他

05F3F2A4  05FB115C  UNICODE "459"

0BE7F87C 这里信息多一些

这里是会话列表的信息
0BDEC6D4  060DDC00  UNICODE "PANMAOXIAOZHUSHOU"
0BDEC6D8  00000011
0BDEC6DC  00000011
0BDEC6E0  00000000
0BDEC6E4  00000000
0BDEC6E8  06132C70  UNICODE "@胖猫小助手"
0BDEC6EC  00000006
0BDEC6F0  00000006
0BDEC6F4  00000000
0BDEC6F8  00000000
0BDEC6FC  0603ADE0  UNICODE "459"

0675D945 lass SyncMgr

1.搜索 wxid wxid_i06tno1tqk0612
2.搜索 msgsource

## 实战2（按照新视频，成功）

特征码：
```asm
10CACC18  82B85208
10CACC1C  00000178
10CACC20  00000000
10CACC24  00000000
10CACC28  2ADAABC6
10CACC2C  0004CE8F
10CACC30  00000000
10CACC34  00000000
10CACC38  00000000
10CACC3C  FFFFFFF3
10CACC40  3169F3BD
10CACC44  68C7E05D
10CACC48  00000001
10CACC4C  00000001
10CACC50  00000002
10CACC54  60630015
10CACC58  0FAED380  UNICODE "filehelper"
10CACC5C  0000000A
10CACC60  0000000A
10CACC64  00000000
10CACC68  00000000
10CACC6C  00000000
10CACC70  00000000
10CACC74  00000000
10CACC78  00000000
10CACC7C  00000000
10CACC80  10DD89F0  UNICODE "尴尬咯哦"
```

```asm
10CACC18  82BC0B28 ; ECX的值
10CACC1C  00000178
10CACC20  00000000
10CACC24  00000000
10CACC28  2ADAABCC
10CACC2C  0004CE8F
10CACC30  00000000
10CACC34  00000000
10CACC38  00000000
10CACC3C  FFFFFFF3
10CACC40  DD6F535A
10CACC44  59E6CD29
10CACC48  0000002A
10CACC4C  00000001
10CACC50  00000002
10CACC54  60630109
10CACC58  0FA25528  UNICODE "filehelper"  ECX + 40
10CACC5C  0000000A ; 消息的发送人          ECX + 44 
10CACC60  0000000A
10CACC64  00000000
10CACC68  00000000
10CACC6C  00000000
10CACC70  00000000
10CACC74  00000000
10CACC78  00000000
10CACC7C  00000000
10CACC80  05A2B2C8  UNICODE "<?xml version="1.0"?>\n<msg bigheadimgurl="http://w"  ECX + 68
10CACC84  000002A5 ; 这里是消息的长度                                                ECX + 6C
10CACC88  000002A5
```

```asm
78F6D44C    50              push eax
78F6D44D    E8 0E8FCBFF     call WeChatWi.78C26360                   ; 这里可以Hook接消息
78F6D452    8B4F 04         mov ecx,dword ptr ds:[edi+0x4]
```

基址：78BA0000
Hook偏移： 78F6D44D - 78BA0000 = 0x3CD44D
原来的Call： 78C26360 - 78BA0000 = 0x86360
接收人：ECX+40
长度：ECX+44 
消息内容：ECX+68 
长度：ECX+6C

## 问题记录

### 勾选了Unicode反而搜索不到收到的消息内容

需要取消勾选Unicode，来搜索接收到的消息。