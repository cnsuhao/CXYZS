/********************************************************************
created:	2014/7/15
author:		LL
desc:		数据库管理类，//!目前只有单一的MysqlHandler，后面考虑封装多个MysqlHandler线程轮流操作数据库。
*********************************************************************/

#ifndef _MysqlManage_
#define _MysqlManage_
#include <windows.h>
#include <mysql.h>
#include <stdio.h>
#include <vector>
#include <time.h>
#include "MysqlDef.h"
#define MANUALCOMMIT "SET AUTOCOMMIT = 0"
#define AUTOCOMMIT "SET AUTOCOMMIT = 1"




#define MAX_CONNECT_URL			128
#define MAX_MYSQL_CONNECT		256
#define MAX_CONNECT_HANDLE      MAX_MYSQL_CONNECT*16

#ifndef BOOL
	#define BOOL bool
#endif
#ifndef TRUE
	#define TRUE true
#endif
#ifndef FALSE
	#define FALSE false
#endif


typedef unsigned char mysqlDateTime[20];
typedef  unsigned __int32(* hashCodeFunc)(unsigned __int32 dwCharID);

//mysqlClient handle
#define MYSQLCLIENT_HANDLE_VALID	1	//有效的
#define MYSQLCLIENT_HANDLE_USED		2	//被使用

#define MAX_NAME_SIZE 32		//名字长度
#define MAX_PASS_SIZE 32		//密码长度

class MysqlHandler
{
public:
	MysqlHandler():m_byState(0),mysql(NULL),res(NULL),m_dwGetCount(0),m_pResultData(NULL),m_dwResultSize(0),m_dwEscapedOffset(0)
	{
		memset(m_pstrEscapedString, 0 , sizeof(m_pstrEscapedString));
	}
	~MysqlHandler(){};
	//初始化数据库
	bool Init(const char* DBIP, const char*  DBAccount, const char* DBPass, const char* DBName, unsigned short Port, bool bAutoTransactions);
	//释放数据库
	void Free()
	{
		ResetResult();
		Close();
	}
	//执行数据库语句,返回0成功
	int ExecSql(const char * sql, unsigned long sqllen);
	//获得上次执行的结果
	bool UseResult();
	//释放执行结果
	void ResetResult();
	//查询数据并填充结果，返回查询到的数量
	int SelectDataByCol(const char* tableName, const mysqlCol *column, const char *where, const char * order);
	//获取当前自增id
	unsigned __int64 GetSerialID(){	return mysql_insert_id(mysql);}	
	//获得上次查询数据
	const unsigned char * GetResultData(){return m_pResultData;}
	//获得上次查询结果长度
	unsigned __int32 ResultSize(){return m_dwResultSize;}
	//获得最后一个错误号
	unsigned int GetLastError(){return mysql_errno(this->mysql);}
	//插入一条数据
	unsigned __int32 InsertDataByCol(const char * tableName, const mysqlCol * column,const unsigned char * data);
	//根据条件删除数据
	int DeleteData(const char * tableName, const char * where);
	//根据条件更新一条数据
	int UpdateDataByCol(const char * tableName, const mysqlCol *column, const unsigned char *data, const char * where);
	//执行语句并返回影响的行数
	int UpdateDataBySql(const char * sql);
	//查询数据并获得结果
	bool SelectDataByColIT(const char * tableName, const mysqlCol * column, const char * where, const char * order);

	//从结果中根据column获取数据到data
	bool SelectDataByColNext(const mysqlCol * column, unsigned char * data);
	
	
	
public:
	//获得结果的行数,查询完自动释放结果
	int GetResultNum()
	{
		//得到结果集
		unsigned int retCount = 0;//查询结果个数
		//MYSQL_RES *result=NULL;
		res = mysql_store_result(mysql);
		if(res != NULL)
		{
			retCount = mysql_num_rows(res);
			mysql_free_result(res);				
		}		
		return retCount;
	}

	bool Commit()
	{
		return !ExecSql("commit",strlen("commit"));
	}

