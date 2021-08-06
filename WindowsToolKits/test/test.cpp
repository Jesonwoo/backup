#include <WindowsToolKits/src/hardware/HardwareInfo.h>
#include <WindowsToolKits/src/common/CoreLog.hpp>
#include <WindowsToolKits/src/usermgr/WinUserManagement.h>
#include <WindowsToolKits/src/windowscontrol/WindowsControl.h>
#include "../CopyFileClass/CopyFileClass.h"
#include <WindowsToolKits/src/shutdownEvent/WndShutdown.h>
#include <iostream>
#include <time.h>
#include <Windows.h>
#include <lm.h>
#include <assert.h>
#include <winbase.h>
#include <vector>
#include <thread>
#pragma comment(lib, "netapi32.lib")
int UserEnum()
{
    LPUSER_INFO_0 pBuf = NULL;
    LPUSER_INFO_0 pTmpBuf;
    DWORD dwLevel = 0;
    DWORD dwPrefMaxLen = MAX_PREFERRED_LENGTH;
    DWORD dwEntriesRead = 0;
    DWORD dwTotalEntries = 0;
    DWORD dwResumeHandle = 0;
    DWORD i;
    DWORD dwTotalCount = 0;
    NET_API_STATUS nStatus;
    LPTSTR pszServerName = NULL;
    do // begin do
    {
        nStatus = NetUserEnum((LPCWSTR)pszServerName,
            dwLevel,
            FILTER_NORMAL_ACCOUNT, // global users
            (LPBYTE*)&pBuf,
            dwPrefMaxLen,
            &dwEntriesRead,
            &dwTotalEntries,
            &dwResumeHandle);
        // If the call succeeds,
        if ((nStatus == NERR_Success) || (nStatus == ERROR_MORE_DATA)) {
            if ((pTmpBuf = pBuf) != NULL) {
                // Loop through the entries.
                for (i = 0; (i < dwEntriesRead); i++) {
                    assert(pTmpBuf != NULL);

                    if (pTmpBuf == NULL) {
                        fprintf(stderr, "An access violation has occurred\n");
                        break;
                    }
                    //  Print the name of the user account.
                    wprintf(L"\t-- %s\n", pTmpBuf->usri0_name);

                    pTmpBuf++;
                    dwTotalCount++;
                }
            }
        }
        // Otherwise, print the system error.
        else
            fprintf(stderr, "A system error has occurred: %d\n", nStatus);
        // Free the allocated buffer.
        if (pBuf != NULL) {
            NetApiBufferFree(pBuf);
            pBuf = NULL;
        }
    }
    // Continue to call NetUserEnum while 
    //  there are more entries. 
    while (nStatus == ERROR_MORE_DATA); // end do
    // Check again for allocated memory.
    if (pBuf != NULL)
        NetApiBufferFree(pBuf);
    // Print the final count of users enumerated.
    fprintf(stderr, "\nTotal of %d entries enumerated\n", dwTotalCount);

    return 0;
}

void test_user_mgr()
{
    UserEnum();
    AddUser("testuser", "123456abcdef!@#");
    UserEnum();
    DeleteUser("testuser");
    UserEnum();
    //AddUserToAdministrators("testuser");
    //AddUserToSharedPath("test", "H:\\shared");
    std::cout << GetLastError() << std::endl;
}

void test_hardware_info()
{
    for (int i = 0; i < 100000; ++i) {
        std::cout << "==========================================" << std::endl;
        clock_t start = clock();
        std::cout << Getuuid() << std::endl;
        std::cout << GetCPUName() << std::endl;
        std::cout << GetCPUProcessId() << std::endl;
        std::cout << GetCPUDeviceId() << std::endl;
        std::cout << GetBaseBoardSerialNumber() << std::endl;
        std::cout << GetBaseBoardModel() << std::endl;
        std::cout << GetBaseBoardProduct() << std::endl;
        std::cout << GetDefaultGraphicCardDesc() << std::endl;
        clock_t end = clock();
        std::cout << "run time: " << (double)(end - start) / CLOCKS_PER_SEC << std::endl;
    }
}

