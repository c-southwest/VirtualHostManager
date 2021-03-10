#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>
#include <sqlext.h>
#include <sql.h>
#include <sqltypes.h>
#include <objbase.h> //����GUID����
#include "ctemplate.h"

//���ݿ���Ϣ
#define DataSource "CloudComputeSystem"
#define SQL_User "sa"
#define SQL_Password "123456789"

//������ƴ�ӳ���������
void BuildCode(char* code, char* base, char* script, char* param);
void Create_Directory(char *name) {
	char base[] = "powershell";
	char script[] = "./Scripts/New-Item#Directory.ps1";
	char param[100] = { 0 };
	char code[1024] = { 0 };

	sprintf(param, "-path D:\\localuser\\%s",name);
	BuildCode(code, base, script, param);
	system(code);//ִ������	
}

void Delete_Directory(char *name) {
	char base[] = "powershell";
	char script[] = "./Scripts/Remove-Item#Directory.ps1";
	char param[100] = { 0 };
	char code[1024] = { 0 };

	sprintf(param, "-path D:\\localuser\\%s", name);
	BuildCode(code, base, script, param);
	system(code);//ִ������	
}

void Create_Site(char* name,char* PhysicalPath, char* ip, char* port, char* hostname, char* protocol) {
	char base[] = "powershell";
	char script[] = "./Scripts/New-IISSite.ps1";
	char param[100] = { 0 };
	char code[1024] = { 0 };

	sprintf(param, "-Name %s -PhysicalPath %s -BindingInformation '%s:%s:%s'  -Protocol %s ", name, PhysicalPath,ip,port,hostname,protocol);

	BuildCode(code, base, script, param);
	system(code);//ִ������
}

void Delete_Site(char* name) {
	char base[] = "powershell";
	char script[] = "./Scripts/Remove-IISSite.ps1";
	char param[100] = { 0 };
	char code[1024] = { 0 };

	sprintf(param, "-Name %s", name);

	BuildCode(code, base, script, param);
	system(code);//ִ������
}

void Start_Site(char* name) {
	char base[] = "powershell";
	char script[] = "./Scripts/Start-IISSite.ps1";
	char param[100] = { 0 };
	char code[1024] = { 0 };

	sprintf(param, "-Name %s",name);

	BuildCode(code, base, script, param);
	system(code);//ִ������
}

void Stop_Site(char* name) {
	char base[] = "powershell";
	char script[] = "./Scripts/Stop-IISSite.ps1";
	char param[100] = { 0 };
	char code[1024] = { 0 };

	sprintf(param, "-Name %s", name);

	BuildCode(code, base, script, param);
	system(code);//ִ������
}

//������ƴ�ӳ���������
void BuildCode(char* code, char* base, char* script, char* param){
	strcat(code,base);
	strcat(code, " ");
	strcat(code, script);
	strcat(code, " ");
	strcat(code, param);
}

//����GUID���м���Ƿ��½�����ҿ��Ը�g_username��ֵ,����Ѿ��ɹ���½���������ڷ���1��û�з���0
int CheckLogin(char *g_username) {
	//ȡ�ñ���Cookie
	char c_guid[50] = { 0 };
	cgiCookieString("guid", c_guid, 50);

	//ȡ�÷�����GUID �� TimeStamp
	char g_guid[50] = { 0 }, g_timestamp[20] = { 0 };
	if (SQL_CheckGUID(g_username, c_guid, g_timestamp) == 0) { //����0����������в����ڿͻ��˵�GUID
		return 0;
	}
	else
	{
		if (GetTimeStamp() - atoi(g_timestamp) >= 60 * 60 * 24) {
			return 0; //����24Сʱ
		}
		else
		{
			return 1;
		}
	}
}


