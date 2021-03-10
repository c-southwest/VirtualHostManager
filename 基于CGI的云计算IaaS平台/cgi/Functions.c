#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>
#include <sqlext.h>
#include <sql.h>
#include <sqltypes.h>
#include <objbase.h> //生成GUID所需
#include "ctemplate.h"

//数据库信息
#define DataSource "CloudComputeSystem"
#define SQL_User "sa"
#define SQL_Password "123456789"

//将参数拼接成完整命令
void BuildCode(char* code, char* base, char* script, char* param);
void Create_Directory(char *name) {
	char base[] = "powershell";
	char script[] = "./Scripts/New-Item#Directory.ps1";
	char param[100] = { 0 };
	char code[1024] = { 0 };

	sprintf(param, "-path D:\\localuser\\%s",name);
	BuildCode(code, base, script, param);
	system(code);//执行命令	
}

void Delete_Directory(char *name) {
	char base[] = "powershell";
	char script[] = "./Scripts/Remove-Item#Directory.ps1";
	char param[100] = { 0 };
	char code[1024] = { 0 };

	sprintf(param, "-path D:\\localuser\\%s", name);
	BuildCode(code, base, script, param);
	system(code);//执行命令	
}

void Create_Site(char* name,char* PhysicalPath, char* ip, char* port, char* hostname, char* protocol) {
	char base[] = "powershell";
	char script[] = "./Scripts/New-IISSite.ps1";
	char param[100] = { 0 };
	char code[1024] = { 0 };

	sprintf(param, "-Name %s -PhysicalPath %s -BindingInformation '%s:%s:%s'  -Protocol %s ", name, PhysicalPath,ip,port,hostname,protocol);

	BuildCode(code, base, script, param);
	system(code);//执行命令
}

void Delete_Site(char* name) {
	char base[] = "powershell";
	char script[] = "./Scripts/Remove-IISSite.ps1";
	char param[100] = { 0 };
	char code[1024] = { 0 };

	sprintf(param, "-Name %s", name);

	BuildCode(code, base, script, param);
	system(code);//执行命令
}

void Start_Site(char* name) {
	char base[] = "powershell";
	char script[] = "./Scripts/Start-IISSite.ps1";
	char param[100] = { 0 };
	char code[1024] = { 0 };

	sprintf(param, "-Name %s",name);

	BuildCode(code, base, script, param);
	system(code);//执行命令
}

void Stop_Site(char* name) {
	char base[] = "powershell";
	char script[] = "./Scripts/Stop-IISSite.ps1";
	char param[100] = { 0 };
	char code[1024] = { 0 };

	sprintf(param, "-Name %s", name);

	BuildCode(code, base, script, param);
	system(code);//执行命令
}

//将参数拼接成完整命令
void BuildCode(char* code, char* base, char* script, char* param){
	strcat(code,base);
	strcat(code, " ");
	strcat(code, script);
	strcat(code, " ");
	strcat(code, param);
}

//依靠GUID进行检查是否登陆，并且可以给g_username赋值,如果已经成功登陆且在期限内返回1，没有返回0
int CheckLogin(char *g_username) {
	//取得本地Cookie
	char c_guid[50] = { 0 };
	cgiCookieString("guid", c_guid, 50);

	//取得服务器GUID 和 TimeStamp
	char g_guid[50] = { 0 }, g_timestamp[20] = { 0 };
	if (SQL_CheckGUID(g_username, c_guid, g_timestamp) == 0) { //等于0代表服务器中不存在客户端的GUID
		return 0;
	}
	else
	{
		if (GetTimeStamp() - atoi(g_timestamp) >= 60 * 60 * 24) {
			return 0; //超过24小时
		}
		else
		{
			return 1;
		}
	}
}


