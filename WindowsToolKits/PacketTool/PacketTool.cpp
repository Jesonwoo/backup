#include <iostream>
#include <sstream>
#include <Windows.h>
#include <map>
#include "PacketTool.h"
#include "inifile.h"
#include "CopyFileClass.h"

// 全局变量
std::map<std::string, std::string> kSelfVariables;   // 用户自定义的变量
std::map<std::string, std::vector<std::string>> kFilePath; // map<文件名，源路径>


void __stdcall MyCallBackFun(DWORD flag, DWORD number, void* args)
{
    if (flag == CopyFileTool::BEGIN) {
        DEBUG_LOG("文件开始拷贝");
    }
    else if (flag == CopyFileTool::FAILED) {
        DEBUG_LOG("其他错误");
    }
    else if (flag == CopyFileTool::FILE_NOT_FOUND) {
        DEBUG_LOG("源文件不存在");
    }
    else if (flag == CopyFileTool::REQUEST_ABORTED) {
        DEBUG_LOG("操作被中断");
    }
    else if (flag == CopyFileTool::TARGET_DIR_NOT_EXIT) {
        DEBUG_LOG("目录不存在");
    }
}

// 解析自定义变量
BOOL AnalyVariable(inifile::IniItem& item) 
{
    size_t begin = 0, end = 0; 
    char key[MAX_PATH];        // 存储关键字key
    std::string path;          // 存储关键字对应value的解析

    // 解析关键字以及对应的值
    while ((begin = item.value.find('$',end)) != (size_t)-1)
    {
        if (begin > end) {
            path.append(item.value.data() + end, begin - end);
        }
        end = item.value.find(')',begin);
        end++;
        
        //拷贝值中的$(...)
        item.value.copy(key, end - begin, begin);
        key[end - begin] = 0;

        if ( kSelfVariables.find(key) == kSelfVariables.end()) {
            // 没找到定义的关键字
            DEBUG_LOG("no find user's Variables[%s]",key);
            return false;
        }else {
            path.append(kSelfVariables[key].data());
        }

        key[0] = 0;
    }
    if (end != item.value.size()) {
        path.append(item.value.data() + end, item.value.size() - end);
    }

    // 将关键字和值插入到map中
    string var;
    var = "$(" + item.key + ")";
    kSelfVariables[var] = path.data();
    return true;
}

void split(const string& s, vector<string>& sv, const char flag = ' ') 
{
    std::istringstream iss(s);
    string temp;
    while (getline(iss, temp, flag)) {
        sv.push_back(temp);
    }
}

// 解析路径
BOOL AnalyPath(string& str)
{
    size_t offset = 0;
    size_t begin = 0,end = 0;
    char key[MAX_PATH];
    while ((begin = str.find('$', offset)) != (size_t)-1)
    {
        end = str.find(')', begin);
        end++;
        memcpy(key, str.data() + begin, end - begin);
        key[end-begin] = 0;
        if (kSelfVariables.find(key) == kSelfVariables.end()) {
            DEBUG_LOG("no find varibility[%s]", key);
            return false;
        }else {
            str.replace(begin, end - begin, kSelfVariables[key]);
        }        
    }
    return true;
}

BOOL AnalySection(inifile::IniFile& ini,string& section)
{
    DWORD ret;
    // 查看节中是否有dest字段
    vector<string> destination;  // 存储本节(section)下默认的目的路径
    vector<string> source;       // 存储本节下的默认源路径

    inifile::IniSection* pSec = ini.getSection(section);
    for (auto& i : *pSec) {
        if (i.key.compare(SOURCE) == 0) {
            source.push_back(i.value);
        }else if (i.key.compare(DESTINATION) == 0) {
            destination.push_back(i.value);
        }
    }
    if (source.empty() || destination.empty()) {
        // src 或者 dest 未设置
        DEBUG_LOG("in this section[%s],don't have src or dest", section.data());
        return false;
    }
    AnalyPath(source[0]);
    AnalyPath(destination[0]);

    // 分割节中file字段
    vector<string> tmp;  // 存储key(file)对应的values
    ret = ini.GetValues(section.data(), FILE, &tmp);
    if (ret == ERR_NOT_FOUND_KEY) {
        DEBUG_LOG("no find key[%s:%s]", FILE,section.data());
        return false;
    }
    
    vector<string> vstr;
    for (auto& i : tmp) {
        split(i, vstr, '+');
    }

    for (auto& i : vstr) {
        // 去除首尾空格
        size_t n = i.find_first_not_of(" ");
        if (n != string::npos) {
            i.erase(0, n);
        }
        n = i.find_last_not_of(" ");
        if (n != string::npos) {
            i.erase(n + 1, i.size() - n);
        }
    }
    searchCatalog(source[0], vstr);
    if (vstr.size() != kFilePath.size()) {
        for (auto& i : vstr) {
            if (kFilePath.find(i.data()) == kFilePath.end()) {
                DEBUG_LOG("[%s] file can't find in %s", i.data(), source[0].data());
            }
        }
    }

    // 判断源路径下是否有多个相同的文件
    for (auto& i : kFilePath) {
        if (i.second.size() > 1) {
            DEBUG_LOG("There are multiple files[%s] with the same name",i.first.data());
            for (auto& j : i.second) {
                DEBUG_LOG("\tdir:%s", j.data());
            }
        }else {
            char path[MAX_PATH];
            char path2[MAX_PATH];
            sprintf_s(path, "%s\\%s", i.second[0].data(), i.first.data());
            sprintf_s(path2, "%s\\%s", destination[0].data(), i.first.data());
            CopyFileTool tool(path,path2);
            ret = tool.init(MyCallBackFun,1, NULL, false);
        }
    }
    
    kFilePath.clear();
    return true;
}

