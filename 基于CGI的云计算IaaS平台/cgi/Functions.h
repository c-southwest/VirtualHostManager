#pragma once
//网站相关函数
void Create_Directory(char *name); //在D:\localuser\下创建文件夹
void Delete_Directory(char *name);

void Create_Site(char* name, char* PhysicalPath, char* ip, char* port, char* hostname, char* protocol); //新建站点
void Delete_Site(char* name);

void Start_Site(char* name); //启动站点

void Stop_Site(char* name); //停止站点


/***********************************************************
int SQL_QueryUser(char* username)
参数：username：用户名
功能：查询对应用户名在数据库中是否存在
返回：用户存在返回1，不存在返回0
***********************************************************/
int SQL_QueryUser(char* username); 

/***********************************************************
int SQL_QueryUser(char* username)
参数：username：用户名
功能：查询对应用户名的密码是否正确
返回：密码正确返回1，不存在返回0
***********************************************************/
int SQL_CheckPassword(char* username, char* password); 

/***********************************************************
int SQL_RegisterAccount(char* username, char* password, char* email, char* phone)
参数：用户名，密码，邮箱，手机号
功能：注册新用户
返回：注册成功返回1，否则返回0
***********************************************************/
int SQL_RegisterAccount(char* username, char* password, char* email, char* phone); 

/***********************************************************
void SQL_SetSession(char* username, char* guid, char* timestamp); 
参数：用户名，GUID唯一标识符，时间戳
功能：将用户登陆产生的GUID标识符以及当前时间的时间戳记录到服务器中，
返回：无
***********************************************************/
void SQL_SetSession(char* username, char* guid, char* timestamp);

/***********************************************************
int SQL_GetSession(char* username, char* guid, char* timestamp)
参数：用户名，用于存储GUID唯一标识符，用于存储时间戳
功能：向服务器取得对应用户的GUID标识符以及时间戳
返回：取得成功返回1，否则返回0
***********************************************************/
int SQL_GetSession(char* username, char* guid, char* timestamp);

/***********************************************************
int SQL_CheckGUID(char* username, char* guid, char* timestamp)
参数：用于存储用户名，GUID唯一标识符，用于存储时间戳
功能：向服务器查询指定GUID标识符，若存在则再取回对应的用户名和时间戳
返回：对应GUID存在返回1，否则返回0
***********************************************************/
int SQL_CheckGUID(char* username, char* guid, char* timestamp);

/***********************************************************
void SQL_DeleteGUID(char* username)
参数：用户名
功能：删除服务器中对应用户的GUID
返回：无
***********************************************************/
void SQL_DeleteGUID(char* username); 

/***********************************************************
int SQL_CheckGUID2(char* c_username, char* g_guid, char* g_timestamp)
参数：用户名，用于存储GUID，用于存储时间戳
功能：向服务器中查询对应用户，并获得其GUID和时间戳
返回：查询成功返回1，否则返回0
***********************************************************/
int SQL_CheckGUID2(char* c_username, char* g_guid, char* g_timestamp);

/***********************************************************
int CheckLogin(char *g_username)
参数：用于存储用户名
功能：检查当前状态是否已经登陆，如果登陆成功可返回对应用户名
返回：已经成功登陆返回1，否则返回0
***********************************************************///
int CheckLogin(char *g_username);

//记录Web服务开通信息，成功返回1，失败返回0
int SQL_RecordSite(char* username, char* ip, char* ftpusername, char* ftppassword, char* domain);


//依靠ftpusername来删除Site
void SQL_DeleteSite(char* ftpusername);

//查询FTP用户是否存在，若存在返回1，不存在返回0
int SQL_QueryFtpUser(char* ftpusername);

//查询Domain是否存在，若存在返回1，不存在返回0
int SQL_QueryDomain(char* domain);

char* createGuid(); //生成GUID

int GetTimeStamp(); //得到时间戳