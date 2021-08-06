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
    // ���캯����ʼ��dll
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

// �ɹ�����CPU�¶�
// ʧ�ܷ��� 0
double CpuInfo::ReadCpuAvgTemp(void) const
{
    if (!m_isLoad) {
        return 0;
    }

    DWORD eax, edx;
    for (size_t i = 0; i < m_coreNumber; i++){
        int mask = 0x01 << i;
        // ���õ�ǰʹ���߳�(CPU)
        SetThreadAffinityMask(GetCurrentThread(), mask);
        // ��ȡ�¶ȼĴ�����eax&0x7f0000���Ի���¶����ݣ�
        Rdmsr(0x19c, &eax, &edx);
        // ʵ���¶�= tjmax - �¶�����
        m_cpuTemp[i] = m_tjmax - ((eax & 0x7f0000) >> 16);
    }

    // ����ƽ���¶�
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

// �ɹ�����CPUʹ����
// ʧ�ܷ��� -1
double CpuInfo::ReadCpuUseRate(void) const
{
    if (!m_isLoad) {
        return -1;
    }
    static HANDLE hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    BOOL res;
    // ��һ��cup�Կ������ܵĿ���ʱ��
    FILETIME preidleTime;
    // ��һ��cup�Կ������ܵ��ں˽���ռ��ʱ��
    FILETIME prekernelTime;
    // ��һ��cup�Կ������ܵ��û�����ռ��ʱ��
    FILETIME preuserTime;
    // ��ǰcup�Կ������ܵĿ���ʱ��
    FILETIME idleTime;
    // ��ǰcup�Կ������ܵ��ں˽���ռ��ʱ��
    FILETIME kernelTime;
    // ��ǰcup�Կ������ܵ��û�����ռ��ʱ��
    FILETIME userTime;
    
    // ��ȡһ��cupռ��ʱ�䣬��0.75����ٴλ�ȡ�µ�ʱ��
    res = GetSystemTimes(&idleTime, &kernelTime, &userTime);
    preidleTime = idleTime;
    prekernelTime = kernelTime;
    preuserTime = userTime;
    // ��ʼֵΪ nonsignaled ������ÿ�δ������Զ�����Ϊnonsignaled
    WaitForSingleObject(hEvent, 750);
    res = GetSystemTimes(&idleTime, &kernelTime, &userTime);
    // һ���ڵ�cup����ʱ��
    uint64_t idle = CompareFileTime(preidleTime, idleTime);
    // һ�����ں˽���cup��ռ��ʱ��
    uint64_t kernel = CompareFileTime(prekernelTime, kernelTime);
    // һ�����û�����ռ��cpu��ʱ��
    uint64_t user = CompareFileTime(preuserTime, userTime);
    // ���ܵ�ʱ��-����ʱ�䣩 / �ܵ�ʱ�� = ռ��cpuʱ���ʹ����
    double cpu = (kernel + user - idle) * 100.0 / (kernel + user);
    return cpu;
}


bool CpuInfo::_CpuInfoInit(void)
{
    // dll�ĳ�ʼ��
    DWORD ret = InitializeOls();
    if (0 == ret){
        // �Ƿ�֧��Cpuid
        if (TRUE == IsCpuid()){
            // �Ƿ�֧��Rdmsr
            if (TRUE == IsMsr()){
                DWORD eax, edx;
                // ���Tjmax
                Rdmsr(0x1A2, &eax, &edx);
                m_tjmax = (eax & 0xff0000) >> 16;

                // ����߼�������������
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
    // ж��dll
    if (m_isLoad) {
        DeinitializeOls();
        delete[] m_cpuTemp;
        m_isLoad = false;
     }
}