//查询用户是否存在，若存在返回1，不存在返回0
int SQL_QueryUser(char* username)
{
	SQLRETURN ret;
	SQLHENV henv;//SQLHANDLE henv 
	SQLHDBC hdbc;//SQLHANDLE hdbc 
	SQLHSTMT hstmt;//SQLHANDLE hstmt 
	ret = SQLAllocHandle(SQL_HANDLE_ENV, NULL, &henv);//申请环境句柄 
	ret = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, SQL_IS_INTEGER);//设置环境属性 
	ret = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);//申请数据库连接句柄 
	ret = SQLConnect(hdbc, (SQLCHAR*)"CloudComputeSystem", SQL_NTS, (SQLCHAR*)"sa", SQL_NTS, (SQLCHAR*)"123456789", SQL_NTS);//连接数据库 
	if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
		ret = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);//申请SQL语句句柄 
		SQLCHAR sql[100];
		sprintf(sql, "SELECT * FROM T_UserInfo where username='%s'", username);
		//printf("sql is %s\n",sql);
		ret = SQLExecDirect(hstmt, sql, SQL_NTS);//直接执行SQL语句 
		if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
			SQLCHAR user[20], pass[20];
			SQLINTEGER u_len, p_len;
			while (SQLFetch(hstmt) != SQL_NO_DATA) {
				SQLGetData(hstmt, 1, SQL_C_CHAR, user, 20, &u_len);
				SQLGetData(hstmt, 2, SQL_C_CHAR, pass, 20, &p_len);
			}
			//printf("user is %s , pass is %s \n", user,pass);
			int number_row;
			ret = SQLRowCount(hstmt, &number_row);
			if (number_row == 0) //没有记录
			{
				return 0;
			}
			else {
				return 1;
			}
			SQLFreeHandle(SQL_HANDLE_STMT, hstmt);//释放语句句柄 
		}
		else printf("查询数据库操作失败！\n");
		SQLDisconnect(hdbc);//断开与数据库的连接 
	}
	else printf("连接数据库失败!\n");
	SQLFreeHandle(SQL_HANDLE_DBC, hdbc);//释放连接句柄 
	SQLFreeHandle(SQL_HANDLE_ENV, henv);//释放环境句柄
}

//查询FTP用户是否存在，若存在返回1，不存在返回0
int SQL_QueryFtpUser(char* ftpusername)
{
	SQLRETURN ret;
	SQLHENV henv;//SQLHANDLE henv 
	SQLHDBC hdbc;//SQLHANDLE hdbc 
	SQLHSTMT hstmt;//SQLHANDLE hstmt 
	ret = SQLAllocHandle(SQL_HANDLE_ENV, NULL, &henv);//申请环境句柄 
	ret = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, SQL_IS_INTEGER);//设置环境属性 
	ret = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);//申请数据库连接句柄 
	ret = SQLConnect(hdbc, (SQLCHAR*)"CloudComputeSystem", SQL_NTS, (SQLCHAR*)"sa", SQL_NTS, (SQLCHAR*)"123456789", SQL_NTS);//连接数据库 
	if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
		ret = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);//申请SQL语句句柄 
		SQLCHAR sql[100];
		sprintf(sql, "SELECT * FROM T_SiteInfo where ftpusername='%s'", ftpusername);
		ret = SQLExecDirect(hstmt, sql, SQL_NTS);//直接执行SQL语句 
		if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
			SQLCHAR user[20], pass[20];
			SQLINTEGER u_len, p_len;
			while (SQLFetch(hstmt) != SQL_NO_DATA) {
				SQLGetData(hstmt, 1, SQL_C_CHAR, user, 20, &u_len);
				SQLGetData(hstmt, 2, SQL_C_CHAR, pass, 20, &p_len);
			}
			int number_row;
			ret = SQLRowCount(hstmt, &number_row);
			if (number_row == 0) //没有记录
			{
				return 0;
			}
			else {
				return 1;
			}
			SQLFreeHandle(SQL_HANDLE_STMT, hstmt);//释放语句句柄 
		}
		else printf("查询数据库操作失败！\n");
		SQLDisconnect(hdbc);//断开与数据库的连接 
	}
	else printf("连接数据库失败!\n");
	SQLFreeHandle(SQL_HANDLE_DBC, hdbc);//释放连接句柄 
	SQLFreeHandle(SQL_HANDLE_ENV, henv);//释放环境句柄
}

