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
    // GetDiskSerialByPartition函数需要管理员权限
    // 返回值: 成功时存储的序列号，失败存储""
    CORE_EXPORT const char* GetDiskSerialByPartition(const char* ch);
}