//��ѯ�û��Ƿ���ڣ������ڷ���1�������ڷ���0
int SQL_QueryUser(char* username)
{
	SQLRETURN ret;
	SQLHENV henv;//SQLHANDLE henv 
	SQLHDBC hdbc;//SQLHANDLE hdbc 
	SQLHSTMT hstmt;//SQLHANDLE hstmt 
	ret = SQLAllocHandle(SQL_HANDLE_ENV, NULL, &henv);//���뻷����� 
	ret = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, SQL_IS_INTEGER);//���û������� 
	ret = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);//�������ݿ����Ӿ�� 
	ret = SQLConnect(hdbc, (SQLCHAR*)"CloudComputeSystem", SQL_NTS, (SQLCHAR*)"sa", SQL_NTS, (SQLCHAR*)"123456789", SQL_NTS);//�������ݿ� 
	if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
		ret = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);//����SQL����� 
		SQLCHAR sql[100];
		sprintf(sql, "SELECT * FROM T_UserInfo where username='%s'", username);
		//printf("sql is %s\n",sql);
		ret = SQLExecDirect(hstmt, sql, SQL_NTS);//ֱ��ִ��SQL��� 
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
			if (number_row == 0) //û�м�¼
			{
				return 0;
			}
			else {
				return 1;
			}
			SQLFreeHandle(SQL_HANDLE_STMT, hstmt);//�ͷ������ 
		}
		else printf("��ѯ���ݿ����ʧ�ܣ�\n");
		SQLDisconnect(hdbc);//�Ͽ������ݿ������ 
	}
	else printf("�������ݿ�ʧ��!\n");
	SQLFreeHandle(SQL_HANDLE_DBC, hdbc);//�ͷ����Ӿ�� 
	SQLFreeHandle(SQL_HANDLE_ENV, henv);//�ͷŻ������
}

//��ѯFTP�û��Ƿ���ڣ������ڷ���1�������ڷ���0
int SQL_QueryFtpUser(char* ftpusername)
{
	SQLRETURN ret;
	SQLHENV henv;//SQLHANDLE henv 
	SQLHDBC hdbc;//SQLHANDLE hdbc 
	SQLHSTMT hstmt;//SQLHANDLE hstmt 
	ret = SQLAllocHandle(SQL_HANDLE_ENV, NULL, &henv);//���뻷����� 
	ret = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, SQL_IS_INTEGER);//���û������� 
	ret = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);//�������ݿ����Ӿ�� 
	ret = SQLConnect(hdbc, (SQLCHAR*)"CloudComputeSystem", SQL_NTS, (SQLCHAR*)"sa", SQL_NTS, (SQLCHAR*)"123456789", SQL_NTS);//�������ݿ� 
	if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
		ret = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);//����SQL����� 
		SQLCHAR sql[100];
		sprintf(sql, "SELECT * FROM T_SiteInfo where ftpusername='%s'", ftpusername);
		ret = SQLExecDirect(hstmt, sql, SQL_NTS);//ֱ��ִ��SQL��� 
		if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
			SQLCHAR user[20], pass[20];
			SQLINTEGER u_len, p_len;
			while (SQLFetch(hstmt) != SQL_NO_DATA) {
				SQLGetData(hstmt, 1, SQL_C_CHAR, user, 20, &u_len);
				SQLGetData(hstmt, 2, SQL_C_CHAR, pass, 20, &p_len);
			}
			int number_row;
			ret = SQLRowCount(hstmt, &number_row);
			if (number_row == 0) //û�м�¼
			{
				return 0;
			}
			else {
				return 1;
			}
			SQLFreeHandle(SQL_HANDLE_STMT, hstmt);//�ͷ������ 
		}
		else printf("��ѯ���ݿ����ʧ�ܣ�\n");
		SQLDisconnect(hdbc);//�Ͽ������ݿ������ 
	}
	else printf("�������ݿ�ʧ��!\n");
	SQLFreeHandle(SQL_HANDLE_DBC, hdbc);//�ͷ����Ӿ�� 
	SQLFreeHandle(SQL_HANDLE_ENV, henv);//�ͷŻ������
}

