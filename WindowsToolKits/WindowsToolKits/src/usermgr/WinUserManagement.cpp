// WinUserManagement.cpp : 定义 DLL 应用程序的导出函数。
//

#include <windows.h> 
#include <lm.h>
#include <assert.h>
#include <wchar.h>
#include <string>
#include <iostream>

#include "common/CoreLog.hpp"
#include "WinUserManagement.h"
#pragma comment(lib, "netapi32.lib")

using namespace std;
static string ExecCmd(const char * cmd)
{
    SECURITY_ATTRIBUTES sa;
    HANDLE hRead, hWrite;
    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.lpSecurityDescriptor = NULL;
    sa.bInheritHandle = TRUE;
    if (!CreatePipe(&hRead, &hWrite, &sa, 0)) {
        return FALSE;
    }

    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    si.cb = sizeof(STARTUPINFO);
    GetStartupInfoA(&si);
    si.hStdError = hWrite;
    si.hStdOutput = hWrite;
    si.wShowWindow = SW_HIDE;
    si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
    if (!CreateProcessA(NULL, (LPSTR)cmd, NULL, NULL, TRUE, NULL, NULL, NULL, &si, &pi)) {
        return FALSE;
    }
    CloseHandle(hWrite);
    char buffer[4096] = { 0 };
    DWORD bytesRead;
    std::string strOutPut = "";
    while (true) {
        if (ReadFile(hRead, buffer, 4095, &bytesRead, NULL) == NULL) {
            break;
        }
        strOutPut += buffer;
    }
    CloseHandle(hRead);
    return strOutPut;
}

int AddUser(const char * name, const char * passwd)
{
    if (name == nullptr || passwd == nullptr) {
        return -1;
    }
    USER_INFO_1 ui;
    DWORD dwLevel = 1;
    DWORD dwError = 0;
    NET_API_STATUS status;
    wchar_t username[32] = { 0 };
    wchar_t userpasswd[128] = { 0 };
    MultiByteToWideChar(CP_ACP, 0, name, 
        (int)strlen(name) + 1, username, (int)(sizeof(username) / sizeof(wchar_t)));
    MultiByteToWideChar(CP_ACP, 0, passwd, 
        (int)strlen(passwd) + 1, userpasswd, (int)(sizeof(userpasswd) / sizeof(wchar_t)));
    // Set up the USER_INFO_1 structure.
    ui.usri1_name = username;
    ui.usri1_password = userpasswd;
    ui.usri1_priv = USER_PRIV_USER;
    ui.usri1_home_dir = NULL;
    ui.usri1_comment = NULL;
    ui.usri1_flags = UF_SCRIPT | UF_DONT_EXPIRE_PASSWD | UF_PASSWD_CANT_CHANGE;
    ui.usri1_script_path = NULL;
    // Call the NetUserAdd function, specifying level 1.
    status = NetUserAdd(nullptr,
        dwLevel,
        (LPBYTE)&ui,
        &dwError);
    // If the call succeeds, inform the user.
    LOGI("%s, name[%s], status[%d]", __FUNCTION__, name, status);
    return status;
}

int AddUserToAdministrators(const char * name)
{
    LOCALGROUP_MEMBERS_INFO_3 account;
    NET_API_STATUS nStatus;
    wchar_t username[32] = { 0 };
    MultiByteToWideChar(CP_ACP, 0, name, 
        (int)strlen(name) + 1, username, sizeof(username) / sizeof(wchar_t));
    account.lgrmi3_domainandname = username;
    nStatus = NetLocalGroupAddMembers(NULL, L"Administrators", 3, (LPBYTE)&account, 1);
    LOGI("%s, name[%s], status[%d]", __FUNCTION__, name, nStatus);
    return nStatus;
}

int DeleteUser(const char * name)
{
    DWORD dwError = 0;
    wchar_t username[32] = { 0 };
    MultiByteToWideChar(CP_ACP, 0, name,
        (int)strlen(name) + 1, username, sizeof(username) / sizeof(wchar_t));
    NET_API_STATUS nStatus;
    // Call the NetUserDel function to delete the share.
    nStatus = NetUserDel(nullptr, username);
    NERR_Success;
    LOGI("%s, name[%s], status[%d]", __FUNCTION__, name, nStatus);
    return nStatus;
}

//int AddUserToSharedPath(const char * name, const char * path)
//{
//    string cmd = "icacls ";
//    cmd.append(path).append(" /grant ").append(name).append(":(OI)(CI)(RX,M) /T");
//    string result = ExecCmd(cmd.c_str());
//    int ret = result.find("处理 0 个文件时失败") != std::string::npos ? 0 : -1;
//    LOGI("add name[%s] to path[%s], status[%d]", name, path, ret);
//    return ret;
//}
