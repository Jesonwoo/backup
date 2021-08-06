#pragma once
#include "common/typedefs.h"
#include<windows.h>
#include "R450-developer/nvapi.h"
/*
example:����Ҫ����ԱȨ�ޣ�
	GpuInfo gpu;
	std::cout << "GPUռ����:"   << gpu.GetGpuLoad()     << std::endl;
	std::cout << "GPU�¶�:"     << gpu.GetAvgTemp()     << std::endl;
	std::cout << "GPU������:"   << gpu.GetGpuTotalMem() << std::endl;
	std::cout << "GPU��������:" << gpu.GetGpuFreeMem()  << std::endl;

*/
class CORE_EXPORT GpuInfo
{
public:
    GpuInfo();
    bool          IsLoad(void);          // �ж��Ƿ���سɹ�
    unsigned long GetGpuLoad(void);
    double        GetAvgTemp(void);
    unsigned int           GetGpuTotalMem(void);
    unsigned int           GetGpuFreeMem(void);
private:
    bool                    m_isLoad;    // �Ƿ���سɹ�
    NvPhysicalGpuHandle     m_phys;      // an array of physical GPU handles
    NvU32                   m_cnt;       // how many entries in the array are valid
    NV_GPU_THERMAL_SETTINGS m_thermal;   // GPU�¶ȴ�������Ϣ
};