void searchCatalog(string& root,vector<string>& fileNames) {
    string temp;
    temp = root;
    temp.append("\\*");
    HANDLE handle;
    WIN32_FIND_DATAA winFindData;
    
    handle = FindFirstFileA(temp.data(), &winFindData);
    if (!FindNextFileA(handle, &winFindData)) {
        FindClose(handle);
        return;
    }
    while (FindNextFileA(handle, &winFindData)) {
        if (winFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            string roottemp;
            roottemp = root;
            roottemp.append("\\");
            roottemp.append(winFindData.cFileName);
            searchCatalog(roottemp, fileNames);
        }else if(winFindData.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE){           
            for (auto& i:fileNames)
            {
                if (i.compare(winFindData.cFileName) == 0) {
                    kFilePath[i].push_back(root);
                    break;
                }
            }         
        }
    }
    // 关闭资源
    FindClose(handle);
}

// PacketTool -p $(SolutionDir) -f init.ini
int main(int argc,char** argv)
{
    DWORD ret;
    char* p =NULL;
    // 1、判断用户是否传入参数
    for (int i = 0; i < argc; i++) {
        if (memcmp(argv[i], "-p", 2) == 0) {
            // 找到-p参数
            kSelfVariables[ROOTDIR]= argv[i + 1];
        }else if (memcmp(argv[i], "-f", 2) == 0) {
            // 临时存储配置文件的路径
            p = argv[i + 1];  
        }
    }
    // 判断是否有传入$(SolutionDir)
    if (kSelfVariables.find(ROOTDIR) == kSelfVariables.end()) {
        // 没传入根路径
        DEBUG_LOG("this program need -p parameter");
        return 1;
    }

    // 配置文件路径
    CHAR filePath[MAX_PATH];    // 存放配置文件的绝对路径
    if (p == NULL) {
        // 用户没传入配置文件的路径，默认工作路径下init.ini文件
        GetCurrentDirectoryA(MAX_PATH, filePath);
        strcat_s(filePath, "\\init.ini");

    }else if (p) {
        // 判断传入的路径是绝对路径还是相对路径
        if (memcmp(p, ".", 1) == 0 || memcmp(p, "..", 2) == 0 || *(p+2) != ':') {
            // 相对路径
            GetCurrentDirectoryA(MAX_PATH, filePath);
            strcat_s(filePath, "\\");
            strcat_s(filePath, p);
        }
        else {
            // 绝对路径
            strcpy_s(filePath, p);
        }
    }
   
    // 加载配置文件
    inifile::IniFile ini;
    ret = ini.Load(filePath);
    if (ret) {
        DEBUG_LOG("init load failed,error:%x", ret);
        return 2;
    }
    // 获取用户自定义的变量，并存入map中
    inifile::IniSection* pSec = ini.getSection(REGISTER_INFO);
    for (auto& i : *pSec) {
        if (i.key.compare("section") != 0) {
            if (!AnalyVariable(i)) {
                return 3;
            }
        }
    }
    // 解析注册的节名
    for (auto& i : *pSec) {
        if (i.key.compare("section") == 0) {
            if (!AnalySection(ini, i.value)) {
                return 4;
            }
        }
    }
    return 0;
}
