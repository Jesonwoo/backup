/*
 * Filename:  StringUtils.cpp
 * Project :  LMPCore
 * Created by Jeson on 9/4/2019.
 * Copyright © 2019 Guangzhou AiYunJi Inc. All rights reserved.
 */
#ifndef STRING_UTILS_H
#define STRING_UTILS_H
#include "common/typedefs.h"
CORE_EXPORT std::string GB2312ToUtf8(const char* gb2312);
CORE_EXPORT std::string Utf8ToGB2312(const char* utf8);
CORE_EXPORT std::string WStringToString(const std::wstring&wstr);
CORE_EXPORT std::wstring StringToWString(const std::string & str);
CORE_EXPORT void WcharToString(std::string & str, wchar_t * wchar);
CORE_EXPORT std::string StrXOR(const std::string & str);

#define STR_XOR(str) StrXOR(str).c_str() 
#endif // !STRING_UTILS_H