	bool Rollback()
	{
		return !ExecSql("rollback",strlen("rollback"));
	}
	int FetchSelectSql(const char * tableName, const mysqlCol * column, const char * where,const char * order ,char * sql);	
	//用结果row填充Select的数据
	bool FullSelectDataByRow(MYSQL_ROW row, unsigned long * lengths, const mysqlCol * temp, unsigned char * tempData);
	const char* EscapeString(const char * data, size_t len = 0)
	{
		int offset = m_dwEscapedOffset;
		if (len == 0)
		{
			m_dwEscapedOffset += (mysql_real_escape_string(mysql, m_pstrEscapedString + offset, data, strlen(data)) + 1);
		}
		else
		{
			m_dwEscapedOffset += (mysql_real_escape_string(mysql, m_pstrEscapedString + offset, data, len) + 1);

		}
		return m_pstrEscapedString + offset;
	}	
private:
	bool ParseMysqlURLString(const char *connString);
	int SetTransactions(bool supportTransactions)
	{       
		if(supportTransactions)
			return ExecSql(MANUALCOMMIT, strlen(MANUALCOMMIT));
		else    
			return ExecSql(AUTOCOMMIT, strlen(AUTOCOMMIT));
	} 
	//初始化并连接数据库
	bool InitMysql()
	{
		mysql=mysql_init(mysql);
		if(mysql==NULL)
		{                       
			//log_error("Initiate mysql error...");
			return false;           
		}                       

		if(mysql_real_connect(mysql, m_pstrHost, m_pstrUser, m_pstrPasswd, m_pstrDbName, m_wdPort, NULL, CLIENT_COMPRESS|CLIENT_INTERACTIVE) == NULL)
		{                               
			//log_error("connect mysql failed...");
			//log_error("reason: %s",mysql_error(mysql));
			mysql_close(mysql); 
			mysql=NULL;
			return false;   
		}                       
		//log_info("connect mysql successful...");
		return true;                    
	}

	void Close()
	{
		if(mysql!=NULL)
		{
			mysql_close(mysql);
			mysql = NULL;
		}
	}

	

	//设置当前Handler被使用
	bool SetUsed()
	{
		if(m_dwGetCount > 1800 ||time(NULL) - m_dwLifeTimeStamp > 1800 || mysql_ping(mysql) != 0)
		{
			Close();
			if(InitMysql() == false)
				return false;
			m_dwLifeTimeStamp = time(NULL);
			m_dwGetCount = 0;
		}

		m_byState |= MYSQLCLIENT_HANDLE_USED;
		m_dwGetCount++;
		m_dwTimeStamp = time(NULL);
#ifdef _MULTI_THREAD_USE_MYSQL_
		m_OwnerThread = pthread_self();	
#endif
		return true;
	}
	//清除使用
	void ClearUsed()
	{
		m_byState ^= MYSQLCLIENT_HANDLE_USED;
		m_dwTimeStamp = time(NULL); 
#ifdef _MULTI_THREAD_USE_MYSQL_
		m_OwnerThread = 0;	
#endif
		ResetResult();
	}

	void ResetEscapedString()
	{
		m_dwEscapedOffset = 0;
	}

	unsigned char State()
	{
		return m_byState;
	}

	unsigned __int32 TimeStamp()
	{
		return m_dwTimeStamp;
	}

	const char * Sql()
	{
		return m_pstrSql;
	}
private:
	unsigned char m_byState;	

	char m_pstrHost[MAX_NAME_SIZE];
	char m_pstrUser[MAX_NAME_SIZE];
	char m_pstrPasswd[MAX_PASS_SIZE];
	unsigned short m_wdPort;
	char m_pstrDbName[MAX_NAME_SIZE];

	MYSQL * mysql;
	MYSQL_RES * res;
	unsigned short m_dwGetCount;

	unsigned __int32 m_dwLifeTimeStamp;
	unsigned __int32 m_dwTimeStamp;

	unsigned char * m_pResultData;
	unsigned __int32 m_dwResultSize;
	int	  m_dwEscapedOffset; 
#ifdef _MULTI_THREAD_USE_MYSQL_
	pthread_t m_OwnerThread;	
#endif

	char m_pstrSql[4096];	
	char m_pstrEscapedString[1024];
};

#endif	//_MysqlManage_


