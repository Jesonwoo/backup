#include "WindowsControl.h"
#include <common/CoreLog.hpp>
//ͨ�õ�����Ȩ�޺����������Ҫ��������Ȩ��
static BOOL EnableShutDownPriv()
{
    HANDLE hToken = NULL;
    TOKEN_PRIVILEGES tkp = { 0 };
    //�򿪵�ǰ�����Ȩ������
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {
        LOGE("Open process token error!");
        return FALSE;
    }
    //��SE_SHUTDOWN_NAME�ض�Ȩ�޵�Ȩ�ޱ�ʶLUID��������tkp��
    if (!LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid)) {
        LOGE("Lookup priveilege value error!");
        CloseHandle(hToken);
        return FALSE;
    }
    tkp.PrivilegeCount = 1;
    tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    //����AdjustTokenPrivileges������������Ҫ��ϵͳȨ��
    if (!AdjustTokenPrivileges(hToken, FALSE, &tkp, sizeof(TOKEN_PRIVILEGES), NULL, NULL)) {
        LOGE("Adjust token privileges error!");
        CloseHandle(hToken);
        return FALSE;
    }
    return TRUE;
}

BOOL ResetWindows(DWORD dwFlags, BOOL bForce)
{
    //�������Ƿ���ȷ
    if (dwFlags != EWX_LOGOFF 
        && dwFlags != EWX_REBOOT 
        && dwFlags != EWX_SHUTDOWN 
        && dwFlags != EWX_POWEROFF) {
        LOGE("Reset windows params not valid");
        return FALSE;
    }
    //���ϵͳ�İ汾��Ϣ�������Ǻ���ȷ���Ƿ���Ҫ����ϵͳȨ��
    OSVERSIONINFO osvi = { 0 };
    //��ò����Ĵ�С�����ṩ��GetVersionEx���ж�����һ���°汾��OSVERSIONINFO�����Ǿɰ汾��
    //�°汾��OSVERSIONINFOEX������汾
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    if (!GetVersionEx(&osvi)) {
        return FALSE;
    }
    //������ϵͳ�İ汾�������NT���͵�ϵͳ����Ҫ����ϵͳȨ��
    if (osvi.dwPlatformId == VER_PLATFORM_WIN32_NT) {
        EnableShutDownPriv();
    }
    //�ж��Ƿ���ǿ�ƹػ���ǿ�ƹر�ϵͳ���̡�
    dwFlags |= (bForce != FALSE) ? EWX_FORCE : EWX_FORCEIFHUNG;
    //����API
    return ExitWindowsEx(dwFlags, 0);
}
