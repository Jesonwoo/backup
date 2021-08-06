#include "DiskInfo.h"


DiskInfoMap::DiskInfoMap()
{}

// ���ش洢������Ϣ��map
std::map<std::string, DiskInfoMap::DiskInfo>& DiskInfoMap::GetDisksInfo(void)
{
    _InitDiskInformation();
    return m_diskInfoMap;
}

void DiskInfoMap::_InitDiskInformation(void)
{
    // �洢��ȡ�����̷�
    char buf[1024] = { 0 };
    DWORD64 qwFreeBytes, qwTotalBytes;
    m_diskInfoMap.clear();

    // ��ȡ��ǰϵͳ�������̷�
    int len = GetLogicalDriveStringsA(1024, buf);
	
    for (int i = 0; i < len; i++){
        if (buf[i] >= 'A' && buf[i] <= 'Z'){
            if (GetDiskFreeSpaceExA(buf + i ,NULL
                , (PULARGE_INTEGER)&qwTotalBytes
                , (PULARGE_INTEGER)&qwFreeBytes)){
                m_diskInfoMap.insert(std::map<std::string,DiskInfo>::value_type(std::string(buf + i),DiskInfo(qwTotalBytes, qwFreeBytes)));
            }
        }
    }
}

DiskInfoMap::DiskInfo::DiskInfo()
    :m_totalBytes(0),m_freeBytes(0)
{
}

DiskInfoMap::DiskInfo::DiskInfo(DWORD64 totalBytes, DWORD64 freeBytes)
    :m_totalBytes(totalBytes),m_freeBytes(freeBytes)
{
}


int64_t DiskInfoMap::DiskInfo::GetTotalMem(void)
{
    return m_totalBytes;
}

int64_t DiskInfoMap::DiskInfo::GetFreeMem(void)
{
    return m_freeBytes;
}