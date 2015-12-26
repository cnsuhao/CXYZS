#ifndef _CMysql_
#define _CMysql_
#include <stdio.h>
#include <iostream>
#include "windows.h"
#include <mysql.h>
#include <mysql++.h>
#include "MysqlDef.h"

//#define DB_Vector vector<map<string, string>>

using namespace std;
using namespace mysqlpp;


#define MAX_QUERY_SIZE 8192



class CMysql
{
	//����
public:
	//DB_Vector rows;

	//����
public:
	CMysql();
	CMysql(const char *host, const char *user, const char *password, const char *name, int port);
	~CMysql();
	//�������ݿ�
	int DbConnect();
	//�Ͽ����ݿ�����
	//void DbDisConnect() { m_pMysql.disconnect(); }
	//���ݿ��ʼ������ 
	bool Init(const char *host, const char *user, const char *password, const char *name, int port);
	//ִ��sql
	INT32	ExecQuery(const char *cpsSQL, StoreQueryResult &rsResult);
	StoreQueryResult DbQuery(const char *sql);
	INT64 DbExecute(const char *sql, eMysqlQueryType queryType);
	//��ѯ��¼
	StoreQueryResult DbSelect(const char *tableName, map<string, string> &fields, map<string, string> &condition, unsigned long offSet = 0, unsigned long limit=0 , const string &orderBy=NULL, const string &groupBy=NULL);
	//��������
	INT64 DbInsert(const char *tableName, map<string, string> &fields);
	//ɾ����¼
	INT64 DbDelete(const char *tableName,map<string, string> &condition);
	//���¼�¼
	INT64 DbUpdate(const char *tableName, map<string, string> &fields, map<string, string> &condition);
	//�����¼,������ھ͸���
	INT64 DbInsertUpdate(const char *tableName, map<string, string> &fields);
	//��ȡ���һ�γ�����Ϣ
	const char *getDbLastError();
	//��ȡ���һ�β�������ID
	UINT64 getDbLastInsertId();
	//��ȡ���һ��ִ�����
	string GetLastQuerySql();
	char* ConvertBinaryToString(const char* pBinaryData,int nLen);
	//�������
	bool FullSelectPlayerData(const char* strSql,SQL_player_info* playInfo);
	bool UpdatePlayerInfo(SQL_player_info* playerInfo);
	bool getGuidInfo(const char* strSql,GuidInfoIt* guidInfo);
	//��������Ϣ�����
	bool FullSelectGangData(StoreQueryResult res,SQL_gang_info* gangInfo);
	bool FullApplyGangInfo(StoreQueryResult res,SQL_apply_info* applyInfo);
	bool UpdateGangInfo(SQL_gang_info* gangInfo);
	//ping
	void DbPing();

// 	string getHostName() { return m_strHost; };
// 	string getUser() { return m_strUser; };
// 	string getPassword() { return m_strPassword; };
// 	string getName() { return m_strName; };
// 	DWORD getPort() { return m_dwPort; };

	//Connection		GetConnection(){return m_pMysql;}
	//���Զ���
private:
	//���ݿ����Ӿ��
	Connection m_pMysql;
	//ִ��sql���
	char m_strQuery[MAX_QUERY_SIZE];
	//���ݿ������
	string m_strHost;
	//�û���
	string m_strUser;
	//����
	string m_strPassword;
	//���ݿ���
	string m_strName;
	//�˿�
	DWORD m_dwPort;
	//����
private:
	string MakeSelect(map<string, string> &fields);
	string MakeInsert(map<string, string> &fields);
	string MakeUpdate(map<string, string> &fields);

private:
	string MakeCondition(map<string, string> &condition);
private:
	string MakeInsertUpdate(map<string, string> &fields);
	//void Init();
};

#endif
