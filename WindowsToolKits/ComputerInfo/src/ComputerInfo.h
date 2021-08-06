#ifndef HARDWAREINFO_H
#define HARDWAREINFO_H
#include <QObject>
#include <Windows.h>
#include "hardware/HardwareInfo.h"

class ComputerInfo : public QObject,public CpuInfo,public GpuInfo
{
    Q_OBJECT
    Q_PROPERTY(bool IsCpuLoad READ (CpuInfo::IsLoad))
    Q_PROPERTY(float cpuTemp READ ReadCpuAvgTemp)
    Q_PROPERTY(double usedRate READ ReadCpuUseRate)
    Q_PROPERTY(bool IsGpuLoad READ (GpuInfo::IsLoad))
    Q_PROPERTY(unsigned long gpuLoad READ GetGpuLoad)
    Q_PROPERTY(double gpuTemp READ GetAvgTemp)
    Q_PROPERTY(double gpuTotalMem READ GetGpuTotalMem)
    Q_PROPERTY(double gpuFreeMem READ GetGpuFreeMem)

public:
    explicit ComputerInfo(QObject *parent = nullptr);

    Q_INVOKABLE QString getMemInfo2Json(void);
    Q_INVOKABLE QString getDiskInfo2Json(void);
    Q_INVOKABLE QString getGpuMemInfo2Json(void);

private:
    quint64     m_totalPhy;
    quint64     m_availPhy;
    qint32      m_totalGpuMem;
    qint32      m_freeGpuMem;
    DiskInfoMap m_disk;
};

#endif // ComputerInfo_H
