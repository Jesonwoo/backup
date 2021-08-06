#pragma once
#include "common/typedefs.h"
#include <string>

/*
example:
	CpuInfo cpu;
    if(cpu.IsLoad()){
	std::cout << "GPUռ����:"   << gpu.GetGpuLoad()     << std::endl;
	std::cout << "GPU�¶�:"     << gpu.GetAvgTemp()     << std::endl;
	std::cout << "GPU������:"   << gpu.GetGpuTotalMem() << std::endl;
	std::cout << "GPU��������:" << gpu.GetGpuFreeMem()  << std::endl;
    }
*/
class CORE_EXPORT CpuInfo
{
public:
    CpuInfo();
    virtual ~CpuInfo();
public:
    bool  IsLoad(void) const;
    double ReadCpuAvgTemp(void)const;         // ��ȡCPUƽ���¶�
    double ReadCpuUseRate(void)const;         // ��ȡCPUƽ��ʹ����
private:
    bool _CpuInfoInit(void);       // ��ʼ������
    void _CpuInfoExit(void);       // �˳�����
private:
    bool m_isLoad;                  // dll�Ƿ���سɹ�
    int* m_cpuTemp;                 // ÿ�����ĵ��¶�
    int  m_coreNumber;              // ��õ�CPU�ĺ�����
    int  m_tjmax;                   // CPU��ߵ��¶ȣ��������ֵʱ���ͻᴥ����ص��¶ȿ��Ƶ�·��
};