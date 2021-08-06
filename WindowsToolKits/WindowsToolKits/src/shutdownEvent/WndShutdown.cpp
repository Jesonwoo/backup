#include "WndShutdown.h"
#include <common/CoreLog.hpp>

//////////////////隐藏窗口的全局变量//////////////////////////////////
CleanFunc kCleanFunc = 0;           // 收到关机消息时调用的处理函数
void*     kCleanFunArgs = 0;        // 关机处理函数的参数

////////////////////服务的全局变量//////////////////////////////////
CleanFunc kServiceCleanFunc = 0;
void*     kServiceCleanFuncArgs = 0;
SERVICE_STATUS_HANDLE kServiceStatusHandle = NULL;    // 全局句柄，保存服务控制请求的句柄
SERVICE_STATUS        kServiceStatus;                 // 保存服务信息的结构

// 隐藏窗口的消息回调函数
LRESULT CALLBACK __WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_QUERYENDSESSION:
        return TRUE;
    case WM_ENDSESSION:
        if (wParam && lParam == ENDSESSION_CLOSEAPP) {
            kCleanFunc(kCleanFunArgs);
        }
        return 0;
    default:
        break;
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

// 创建一个隐藏窗口
bool CreateHideWindow(void)
{
    // 窗口属性初始化
    HINSTANCE hIns = GetModuleHandle(0);
    WNDCLASSEX wc;
    wc.cbSize = sizeof(wc);                             // 定义结构大小
    wc.style = CS_HREDRAW | CS_VREDRAW;                 // 如果改变了客户区域的宽度或高度，则重新绘制整个窗口 
    wc.cbClsExtra = 0;                                  // 窗口结构的附加字节数
    wc.cbWndExtra = 0;                                  // 窗口实例的附加字节数
    wc.hInstance = hIns;                                // 本模块的实例句柄
    wc.hIcon = NULL;                                    // 图标的句柄
    wc.hIconSm = NULL;                                  // 和窗口类关联的小图标的句柄
    wc.hbrBackground = (HBRUSH)COLOR_WINDOW;            // 背景画刷的句柄
    wc.hCursor = NULL;                                  // 光标的句柄
    wc.lpfnWndProc = __WndProc;                         // 窗口处理函数的指针
    wc.lpszMenuName = NULL;                             // 指向菜单的指针
    wc.lpszClassName = TEXT("PAICS_ShutDown");          // 指向类名称的指针

    // 为窗口注册一个窗口类
    DWORD ret;
    if (!RegisterClassEx(&wc)) {
        ret = GetLastError();
        LOGE("RegisterClassEx failed,error:0x%.8x[%d]",ret,ret);
        return false;
    }

    // 创建窗口
    HWND hWnd = CreateWindowEx(
        WS_EX_TOPMOST,              // 窗口扩展样式：顶级窗口
        TEXT("PAICS_ShutDown"),     // 窗口类名
        TEXT("PAICS"),              // 窗口标题
        WS_OVERLAPPEDWINDOW,        // 窗口样式：重叠窗口
        0,                          // 窗口初始x坐标
        0,                          // 窗口初始y坐标
        800,                        // 窗口宽度
        600,                        // 窗口高度
        0,                          // 父窗口句柄
        0,                          // 菜单句柄 
        hIns,                       // 与窗口关联的模块实例的句柄
        0                           // 用来传递给窗口WM_CREATE消息
    );
    if (hWnd == 0) {
        ret = GetLastError();
        LOGE("CreateWindowEx failed,error:0x%.8x[%d]", ret, ret);
        return false;
    }
    //UpdateWindow(hWnd);
    //ShowWindow(hWnd, SW_SHOW);

    // 消息循环（没有会导致窗口卡死）
    MSG msg = { 0 };
    while (msg.message != WM_QUIT) {
        // 从消息队列中删除一条消息
        if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
            DispatchMessage(&msg);
        }
    }
    return true;
}

// 设置关机事件发生时需要调用的函数
// func: 关机事件发生时需要任务清理的函数，为NULL则不监听事件
// args: 传递给func函数的参数
bool SetShutdownProc(CleanFunc func, void* args)
{
    DWORD ret;
    if (func == NULL) {
        return false;
    }

    kCleanFunc = func;
    kCleanFunArgs = args;

    HANDLE hthread = CreateThread(NULL, 0
        , (LPTHREAD_START_ROUTINE)CreateHideWindow
        , NULL, 0, NULL);
    if (hthread == NULL) {
        ret = GetLastError();
        LOGE("CreateThread failed,error:0x%.8x[%d]", ret, ret);
        return false;
    }
    CloseHandle(hthread);
    return true;
}

DWORD WINAPI ServiceHandle(
    DWORD    dwControl,
    DWORD    dwEventType,
    LPVOID   lpEventData,
    LPVOID   lpContext
)
{
    switch (dwControl)
    {
    case SERVICE_CONTROL_INTERROGATE:
        return NO_ERROR;
    case SERVICE_CONTROL_PRESHUTDOWN:
        // 关机处理函数
        kServiceCleanFunc(kServiceCleanFuncArgs);
        kServiceStatus.dwCurrentState = SERVICE_STOPPED;
        SetServiceStatus(kServiceStatusHandle, &kServiceStatus);
        return NO_ERROR;
    case SERVICE_CONTROL_STOP:
        kServiceStatus.dwCurrentState = SERVICE_STOPPED;
        SetServiceStatus(kServiceStatusHandle, &kServiceStatus);
        return NO_ERROR;
    default:
        return ERROR_CALL_NOT_IMPLEMENTED;
    }  
}

// 设置服务程序关机事件监听
// ServiceName: 服务的名称（注册时服务的名称）
// func: 清理函数
// args: 清理函数的参数
// 返回值: 成功返回true，失败返回false
// tip: 服务控制请求函数支持参数 STOP 和 PRESHUTDOWN
bool CORE_EXPORT SetServiceShutdownProc(LPCWSTR ServiceName
                                        , CleanFunc func, void * args)
{   
    if (!func) {
        return false;
    }

    DWORD dwRet;
    kServiceStatusHandle = RegisterServiceCtrlHandlerEx(ServiceName, ServiceHandle, NULL);
    if (!kServiceStatusHandle) {
        dwRet = GetLastError();
        LOGE("RegisterService Fail,error code:0x%.8x[%d]", dwRet, dwRet);
        return false;
    }
    kServiceCleanFunc = func;
    kServiceCleanFuncArgs = args;

    kServiceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    kServiceStatus.dwCurrentState = SERVICE_START_PENDING;
    kServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_PRESHUTDOWN;
    kServiceStatus.dwCheckPoint = 0;
    kServiceStatus.dwServiceSpecificExitCode = 0;
    kServiceStatus.dwWaitHint = 0;
    kServiceStatus.dwWin32ExitCode = 0;
    if (!SetServiceStatus(kServiceStatusHandle, &kServiceStatus)) {
        dwRet = GetLastError();
        LOGE("SetServiceStatus 0x01 Fail,error code:0x%.8x[%d]", dwRet, dwRet);
        return false;
    }

    kServiceStatus.dwWin32ExitCode = S_OK;
    kServiceStatus.dwCheckPoint = 0;
    kServiceStatus.dwWaitHint = 0;
    kServiceStatus.dwCurrentState = SERVICE_RUNNING;
    if (!SetServiceStatus(kServiceStatusHandle, &kServiceStatus)) {
        dwRet = GetLastError();
        LOGE("SetServiceStatus 0x02 Fail,error code:0x%.8x[%d]", dwRet, dwRet);
        return false;
    }
    return true;
}