//查询Domain是否存在，若存在返回1，不存在返回0
int SQL_QueryDomain(char* domain)
{
	SQLRETURN ret;
	SQLHENV henv;//SQLHANDLE henv 
	SQLHDBC hdbc;//SQLHANDLE hdbc 
	SQLHSTMT hstmt;//SQLHANDLE hstmt 
	ret = SQLAllocHandle(SQL_HANDLE_ENV, NULL, &henv);//申请环境句柄 
	ret = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, SQL_IS_INTEGER);//设置环境属性 
	ret = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);//申请数据库连接句柄 
	ret = SQLConnect(hdbc, (SQLCHAR*)"CloudComputeSystem", SQL_NTS, (SQLCHAR*)"sa", SQL_NTS, (SQLCHAR*)"123456789", SQL_NTS);//连接数据库 
																															 //ret = SQLConnect(hdbc, (SQLCHAR*)DataSource, SQL_NTS, (SQLCHAR*)SQL_User, SQL_NTS, (SQLCHAR*)SQL_Password, SQL_NTS);//连接数据库 
	if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
		ret = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);//申请SQL语句句柄 
		SQLCHAR sql[100];
		sprintf(sql, "SELECT * FROM T_SiteInfo where domain='%s'", domain);
		//printf("sql is %s\n",sql);
		ret = SQLExecDirect(hstmt, sql, SQL_NTS);//直接执行SQL语句 
		if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
			SQLCHAR user[20], pass[20];
			SQLINTEGER u_len, p_len;
			while (SQLFetch(hstmt) != SQL_NO_DATA) {
				SQLGetData(hstmt, 1, SQL_C_CHAR, user, 20, &u_len);
				SQLGetData(hstmt, 2, SQL_C_CHAR, pass, 20, &p_len);
			}
			int number_row;
			ret = SQLRowCount(hstmt, &number_row);
			if (number_row == 0) //没有记录
			{
				return 0;
			}
			else {
				return 1;
			}
			SQLFreeHandle(SQL_HANDLE_STMT, hstmt);//释放语句句柄 
		}
		else printf("查询数据库操作失败！\n");
		SQLDisconnect(hdbc);//断开与数据库的连接 
	}
	else printf("连接数据库失败!\n");
	SQLFreeHandle(SQL_HANDLE_DBC, hdbc);//释放连接句柄 
	SQLFreeHandle(SQL_HANDLE_ENV, henv);//释放环境句柄
}


//查询密码是否正确，正确返回1，不正确返回0
int SQL_CheckPassword(char* username, char* password)
{
	SQLRETURN ret;
	SQLHENV henv;//SQLHANDLE henv 
	SQLHDBC hdbc;//SQLHANDLE hdbc 
	SQLHSTMT hstmt;//SQLHANDLE hstmt 
	ret = SQLAllocHandle(SQL_HANDLE_ENV, NULL, &henv);//申请环境句柄 
	ret = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, SQL_IS_INTEGER);//设置环境属性 
	ret = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);//申请数据库连接句柄 
	ret = SQLConnect(hdbc, (SQLCHAR*)DataSource, SQL_NTS, (SQLCHAR*)SQL_User, SQL_NTS, (SQLCHAR*)SQL_Password, SQL_NTS);//连接数据库 
	if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
		ret = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);//申请SQL语句句柄 
		SQLCHAR sql[100];
		sprintf(sql, "SELECT * FROM T_UserInfo where username='%s' AND password='%s'", username,password);
		ret = SQLExecDirect(hstmt, sql, SQL_NTS);//直接执行SQL语句 
		if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
			SQLCHAR user[20], pass[20];
			SQLINTEGER u_len, p_len;
			while (SQLFetch(hstmt) != SQL_NO_DATA) {
				SQLGetData(hstmt, 1, SQL_C_CHAR, user, 20, &u_len);
				SQLGetData(hstmt, 2, SQL_C_CHAR, pass, 20, &p_len);
			}
			int number_row;
			ret = SQLRowCount(hstmt, &number_row);
			if (number_row == 0) //没有记录
			{
				return 0;
			}
			else {
				return 1;
			}
			SQLFreeHandle(SQL_HANDLE_STMT, hstmt);//释放语句句柄 
		}
		else printf("查询数据库操作失败！\n");

		SQLDisconnect(hdbc);//断开与数据库的连接 
	}
	else printf("连接数据库失败!\n");
	SQLFreeHandle(SQL_HANDLE_DBC, hdbc);//释放连接句柄 
	SQLFreeHandle(SQL_HANDLE_ENV, henv);//释放环境句柄
}

