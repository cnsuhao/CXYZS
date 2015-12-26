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
	//属性
public:
	//DB_Vector rows;

	//方法
public:
	CMysql();
	CMysql(const char *host, const char *user, const char *password, const char *name, int port);
	~CMysql();
	//连接数据库
	int DbConnect();
	//断开数据库连接
	//void DbDisConnect() { m_pMysql.disconnect(); }
	//数据库初始化函数 
	bool Init(const char *host, const char *user, const char *password, const char *name, int port);
	//执行sql
	INT32	ExecQuery(const char *cpsSQL, StoreQueryResult &rsResult);
	StoreQueryResult DbQuery(const char *sql);
	INT64 DbExecute(const char *sql, eMysqlQueryType queryType);
	//查询记录
	StoreQueryResult DbSelect(const char *tableName, map<string, string> &fields, map<string, string> &condition, unsigned long offSet = 0, unsigned long limit=0 , const string &orderBy=NULL, const string &groupBy=NULL);
	//插入数据
	INT64 DbInsert(const char *tableName, map<string, string> &fields);
	//删除记录
	INT64 DbDelete(const char *tableName,map<string, string> &condition);
	//更新记录
	INT64 DbUpdate(const char *tableName, map<string, string> &fields, map<string, string> &condition);
	//插入记录,如果存在就跟新
	INT64 DbInsertUpdate(const char *tableName, map<string, string> &fields);
	//提取最后一次出错信息
	const char *getDbLastError();
	//提取最后一次插入自增ID
	UINT64 getDbLastInsertId();
	//提取最后一次执行语句
	string GetLastQuerySql();
	char* ConvertBinaryToString(const char* pBinaryData,int nLen);
	//填充结果集
	bool FullSelectPlayerData(const char* strSql,SQL_player_info* playInfo);
	bool UpdatePlayerInfo(SQL_player_info* playerInfo);
	bool getGuidInfo(const char* strSql,GuidInfoIt* guidInfo);
	//填充帮派信息结果集
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
	//属性定义
private:
	//数据库连接句柄
	Connection m_pMysql;
	//执行sql语句
	char m_strQuery[MAX_QUERY_SIZE];
	//数据库服务器
	string m_strHost;
	//用户名
	string m_strUser;
	//密码
	string m_strPassword;
	//数据库名
	string m_strName;
	//端口
	DWORD m_dwPort;
	//方法
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
