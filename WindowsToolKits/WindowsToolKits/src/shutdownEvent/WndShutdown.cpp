#include "WndShutdown.h"
#include <common/CoreLog.hpp>

//////////////////���ش��ڵ�ȫ�ֱ���//////////////////////////////////
CleanFunc kCleanFunc = 0;           // �յ��ػ���Ϣʱ���õĴ�����
void*     kCleanFunArgs = 0;        // �ػ��������Ĳ���

////////////////////�����ȫ�ֱ���//////////////////////////////////
CleanFunc kServiceCleanFunc = 0;
void*     kServiceCleanFuncArgs = 0;
SERVICE_STATUS_HANDLE kServiceStatusHandle = NULL;    // ȫ�־������������������ľ��
SERVICE_STATUS        kServiceStatus;                 // ���������Ϣ�Ľṹ

// ���ش��ڵ���Ϣ�ص�����
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

// ����һ�����ش���
bool CreateHideWindow(void)
{
    // �������Գ�ʼ��
    HINSTANCE hIns = GetModuleHandle(0);
    WNDCLASSEX wc;
    wc.cbSize = sizeof(wc);                             // ����ṹ��С
    wc.style = CS_HREDRAW | CS_VREDRAW;                 // ����ı��˿ͻ�����Ŀ�Ȼ�߶ȣ������»����������� 
    wc.cbClsExtra = 0;                                  // ���ڽṹ�ĸ����ֽ���
    wc.cbWndExtra = 0;                                  // ����ʵ���ĸ����ֽ���
    wc.hInstance = hIns;                                // ��ģ���ʵ�����
    wc.hIcon = NULL;                                    // ͼ��ľ��
    wc.hIconSm = NULL;                                  // �ʹ����������Сͼ��ľ��
    wc.hbrBackground = (HBRUSH)COLOR_WINDOW;            // ������ˢ�ľ��
    wc.hCursor = NULL;                                  // ���ľ��
    wc.lpfnWndProc = __WndProc;                         // ���ڴ�������ָ��
    wc.lpszMenuName = NULL;                             // ָ��˵���ָ��
    wc.lpszClassName = TEXT("PAICS_ShutDown");          // ָ�������Ƶ�ָ��

    // Ϊ����ע��һ��������
    DWORD ret;
    if (!RegisterClassEx(&wc)) {
        ret = GetLastError();
        LOGE("RegisterClassEx failed,error:0x%.8x[%d]",ret,ret);
        return false;
    }

    // ��������
    HWND hWnd = CreateWindowEx(
        WS_EX_TOPMOST,              // ������չ��ʽ����������
        TEXT("PAICS_ShutDown"),     // ��������
        TEXT("PAICS"),              // ���ڱ���
        WS_OVERLAPPEDWINDOW,        // ������ʽ���ص�����
        0,                          // ���ڳ�ʼx����
        0,                          // ���ڳ�ʼy����
        800,                        // ���ڿ��
        600,                        // ���ڸ߶�
        0,                          // �����ھ��
        0,                          // �˵���� 
        hIns,                       // �봰�ڹ�����ģ��ʵ���ľ��
        0                           // �������ݸ�����WM_CREATE��Ϣ
    );
    if (hWnd == 0) {
        ret = GetLastError();
        LOGE("CreateWindowEx failed,error:0x%.8x[%d]", ret, ret);
        return false;
    }
    //UpdateWindow(hWnd);
    //ShowWindow(hWnd, SW_SHOW);

    // ��Ϣѭ����û�лᵼ�´��ڿ�����
    MSG msg = { 0 };
    while (msg.message != WM_QUIT) {
        // ����Ϣ������ɾ��һ����Ϣ
        if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
            DispatchMessage(&msg);
        }
    }
    return true;
}

// ���ùػ��¼�����ʱ��Ҫ���õĺ���
// func: �ػ��¼�����ʱ��Ҫ��������ĺ�����ΪNULL�򲻼����¼�
// args: ���ݸ�func�����Ĳ���
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
        // �ػ�������
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

// ���÷������ػ��¼�����
// ServiceName: ��������ƣ�ע��ʱ��������ƣ�
// func: ������
// args: �������Ĳ���
// ����ֵ: �ɹ�����true��ʧ�ܷ���false
// tip: �������������֧�ֲ��� STOP �� PRESHUTDOWN
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