//��ѯDomain�Ƿ���ڣ������ڷ���1�������ڷ���0
int SQL_QueryDomain(char* domain)
{
	SQLRETURN ret;
	SQLHENV henv;//SQLHANDLE henv 
	SQLHDBC hdbc;//SQLHANDLE hdbc 
	SQLHSTMT hstmt;//SQLHANDLE hstmt 
	ret = SQLAllocHandle(SQL_HANDLE_ENV, NULL, &henv);//���뻷����� 
	ret = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, SQL_IS_INTEGER);//���û������� 
	ret = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);//�������ݿ����Ӿ�� 
	ret = SQLConnect(hdbc, (SQLCHAR*)"CloudComputeSystem", SQL_NTS, (SQLCHAR*)"sa", SQL_NTS, (SQLCHAR*)"123456789", SQL_NTS);//�������ݿ� 
																															 //ret = SQLConnect(hdbc, (SQLCHAR*)DataSource, SQL_NTS, (SQLCHAR*)SQL_User, SQL_NTS, (SQLCHAR*)SQL_Password, SQL_NTS);//�������ݿ� 
	if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
		ret = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);//����SQL����� 
		SQLCHAR sql[100];
		sprintf(sql, "SELECT * FROM T_SiteInfo where domain='%s'", domain);
		//printf("sql is %s\n",sql);
		ret = SQLExecDirect(hstmt, sql, SQL_NTS);//ֱ��ִ��SQL��� 
		if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
			SQLCHAR user[20], pass[20];
			SQLINTEGER u_len, p_len;
			while (SQLFetch(hstmt) != SQL_NO_DATA) {
				SQLGetData(hstmt, 1, SQL_C_CHAR, user, 20, &u_len);
				SQLGetData(hstmt, 2, SQL_C_CHAR, pass, 20, &p_len);
			}
			int number_row;
			ret = SQLRowCount(hstmt, &number_row);
			if (number_row == 0) //û�м�¼
			{
				return 0;
			}
			else {
				return 1;
			}
			SQLFreeHandle(SQL_HANDLE_STMT, hstmt);//�ͷ������ 
		}
		else printf("��ѯ���ݿ����ʧ�ܣ�\n");
		SQLDisconnect(hdbc);//�Ͽ������ݿ������ 
	}
	else printf("�������ݿ�ʧ��!\n");
	SQLFreeHandle(SQL_HANDLE_DBC, hdbc);//�ͷ����Ӿ�� 
	SQLFreeHandle(SQL_HANDLE_ENV, henv);//�ͷŻ������
}


//��ѯ�����Ƿ���ȷ����ȷ����1������ȷ����0
int SQL_CheckPassword(char* username, char* password)
{
	SQLRETURN ret;
	SQLHENV henv;//SQLHANDLE henv 
	SQLHDBC hdbc;//SQLHANDLE hdbc 
	SQLHSTMT hstmt;//SQLHANDLE hstmt 
	ret = SQLAllocHandle(SQL_HANDLE_ENV, NULL, &henv);//���뻷����� 
	ret = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, SQL_IS_INTEGER);//���û������� 
	ret = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);//�������ݿ����Ӿ�� 
	ret = SQLConnect(hdbc, (SQLCHAR*)DataSource, SQL_NTS, (SQLCHAR*)SQL_User, SQL_NTS, (SQLCHAR*)SQL_Password, SQL_NTS);//�������ݿ� 
	if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
		ret = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);//����SQL����� 
		SQLCHAR sql[100];
		sprintf(sql, "SELECT * FROM T_UserInfo where username='%s' AND password='%s'", username,password);
		ret = SQLExecDirect(hstmt, sql, SQL_NTS);//ֱ��ִ��SQL��� 
		if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
			SQLCHAR user[20], pass[20];
			SQLINTEGER u_len, p_len;
			while (SQLFetch(hstmt) != SQL_NO_DATA) {
				SQLGetData(hstmt, 1, SQL_C_CHAR, user, 20, &u_len);
				SQLGetData(hstmt, 2, SQL_C_CHAR, pass, 20, &p_len);
			}
			int number_row;
			ret = SQLRowCount(hstmt, &number_row);
			if (number_row == 0) //û�м�¼
			{
				return 0;
			}
			else {
				return 1;
			}
			SQLFreeHandle(SQL_HANDLE_STMT, hstmt);//�ͷ������ 
		}
		else printf("��ѯ���ݿ����ʧ�ܣ�\n");

		SQLDisconnect(hdbc);//�Ͽ������ݿ������ 
	}
	else printf("�������ݿ�ʧ��!\n");
	SQLFreeHandle(SQL_HANDLE_DBC, hdbc);//�ͷ����Ӿ�� 
	SQLFreeHandle(SQL_HANDLE_ENV, henv);//�ͷŻ������
}

