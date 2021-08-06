#include "ComputerInfo.h"

ComputerInfo::ComputerInfo(QObject *parent)
    : QObject(parent)
    ,m_totalPhy(0),m_availPhy(0)
{
}

QString ComputerInfo::getMemInfo2Json(void)
{
   
    QString str="{\"totalMem\":"+QString::number(GetTotalMem())+","
            "\"freeMem\":"+QString::number(GetFreeMem()) +"}";
    return str;
}

QString ComputerInfo::getDiskInfo2Json(void)
{
    QString str="{\"disk\":[";
    DiskInfoMap diskInfoMap;
    DWORD64 totalBytes, freeBytes;
    std::map<std::string, DiskInfoMap::DiskInfo> map = diskInfoMap.GetDisksInfo();
    std::map<std::string, DiskInfoMap::DiskInfo>::iterator i = map.begin();
    for (size_t idx=0;idx < map.size() ;i++){
        str=str+"{"+"\"diskName\":\""+i->first.c_str()+"\\\","+"\"capacity\":["+\
                QString::number(i->second.GetTotalMem()) +","+QString::number(i->second.GetFreeMem())+"]}";
        str+=(++idx < map.size()?",":"");
    }

    str+="]}";
    return str;
}

QString ComputerInfo::getGpuMemInfo2Json(void)
{
    QString str = "{\"totalMem\":" + QString::number(GetGpuTotalMem()) + ","
        "\"freeMem\":" + QString::number(GetGpuFreeMem()) + "}";
    return str;
}



















