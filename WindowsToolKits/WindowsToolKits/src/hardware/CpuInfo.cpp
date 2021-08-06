#include "CpuInfo.h"
#include "../../WinRing0/OlsApi.h"
#include "common/CoreLog.hpp"
#pragma comment(lib,"3rd/lib/WinRing0x64.lib")

static const char* kCpuErrorCode2Str[] = {
    "OLS_DLL_NO_ERROR",
    "OLS_DLL_UNSUPPORTED_PLATFORM",
    "OLS_DLL_DRIVER_NOT_LOADED",
    "OLS_DLL_DRIVER_NOT_FOUND",
    "OLS_DLL_DRIVER_UNLOADED",
    "OLS_DLL_DRIVER_NOT_LOADED_ON_NETWORK",
    "",
    "",
    "",
    "OLS_DLL_UNKNOWN_ERROR"
};

CpuInfo::CpuInfo()
    :m_isLoad(false), m_coreNumber(0)
    , m_cpuTemp(NULL)
{
    // 构造函数初始化dll
    m_isLoad = _CpuInfoInit();
}


CpuInfo::~CpuInfo()
{
    if (m_isLoad) {
        _CpuInfoExit();
    }
}


bool CpuInfo::IsLoad(void) const
{
    return m_isLoad;
}

// 成功返回CPU温度
// 失败返回 0
double CpuInfo::ReadCpuAvgTemp(void) const
{
    if (!m_isLoad) {
        return 0;
    }

    DWORD eax, edx;
    for (size_t i = 0; i < m_coreNumber; i++){
        int mask = 0x01 << i;
        // 设置当前使用线程(CPU)
        SetThreadAffinityMask(GetCurrentThread(), mask);
        // 读取温度寄存器（eax&0x7f0000可以获得温度数据）
        Rdmsr(0x19c, &eax, &edx);
        // 实际温度= tjmax - 温度数据
        m_cpuTemp[i] = m_tjmax - ((eax & 0x7f0000) >> 16);
    }

    // 计算平均温度
	double avgTemp = 0;
    for (size_t i = 0; i < m_coreNumber; i++){
        avgTemp += m_cpuTemp[i];
    }
    avgTemp /= m_coreNumber;
    return avgTemp;
}

unsigned __int64 CompareFileTime(FILETIME time1, FILETIME time2)
{
    unsigned __int64 a = (unsigned __int64)time1.dwHighDateTime << 32 | time1.dwLowDateTime;
    unsigned __int64 b = (unsigned __int64)time2.dwHighDateTime << 32 | time2.dwLowDateTime;
    return (b - a);
}

// 成功返回CPU使用率
// 失败返回 -1
double CpuInfo::ReadCpuUseRate(void) const
{
    if (!m_isLoad) {
        return -1;
    }
    static HANDLE hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    BOOL res;
    // 上一秒cup自开机起总的空闲时间
    FILETIME preidleTime;
    // 上一秒cup自开机起总的内核进程占用时间
    FILETIME prekernelTime;
    // 上一秒cup自开机起总的用户进程占用时间
    FILETIME preuserTime;
    // 当前cup自开机起总的空闲时间
    FILETIME idleTime;
    // 当前cup自开机起总的内核进程占用时间
    FILETIME kernelTime;
    // 当前cup自开机起总的用户进程占用时间
    FILETIME userTime;
    
    // 获取一次cup占用时间，等0.75秒后再次获取新的时间
    res = GetSystemTimes(&idleTime, &kernelTime, &userTime);
    preidleTime = idleTime;
    prekernelTime = kernelTime;
    preuserTime = userTime;
    // 初始值为 nonsignaled ，并且每次触发后自动设置为nonsignaled
    WaitForSingleObject(hEvent, 750);
    res = GetSystemTimes(&idleTime, &kernelTime, &userTime);
    // 一秒内的cup空闲时间
    uint64_t idle = CompareFileTime(preidleTime, idleTime);
    // 一秒内内核进程cup的占用时间
    uint64_t kernel = CompareFileTime(prekernelTime, kernelTime);
    // 一秒内用户进程占用cpu的时间
    uint64_t user = CompareFileTime(preuserTime, userTime);
    // （总的时间-空闲时间） / 总的时间 = 占用cpu时间的使用率
    double cpu = (kernel + user - idle) * 100.0 / (kernel + user);
    return cpu;
}


bool CpuInfo::_CpuInfoInit(void)
{
    // dll的初始化
    DWORD ret = InitializeOls();
    if (0 == ret){
        // 是否支持Cpuid
        if (TRUE == IsCpuid()){
            // 是否支持Rdmsr
            if (TRUE == IsMsr()){
                DWORD eax, edx;
                // 获得Tjmax
                Rdmsr(0x1A2, &eax, &edx);
                m_tjmax = (eax & 0xff0000) >> 16;

                // 获得逻辑处理器的数量
                SYSTEM_INFO sysInfo;
                GetSystemInfo(&sysInfo);
                m_coreNumber = sysInfo.dwNumberOfProcessors;
                m_cpuTemp = new int[m_coreNumber] {0};
                return true;
            }
        }
    }else{

        LOGW("CpuError:%s", kCpuErrorCode2Str[ret]);
    }
    return false;
}

void CpuInfo::_CpuInfoExit(void)
{
    // 卸载dll
    if (m_isLoad) {
        DeinitializeOls();
        delete[] m_cpuTemp;
        m_isLoad = false;
     }
}