//ע���˻����ɹ�����1��ʧ�ܷ���0
int SQL_RegisterAccount(char* username, char* password, char* email, char* phone)
{
	SQLRETURN ret;
	SQLHENV henv;//SQLHANDLE henv 
	SQLHDBC hdbc;//SQLHANDLE hdbc 
	SQLHSTMT hstmt;//SQLHANDLE hstmt 
	ret = SQLAllocHandle(SQL_HANDLE_ENV, NULL, &henv);//���뻷����� 
	ret = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, SQL_IS_INTEGER);//���û������� 
	ret = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);//�������ݿ����Ӿ�� 
	ret = SQLConnect(hdbc, (SQLCHAR*)DataSource, SQL_NTS, (SQLCHAR*)SQL_User, SQL_NTS, (SQLCHAR*)SQL_Password, SQL_NTS);//�������ݿ� 
	if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
		ret = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);//����SQL����� 
		SQLCHAR sql[100];
		sprintf(sql, "insert into T_UserInfo values('%s','%s','%s','%s')", username,password,email,phone);
		ret = SQLExecDirect(hstmt, sql, SQL_NTS);//ֱ��ִ��SQL��� 
		if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
			return 1;
			SQLFreeHandle(SQL_HANDLE_STMT, hstmt);//�ͷ������ 
		}
		else return 0;
		SQLDisconnect(hdbc);//�Ͽ������ݿ������ 
	}
	else return 0;;
	SQLFreeHandle(SQL_HANDLE_DBC, hdbc);//�ͷ����Ӿ�� 
	SQLFreeHandle(SQL_HANDLE_ENV, henv);//�ͷŻ������
}

//����Session���ɹ�����1��ʧ�ܷ���0
void SQL_SetSession(char* username, char* guid, char* timestamp)
{
	SQLRETURN ret;
	SQLHENV henv;//SQLHANDLE henv 
	SQLHDBC hdbc;//SQLHANDLE hdbc 
	SQLHSTMT hstmt;//SQLHANDLE hstmt 
	ret = SQLAllocHandle(SQL_HANDLE_ENV, NULL, &henv);//���뻷����� 
	ret = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, SQL_IS_INTEGER);//���û������� 
	ret = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);//�������ݿ����Ӿ�� 
	ret = SQLConnect(hdbc, (SQLCHAR*)DataSource, SQL_NTS, (SQLCHAR*)SQL_User, SQL_NTS, (SQLCHAR*)SQL_Password, SQL_NTS);//�������ݿ� 
	if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
		ret = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);//����SQL����� 
		SQLCHAR sql[100];
		sprintf(sql, "insert into T_Session values('%s','%s','%s')", username, guid, timestamp);
		ret = SQLExecDirect(hstmt, sql, SQL_NTS);//ֱ��ִ��SQL��� 
		if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
			//return 1;
			SQLFreeHandle(SQL_HANDLE_STMT, hstmt);//�ͷ������ 
		}
		SQLDisconnect(hdbc);//�Ͽ������ݿ������ 
	}
	SQLFreeHandle(SQL_HANDLE_DBC, hdbc);//�ͷ����Ӿ�� 
	SQLFreeHandle(SQL_HANDLE_ENV, henv);//�ͷŻ������
}

//��ö�Ӧ�û�session��GUID��Timestamp
int SQL_GetSession(char* username, char* guid, char* timestamp)
{
	SQLRETURN ret;
	SQLHENV henv;//SQLHANDLE henv 
	SQLHDBC hdbc;//SQLHANDLE hdbc 
	SQLHSTMT hstmt;//SQLHANDLE hstmt 
	ret = SQLAllocHandle(SQL_HANDLE_ENV, NULL, &henv);//���뻷����� 
	ret = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, SQL_IS_INTEGER);//���û������� 
	ret = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);//�������ݿ����Ӿ�� 
	ret = SQLConnect(hdbc, (SQLCHAR*)DataSource, SQL_NTS, (SQLCHAR*)SQL_User, SQL_NTS, (SQLCHAR*)SQL_Password, SQL_NTS);//�������ݿ� 
	if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
		ret = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);//����SQL����� 
		SQLCHAR sql[100];
		sprintf(sql, "SELECT * FROM T_Session where username='%s'", username);
		ret = SQLExecDirect(hstmt, sql, SQL_NTS);//ֱ��ִ��SQL��� 
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
			if (number_row == 0) //û�м�¼
			{
				return 0;
			}
			else {
				return 1;
			}
			SQLFreeHandle(SQL_HANDLE_STMT, hstmt);//�ͷ������ 
		}
		else printf("��ѯ���ݿ����ʧ�ܣ�\n");

		SQLDisconnect(hdbc);//�Ͽ������ݿ������ 
	}
	else printf("�������ݿ�ʧ��!\n");
	SQLFreeHandle(SQL_HANDLE_DBC, hdbc);//�ͷ����Ӿ�� 
	SQLFreeHandle(SQL_HANDLE_ENV, henv);//�ͷŻ������
}