//注册账户，成功返回1，失败返回0
int SQL_RegisterAccount(char* username, char* password, char* email, char* phone)
{
	SQLRETURN ret;
	SQLHENV henv;//SQLHANDLE henv 
	SQLHDBC hdbc;//SQLHANDLE hdbc 
	SQLHSTMT hstmt;//SQLHANDLE hstmt 
	ret = SQLAllocHandle(SQL_HANDLE_ENV, NULL, &henv);//申请环境句柄 
	ret = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, SQL_IS_INTEGER);//设置环境属性 
	ret = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);//申请数据库连接句柄 
	ret = SQLConnect(hdbc, (SQLCHAR*)DataSource, SQL_NTS, (SQLCHAR*)SQL_User, SQL_NTS, (SQLCHAR*)SQL_Password, SQL_NTS);//连接数据库 
	if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
		ret = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);//申请SQL语句句柄 
		SQLCHAR sql[100];
		sprintf(sql, "insert into T_UserInfo values('%s','%s','%s','%s')", username,password,email,phone);
		ret = SQLExecDirect(hstmt, sql, SQL_NTS);//直接执行SQL语句 
		if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
			return 1;
			SQLFreeHandle(SQL_HANDLE_STMT, hstmt);//释放语句句柄 
		}
		else return 0;
		SQLDisconnect(hdbc);//断开与数据库的连接 
	}
	else return 0;;
	SQLFreeHandle(SQL_HANDLE_DBC, hdbc);//释放连接句柄 
	SQLFreeHandle(SQL_HANDLE_ENV, henv);//释放环境句柄
}

//设置Session，成功返回1，失败返回0
void SQL_SetSession(char* username, char* guid, char* timestamp)
{
	SQLRETURN ret;
	SQLHENV henv;//SQLHANDLE henv 
	SQLHDBC hdbc;//SQLHANDLE hdbc 
	SQLHSTMT hstmt;//SQLHANDLE hstmt 
	ret = SQLAllocHandle(SQL_HANDLE_ENV, NULL, &henv);//申请环境句柄 
	ret = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, SQL_IS_INTEGER);//设置环境属性 
	ret = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);//申请数据库连接句柄 
	ret = SQLConnect(hdbc, (SQLCHAR*)DataSource, SQL_NTS, (SQLCHAR*)SQL_User, SQL_NTS, (SQLCHAR*)SQL_Password, SQL_NTS);//连接数据库 
	if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
		ret = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);//申请SQL语句句柄 
		SQLCHAR sql[100];
		sprintf(sql, "insert into T_Session values('%s','%s','%s')", username, guid, timestamp);
		ret = SQLExecDirect(hstmt, sql, SQL_NTS);//直接执行SQL语句 
		if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
			//return 1;
			SQLFreeHandle(SQL_HANDLE_STMT, hstmt);//释放语句句柄 
		}
		SQLDisconnect(hdbc);//断开与数据库的连接 
	}
	SQLFreeHandle(SQL_HANDLE_DBC, hdbc);//释放连接句柄 
	SQLFreeHandle(SQL_HANDLE_ENV, henv);//释放环境句柄
}

