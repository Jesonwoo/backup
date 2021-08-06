#include "HardwareInfo.h"
#include <Windows.h>
#include <winbase.h>
#include <DXGI.h> 
#include <common/CoreLog.hpp>
#include "common/StringUtils.h"
#include <mutex>
#include <string>

using namespace std;
#define BUFFER_SIZE 128

static std::string kUUID;
static std::string kCPUName;
static std::string kCPUPorcessId;
static std::string kCPUDeviceId;
static std::string kBaseBoardSerialNumber;
static std::string kBaseBoardModel;
static std::string kBaseBoardProduct;
static std::string kDefaultGraphicCardDesc;
static std::mutex kMutex;

string GetDevcieInfo(const string& cmd, const string & target) {
    char buffer[BUFFER_SIZE];
    string result;
    FILE* pipe = _popen(cmd.c_str(), "r"); //打开管道，并执行命令 
    if (!pipe) {
        return result;
    }
    if (fgets(buffer, BUFFER_SIZE, pipe)) {
        while (!feof(pipe)) {
            string buf(buffer);
            if (buf.find(target) == string::npos) {
                for (auto & c : buf) {
                    if (c != ' ' && c != '\r' && c != '\n') {
                        result.push_back(c);
                    }
                }
            }
            fgets(buffer, BUFFER_SIZE, pipe);
        }
    }
    _pclose(pipe); // 关闭管道 
    return result;
}

static std::string GetHardInformationByCmd(const char * cmd, const char * endsearch)
{
    const long kCommandSize = 10000;  // size of output buffer	
    const std::string search = endsearch;

    BOOL   bret = false;
    HANDLE readPipe = NULL;
    HANDLE writePipe = NULL;
    PROCESS_INFORMATION pi;   // process information
    STARTUPINFOA        si;	  // Control command line window information
    SECURITY_ATTRIBUTES sa;   // security attributes

    // Place the output buffer for the command line results
    char			outBuffer[kCommandSize + 1] = { 0 };
    std::string	    strBuffer;
    std::string     information;
    unsigned long   count = 0;
    size_t          ipos = 0;

    memset(&pi, 0, sizeof(pi));
    memset(&si, 0, sizeof(si));
    memset(&sa, 0, sizeof(sa));

    pi.hProcess = NULL;
    pi.hThread = NULL;
    si.cb = sizeof(STARTUPINFO);
    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.lpSecurityDescriptor = NULL;
    sa.bInheritHandle = true;

    // Create pipe
    bret = CreatePipe(&readPipe, &writePipe, &sa, 0);
    if (!bret) {
        goto END;
    }

    // Sets the command line window information to the specified read - write channel
    GetStartupInfoA(&si);
    si.hStdError = writePipe;
    si.hStdOutput = writePipe;
    si.wShowWindow = SW_HIDE; // hide window
    si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;

    // Create a process to get the command line
    bret = CreateProcessA(NULL, (char *)cmd, NULL, NULL, true, 0, NULL, NULL, &si, &pi);
    if (!bret) {
        goto END;
    }

    // Read data
    WaitForSingleObject(pi.hProcess, 500/*INFINITE*/);
    bret = ReadFile(readPipe, outBuffer, kCommandSize, &count, 0);
    if (!bret) {
        goto END;
    }

    bret = false;
    strBuffer = outBuffer;
    if ((ipos = strBuffer.find(endsearch)) < 0) {
        goto END;
    } else {
        strBuffer = strBuffer.substr(ipos + search.length());
    }

    memset(outBuffer, 0x00, sizeof(outBuffer));
    strcpy_s(outBuffer, strBuffer.c_str());

    // Remove spacing \r \n
    for (int i = 0; i < strlen(outBuffer); i++) {
        if (outBuffer[i] != ' ' && outBuffer[i] != '\n' && outBuffer[i] != '\r') {
            information.push_back(outBuffer[i]);
        }
    }

    bret = true;
END:
    CloseHandle(writePipe);
    CloseHandle(readPipe);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return information;
}

const char * Getuuid()
{
    std::lock_guard<std::mutex> lock(kMutex);
    //if (kUUID.empty()) {
    kUUID = GetHardInformationByCmd("wmic csproduct get UUID", "UUID");
    //}
    LOGI("Get uuid length{%d}", kUUID.size());
    return kUUID.c_str();
}

const char * GetCPUName()
{
    std::lock_guard<std::mutex> lock(kMutex);
    //if (kCPUName.empty()) {
    kCPUName = GetHardInformationByCmd("wmic CPU get Name", "Name");
    //}
    return kCPUName.c_str();
}

const char * GetCPUProcessId()
{
    std::lock_guard<std::mutex> lock(kMutex);
    //if (kCPUPorcessId.empty()) {
    kCPUPorcessId = GetHardInformationByCmd("wmic CPU get ProcessorId", "ProcessorId");
    //}
    return kCPUPorcessId.c_str();
}

const char * GetCPUDeviceId()
{
    std::lock_guard<std::mutex> lock(kMutex);
    //if (kCPUDeviceId.empty()) {
    kCPUDeviceId = GetHardInformationByCmd("wmic CPU get DeviceID", "DeviceID");
    //}
    return kCPUDeviceId.c_str();
}

const char * GetBaseBoardSerialNumber()
{
    std::lock_guard<std::mutex> lock(kMutex);
    //if (kBaseBoardSerialNumber.empty()) {
    kBaseBoardSerialNumber = GetHardInformationByCmd("wmic BaseBoard get SerialNumber", "SerialNumber");
    //}
    return kBaseBoardSerialNumber.c_str();
}

