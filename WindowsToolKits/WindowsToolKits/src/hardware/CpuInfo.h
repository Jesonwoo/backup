#pragma once
#include "common/typedefs.h"
#include <string>

/*
example:
	CpuInfo cpu;
    if(cpu.IsLoad()){
	std::cout << "GPU占用率:"   << gpu.GetGpuLoad()     << std::endl;
	std::cout << "GPU温度:"     << gpu.GetAvgTemp()     << std::endl;
	std::cout << "GPU总容量:"   << gpu.GetGpuTotalMem() << std::endl;
	std::cout << "GPU空闲容量:" << gpu.GetGpuFreeMem()  << std::endl;
    }
*/
class CORE_EXPORT CpuInfo
{
public:
    CpuInfo();
    virtual ~CpuInfo();
public:
    bool  IsLoad(void) const;
    double ReadCpuAvgTemp(void)const;         // 读取CPU平均温度
    double ReadCpuUseRate(void)const;         // 读取CPU平均使用率
private:
    bool _CpuInfoInit(void);       // 初始化函数
    void _CpuInfoExit(void);       // 退出函数
private:
    bool m_isLoad;                  // dll是否加载成功
    int* m_cpuTemp;                 // 每个核心的温度
    int  m_coreNumber;              // 获得的CPU的核心数
    int  m_tjmax;                   // CPU最高的温度（超过这个值时，就会触发相关的温度控制电路）
};