//获得对应用户session的GUID和Timestamp
int SQL_GetSession(char* username, char* guid, char* timestamp)
{
	SQLRETURN ret;
	SQLHENV henv;//SQLHANDLE henv 
	SQLHDBC hdbc;//SQLHANDLE hdbc 
	SQLHSTMT hstmt;//SQLHANDLE hstmt 
	ret = SQLAllocHandle(SQL_HANDLE_ENV, NULL, &henv);//申请环境句柄 
	ret = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, SQL_IS_INTEGER);//设置环境属性 
	ret = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);//申请数据库连接句柄 
	ret = SQLConnect(hdbc, (SQLCHAR*)DataSource, SQL_NTS, (SQLCHAR*)SQL_User, SQL_NTS, (SQLCHAR*)SQL_Password, SQL_NTS);//连接数据库 
	if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
		ret = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);//申请SQL语句句柄 
		SQLCHAR sql[100];
		sprintf(sql, "SELECT * FROM T_Session where username='%s'", username);
		ret = SQLExecDirect(hstmt, sql, SQL_NTS);//直接执行SQL语句 
		if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
			SQLCHAR user[20];//, guid[50],timestamp[20];
			SQLINTEGER u_len, g_len, t_len;
			while (SQLFetch(hstmt) != SQL_NO_DATA) {
				SQLGetData(hstmt, 1, SQL_C_CHAR, user, 20, &u_len);
				SQLGetData(hstmt, 2, SQL_C_CHAR, guid, 50, &g_len);
				SQLGetData(hstmt, 3, SQL_C_CHAR, timestamp, 20, &t_len);
			}
			int number_row;
			ret = SQLRowCount(hstmt, &number_row);
			if (number_row == 0) //没有记录
			{
				return 0;
			}
			else {
				return 1;
			}
			SQLFreeHandle(SQL_HANDLE_STMT, hstmt);//释放语句句柄 
		}
		else printf("查询数据库操作失败！\n");

		SQLDisconnect(hdbc);//断开与数据库的连接 
	}
	else printf("连接数据库失败!\n");
	SQLFreeHandle(SQL_HANDLE_DBC, hdbc);//释放连接句柄 
	SQLFreeHandle(SQL_HANDLE_ENV, henv);//释放环境句柄
}

//依靠GUID来查询GUID,没找到返回0，有则返回1
int SQL_CheckGUID(char* g_username, char* c_guid, char* g_timestamp)
{
	SQLRETURN ret;
	SQLHENV henv;//SQLHANDLE henv 
	SQLHDBC hdbc;//SQLHANDLE hdbc 
	SQLHSTMT hstmt;//SQLHANDLE hstmt 
	ret = SQLAllocHandle(SQL_HANDLE_ENV, NULL, &henv);//申请环境句柄 
	ret = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, SQL_IS_INTEGER);//设置环境属性 
	ret = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);//申请数据库连接句柄 
	ret = SQLConnect(hdbc, (SQLCHAR*)DataSource, SQL_NTS, (SQLCHAR*)SQL_User, SQL_NTS, (SQLCHAR*)SQL_Password, SQL_NTS);//连接数据库 
	if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
		ret = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);//申请SQL语句句柄 
		SQLCHAR sql[100];
		sprintf(sql, "SELECT * FROM T_Session where guid='%s'", c_guid);
		ret = SQLExecDirect(hstmt, sql, SQL_NTS);//直接执行SQL语句 
		if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
			//SQLCHAR user[20];//, guid[50],timestamp[20];
			SQLINTEGER u_len, g_len, t_len;
			SQLCHAR guid[50];
			while (SQLFetch(hstmt) != SQL_NO_DATA) {
				SQLGetData(hstmt, 1, SQL_C_CHAR, g_username, 20, &u_len);
				SQLGetData(hstmt, 2, SQL_C_CHAR, guid, 50, &g_len);
				SQLGetData(hstmt, 3, SQL_C_CHAR, g_timestamp, 20, &t_len);
			}
			int number_row;
			ret = SQLRowCount(hstmt, &number_row);
			if (number_row == 0) //没有记录
			{
				return 0;
			}
			else {
				return 1;
			}
			SQLFreeHandle(SQL_HANDLE_STMT, hstmt);//释放语句句柄 
		}
		else printf("查询数据库操作失败！\n");
		SQLDisconnect(hdbc);//断开与数据库的连接 
	}
	else printf("连接数据库失败!\n");
	SQLFreeHandle(SQL_HANDLE_DBC, hdbc);//释放连接句柄 
	SQLFreeHandle(SQL_HANDLE_ENV, henv);//释放环境句柄
}

