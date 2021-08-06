#include "CopyFileClass.h"
#include <stdio.h>
#include <new>
#include <shlobj_core.h>


CopyFileTool::CopyFileTool(const WCHAR* lpExistingFileName, const WCHAR* lpNewFileName)
    : m_isCancle(FALSE), m_progress(0.0), m_hThread(NULL)
    , m_existingFileName(lpExistingFileName), m_newFileName(lpNewFileName)
    , m_callBackFun(NULL), m_number(0), m_args(NULL)
{
}
CopyFileTool::CopyFileTool(std::wstring & ExistingFileName, std::wstring & NewFileName)
    : m_isCancle(FALSE), m_progress(0.0), m_hThread(NULL)
    , m_existingFileName(ExistingFileName), m_newFileName(NewFileName)
    , m_callBackFun(NULL), m_number(0), m_args(NULL)
{
}
CopyFileTool::CopyFileTool(const char* lpExistingFileName, const char* lpNewFileName)
    : m_isCancle(FALSE), m_progress(0.0), m_hThread(NULL)
    , m_callBackFun(NULL), m_number(0), m_args(NULL)
{
    if (!lpExistingFileName || !lpNewFileName) {
        return;
    }
    int len;
    wchar_t* pwszDst;

    len = MultiByteToWideChar(CP_ACP, 0, lpExistingFileName, -1, NULL, 0);
    if (len == 0) {
        return;
    }
    pwszDst = new wchar_t[len];
    MultiByteToWideChar(CP_ACP, 0, lpExistingFileName, -1, pwszDst, len);
    m_existingFileName = pwszDst;
    delete[] pwszDst;

    len = MultiByteToWideChar(CP_ACP, 0, lpNewFileName, -1, NULL, 0);
    if (len == 0) {
        return;
    }
    pwszDst = new wchar_t[len];
    MultiByteToWideChar(CP_ACP, 0, lpNewFileName, -1, pwszDst, len);
    m_newFileName = pwszDst;
    delete[] pwszDst;
}
CopyFileTool::~CopyFileTool()
{
    CancleOperator();
    WaitForSingleObject(m_hThread, INFINITE);
}

// ProgressRoutine：不对外暴露
// remark:调用用户回调函数报告开始拷贝，以及更新拷贝进度
static DWORD ProgressRoutine(
    LARGE_INTEGER TotalFileSize,
    LARGE_INTEGER TotalBytesTransferred,
    LARGE_INTEGER StreamSize,
    LARGE_INTEGER StreamBytesTransferred,
    DWORD dwStreamNumber,
    DWORD dwCallbackReason,
    HANDLE hSourceFile,
    HANDLE hDestinationFile,
    LPVOID lpData
) {
    CopyFileTool* pTool = (CopyFileTool*)lpData;
    if (dwCallbackReason == CALLBACK_STREAM_SWITCH) {
        // 报告开始拷贝
        pTool->m_callBackFun(CopyFileTool::BEGIN, pTool->m_number, pTool->m_args);
    }
    else if (dwCallbackReason == CALLBACK_CHUNK_FINISHED) {
        // 报告进度     
        pTool->m_progress = (double)TotalBytesTransferred.QuadPart / TotalFileSize.QuadPart;
    }
    return PROGRESS_CONTINUE;
}

// CopyOperatorImpl：不对外暴露
// args: CopyFileTool对象指针
// 返回值: 成功返回0，失败返回-1
// remark: 拷贝操作的具体实现
static DWORD CopyOperatorImpl(void* args)
{
    DWORD ret;
    CopyFileTool* pTool = (CopyFileTool*)args;
    DWORD copyFlags = COPY_FILE_NO_BUFFERING | COPY_FILE_ALLOW_DECRYPTED_DESTINATION;
    ret = CopyFileExW(pTool->m_existingFileName.data(), pTool->m_newFileName.data()
        , ProgressRoutine, args, &pTool->m_isCancle, copyFlags);
    if (!ret) {
        ret = GetLastError();
        if (ret == ERROR_FILE_NOT_FOUND) {
            // 源文件不存在
            pTool->m_callBackFun(CopyFileTool::FILE_NOT_FOUND, pTool->m_number, pTool->m_args);
        }
        else if (ret == 0) {
            // 主动取消操作
            pTool->m_callBackFun(CopyFileTool::REQUEST_ABORTED, pTool->m_number, pTool->m_args);
        }
        else if (ret == 3) {
            // 目录不存在
            pTool->m_callBackFun(CopyFileTool::TARGET_DIR_NOT_EXIT, pTool->m_number, pTool->m_args);
        }
        else {
            // 其他未知错误
            pTool->m_callBackFun(CopyFileTool::FAILED, pTool->m_number, pTool->m_args);
        }
        return ret;
    }
    return ret;
}

bool CopyFileTool::init(CallBackFun func, DWORD number, void* args, BOOL asynchronous)
{
    if (!m_existingFileName.empty() && !m_newFileName.empty() && func) {
        m_callBackFun = func;    // 构造函数
        m_number = number;  // 序号
        m_args = args;    // 回调函数用户自定义参数

        // 判断目标路径是否存在，不存在则创建，存在则无操作
        DWORD ret;
        WCHAR* pointer = NULL;
        wchar_t *path = new (std::nothrow) WCHAR[m_newFileName.capacity()];
        if (path == NULL) {
            return false;
        }
        m_newFileName.copy(path, m_newFileName.capacity());
        pointer = wcsrchr(path, L'\\');
        *pointer = 0;
        ret = SHCreateDirectoryExW(NULL, path, NULL);
        delete[] path;
        if (ret == ERROR_SUCCESS || ret == ERROR_ALREADY_EXISTS) {

        }
        else if (ret == ERROR_FILENAME_EXCED_RANGE) {
            func(FILENAME_EXCED_RANGE, number, args);
            return false;
        }
        else if (ret == ERROR_PATH_NOT_FOUND) {
            func(TARGET_DIR_NOT_EXIT, number, args);
            return false;
        }
        else if (ret == ERROR_BAD_PATHNAME) {
            func(RELATIVE_PATHNAME, number, args);
            return false;
        }
        else {
            func(FAILED, number, args);
            return false;
        }
        // 判断是否要异步处理还是同步处理（默认）
        if (asynchronous) {
            // 创建处理线程       
            m_hThread = CreateThread(NULL, 0, CopyOperatorImpl, this, 0, NULL);
            if (!m_hThread) {
                return false;
            }
        }
        else {
            CopyOperatorImpl(this);
        }
    }
    return true;
}


void CopyFileTool::CancleOperator()
{
    m_isCancle = TRUE;
}

double CopyFileTool::GetProgress(void)
{
    return m_progress;
}