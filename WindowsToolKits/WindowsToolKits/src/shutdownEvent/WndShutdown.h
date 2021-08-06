#pragma once
#include<Windows.h>
#include "common/typedefs.h"
/*
    适用于窗口和控制台的关机事件监听。
    思想:通过创建一个隐藏窗口，在隐藏窗口的消息回调函数中监听关机事件，并调用CleanFunc(args)
    tip:普通用户权限即可，清理过程超过5s则会在Window关机界面中显示: xxx程序阻止关机
*/
extern "C" {
    typedef void(_stdcall* CleanFunc)(void* args);
    bool CORE_EXPORT SetShutdownProc(CleanFunc func, void* args);
    
}


/*
    适用服务的关机事件监听。
    思想：在服务的控制请求函数中监听关机事件.
    tip:一般情况下有3分钟时间处理
*/
extern "C" {
    bool CORE_EXPORT SetServiceShutdownProc(LPCWSTR ServiceName, CleanFunc func, void* args);
}