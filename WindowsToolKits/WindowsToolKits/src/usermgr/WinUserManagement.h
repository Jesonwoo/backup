#pragma once
#include <common/typedefs.h>
extern "C" {
    /**
    * 添加Windows用户
    * name: 用户名
    * passwd：用户密码
    * 返回值：成功返回0，其他为失败，以下为错误码：
        ​ 2224：表示用户已经存在
        ​ 2245：表示密码过短
    */
    CORE_EXPORT int AddUser(const char * name, const char * passwd);
    /**
    * 添加Windows用户到Administrators用户组里面
    * name: 用户名
    * 返回值：成功返回0，其他为失败，以下为错误码：
            ​ 2221：表示用户不存在
    */
    CORE_EXPORT int AddUserToAdministrators(const char * name);
    /**
    * 删除Windows用户
    * name: 用户名
    * 返回值：成功返回0，其他为失败，以下为错误码：
            ​ 2221：表示用户不存在
    */
    CORE_EXPORT int DeleteUser(const char * name);
    //CORE_EXPORT int AddUserToSharedPath(const char * name, const char * path);
}