//依靠username来查询GUID,没找到返回0，有则返回1
int SQL_CheckGUID2(char* c_username, char* g_guid, char* g_timestamp)
{
	SQLRETURN ret;
	SQLHENV henv;//SQLHANDLE henv 
	SQLHDBC hdbc;//SQLHANDLE hdbc 
	SQLHSTMT hstmt;//SQLHANDLE hstmt 
	ret = SQLAllocHandle(SQL_HANDLE_ENV, NULL, &henv);//申请环境句柄 
	ret = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, SQL_IS_INTEGER);//设置环境属性 
	ret = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);//申请数据库连接句柄 
	ret = SQLConnect(hdbc, (SQLCHAR*)DataSource, SQL_NTS, (SQLCHAR*)SQL_User, SQL_NTS, (SQLCHAR*)SQL_Password, SQL_NTS);//连接数据库 
	if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
		ret = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);//申请SQL语句句柄 
		SQLCHAR sql[100];
		sprintf(sql, "SELECT * FROM T_Session where username='%s'", c_username);
		ret = SQLExecDirect(hstmt, sql, SQL_NTS);//直接执行SQL语句 
		if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
			//SQLCHAR user[20];//, guid[50],timestamp[20];
			SQLINTEGER u_len, g_len, t_len;
			SQLCHAR username[20];
			while (SQLFetch(hstmt) != SQL_NO_DATA) {
				SQLGetData(hstmt, 1, SQL_C_CHAR, username, 20, &u_len);
				SQLGetData(hstmt, 2, SQL_C_CHAR, g_guid, 50, &g_len);
				SQLGetData(hstmt, 3, SQL_C_CHAR, g_timestamp, 20, &t_len);
			}
			int number_row;
			ret = SQLRowCount(hstmt, &number_row);
			if (number_row == 0) //没有记录
			{
				return 0;
			}
			else {
				return 1;
			}
			SQLFreeHandle(SQL_HANDLE_STMT, hstmt);//释放语句句柄 
		}
		else printf("查询数据库操作失败！\n");

		SQLDisconnect(hdbc);//断开与数据库的连接 
	}
	else printf("连接数据库失败!\n");
	SQLFreeHandle(SQL_HANDLE_DBC, hdbc);//释放连接句柄 
	SQLFreeHandle(SQL_HANDLE_ENV, henv);//释放环境句柄
}

//依靠username来删除GUID
void SQL_DeleteGUID(char* username)
{
	SQLRETURN ret;
	SQLHENV henv;//SQLHANDLE henv 
	SQLHDBC hdbc;//SQLHANDLE hdbc 
	SQLHSTMT hstmt;//SQLHANDLE hstmt 
	ret = SQLAllocHandle(SQL_HANDLE_ENV, NULL, &henv);//申请环境句柄 
	ret = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, SQL_IS_INTEGER);//设置环境属性 
	ret = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);//申请数据库连接句柄 
	ret = SQLConnect(hdbc, (SQLCHAR*)DataSource, SQL_NTS, (SQLCHAR*)SQL_User, SQL_NTS, (SQLCHAR*)SQL_Password, SQL_NTS);//连接数据库 
	if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
		ret = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);//申请SQL语句句柄 
		SQLCHAR sql[100];
		sprintf(sql, "DELETE FROM T_Session WHERE username='%s'", username);
		ret = SQLExecDirect(hstmt, sql, SQL_NTS);//直接执行SQL语句 
		if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
			SQLFreeHandle(SQL_HANDLE_STMT, hstmt);//释放语句句柄 
		}
		else printf("查询数据库操作失败！\n");

		SQLDisconnect(hdbc);//断开与数据库的连接 
	}
	else printf("连接数据库失败!\n");
	SQLFreeHandle(SQL_HANDLE_DBC, hdbc);//释放连接句柄 
	SQLFreeHandle(SQL_HANDLE_ENV, henv);//释放环境句柄
}

