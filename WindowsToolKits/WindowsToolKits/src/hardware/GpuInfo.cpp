#include "GpuInfo.h"
#pragma comment(lib,"3rd/lib/nvapi64.lib")

GpuInfo::GpuInfo()
    :m_isLoad(false)
{
    NvAPI_Status ret;

    // ��ȡGPU����������Ͷ�Ӧ���
    ret = NvAPI_EnumPhysicalGPUs(&m_phys, &m_cnt);
    if (ret == NVAPI_OK){
        m_isLoad = true;
    }
}

bool GpuInfo::IsLoad(void)
{
    return m_isLoad;
}

// �ɹ�����GPU����ռ���ʣ�
// ʧ�ܷ���-1
unsigned long GpuInfo::GetGpuLoad(void)
{
    if (!m_isLoad) {
        return -1;
    }
    NV_GPU_DYNAMIC_PSTATES_INFO_EX infoEx;
    NvAPI_Status ret;
    infoEx.version = NV_GPU_DYNAMIC_PSTATES_INFO_EX_VER;

    ret = NvAPI_GPU_GetDynamicPstatesInfoEx(m_phys, &infoEx);
    if (!ret == NVAPI_OK) {
        return (unsigned long)-1;
    }

    return infoEx.utilization[0].bIsPresent
        ? infoEx.utilization[0].percentage
        : (unsigned long)-1;
}

// �ɹ�����GPUƽ���¶�
// ʧ�ܷ��� 0 
double GpuInfo::GetAvgTemp(void)
{
    if (!m_isLoad) {
        return 0;
    }
    NvAPI_Status ret;
    m_thermal.version = NV_GPU_THERMAL_SETTINGS_VER;
    ret = NvAPI_GPU_GetThermalSettings(m_phys, NVAPI_THERMAL_TARGET_ALL, &m_thermal);
    if (!ret == NVAPI_OK) {
        return 0;
    }

    double avgTemp = 0;
    for (NvU32 i = 0; i < m_thermal.count; i++){
        avgTemp += m_thermal.sensor[i].currentTemp;
    }
    return avgTemp /= m_thermal.count;
}

// �ɹ�����GPU�ڴ�������
// ʧ�ܷ��� -1
unsigned int GpuInfo::GetGpuTotalMem(void)
{
    if (!m_isLoad){
        return -1;
    }
    NvAPI_Status ret;
    NV_DISPLAY_DRIVER_MEMORY_INFO memInfo;
    memInfo.version = NV_DISPLAY_DRIVER_MEMORY_INFO_VER_3;

    ret = NvAPI_GPU_GetMemoryInfo(m_phys, &memInfo);
    if (ret != NVAPI_OK) {
        return -1;
    }
    return memInfo.dedicatedVideoMemory;
}

// �ɹ�����GPU�ڴ��������
// ʧ�ܷ��� -1
unsigned int GpuInfo::GetGpuFreeMem(void)
{
    if (!m_isLoad) {
        return -1;
    }
    NvAPI_Status ret;
    NV_DISPLAY_DRIVER_MEMORY_INFO memInfo;
    memInfo.version = NV_DISPLAY_DRIVER_MEMORY_INFO_VER_3;

    ret = NvAPI_GPU_GetMemoryInfo(m_phys, &memInfo);
    if (ret != NVAPI_OK) {
        return -1;
    }
    return memInfo.curAvailableDedicatedVideoMemory;
}