//����GUID����ѯGUID,û�ҵ�����0�����򷵻�1
int SQL_CheckGUID(char* g_username, char* c_guid, char* g_timestamp)
{
	SQLRETURN ret;
	SQLHENV henv;//SQLHANDLE henv 
	SQLHDBC hdbc;//SQLHANDLE hdbc 
	SQLHSTMT hstmt;//SQLHANDLE hstmt 
	ret = SQLAllocHandle(SQL_HANDLE_ENV, NULL, &henv);//���뻷����� 
	ret = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, SQL_IS_INTEGER);//���û������� 
	ret = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);//�������ݿ����Ӿ�� 
	ret = SQLConnect(hdbc, (SQLCHAR*)DataSource, SQL_NTS, (SQLCHAR*)SQL_User, SQL_NTS, (SQLCHAR*)SQL_Password, SQL_NTS);//�������ݿ� 
	if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
		ret = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);//����SQL����� 
		SQLCHAR sql[100];
		sprintf(sql, "SELECT * FROM T_Session where guid='%s'", c_guid);
		ret = SQLExecDirect(hstmt, sql, SQL_NTS);//ֱ��ִ��SQL��� 
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
			if (number_row == 0) //û�м�¼
			{
				return 0;
			}
			else {
				return 1;
			}
			SQLFreeHandle(SQL_HANDLE_STMT, hstmt);//�ͷ������ 
		}
		else printf("��ѯ���ݿ����ʧ�ܣ�\n");
		SQLDisconnect(hdbc);//�Ͽ������ݿ������ 
	}
	else printf("�������ݿ�ʧ��!\n");
	SQLFreeHandle(SQL_HANDLE_DBC, hdbc);//�ͷ����Ӿ�� 
	SQLFreeHandle(SQL_HANDLE_ENV, henv);//�ͷŻ������
}

//����username����ѯGUID,û�ҵ�����0�����򷵻�1
int SQL_CheckGUID2(char* c_username, char* g_guid, char* g_timestamp)
{
	SQLRETURN ret;
	SQLHENV henv;//SQLHANDLE henv 
	SQLHDBC hdbc;//SQLHANDLE hdbc 
	SQLHSTMT hstmt;//SQLHANDLE hstmt 
	ret = SQLAllocHandle(SQL_HANDLE_ENV, NULL, &henv);//���뻷����� 
	ret = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, SQL_IS_INTEGER);//���û������� 
	ret = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);//�������ݿ����Ӿ�� 
	ret = SQLConnect(hdbc, (SQLCHAR*)DataSource, SQL_NTS, (SQLCHAR*)SQL_User, SQL_NTS, (SQLCHAR*)SQL_Password, SQL_NTS);//�������ݿ� 
	if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
		ret = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);//����SQL����� 
		SQLCHAR sql[100];
		sprintf(sql, "SELECT * FROM T_Session where username='%s'", c_username);
		ret = SQLExecDirect(hstmt, sql, SQL_NTS);//ֱ��ִ��SQL��� 
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
			if (number_row == 0) //û�м�¼
			{
				return 0;
			}
			else {
				return 1;
			}
			SQLFreeHandle(SQL_HANDLE_STMT, hstmt);//�ͷ������ 
		}
		else printf("��ѯ���ݿ����ʧ�ܣ�\n");

		SQLDisconnect(hdbc);//�Ͽ������ݿ������ 
	}
	else printf("�������ݿ�ʧ��!\n");
	SQLFreeHandle(SQL_HANDLE_DBC, hdbc);//�ͷ����Ӿ�� 
	SQLFreeHandle(SQL_HANDLE_ENV, henv);//�ͷŻ������
}

//����username��ɾ��GUID
void SQL_DeleteGUID(char* username)
{
	SQLRETURN ret;
	SQLHENV henv;//SQLHANDLE henv 
	SQLHDBC hdbc;//SQLHANDLE hdbc 
	SQLHSTMT hstmt;//SQLHANDLE hstmt 
	ret = SQLAllocHandle(SQL_HANDLE_ENV, NULL, &henv);//���뻷����� 
	ret = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, SQL_IS_INTEGER);//���û������� 
	ret = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);//�������ݿ����Ӿ�� 
	ret = SQLConnect(hdbc, (SQLCHAR*)DataSource, SQL_NTS, (SQLCHAR*)SQL_User, SQL_NTS, (SQLCHAR*)SQL_Password, SQL_NTS);//�������ݿ� 
	if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
		ret = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);//����SQL����� 
		SQLCHAR sql[100];
		sprintf(sql, "DELETE FROM T_Session WHERE username='%s'", username);
		ret = SQLExecDirect(hstmt, sql, SQL_NTS);//ֱ��ִ��SQL��� 
		if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
			SQLFreeHandle(SQL_HANDLE_STMT, hstmt);//�ͷ������ 
		}
		else printf("��ѯ���ݿ����ʧ�ܣ�\n");

		SQLDisconnect(hdbc);//�Ͽ������ݿ������ 
	}
	else printf("�������ݿ�ʧ��!\n");
	SQLFreeHandle(SQL_HANDLE_DBC, hdbc);//�ͷ����Ӿ�� 
	SQLFreeHandle(SQL_HANDLE_ENV, henv);//�ͷŻ������
}

