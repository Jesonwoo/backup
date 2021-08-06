/*
 * Filename:  StringUtils.cpp
 * Project :  LMPCore
 * Created by Jeson on 9/4/2019.
 * Copyright © 2019 Guangzhou AiYunJi Inc. All rights reserved.
 */
#include "StringUtils.h"

std::string Utf8ToGB2312(const char* utf8)
{
    int len = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0);
    wchar_t* wstr = new wchar_t[len + 1];
    memset(wstr, 0, len + 1);
    MultiByteToWideChar(CP_UTF8, 0, utf8, -1, wstr, len);
    len = WideCharToMultiByte(CP_ACP, 0, wstr, -1, NULL, 0, NULL, NULL);
    char* str = new char[len + 1];
    memset(str, 0, len + 1);
    WideCharToMultiByte(CP_ACP, 0, wstr, -1, str, len, NULL, NULL);
    std::string strTemp = str;
    if (wstr)
        delete[] wstr;
    if (str)
        delete[] str;

    return strTemp;
}

std::string GB2312ToUtf8(const char* gb2312)
{
    int len = MultiByteToWideChar(CP_ACP, 0, gb2312, -1, NULL, 0);
    wchar_t* wstr = new wchar_t[len + 1];
    memset(wstr, 0, len + 1);
    MultiByteToWideChar(CP_ACP, 0, gb2312, -1, wstr, len);
    len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
    char* str = new char[len + 1];
    memset(str, 0, len + 1);
    WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, len, NULL, NULL);
    std::string strTemp = str;
    if (wstr)
        delete[] wstr;
    if (str)
        delete[] str;

    return strTemp;
}

std::string WStringToString(const std::wstring &wstr)
{
    std::string str(wstr.length(), ' ');
    std::copy(wstr.begin(), wstr.end(), str.begin());
    return str;
}


std::wstring StringToWString(const std::string & str)
{
    std::wstring result;
    int len = MultiByteToWideChar(CP_ACP, 0, str.c_str(), (int)str.size(), NULL, 0);
    TCHAR* buffer = new TCHAR[len + 1];
    MultiByteToWideChar(CP_ACP, 0, str.c_str(), (int)str.size(), buffer, len);
    buffer[len] = '\0';
    result.append(buffer);
    delete[] buffer;
    return result;
}

void WcharToString(std::string & str, wchar_t *wchar)
{
    wchar_t * wstr = wchar;
    DWORD dwNum = WideCharToMultiByte(CP_OEMCP, NULL, wstr, -1, NULL, 0, NULL, FALSE);
    char *psText;
    psText = new char[dwNum];
    WideCharToMultiByte(CP_OEMCP, NULL, wstr, -1, psText, dwNum, NULL, FALSE);
    str = psText;
    delete[]psText;
}

CORE_EXPORT std::string StrXOR(const std::string & str)
{
    // choose a power of two => then compiler can replace "modulo x" by much faster "and (x-1)"
    const size_t passwordLength = 16;
    // at least as long as passwordLength, can be longer, too ...
    static const unsigned char password[passwordLength] =
    {
        0x68, 0x19, 0x0A, 0x5D,
        0x26, 0x81, 0xEA, 0x6F,
        0x57, 0xE1, 0xF4, 0xD5,
        0xBC, 0xF5, 0xF1, 0x34,
    };
    // out = in XOR NOT(password)
    std::string result = str;
    for (size_t i = 0; i < str.length(); i++) {
        result[i] ^= ~password[i % passwordLength];
    }
    return result;
}
