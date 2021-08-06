## WindowsToolKits.dll接口文档

### 1.Windows用户相关接口

~~~~c++
/**
* 添加Windows用户
* name: 用户名
* passwd：用户密码
* 返回值：成功返回0，其他为失败，以下为错误码：
      	​ 2224：表示用户已经存在
      	​ 2245：表示密码过短
*/
int AddUser(const char * name, const char * passwd);
/**
* 添加Windows用户到Administrators用户组里面
* name: 用户名
* 返回值：成功返回0，其他为失败，以下为错误码：
		​ 2221：表示用户不存在
*/
int AddUserToAdministrators(const char * name);
/**
* 删除Windows用户
* name: 用户名
* 返回值：成功返回0，其他为失败，以下为错误码：
		​ 2221：表示用户不存在
*/
int DeleteUser(const char * name);
~~~~

### 2.Windows硬件相关接口

~~~~c++
/**
* 获取UUID
* 返回值：成功返回字符串，失败返回0
*/
const char * Getuuid();
/**
* 获取CPU名称
* 返回值：成功返回字符串，失败返回0
*/
const char * GetCPUName();
/**
* 获取CPU进程id
* 返回值：成功返回字符串，失败返回0
*/
const char * GetCPUProcessId();
/**
* 获取CPU设备id
* 返回值：成功返回字符串，失败返回0
*/
const char * GetCPUDeviceId();
/**
* 获取主板序列号
* 返回值：成功返回字符串，失败返回0
*/
const char * GetBaseBoardSerialNumber();
/**
* 获取主板model，如主板厂家为提供则返回字符串为空
* 返回值：成功返回字符串，失败返回0
*/
const char * GetBaseBoardModel();
/**
* 获取主板产品说明
* 返回值：成功返回字符串，失败返回0
*/
const char * GetBaseBoardProduct();
/**
* 获取默认显卡的描述，即显卡名称
* 返回值：成功返回字符串，失败返回0
*/
const char * GetDefaultGraphicCardDesc();
/**
* GetDiskSerialByPartition函数需要管理员权限
* ch:盘符，例如 "C" 或者 "D" 等,
*    函数内部只对参数ch的第一个字符进行判断，其他字符忽略
* 返回值: 成功时存储序列号，失败存储""
*/
const char* GetDiskSerialByPartition(char* ch);
~~~~

### 3.Windows控制相关接口

~~~~c++
/**
* 重置Windows电脑
* flag：重置电脑的方式
	关机：0x00000001
	重启：0x00000002
	关机并断电：0x00000008
* force：是否强制执行重置电脑，true为强制，false为非强制
* 返回值：成功true，失败返回false
*/
bool ResetWindows(DWORD flag, BOOL force);
~~~~

/**
* 获取内存总容量
* 返回值：成功返回内存总容量，失败返回 -1
* int64_t GetTotalMem(void);
*/

/**
* 获取内存空闲容量
* 返回值：成功返回内存总容量，失败返回 -1
* int64_t GetFreeMem(void);
*/

/**
* (!需要管理员权限，加载驱动需要管理员权限)
* 获取是否成功加载CPU相关dll和sys
* 返回值：成功返回true，失败返回false
* bool  CpuInfo::IsLoad(void) const;
*/

/**
* 获取CPU平均温度
* 返回值：成功返回CPU温度，失败返回 0
* double CpuInfo::ReadCpuAvgTemp(void)const;
*/

/**
* 获取CPU使用率
* 返回值：成功返回CPU使用率，失败返回 -1
* double CpuInfo::ReadCpuUseRate(void)const;
*/

/**
* 获取是否成功加载GPU相关dll和sys
* 返回值：成功返回true，失败返回false
* bool  GpuInfo::IsLoad(void) ;
*/

/**
* 获取GPU平均温度
* 返回值：成功返回GPU平均温度，失败返回 0 
* double GpuInfo::GetAvgTemp(void);
*/

/**
* 获取GPU使用率
* 返回值：成功返回GPU核心占用率， 失败返回-1
* unsigned long GpuInfo::GetGpuLoad(void);
*/

/**
* 获取GPU内存总容量
* 返回值：成功返回内存总容量，失败返回 -1
* unsigned int GpuInfo::GetGpuTotalMem(void);
*/

/**
* 获取GPU内存空闲容量
* 返回值：成功返回内存总容量，失败返回 -1
* unsigned int GpuInfo::GetGpuFreeMem(void);
*/

/**
* 获取所有磁盘信息
* 返回值：返回存储磁盘信息的map
* std::map<std::string, DiskInfoMap::DiskInfo>& DiskInfoMap::GetDisksInfo(void);
*/

/**
* 获取某个磁盘总容量
* 返回值：返回磁盘总容量
* int64_t DiskInfoMap::DiskInfo::GetTotalMem(void);
*/

/**
* 获取某个磁盘空闲容量
* 返回值：返回磁盘空闲容量
* int64_t DiskInfoMap::DiskInfo::GetFreeMem(void);
*/



