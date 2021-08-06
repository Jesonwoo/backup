#pragma once
#include "common/typedefs.h"
#include<windows.h>
#include "R450-developer/nvapi.h"
/*
example:（需要管理员权限）
	GpuInfo gpu;
	std::cout << "GPU占用率:"   << gpu.GetGpuLoad()     << std::endl;
	std::cout << "GPU温度:"     << gpu.GetAvgTemp()     << std::endl;
	std::cout << "GPU总容量:"   << gpu.GetGpuTotalMem() << std::endl;
	std::cout << "GPU空闲容量:" << gpu.GetGpuFreeMem()  << std::endl;

*/
class CORE_EXPORT GpuInfo
{
public:
    GpuInfo();
    bool          IsLoad(void);          // 判断是否加载成功
    unsigned long GetGpuLoad(void);
    double        GetAvgTemp(void);
    unsigned int           GetGpuTotalMem(void);
    unsigned int           GetGpuFreeMem(void);
private:
    bool                    m_isLoad;    // 是否加载成功
    NvPhysicalGpuHandle     m_phys;      // an array of physical GPU handles
    NvU32                   m_cnt;       // how many entries in the array are valid
    NV_GPU_THERMAL_SETTINGS m_thermal;   // GPU温度传感器信息
};