//��¼Web����ͨ��Ϣ���ɹ�����1��ʧ�ܷ���0
int SQL_RecordSite(char* username, char* ip, char* ftpusername, char* ftppassword,char* domain)
{
	SQLRETURN ret;
	SQLHENV henv;//SQLHANDLE henv 
	SQLHDBC hdbc;//SQLHANDLE hdbc 
	SQLHSTMT hstmt;//SQLHANDLE hstmt 
	ret = SQLAllocHandle(SQL_HANDLE_ENV, NULL, &henv);//���뻷����� 
	ret = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, SQL_IS_INTEGER);//���û������� 
	ret = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);//�������ݿ����Ӿ�� 
	ret = SQLConnect(hdbc, (SQLCHAR*)DataSource, SQL_NTS, (SQLCHAR*)SQL_User, SQL_NTS, (SQLCHAR*)SQL_Password, SQL_NTS);//�������ݿ� 
	if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
		ret = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);//����SQL����� 
		SQLCHAR sql[100];
		sprintf(sql, "insert into T_SiteInfo values('%s','%s','%s','%s','%s')", username,ip , ftpusername, ftppassword, domain);
		ret = SQLExecDirect(hstmt, sql, SQL_NTS);//ֱ��ִ��SQL��� 
		if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
			return 1;
			SQLFreeHandle(SQL_HANDLE_STMT, hstmt);//�ͷ������ 
		}
		else return 0;
		SQLDisconnect(hdbc);//�Ͽ������ݿ������ 
	}
	else return 0;;
	SQLFreeHandle(SQL_HANDLE_DBC, hdbc);//�ͷ����Ӿ�� 
	SQLFreeHandle(SQL_HANDLE_ENV, henv);//�ͷŻ������
}

//����ftpusername��ɾ��Site
void SQL_DeleteSite(char* ftpusername)
{
	SQLRETURN ret;
	SQLHENV henv;//SQLHANDLE henv 
	SQLHDBC hdbc;//SQLHANDLE hdbc 
	SQLHSTMT hstmt;//SQLHANDLE hstmt 
	ret = SQLAllocHandle(SQL_HANDLE_ENV, NULL, &henv);//���뻷����� 
	ret = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, SQL_IS_INTEGER);//���û������� 
	ret = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);//�������ݿ����Ӿ�� 
	ret = SQLConnect(hdbc, (SQLCHAR*)DataSource, SQL_NTS, (SQLCHAR*)SQL_User, SQL_NTS, (SQLCHAR*)SQL_Password, SQL_NTS);//�������ݿ� 
	if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
		ret = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);//����SQL����� 
		SQLCHAR sql[100];
		sprintf(sql, "DELETE FROM T_SiteInfo WHERE ftpusername='%s'", ftpusername);
		ret = SQLExecDirect(hstmt, sql, SQL_NTS);//ֱ��ִ��SQL��� 
		if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
			SQLFreeHandle(SQL_HANDLE_STMT, hstmt);//�ͷ������ 
		}
		else {
			cgiHeaderContentType("text/html;charset=gb2312");
			showInfoPage("<h2>������������ϵ����ԱQQ��1367248805��<a href='index.cgi'>������������</a></h2> ");
		}
		SQLDisconnect(hdbc);//�Ͽ������ݿ������ 
	}
	else printf("�������ݿ�ʧ��!\n");
	SQLFreeHandle(SQL_HANDLE_DBC, hdbc);//�ͷ����Ӿ�� 
	SQLFreeHandle(SQL_HANDLE_ENV, henv);//�ͷŻ������
}

//����GUID��ʶ��ָ��
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

//����ʱ���
int GetTimeStamp() {
	time_t t;
	t = time(NULL);
	int ii = time(&t);
	return ii;
}