const char * GetBaseBoardModel()
{
    std::lock_guard<std::mutex> lock(kMutex);
    //if (kBaseBoardModel.empty()) {
    kBaseBoardModel = GetHardInformationByCmd("wmic BaseBoard get Model", "Model");
    //}
    return kBaseBoardModel.c_str();
}

const char * GetBaseBoardProduct()
{
    std::lock_guard<std::mutex> lock(kMutex);
    //if (kBaseBoardProduct.empty()) {
    kBaseBoardProduct = GetHardInformationByCmd("wmic BaseBoard get Product", "Product");
    //}
    return kBaseBoardProduct.c_str();
}

static bool GetGraphicsCardDescriptions(std::vector<std::string> & graphicsCardDescriptions)
{
    IDXGIFactory * pFactory;
    IDXGIAdapter * adapter;
    std::vector <IDXGIAdapter*> adapters;
    int adapterNum = 0;

    HRESULT hr = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)(&pFactory));
    if (FAILED(hr)) {
        return false;
    }

    // enum adapters
    while (pFactory->EnumAdapters(adapterNum, &adapter) != DXGI_ERROR_NOT_FOUND) {
        adapters.push_back(adapter);
        ++adapterNum;
    }

    graphicsCardDescriptions.clear();
    for (size_t i = 0; i < adapters.size(); i++) {
        DXGI_ADAPTER_DESC adapterDesc;
        adapters[i]->GetDesc(&adapterDesc);
        std::wstring wstr(adapterDesc.Description);
        std::string desc = WStringToString(wstr);
        graphicsCardDescriptions.push_back(desc);
    }
    adapters.clear();
    return true;
}

const char * GetDefaultGraphicCardDesc()
{
    std::lock_guard<std::mutex> lock(kMutex);
    //if (kDefaultGraphicCardDesc.empty()) {
    std::vector<std::string> graphicsCardDescriptions;
    if (GetGraphicsCardDescriptions(graphicsCardDescriptions)) {
        kDefaultGraphicCardDesc = graphicsCardDescriptions[0];
    }
    //}
    return kDefaultGraphicCardDesc.c_str();
}

// 成功返回内存总容量
// 失败返回 -1
int64_t GetTotalMem(void)
{
    MEMORYSTATUSEX sysMemStatus;
    sysMemStatus.dwLength = sizeof(sysMemStatus);
    if (!GlobalMemoryStatusEx(&sysMemStatus)) {
        // 函数调用失败,使用GetLastError获取失败原因
        return -1;
    }
    return sysMemStatus.ullTotalPhys;
}

// 成功返回内存空闲容量
// 失败返回 -1
int64_t GetFreeMem(void)
{
    MEMORYSTATUSEX sysMemStatus;
    sysMemStatus.dwLength = sizeof(sysMemStatus);
    if (!GlobalMemoryStatusEx(&sysMemStatus)) {
        // 函数调用失败,使用GetLastError获取失败原因
        return -1;
    }
    return sysMemStatus.ullAvailPhys;
}

DWORD GetPhysicalDriveFromPartitionLetter(CHAR letter)
{
    HANDLE hDevice;                 // handle to the drive to be examined
    BOOL result;                    // results flag
    DWORD readed;                   // discard results
    STORAGE_DEVICE_NUMBER number;   // use this to get disk numbers
    CHAR path[MAX_PATH];

    sprintf_s(path, "\\\\.\\%c:", letter);
    hDevice = CreateFileA(path,          // drive to open
        GENERIC_READ | GENERIC_WRITE,    // access to the drive
        FILE_SHARE_READ | FILE_SHARE_WRITE,    //share mode
        NULL,             // default security attributes
        OPEN_EXISTING,    // disposition
        0,                // file attributes
        NULL);            // do not copy file attribute
    if (hDevice == INVALID_HANDLE_VALUE){
        LOGE("CreateFile[%c] Error: %ld", letter, GetLastError());
        return DWORD(-1);
    }

    result = DeviceIoControl(
        hDevice,                         // handle to device
        IOCTL_STORAGE_GET_DEVICE_NUMBER, // dwIoControlCode
        NULL,                            // lpInBuffer
        0,                               // nInBufferSize
        &number,                // output buffer
        sizeof(number),         // size of output buffer
        &readed,                // number of bytes returned
        NULL                    // OVERLAPPED structure
    );
    if (!result) {
        LOGE( "IOCTL_STORAGE_GET_DEVICE_NUMBER Error: %ld", GetLastError());
        CloseHandle(hDevice);
        return (DWORD)-1;
    }
    // 关闭资源
    CloseHandle(hDevice);
    return number.DeviceNumber;
}

const char* GetDiskSerialByPartition(const char* ch)
{
    std::lock_guard<std::mutex> lock(kMutex);
    static string result;
    DWORD DeviceNumber;
    string str = "wmic diskdrive where deviceid='\\\\\\\\.\\\\PHYSICALDRIVE";

    if ((*ch >= 'a' && *ch <= 'z') || (*ch >= 'A' && *ch <= 'Z')) {
        DeviceNumber = GetPhysicalDriveFromPartitionLetter(*ch);
        if (DeviceNumber != (DWORD)-1) {
            str += to_string(DeviceNumber);
            str += "\' get SerialNumber";
            result = GetHardInformationByCmd(str.c_str(), "SerialNumber");
            return result.c_str();
        }
    }
    result.clear();
    return result.c_str();
}