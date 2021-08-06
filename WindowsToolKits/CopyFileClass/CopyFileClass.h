#pragma once
#include<Windows.h>
#include "common/typedefs.h"
#include<string>

typedef void(__stdcall* CallBackFun)(DWORD flag, DWORD number, void* args);

class CORE_EXPORT CopyFileTool
{
public:
    enum {
        BEGIN,                 // 操作开始
        FAILED,                // 未知错误
        FILE_NOT_FOUND,        // 源文件不存在
        REQUEST_ABORTED,       // 操作被拒绝
        TARGET_DIR_NOT_EXIT,   // 路径非法
        FILENAME_EXCED_RANGE,  // 路径过长
        RELATIVE_PATHNAME      // 目标路径是绝对路径
    };
public:
    // CopyFileTool
    // ExistingFileName: 存在的文件名
    // NewFileName:      新的文件名
    // remark: 路径必须使用\\,且为绝对路径,且路径长度不能超过248（包括结尾符号）
    //          例如: "D:\\dir1\\dir2\\file"
    CopyFileTool(const WCHAR* ExistingFileName, const WCHAR* NewFileName);
    CopyFileTool(std::wstring& ExistingFileName, std::wstring& NewFileName);
    CopyFileTool(const char* lpExistingFileName, const char* lpNewFileName);
    virtual ~CopyFileTool();

    // init：  进行初始化操作
    // func:   回调函数，用于报告是否初始化成功
    // number: 标识哪个对象成功
    // 返回值: 初始化成功返回true，失败返回false
    // remark: 当ExistingFileName或NewFileName 
    //         或回调函数func中其中任意一个参数为空，则初始化失败；
    //         目标路径不正确也会导致初始化失败
    bool   init(CallBackFun func, DWORD number, void* args, BOOL asynchronous = false);

    // CancleOperator：取消当前对象的拷贝操作
    // remark:若文件未拷贝完，则取消当前拷贝操作。
    //        若文件已拷贝完，则无任何操作
    void   CancleOperator(void);

    // GetProgress: 获取当前拷贝进度
    double GetProgress(void);
private:
    // 回调函数
    friend DWORD ProgressRoutine(
        LARGE_INTEGER TotalFileSize,
        LARGE_INTEGER TotalBytesTransferred,
        LARGE_INTEGER StreamSize,
        LARGE_INTEGER StreamBytesTransferred,
        DWORD dwStreamNumber,
        DWORD dwCallbackReason,
        HANDLE hSourceFile,
        HANDLE hDestinationFile,
        LPVOID lpData
    );

    friend DWORD CopyOperatorImpl(void* args);
private:
    CallBackFun m_callBackFun;           // 回调函数
    DWORD  m_number;                     // 序号，用于标识哪个对象
    void*  m_args;                       // 回调参数
    BOOL   m_isCancle;                   // 是否取消当前操作
    double m_progress;                   // 操作进度
    HANDLE m_hThread;                    // 线程句柄
    std::wstring m_existingFileName;     // 存在的文件名
    std::wstring m_newFileName;          // 新的文件名
};