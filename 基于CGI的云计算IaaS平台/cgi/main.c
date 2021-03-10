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

//数据库信息
#define DataSource "CloudComputeSystem"
#define SQL_User "sa"
#define SQL_Password "123456789"

void showInfoPage(char* content);
int cgiMain() {
	//请求参数
	char action[20] = { 0 };	//标识当前请求的类型，如 登陆，注册 等
	char username[20] = { 0 };
	char password[20] = { 0 };
	char email[20] = { 0 };
	char phone[20] = { 0 };
	char ip[20] = { 0 };
	char ftpusername[20] = { 0 };
	char ftppassword[20] = { 0 };
	char domain[20] = { 0 };

	//获得请求参数
	cgiFormString("action", &action, 20);
	cgiFormString("username", &username, 20);
	cgiFormString("password", &password, 20);
	cgiFormString("email", &email, 20);
	cgiFormString("phone", &phone, 20);
	cgiFormString("ip", &ip, 20);
	cgiFormString("ftpusername", &ftpusername, 20);
	cgiFormString("ftppassword", &ftppassword, 20);
	cgiFormString("domain", &domain, 20);
	
	//根据action的请求类型来进行相应操作
	if (strcmpi(action, "register") == 0) {
		if (SQL_QueryUser(username) != 0) {
			//printf("您所要注册的账号已经存在，请更换用户名！");
			cgiHeaderContentType("text/html;charset=gb2312");
			showInfoPage("<h2>您所要注册的账号已经存在！<a href='register.html'>请点击返回重新注册</a></h2> ");
		}
		else
		{
			SQL_RegisterAccount(username, password, email, phone);
			if (SQL_QueryUser(username) != 0) {
				//printf("注册成功！");
				cgiHeaderContentType("text/html;charset=gb2312");
				showInfoPage("<h2>注册成功！<a href='login.html'>点击进行登陆</a></h2> ");
			}
		}
	}
	else if (strcmpi(action, "login") == 0) {

		if (SQL_QueryUser(username) == 0) {
			//printf("账号不存在");
			cgiHeaderContentType("text/html;charset=gb2312");
			showInfoPage("<h2>账号不存在！<a href='login.html'>请点击返回重试</a></h2> ");
		}
		else if (SQL_CheckPassword(username, password) == 0)
		{
			//printf("密码错误");
			cgiHeaderContentType("text/html;charset=gb2312");
			showInfoPage("<h2>密码错误！<a href='login.html'>请点击返回重试</a></h2> ");
		}
		else {
			////printf("登陆成功");
			char g_username[20] = { 0 },g_guid[50],g_timestamp[20];
			CheckLogin(g_username);
			if (SQL_CheckGUID2(g_username,g_guid,g_timestamp) != 0) {
				SQL_DeleteGUID(g_username);//依据用户名删除GUID
			}
			//
			char *guid = createGuid();
			cgiHeaderCookieSetString("guid", guid, 60 * 60 * 24, "", "www.ymbc.net");
			cgiHeaderContentType("text/html;charset=gb2312");
			showInfoPage("<h2>登陆成功！<a href='index.cgi'>请点击返回</a></h2> ");
			char time[20];
			sprintf(time, "%d", GetTimeStamp());
			SQL_SetSession(username, guid, time); //将GUID添加进服务器数据库
		}

	}
	else if (strcmpi(action, "control") == 0) {
		cgiHeaderContentType("text/html;charset=gb2312");
		char ip[20], ftpusername[20], ftppassword[20], domain[20];
		TMPL_varlist * ftplist = 0;
		TMPL_varlist * mainlist = 0;
		TMPL_loop* ftploop = 0;
		char g_username[20] = { 0 };
		CheckLogin(g_username); //得到已经登陆的用户名

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
				sprintf(sql, "SELECT * FROM T_SiteInfo where username='%s'", g_username);
				ret = SQLExecDirect(hstmt, sql, SQL_NTS);//直接执行SQL语句 
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
					if (number_row == 0) //没有记录
					{
						//return 0;
					}
					else {
						//return 1;
					}
					SQLFreeHandle(SQL_HANDLE_STMT, hstmt);//释放语句句柄 
				}
				else printf("查询数据库操作失败！\n");
		
				SQLDisconnect(hdbc);//断开与数据库的连接 
			}
			else printf("连接数据库失败!\n");
			SQLFreeHandle(SQL_HANDLE_DBC, hdbc);//释放连接句柄 
			SQLFreeHandle(SQL_HANDLE_ENV, henv);//释放环境句柄
	
		TMPL_write("control.html", 0, 0, mainlist, cgiOut, cgiOut);
	}
	else if (strcmpi(action, "logout") == 0) {
		//删除服务器对应的GUID
		char g_username[20] = { 0 };
		CheckLogin(g_username); //得到已经登陆的用户名
		SQL_DeleteGUID(g_username);//依据用户名删除GUID
		cgiHeaderLocation("http://www.ymbc.net");

	}
	else if (strcmpi(action, "new") == 0) {
		char g_username[20] = { 0 };
		if (CheckLogin(g_username)==0) {
			cgiHeaderContentType("text/html;charset=gb2312");
			showInfoPage("<h2>你好像还没登陆！<a href='login.html'>请点击返回重试</a></h2> ");
		}
		else //已经是登陆状态
		{
			if (SQL_QueryFtpUser(ftpusername)==1) { //查询对应ftp用户名存在
				cgiHeaderContentType("text/html;charset=gb2312");
				showInfoPage("<h2>你填写的FTP用户名已经存在，请修改！<a href='new.html'>请点击返回重试</a></h2> ");
			}else{
				if (SQL_QueryDomain(domain) == 1) {
					cgiHeaderContentType("text/html;charset=gb2312");
					showInfoPage("<h2>你填写的域名已经存在，请修改！<a href='new.html'>请点击返回重试</a></h2> ");
				}
				else {
					char physicalPath[50] = { 0 };
					sprintf(physicalPath, "D:\\localuser\\%s", ftpusername);
					Create_Directory(ftpusername);
					Create_Site(ftpusername, physicalPath, ip, "80", domain, "http");
					SQL_RecordSite(g_username, ip, ftpusername, ftppassword, domain);
					//创建计算机账户
					char code[100] = { 0 };
					strcat(code, "net user ");
					strcat(code, ftpusername);
					strcat(code, " ");
					strcat(code, ftppassword);
					strcat(code, " /add");
					system(code);
					cgiHeaderContentType("text/html;charset=gb2312");
					showInfoPage("<form action=\"index.cgi\" method=\"POST\"><h2>开通完毕！<input type=\"hidden\" name=\"action\" value=\"control\"/><button type=\"submit\">请点击查看</button></h2></form> ");
				}
			}	
		}
	}
	else if (strcmpi(action, "start") == 0) {
		Start_Site(ftpusername);
		cgiHeaderContentType("text/html;charset=gb2312");
		showInfoPage("<form action=\"index.cgi\" method=\"POST\"><h2>启动完毕！<input type=\"hidden\" name=\"action\" value=\"control\"/><button type=\"submit\">点击返回</button></h2></form> ");

	}
	else if (strcmpi(action, "stop") == 0) {
		Stop_Site(ftpusername);
		cgiHeaderContentType("text/html;charset=gb2312");
		showInfoPage("<form action=\"index.cgi\" method=\"POST\"><h2>停止完毕！<input type=\"hidden\" name=\"action\" value=\"control\"/><button type=\"submit\">点击返回</button></h2></form> ");
	}
	else if (strcmpi(action, "delete") == 0) {
		char physicalPath[50] = { 0 };
		sprintf(physicalPath, "D:\\localuser\\%s", ftpusername);
		Delete_Directory(ftpusername);
		Delete_Site(ftpusername);
		SQL_DeleteSite(ftpusername);
		//删除计算机账户
		char code[100] = { 0 };
		strcat(code, "net user ");
		strcat(code, ftpusername);
		strcat(code, " /delete");
		system(code);
		cgiHeaderContentType("text/html;charset=gb2312");
		showInfoPage("<form action=\"index.cgi\" method=\"POST\"><h2>删除完毕！<input type=\"hidden\" name=\"action\" value=\"control\"/><button type=\"submit\">点击返回</button></h2></form> ");
	}
	else {

		char g_username[20] = {0};
		if (CheckLogin(g_username)==0) {  //表示没有登陆，加载默认首页

			cgiHeaderContentType("text/html;charset=gb2312");
			TMPL_varlist *myList = 0;
			myList = TMPL_add_var(myList, "button_register", "<a class=\"btn btn-outline-success\" href=\"register.html\">注册</a>", 0);
			myList = TMPL_add_var(myList, "button_login", "<a class=\"btn btn-primary\" href=\"login.html\">登陆</a>", 0);
			TMPL_write("index.html", 0, 0, myList, cgiOut, cgiOut);
		}
		else {
			cgiHeaderContentType("text/html;charset=gb2312");
			TMPL_varlist *myList = 0;
			char str[1024] = { 0 };
			sprintf(str, "<p class=\"mb-2 mt-2\"><span class=\"fa fa-user\"></span>%s</p> &nbsp;<form action=\"index.cgi\" method=\"POST\"><input type=\"hidden\" name=\"action\" value=\"control\"/> <button formtarget=\"_blank\" type=\"submit\" class=\"btn btn-outline-primary\">控制面板</button></form><form action=\"index.cgi\" method=\"POST\"><input type=\"hidden\" name=\"action\" value=\"logout\"/> <button type=\"submit\" class=\"btn btn-danger\">注销</button></form>",g_username);
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