//记录Web服务开通信息，成功返回1，失败返回0
int SQL_RecordSite(char* username, char* ip, char* ftpusername, char* ftppassword,char* domain)
{
	SQLRETURN ret;
	SQLHENV henv;//SQLHANDLE henv 
	SQLHDBC hdbc;//SQLHANDLE hdbc 
	SQLHSTMT hstmt;//SQLHANDLE hstmt 
	ret = SQLAllocHandle(SQL_HANDLE_ENV, NULL, &henv);//申请环境句柄 
	ret = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, SQL_IS_INTEGER);//设置环境属性 
	ret = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);//申请数据库连接句柄 
	ret = SQLConnect(hdbc, (SQLCHAR*)DataSource, SQL_NTS, (SQLCHAR*)SQL_User, SQL_NTS, (SQLCHAR*)SQL_Password, SQL_NTS);//连接数据库 
	if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
		ret = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);//申请SQL语句句柄 
		SQLCHAR sql[100];
		sprintf(sql, "insert into T_SiteInfo values('%s','%s','%s','%s','%s')", username,ip , ftpusername, ftppassword, domain);
		ret = SQLExecDirect(hstmt, sql, SQL_NTS);//直接执行SQL语句 
		if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
			return 1;
			SQLFreeHandle(SQL_HANDLE_STMT, hstmt);//释放语句句柄 
		}
		else return 0;
		SQLDisconnect(hdbc);//断开与数据库的连接 
	}
	else return 0;;
	SQLFreeHandle(SQL_HANDLE_DBC, hdbc);//释放连接句柄 
	SQLFreeHandle(SQL_HANDLE_ENV, henv);//释放环境句柄
}

//依靠ftpusername来删除Site
void SQL_DeleteSite(char* ftpusername)
{
	SQLRETURN ret;
	SQLHENV henv;//SQLHANDLE henv 
	SQLHDBC hdbc;//SQLHANDLE hdbc 
	SQLHSTMT hstmt;//SQLHANDLE hstmt 
	ret = SQLAllocHandle(SQL_HANDLE_ENV, NULL, &henv);//申请环境句柄 
	ret = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, SQL_IS_INTEGER);//设置环境属性 
	ret = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);//申请数据库连接句柄 
	ret = SQLConnect(hdbc, (SQLCHAR*)DataSource, SQL_NTS, (SQLCHAR*)SQL_User, SQL_NTS, (SQLCHAR*)SQL_Password, SQL_NTS);//连接数据库 
	if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
		ret = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);//申请SQL语句句柄 
		SQLCHAR sql[100];
		sprintf(sql, "DELETE FROM T_SiteInfo WHERE ftpusername='%s'", ftpusername);
		ret = SQLExecDirect(hstmt, sql, SQL_NTS);//直接执行SQL语句 
		if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
			SQLFreeHandle(SQL_HANDLE_STMT, hstmt);//释放语句句柄 
		}
		else {
			cgiHeaderContentType("text/html;charset=gb2312");
			showInfoPage("<h2>发生错误，请联系管理员QQ：1367248805！<a href='index.cgi'>请点击返回重试</a></h2> ");
		}
		SQLDisconnect(hdbc);//断开与数据库的连接 
	}
	else printf("连接数据库失败!\n");
	SQLFreeHandle(SQL_HANDLE_DBC, hdbc);//释放连接句柄 
	SQLFreeHandle(SQL_HANDLE_ENV, henv);//释放环境句柄
}

//返回GUID标识符指针
char* createGuid()
{
	static char _guid[100];
	GUID guid = { 0 };
	CoCreateGuid(&guid);
	sprintf(_guid, "{%08X-%04X-%04x-%02X%02X-%02X%02X%02X%02X%02X%02X}",
		guid.Data1,
		guid.Data2,
		guid.Data3,
		guid.Data4[0],
		guid.Data4[1],
		guid.Data4[2],
		guid.Data4[3],
		guid.Data4[4],
		guid.Data4[5],
		guid.Data4[6],
		guid.Data4[7]);
	return _guid;
}

//返回时间戳
int GetTimeStamp() {
	time_t t;
	t = time(NULL);
	int ii = time(&t);
	return ii;
}