void get_hardware_info()
{

    //// 内存信息
    //std::cout << "物理内存总容量:" << GetTotalMem() << std::endl;
    //std::cout << "物理内存空闲容量:" << GetFreeMem() << std::endl;

    // CPU信息(加载cpu相关驱动需要以管理员权限运行)
    CpuInfo myCpu;      //生成对象
    if (myCpu.IsLoad()) {
        std::cout << "CPU平均温度" << myCpu.ReadCpuAvgTemp() << std::endl;
        std::cout << "CPU使用率:" << myCpu.ReadCpuUseRate() << std::endl;
    }
    Sleep(2000);
    //// GPU信息
    //GpuInfo gpu;
    //std::cout << "GPU占用率:" << gpu.GetGpuLoad() << std::endl;
    //std::cout << "GPU温度:" << gpu.GetAvgTemp() << std::endl;
    //std::cout << "GPU总容量:" << gpu.GetGpuTotalMem() << std::endl;
    //std::cout << "GPU空闲容量:" << gpu.GetGpuFreeMem() << std::endl;


    //// 磁盘信息
    //DiskInfoMap diskInfoMap;

    //while (1) {
    //    std::map<std::string, DiskInfoMap::DiskInfo> map = diskInfoMap.GetDisksInfo();
    //    for (auto i = map.begin(); i != map.end(); i++) {
    //        std::cout << i->first.c_str() << std::endl;
    //        std::cout << "总容量:" << i->second.GetTotalMem() << std::endl;
    //        std::cout << "空闲容量:" << i->second.GetFreeMem() << std::endl;
    //    }
    //    Sleep(2000);
    //    system("cls");
    //}
}
void __stdcall MyCallBackFun(DWORD flag, DWORD number, void* args)
{
    if (flag == CopyFileTool::BEGIN) {
        printf("文件开始拷贝\n");
    }
    else if (flag == CopyFileTool::FAILED) {
        printf("其他错误\n");
    }
    else if (flag == CopyFileTool::FILE_NOT_FOUND) {
        printf("源文件不存在\n");
    }
    else if (flag == CopyFileTool::REQUEST_ABORTED) {
        printf("操作被中断\n");
    }
    else if (flag == CopyFileTool::TARGET_DIR_NOT_EXIT) {
        printf("目录不存在\n");
    }
}
void copyFile(void) 
{
    DWORD ret;
    CopyFileTool tool(L"D:\\迅雷下载\\1.iso", L"D:\\123.iso");
    ret = tool.init(MyCallBackFun, 1, NULL);
    if (ret) {
        double p = tool.GetProgress();
        while (p < 1.0) {
            printf("进度为%f\n", p);
            p = tool.GetProgress();
            if (p >= 0.2) {
                tool.CancleOperator();
                break;
            }
            //if (p >= 1.0) {
            //    break;
            //}
        }
    }
    getchar();
}
void __stdcall MyMessagebox(void* args)
{
    FILE* g_file;
    if (fopen_s(&g_file, "C:\\shutdown.log", "w+") == 0) {
        fwrite("service begin cleanning\n", strlen("service begin cleanning\n"), 1, g_file);
        fclose(g_file);
    }
}

void wndShutdonwTest(void)
{
    SetShutdownProc(MyMessagebox, (void*)"Don't shutdown!");
}

int main()
{
    int threadcount = 10;
    std::vector<std::thread*> threadList;
    for (int i = 0; i < threadcount; ++i) {
        threadList.push_back(new std::thread(test_hardware_info));
    }
    for (std::thread * i : threadList) {
        i->join();
    }

    //get_hardware_info();
    //test_hardware_info();
    //test_user_mgr();
    //if (ResetWindows(EWX_REBOOT, true)) {
    //    std::cout << "start reboot" << std::endl;
    //}
    system("pause");
    return 0;
}
