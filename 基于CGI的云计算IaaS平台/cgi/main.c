#pragma warning(disable : 4996)
#include <stdio.h>
#include <stdlib.h>
#include <winsock.h>
#include "Functions.h"
#include "cgic.h"
#include <Windows.h>
#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>
#include <string.h>
#include "ctemplate.h"

//���ݿ���Ϣ
#define DataSource "CloudComputeSystem"
#define SQL_User "sa"
#define SQL_Password "123456789"

void showInfoPage(char* content);
int cgiMain() {
	//�������
	char action[20] = { 0 };	//��ʶ��ǰ��������ͣ��� ��½��ע�� ��
	char username[20] = { 0 };
	char password[20] = { 0 };
	char email[20] = { 0 };
	char phone[20] = { 0 };
	char ip[20] = { 0 };
	char ftpusername[20] = { 0 };
	char ftppassword[20] = { 0 };
	char domain[20] = { 0 };

	//����������
	cgiFormString("action", &action, 20);
	cgiFormString("username", &username, 20);
	cgiFormString("password", &password, 20);
	cgiFormString("email", &email, 20);
	cgiFormString("phone", &phone, 20);
	cgiFormString("ip", &ip, 20);
	cgiFormString("ftpusername", &ftpusername, 20);
	cgiFormString("ftppassword", &ftppassword, 20);
	cgiFormString("domain", &domain, 20);
	
	//����action������������������Ӧ����
	if (strcmpi(action, "register") == 0) {
		if (SQL_QueryUser(username) != 0) {
			//printf("����Ҫע����˺��Ѿ����ڣ�������û�����");
			cgiHeaderContentType("text/html;charset=gb2312");
			showInfoPage("<h2>����Ҫע����˺��Ѿ����ڣ�<a href='register.html'>������������ע��</a></h2> ");
		}
		else
		{
			SQL_RegisterAccount(username, password, email, phone);
			if (SQL_QueryUser(username) != 0) {
				//printf("ע��ɹ���");
				cgiHeaderContentType("text/html;charset=gb2312");
				showInfoPage("<h2>ע��ɹ���<a href='login.html'>������е�½</a></h2> ");
			}
		}
	}
	else if (strcmpi(action, "login") == 0) {

		if (SQL_QueryUser(username) == 0) {
			//printf("�˺Ų�����");
			cgiHeaderContentType("text/html;charset=gb2312");
			showInfoPage("<h2>�˺Ų����ڣ�<a href='login.html'>������������</a></h2> ");
		}
		else if (SQL_CheckPassword(username, password) == 0)
		{
			//printf("�������");
			cgiHeaderContentType("text/html;charset=gb2312");
			showInfoPage("<h2>�������<a href='login.html'>������������</a></h2> ");
		}
		else {
			////printf("��½�ɹ�");
			char g_username[20] = { 0 },g_guid[50],g_timestamp[20];
			CheckLogin(g_username);
			if (SQL_CheckGUID2(g_username,g_guid,g_timestamp) != 0) {
				SQL_DeleteGUID(g_username);//�����û���ɾ��GUID
			}
			//
			char *guid = createGuid();
			cgiHeaderCookieSetString("guid", guid, 60 * 60 * 24, "", "www.ymbc.net");
			cgiHeaderContentType("text/html;charset=gb2312");
			showInfoPage("<h2>��½�ɹ���<a href='index.cgi'>��������</a></h2> ");
			char time[20];
			sprintf(time, "%d", GetTimeStamp());
			SQL_SetSession(username, guid, time); //��GUID��ӽ����������ݿ�
		}

	}
	else if (strcmpi(action, "control") == 0) {
		cgiHeaderContentType("text/html;charset=gb2312");
		char ip[20], ftpusername[20], ftppassword[20], domain[20];
		TMPL_varlist * ftplist = 0;
		TMPL_varlist * mainlist = 0;
		TMPL_loop* ftploop = 0;
		char g_username[20] = { 0 };
		CheckLogin(g_username); //�õ��Ѿ���½���û���

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
				sprintf(sql, "SELECT * FROM T_SiteInfo where username='%s'", g_username);
				ret = SQLExecDirect(hstmt, sql, SQL_NTS);//ֱ��ִ��SQL��� 
				if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
					SQLCHAR user[20];//, guid[50],timestamp[20];
					SQLINTEGER i_len,f_len,u_len, p_len, d_len;
					while (SQLFetch(hstmt) != SQL_NO_DATA) {
						SQLGetData(hstmt, 2, SQL_C_CHAR, ip, 20, &i_len);
						SQLGetData(hstmt, 3, SQL_C_CHAR, ftpusername, 50, &f_len);
						SQLGetData(hstmt, 4, SQL_C_CHAR, ftppassword, 20, &p_len);
						SQLGetData(hstmt, 5, SQL_C_CHAR, domain, 20, &d_len);
						ftplist = TMPL_add_var(0, "IP", ip, "FTP_Username", ftpusername, "FTP_Password", ftppassword, "Domain", domain);
						ftploop = TMPL_add_varlist(ftploop, ftplist);
						
					}
					mainlist = TMPL_add_loop(mainlist, "FTPLOOP", ftploop);
					int number_row;
					ret = SQLRowCount(hstmt, &number_row);
					if (number_row == 0) //û�м�¼
					{
						//return 0;
					}
					else {
						//return 1;
					}
					SQLFreeHandle(SQL_HANDLE_STMT, hstmt);//�ͷ������ 
				}
				else printf("��ѯ���ݿ����ʧ�ܣ�\n");
		
				SQLDisconnect(hdbc);//�Ͽ������ݿ������ 
			}
			else printf("�������ݿ�ʧ��!\n");
			SQLFreeHandle(SQL_HANDLE_DBC, hdbc);//�ͷ����Ӿ�� 
			SQLFreeHandle(SQL_HANDLE_ENV, henv);//�ͷŻ������
	
		TMPL_write("control.html", 0, 0, mainlist, cgiOut, cgiOut);
	}
	else if (strcmpi(action, "logout") == 0) {
		//ɾ����������Ӧ��GUID
		char g_username[20] = { 0 };
		CheckLogin(g_username); //�õ��Ѿ���½���û���
		SQL_DeleteGUID(g_username);//�����û���ɾ��GUID
		cgiHeaderLocation("http://www.ymbc.net");

	}
	else if (strcmpi(action, "new") == 0) {
		char g_username[20] = { 0 };
		if (CheckLogin(g_username)==0) {
			cgiHeaderContentType("text/html;charset=gb2312");
			showInfoPage("<h2>�����û��½��<a href='login.html'>������������</a></h2> ");
		}
		else //�Ѿ��ǵ�½״̬
		{
			if (SQL_QueryFtpUser(ftpusername)==1) { //��ѯ��Ӧftp�û�������
				cgiHeaderContentType("text/html;charset=gb2312");
				showInfoPage("<h2>����д��FTP�û����Ѿ����ڣ����޸ģ�<a href='new.html'>������������</a></h2> ");
			}else{
				if (SQL_QueryDomain(domain) == 1) {
					cgiHeaderContentType("text/html;charset=gb2312");
					showInfoPage("<h2>����д�������Ѿ����ڣ����޸ģ�<a href='new.html'>������������</a></h2> ");
				}
				else {
					char physicalPath[50] = { 0 };
					sprintf(physicalPath, "D:\\localuser\\%s", ftpusername);
					Create_Directory(ftpusername);
					Create_Site(ftpusername, physicalPath, ip, "80", domain, "http");
					SQL_RecordSite(g_username, ip, ftpusername, ftppassword, domain);
					//����������˻�
					char code[100] = { 0 };
					strcat(code, "net user ");
					strcat(code, ftpusername);
					strcat(code, " ");
					strcat(code, ftppassword);
					strcat(code, " /add");
					system(code);
					cgiHeaderContentType("text/html;charset=gb2312");
					showInfoPage("<form action=\"index.cgi\" method=\"POST\"><h2>��ͨ��ϣ�<input type=\"hidden\" name=\"action\" value=\"control\"/><button type=\"submit\">�����鿴</button></h2></form> ");
				}
			}	
		}
	}
	else if (strcmpi(action, "start") == 0) {
		Start_Site(ftpusername);
		cgiHeaderContentType("text/html;charset=gb2312");
		showInfoPage("<form action=\"index.cgi\" method=\"POST\"><h2>������ϣ�<input type=\"hidden\" name=\"action\" value=\"control\"/><button type=\"submit\">�������</button></h2></form> ");

	}
	else if (strcmpi(action, "stop") == 0) {
		Stop_Site(ftpusername);
		cgiHeaderContentType("text/html;charset=gb2312");
		showInfoPage("<form action=\"index.cgi\" method=\"POST\"><h2>ֹͣ��ϣ�<input type=\"hidden\" name=\"action\" value=\"control\"/><button type=\"submit\">�������</button></h2></form> ");
	}
	else if (strcmpi(action, "delete") == 0) {
		char physicalPath[50] = { 0 };
		sprintf(physicalPath, "D:\\localuser\\%s", ftpusername);
		Delete_Directory(ftpusername);
		Delete_Site(ftpusername);
		SQL_DeleteSite(ftpusername);
		//ɾ��������˻�
		char code[100] = { 0 };
		strcat(code, "net user ");
		strcat(code, ftpusername);
		strcat(code, " /delete");
		system(code);
		cgiHeaderContentType("text/html;charset=gb2312");
		showInfoPage("<form action=\"index.cgi\" method=\"POST\"><h2>ɾ����ϣ�<input type=\"hidden\" name=\"action\" value=\"control\"/><button type=\"submit\">�������</button></h2></form> ");
	}
	else {

		char g_username[20] = {0};
		if (CheckLogin(g_username)==0) {  //��ʾû�е�½������Ĭ����ҳ

			cgiHeaderContentType("text/html;charset=gb2312");
			TMPL_varlist *myList = 0;
			myList = TMPL_add_var(myList, "button_register", "<a class=\"btn btn-outline-success\" href=\"register.html\">ע��</a>", 0);
			myList = TMPL_add_var(myList, "button_login", "<a class=\"btn btn-primary\" href=\"login.html\">��½</a>", 0);
			TMPL_write("index.html", 0, 0, myList, cgiOut, cgiOut);
		}
		else {
			cgiHeaderContentType("text/html;charset=gb2312");
			TMPL_varlist *myList = 0;
			char str[1024] = { 0 };
			sprintf(str, "<p class=\"mb-2 mt-2\"><span class=\"fa fa-user\"></span>%s</p> &nbsp;<form action=\"index.cgi\" method=\"POST\"><input type=\"hidden\" name=\"action\" value=\"control\"/> <button formtarget=\"_blank\" type=\"submit\" class=\"btn btn-outline-primary\">�������</button></form><form action=\"index.cgi\" method=\"POST\"><input type=\"hidden\" name=\"action\" value=\"logout\"/> <button type=\"submit\" class=\"btn btn-danger\">ע��</button></form>",g_username);
			myList = TMPL_add_var(myList, "userinfo", str, 0);
			TMPL_write("index.html", 0, 0, myList, cgiOut, cgiOut);
		}
	}
	return 0;
}


void showInfoPage(char* content) {
	TMPL_varlist *myList = 0;
	char info[1024] = { 0 };
	sprintf(info, "%s", content);
	myList = TMPL_add_var(myList, "info", info, 0);
	TMPL_write("info.html", 0, 0, myList, cgiOut, cgiOut);
}