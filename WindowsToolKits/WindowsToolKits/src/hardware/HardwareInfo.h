#pragma once
#include "common/typedefs.h"
#include "CpuInfo.h"
#include "DiskInfo.h"
#include "GpuInfo.h"

extern "C" {
    // Get windows hardware information
    CORE_EXPORT const char * Getuuid();
    CORE_EXPORT const char * GetCPUName();
    CORE_EXPORT const char * GetCPUProcessId();
    CORE_EXPORT const char * GetCPUDeviceId();
    CORE_EXPORT const char * GetBaseBoardSerialNumber();
    CORE_EXPORT const char * GetBaseBoardModel();
    CORE_EXPORT const char * GetBaseBoardProduct();
    CORE_EXPORT const char * GetDefaultGraphicCardDesc();
    CORE_EXPORT int64_t GetTotalMem(void);
    CORE_EXPORT int64_t GetFreeMem(void);
    // GetDiskSerialByPartition������Ҫ����ԱȨ��
    // ����ֵ: �ɹ�ʱ�洢�����кţ�ʧ�ܴ洢""
    CORE_EXPORT const char* GetDiskSerialByPartition(const char* ch);
}

