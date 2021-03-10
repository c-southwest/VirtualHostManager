#pragma once
//��վ��غ���
void Create_Directory(char *name); //��D:\localuser\�´����ļ���
void Delete_Directory(char *name);

void Create_Site(char* name, char* PhysicalPath, char* ip, char* port, char* hostname, char* protocol); //�½�վ��
void Delete_Site(char* name);

void Start_Site(char* name); //����վ��

void Stop_Site(char* name); //ֹͣվ��


/***********************************************************
int SQL_QueryUser(char* username)
������username���û���
���ܣ���ѯ��Ӧ�û��������ݿ����Ƿ����
���أ��û����ڷ���1�������ڷ���0
***********************************************************/
int SQL_QueryUser(char* username); 

/***********************************************************
int SQL_QueryUser(char* username)
������username���û���
���ܣ���ѯ��Ӧ�û����������Ƿ���ȷ
���أ�������ȷ����1�������ڷ���0
***********************************************************/
int SQL_CheckPassword(char* username, char* password); 

/***********************************************************
int SQL_RegisterAccount(char* username, char* password, char* email, char* phone)
�������û��������룬���䣬�ֻ���
���ܣ�ע�����û�
���أ�ע��ɹ�����1�����򷵻�0
***********************************************************/
int SQL_RegisterAccount(char* username, char* password, char* email, char* phone); 

/***********************************************************
void SQL_SetSession(char* username, char* guid, char* timestamp); 
�������û�����GUIDΨһ��ʶ����ʱ���
���ܣ����û���½������GUID��ʶ���Լ���ǰʱ���ʱ�����¼���������У�
���أ���
***********************************************************/
void SQL_SetSession(char* username, char* guid, char* timestamp);

/***********************************************************
int SQL_GetSession(char* username, char* guid, char* timestamp)
�������û��������ڴ洢GUIDΨһ��ʶ�������ڴ洢ʱ���
���ܣ��������ȡ�ö�Ӧ�û���GUID��ʶ���Լ�ʱ���
���أ�ȡ�óɹ�����1�����򷵻�0
***********************************************************/
int SQL_GetSession(char* username, char* guid, char* timestamp);

/***********************************************************
int SQL_CheckGUID(char* username, char* guid, char* timestamp)
���������ڴ洢�û�����GUIDΨһ��ʶ�������ڴ洢ʱ���
���ܣ����������ѯָ��GUID��ʶ��������������ȡ�ض�Ӧ���û�����ʱ���
���أ���ӦGUID���ڷ���1�����򷵻�0
***********************************************************/
int SQL_CheckGUID(char* username, char* guid, char* timestamp);

/***********************************************************
void SQL_DeleteGUID(char* username)
�������û���
���ܣ�ɾ���������ж�Ӧ�û���GUID
���أ���
***********************************************************/
void SQL_DeleteGUID(char* username); 

/***********************************************************
int SQL_CheckGUID2(char* c_username, char* g_guid, char* g_timestamp)
�������û��������ڴ洢GUID�����ڴ洢ʱ���
���ܣ���������в�ѯ��Ӧ�û����������GUID��ʱ���
���أ���ѯ�ɹ�����1�����򷵻�0
***********************************************************/
int SQL_CheckGUID2(char* c_username, char* g_guid, char* g_timestamp);

/***********************************************************
int CheckLogin(char *g_username)
���������ڴ洢�û���
���ܣ���鵱ǰ״̬�Ƿ��Ѿ���½�������½�ɹ��ɷ��ض�Ӧ�û���
���أ��Ѿ��ɹ���½����1�����򷵻�0
***********************************************************///
int CheckLogin(char *g_username);

//��¼Web����ͨ��Ϣ���ɹ�����1��ʧ�ܷ���0
int SQL_RecordSite(char* username, char* ip, char* ftpusername, char* ftppassword, char* domain);


//����ftpusername��ɾ��Site
void SQL_DeleteSite(char* ftpusername);

//��ѯFTP�û��Ƿ���ڣ������ڷ���1�������ڷ���0
int SQL_QueryFtpUser(char* ftpusername);

//��ѯDomain�Ƿ���ڣ������ڷ���1�������ڷ���0
int SQL_QueryDomain(char* domain);

char* createGuid(); //����GUID

int GetTimeStamp(); //�õ�ʱ���