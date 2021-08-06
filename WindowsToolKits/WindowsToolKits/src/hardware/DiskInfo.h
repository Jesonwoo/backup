#pragma once
#include "common/typedefs.h"
#include<string>
#include<map>
#include<windows.h>
/*
example:
	DiskInfoMap diskInfoMap;
	std::map<std::string, DiskInfoMap::DiskInfo> map = diskInfoMap.GetDisksInfo();

	for (auto i = map.begin(); i != map.end(); i++){
		std::cout << i->first.c_str() << std::endl;
		std::cout << "������:" << i->second.GetTotalMem()  << std::endl;
		std::cout << "��������:" << i->second.GetFreeMem() << std::endl;
	}
*/

class CORE_EXPORT DiskInfoMap
{ 
public:
    class DiskInfo;
    DiskInfoMap();
    std::map<std::string, DiskInfoMap::DiskInfo>& GetDisksInfo(void);
private:
    void _InitDiskInformation(void);
	
public:
    class CORE_EXPORT DiskInfo
    {
    public:
        DiskInfo();
        DiskInfo(DWORD64 totalBytes, DWORD64 freeBytes);
        int64_t GetTotalMem(void);
        int64_t GetFreeMem(void);
    private:
        DWORD64     m_totalBytes;     // ������
        DWORD64     m_freeBytes;      // ��������
    };
private:
    std::map<std::string, DiskInfoMap::DiskInfo> m_diskInfoMap;
};
