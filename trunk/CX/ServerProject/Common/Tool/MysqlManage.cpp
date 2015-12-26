#include <errno.h>
#ifndef __USE_UNIX98
#define __USE_UNIX98
#endif

#include <string.h> 
#include<stdlib.h>
#include "MysqlManage.h"

time_t StrtoDataTime(const char* pStrTime)
{
	struct tm tm;
	//!strptime(pStrTime, "%Y-%m-%d %H:%M:%S", &tm);

	return mktime(&tm);
}

char* DateTimetoStr(time_t time)
{
	static char szTime[20];
	struct tm *tmp;

	tmp = localtime(&time);
	if (NULL == tmp)
		return NULL;

	if (strftime(szTime, sizeof(szTime), "%Y-%m-%d %H:%M:%S", tmp) == 0)
		return NULL;

	return szTime;
}

bool MysqlHandler::Init(const char* DBIP, const char*  DBAccount, const char* DBPass, const char* DBName, unsigned short Port, bool bAutoTransactions)
{
	//设置连接参数
	strcpy(m_pstrHost, DBIP);
	strcpy(m_pstrUser, DBAccount);
	strcpy(m_pstrPasswd, DBPass);
	strcpy(m_pstrDbName, DBName);
	m_wdPort = Port;

	if(InitMysql() == false)
		return false;
	m_byState |= MYSQLCLIENT_HANDLE_VALID;
	mysql_query(mysql, "SET NAMES utf8");	//设置编码格式,否则在cmd下无法显示中文
	m_dwLifeTimeStamp = time(NULL);
	return !SetTransactions(bAutoTransactions);
}

int MysqlHandler::ExecSql(const char * sql, unsigned long sqllen)
{
	if(sql == NULL || sqllen == 0 || mysql == NULL)
	{
		printf("invalid mysql handle or sql statement.");
		return -1;
	}

    //struct timeval tvStart, tvStop; 
    //gettimeofday(&tvStart,NULL);
	//strncpy(m_pstrSql, sql, sizeof(m_pstrSql));
	int ret = mysql_real_query(mysql, sql, sqllen);
	if(ret)
	{
		printf("%s\n",mysql_error(mysql));
		printf("%s\n",sql);
	}       
    /*else 
    {
        gettimeofday(&tvStop,NULL);
        if(tvStop.tv_sec - tvStart.tv_sec >= 1)
            log_info("%s execsql use (%lu/%lu) sql=%.*s .%s"
                    , Red
                    , tvStop.tv_sec - tvStart.tv_sec
                    , (tvStop.tv_usec -tvStart.tv_usec)/1000
                    , sizeof(m_pstrSql)
                    , m_pstrSql
                    , None);
    }*/
	return ret;
}

bool MysqlHandler::UseResult()
{       
	//得到结果集
	res = mysql_use_result(mysql);
	if(res == NULL)
	{
		//pthread_mutex_unlock(&resultITLock);
		//log_error(mysql_error(mysql));
		return false;
	}
	else
		return true;
}

void MysqlHandler::ResetResult()
{
	if(m_pResultData)
	{
		delete[] m_pResultData;
		m_pResultData = NULL;
	}
	m_dwResultSize = 0;
	if(res != NULL)
	{
		mysql_free_result(res);
		res = NULL;
	}
}	

int MysqlHandler::UpdateDataBySql(const char * sql)
{
	ExecSql(sql, strlen(sql));
	unsigned long ret = (unsigned long)mysql_affected_rows(mysql);
	return ret;
}

int MysqlHandler::FetchSelectSql(const char * tableName, const mysqlCol * column, const char *where,const char * order ,char * sql)
{
	unsigned int retsize=0;//查询结果大小
	const mysqlCol * temp;
	int len, sqllen = 0;;
	//生成SQL,计算返回结果大小
	bool first= true;
	strcat(sql,"SELECT "); sqllen += 7/*strlen("SELECT ")*/;
	temp = column;
	while(temp->name)
	{
		if(temp->dataSize == 0)
		{
			//log_error("invalid column data size.");
			return -1;//retval;//字段大小为零，错误
		}
		retsize += temp->dataSize;//计算返回值的大小
		len = strlen(temp->name);
		if(len > 0)//有效column
		{
			if(first) 
			{
				first = false;
			}
			else 
			{
				strcat(sql,", "); 
				sqllen += 2/*strlen(", ")*/;
			}
			strcat(sql, "`");
			strcat(sql,temp->name); 
			strcat(sql, "`");
			sqllen += (len+2);
		}
		temp++;
	}
	len = strlen(tableName);
	if(len > 0)
	{
		strcat(sql," FROM "); 
		sqllen += 6/*strlen(" FROM ")*/;
		strcat(sql,tableName); 
		sqllen += len;
	}
	if(where != NULL)
	{
		strcat(sql," WHERE "); 
		sqllen += 7/*strlen(" WHERE ")*/;
		strcat(sql,where); 
		sqllen += strlen(where);
	}
	if(order != NULL)
	{
		strcat(sql," ORDER BY ");
		sqllen += 10/*strlen(" ORDER BY ")*/;
		strcat(sql,order); 
		sqllen += strlen(order);
	}

	if(ExecSql(sql, sqllen) != 0)
		return -1;//retval;
	else
		return retsize;
}

bool MysqlHandler::FullSelectDataByRow(MYSQL_ROW row, unsigned long * lengths, const mysqlCol * temp,unsigned char * tempData)
{
	int offset = 0;//column数据偏移量
	int i = 0;
	while(temp->name)
	{
		if(strlen(temp->name) != 0)
		{
			switch(temp->mysqlType)
			{
				case MYSQL_INT8:
					if (row[i])
						*(char *)(tempData+offset)=strtol(row[i],(char **)NULL,10);
					else
						*(char *)(tempData+offset)=0;
				case MYSQL_UINT8:
					if (row[i])
						*(unsigned char *)(tempData+offset)=strtol(row[i],(char **)NULL,10);
					else
						*(unsigned char *)(tempData+offset)=0;
					break;
				case MYSQL_UINT16:
					if (row[i]) 
						*(unsigned short *)(tempData+offset)=strtoul(row[i],(char **)NULL,10);
					else
						*(unsigned short *)(tempData+offset)=0;
					break;
				case MYSQL_UINT32:
					if (row[i])
						*(unsigned __int32 *)(tempData+offset)=strtoul(row[i],(char **)NULL,10);
					else
						*(unsigned __int32 *)(tempData+offset)=0L;
					break;
				case MYSQL_UINT64:
					if (row[i])
						*(unsigned __int64 *)(tempData+offset)=_strtoui64(row[i],(char **)NULL,10);
					else
						*(unsigned __int64 *)(tempData+offset)=0LL;
					break;
				case MYSQL_INT64:
					if (row[i])
						*(__int64 *)(tempData+offset)=strtol(row[i],(char **)NULL,10);
					else
						*(__int64 *)(tempData+offset)=0LL;
					break;
				case MYSQL_INT32:
					if (row[i])
						*(int *)(tempData+offset)=strtol(row[i],(char **)NULL,10);
					else
						*(int *)(tempData+offset)=0LL;
					break;
				case MYSQL_FLOAT:
					if (row[i])
						*(float *)(tempData+offset)=atof(row[i]);
					else
						*(float *)(tempData+offset)=0.0;
					break;
				case MYSQL_INT16:
					if (row[i])
						*(short *)(tempData+offset)=strtol(row[i],(char **)NULL,10);
					else
						*(short *)(tempData+offset)=0;
					break;
				
				case MYSQL_STR:
				case MYSQL_BIN:
					memset(tempData+offset, 0, temp->dataSize);
					if (row[i])
					{
						memcpy(tempData+offset,row[i],temp->dataSize>lengths[i]?lengths[i]:temp->dataSize);
					}
					break;
				case MYSQL_DATETIME:
					if (row[i])
						*(unsigned __int32 *)(tempData+offset)= StrtoDataTime(row[i]);
					else
						*(unsigned __int32 *)(tempData+offset)=0L;
					break;

					break;
				/*case MYSQL_ZIP:
					{
						bzero(tempData+offset,temp->dataSize);
						if (row[i]) 
						{
							int retcode;
							uLong destLen = temp->dataSize;
							retcode = uncompress(tempData+offset, &destLen, (Bytef *)row[i], lengths[i])
								;
							switch(retcode) 
							{
								case Z_OK:
									break;
								case Z_MEM_ERROR:
									bzero(tempData+offset,temp->dataSize);
									//log_error( "(%s, %d)Z_MEM_ERROR, zero value instead.", __FUNCTION__, __LINE__);
									break;
								case Z_BUF_ERROR:
									bzero(tempData+offset,temp->dataSize);
									//log_error( "(%s, %d)Z_BUF_ERROR, zero value instead.", __FUNCTION__, __LINE__);
									break;
								case Z_DATA_ERROR:
									bzero(tempData+offset,temp->dataSize);
									//log_error( "(%s, %d)Z_DATA_ERROR, zero value instead.", __FUNCTION__, __LINE__);
									break;
							}
						}
					}
					break;*/
				default:
					//log_error("invalid titan mysql type(%d). ->%s",temp->mysqlType,temp->name);
					return FALSE;
			}
			i++;
		}
		offset+=temp->dataSize;
		temp++;
	}
	return TRUE;
}

int MysqlHandler::SelectDataByCol(const char* tableName, const mysqlCol *column, const char *where, const char * order) 
{
	char sql[4096] = "";
	int retval = -1;//返回值
	unsigned int retsize = 0;//查询结果大小

	if(tableName == NULL || column == NULL || mysql == NULL)
	{
		//log_error("null pointer error. ---- %s",__FUNCTION__);
		return retval;
	}       
	//生成Sql，并执行
	retsize = FetchSelectSql(tableName, column, where, order, sql);
	if(retsize < 1) 
		return retval;

	//得到结果集
	unsigned int retCount = 0;//查询结果个数
	//MYSQL_RES *result=NULL;
	res = mysql_store_result(mysql);
	if(res == NULL)
	{
		//log_error(mysql_error(mysql));
		return retval;
	}

	retCount = mysql_num_rows(res);
	if(retCount == 0)
	{
		mysql_free_result(res);
		return retCount;
	}

	if(m_pResultData != NULL)
	{
		delete[] m_pResultData;
		m_pResultData = NULL;
		m_dwResultSize = 0;
	}

	m_pResultData = new unsigned char[retCount * retsize / sizeof(unsigned char)];
	m_dwResultSize = retCount * retsize;

	if(m_pResultData == NULL)
	{
		//log_error("malloc mem error .");
		mysql_free_result(res);
		return retval;
	}

	memset(m_pResultData, 0, m_dwResultSize);

	//填充返回值
	MYSQL_ROW row;
	unsigned char * tempData = m_pResultData;
	while ((row = mysql_fetch_row(res)))
	{
		unsigned long * lengths= mysql_fetch_lengths(res);
		if(!FullSelectDataByRow(row, lengths, column, tempData))
		{
			delete[] m_pResultData;
			m_dwResultSize = 0;
			mysql_free_result(res);
			return retval;
		}
		tempData += retsize;
	}

	mysql_free_result(res);
	return retCount;
} 

unsigned __int32 MysqlHandler::InsertDataByCol(const char * tableName, const mysqlCol * column,const unsigned char * data)
{
	char sql[1024*1024] = "";
	const mysqlCol * temp;
	int len, sqllen = 0;
	if(tableName == NULL || data == NULL || column == NULL || mysql == NULL)
	{
		//log_error("null pointer error. ---- %s",__FUNCTION__);
		return false;
	}

	//生成SQL
	strcat(sql,"INSERT INTO "); 
	sqllen += 12/*strlen("INSERT INTO ")*/;
	strcat(sql,tableName); 
	sqllen += strlen(tableName);
	strcat(sql," ( "); 
	sqllen += 3/*strlen(" ( ")*/;
	temp = column;
	bool first = true;
	while(temp->name)
	{
		len = strlen(temp->name);
		if(len > 0)
		{
			if(first) 
			{
				first = false;
			}
			else 
			{
				strcat(sql,", "); 
				sqllen += 2/*strlen(", ")*/;
			}
			strcat(sql,temp->name); 
			sqllen += len;
		}
		if(temp->dataSize == 0)
		{
			//log_error("invalid column data size.");
			return false;
		}
		temp++;
	}
	strcat(sql,") VALUES( "); 
	sqllen += 10/*strlen(") VALUES( ")*/;

	first = true;
	temp = column;
	int offset = 0;
	while(temp->name)
	{
		if(strlen(temp->name)!=0)
		{
			if(first) 
			{
				first = false;
			}
			else 
			{
				strcat(sql,", "); 
				sqllen += 2/*strlen(", ")*/;
			}
			//values内容
			switch(temp->mysqlType)
			{

				case MYSQL_INT8:
					{
						sqllen += sprintf(&sql[sqllen], "%d", *(char *)(data+offset));
					}
					break;
				case MYSQL_INT16:
					{
						sqllen += sprintf(&sql[sqllen], "%d", *(__int16 *)(data+offset));
					}
					break;
				case MYSQL_INT32:
					{
						sqllen += sprintf(&sql[sqllen], "%d", *(__int32 *)(data+offset));
					}
					break;
				case MYSQL_INT64:
					{
						sqllen += sprintf(&sql[sqllen], "%d", *(__int64 *)(data+offset));
					}
					break;
				case MYSQL_UINT8:
					{
						sqllen += sprintf(&sql[sqllen], "%u", *(unsigned char *)(data+offset));
					}
					break;
				case MYSQL_UINT16:
					{
						sqllen += sprintf(&sql[sqllen], "%u", *(unsigned short *)(data+offset));
					}
					break;
				case MYSQL_UINT32:
					{
						sqllen += sprintf(&sql[sqllen], "%u", *(unsigned __int32 *)(data+offset));
					}
					break;
				case MYSQL_UINT64:
					{
						sqllen += sprintf(&sql[sqllen], "%llu", *(unsigned __int64 *)(data+offset));
					}
					break;
				case MYSQL_FLOAT:
					{
						sqllen += sprintf(&sql[sqllen], "%f", *(float *)(data+offset));
					}
					break;
				
				case MYSQL_STR:
					{
						strcat(sql,"\'"); 
						sqllen++/*sqllen += strlen("\'")*/;
						len=strlen((char *)(data+offset));
						if((unsigned __int32)len > temp->dataSize)
							len = temp->dataSize;
						sqllen += mysql_real_escape_string(mysql, &sql[sqllen],(char *)(data+offset),len);
						strcat(sql,"\'"); 
						sqllen++/*sqllen += strlen("\'")*/;
					}
					break;
				case MYSQL_DATETIME:
					{
						strcat(sql,"\'"); 
						sqllen++/*sqllen += strlen("\'")*/;
						char* pszTime = DateTimetoStr(*(unsigned __int32 *)(data+offset));
						len=strlen(pszTime);
						/*if((unsigned __int32)len > temp->dataSize)
							len = temp->dataSize;
						*/
						sqllen += mysql_real_escape_string(mysql, &sql[sqllen], pszTime, len);
						strcat(sql,"\'"); 
						sqllen++/*sqllen += strlen("\'")*/;

					}
					break;
				case MYSQL_BIN:
					{
						strcat(sql,"\'"); 
						sqllen++/*sqllen += strlen("\'")*/;
						sqllen += mysql_real_escape_string(mysql, &sql[sqllen],(char *)(data+offset),temp->dataSize);
						strcat(sql,"\'"); 
						sqllen++/*sqllen += strlen("\'")*/;	
					}
					break;
				/*case MYSQL_ZIP:
					{
						//计算压缩目的缓冲长度,must be at least 0.1% larger than sourceLen plus 12 bytes
						uLong destLen = temp->dataSize * 120 / 100 + 12;
						Bytef * destBuffer = new Bytef[destLen];
						strcat(sql,"\'"); 
						sqllen++;
						if (destBuffer) 
						{
							int retcode = compress(destBuffer, &destLen, (Bytef *)(data+offset),temp->dataSize);
							switch(retcode) 
							{
								case Z_OK:
									sqllen += mysql_real_escape_string(mysql,&sql[sqllen],(char *)destBuffer,destLen);
									break;
								case Z_MEM_ERROR:
									//log_fatal( "(%s, %d)Z_MEM_ERROR, NULL value instead.", __FUNCTION__, __LINE__);
									break;
								case Z_BUF_ERROR:
									//log_fatal( "(%s, %d)Z_BUF_ERROR, NULL value instead.", __FUNCTION__, __LINE__);
									break;
							}
							delete[] destBuffer;
						}
						else 
						{
							//log_fatal( "(%s, %d)Not enough memory, NULL value instead.", __FUNCTION__, __LINE__);
						}
						strcat(sql,"\'"); 
						sqllen++;
					}
					break;*/
				default:
					//log_error("invalid titan mysql type.");
					return false;
			}
		}
		offset += temp->dataSize;
		temp++;
	}
	strcat(sql,")"); 
	sqllen++;

	//执行SQL ,原子操作
	unsigned __int32 ret = 0;
	//pthread_mutex_lock(&insertLock);
	if(!ExecSql(sql, sqllen/*strlen(sql)*/))
	{
		ret = mysql_insert_id(mysql);
		///! if not generate auto_id,maybe error [chib]
		/*
		if(ret == 0) 
			ret = 1;
			*/
	}
	//pthread_mutex_unlock(&insertLock);
	return ret;
}

int MysqlHandler::DeleteData(const char * tableName, const char * where)
{
	char sql[4096] = "";
	if(tableName == NULL || mysql == NULL)
	{
		//log_error("null pointer error. ---- %s",__FUNCTION__);
		return 0;
	}
	strcat(sql, "DELETE FROM ");
	strcat(sql, tableName);
	if(where)
	{
		strcat(sql, " WHERE ");
		strcat(sql, where);
	}
	ExecSql(sql, strlen(sql));
	unsigned long ret= (unsigned long)mysql_affected_rows(mysql);
	return ret;
}

int MysqlHandler::UpdateDataByCol(const char * tableName, const mysqlCol *column, const unsigned char *data, const char * where)
{
	char sql[1024*1024] = "";
	int len, sqllen = 0;
	const mysqlCol * temp;
	if(tableName == NULL || column == NULL || data == NULL || mysql == NULL)
	{
		//log_error("null pointer error. ---- %s",__FUNCTION__);
		return 0;
	}

	//生成SQL
	strcat(sql,"UPDATE "); 
	sqllen += 7/*strlen("UPDATE ")*/;
	strcat(sql, tableName); 
	sqllen += strlen(tableName);
	strcat(sql, " SET "); 
	sqllen += 5/*strlen(" SET ")*/;
	temp = column;
	bool first = true;
	int offset = 0;
	while(temp->name)
	{
		if(temp->dataSize == 0)
		{
			//log_error("invalid column data size.");
			return 0;
		}
		len = strlen(temp->name);
		if(len > 0)
		{
			if(first) 
			{
				first = false;
			}
			else 
			{
				strcat(sql,", "); 
				sqllen += 2/*strlen(", ")*/;
			}
			strcat(sql, temp->name); 
			sqllen += len;
			strcat(sql, "="); 
			sqllen++/*sqllen += strlen("=")*/;
			//set内容
			switch(temp->mysqlType)
			{
				case MYSQL_INT8:
					{
						sqllen += sprintf(&sql[sqllen],"%d",*(char *)(data+offset));
					}
					break;
				case MYSQL_INT16:
					{
						sqllen += sprintf(&sql[sqllen],"%d",*(__int16 *)(data+offset));
					}
					break;
				case MYSQL_INT32:
					{
						sqllen += sprintf(&sql[sqllen],"%d",*(__int32 *)(data+offset));
					}
					break;
				case MYSQL_INT64:
					{
						sqllen += sprintf(&sql[sqllen],"%ll",*(__int64 *)(data+offset));
					}
					break;
				case MYSQL_UINT8:
					{
						sqllen += sprintf(&sql[sqllen],"%u",*(unsigned char *)(data+offset));
					}
					break;
				case MYSQL_UINT16:
					{
						sqllen += sprintf(&sql[sqllen],"%u",*(unsigned __int16 *)(data+offset));
					}
					break;
				case MYSQL_UINT32:
					{
						sqllen += sprintf(&sql[sqllen],"%u",*(unsigned __int32 *)(data+offset));
					}
					break;
				case MYSQL_UINT64:
					{
						sqllen += sprintf(&sql[sqllen],"%llu",*(unsigned __int64 *)(data+offset));
					}
					break;
				case MYSQL_FLOAT:
					{
						sqllen += sprintf(&sql[sqllen], "%f", *(float *)(data+offset));
					}
					break;
				
				case MYSQL_STR:
					{
						strcat(sql,"\'"); 
						sqllen++/*sqllen += strlen("\'")*/;
						len=strlen((char *)(data+offset));
						if((unsigned __int32)len > temp->dataSize) 
							len = temp->dataSize;
						sqllen += mysql_real_escape_string(mysql, &sql[sqllen],(char *)(data+offset),len);
						strcat(sql,"\'"); 
						sqllen++/*sqllen += strlen("\'")*/;
					}
					break;
				case MYSQL_DATETIME:
					{
						strcat(sql,"\'");
						sqllen++/*sqllen += strlen("\'")*/;
						char* pszTime = DateTimetoStr(*(unsigned __int32 *)(data+offset));
						len=strlen(pszTime);
						sqllen += mysql_real_escape_string(mysql, &sql[sqllen], pszTime, len);
						strcat(sql,"\'");
						sqllen++/*sqllen += strlen("\'")*/;

					}
					break;

				case MYSQL_BIN:
					{
						strcat(sql,"\'"); 
						sqllen++/*sqllen += strlen("\'")*/;
						sqllen += mysql_real_escape_string(mysql, &sql[sqllen],(char *)(data+offset),temp->dataSize);
						strcat(sql,"\'"); 
						sqllen++/*sqllen += strlen("\'")*/;
					}
					break;
				/*case MYSQL_ZIP:
					{
						//计算压缩目的缓冲长度,must be at least 0.1% larger than sourceLen plus 12 bytes
						uLong destLen = temp->dataSize * 120 / 100 + 12;
						Bytef * destBuffer = new Bytef[destLen];
						strcat(sql,"\'"); 
						sqllen++;
						if (destBuffer) 
						{
							int retcode = compress(destBuffer, &destLen, (Bytef *)(data+offset),temp->dataSize);
							switch(retcode) 
							{
								case Z_OK:
									sqllen += mysql_real_escape_string(mysql,&sql[sqllen],(char *)destBuffer,destLen);
									break;
								case Z_MEM_ERROR:
									//log_fatal( "(%s, %d)Z_MEM_ERROR, NULL value instead.", __FUNCTION__, __LINE__);
									break;
								case Z_BUF_ERROR:
									//log_fatal( "(%s, %d)Z_BUF_ERROR, NULL value instead.", __FUNCTION__, __LINE__);
									break;
							}
							delete[] destBuffer;
						}
						else 
						{
							//log_fatal( "(%s, %d)Not enough memory, NULL value instead.", __FUNCTION__, __LINE__);
						}
						strcat(sql,"\'"); 
						sqllen++;
					}
					break;*/
				default:
					//log_error("invalid ares mysql type.");
					return FALSE;
			}
		}
		offset += temp->dataSize;
		temp++;
	}
	if(where != NULL)
	{
		strcat(sql," WHERE "); 
		sqllen += 7/*strlen(" WHERE ")*/;
		strcat(sql,where); 
		strcat(sql, ";");
		sqllen += strlen(where);
		sqllen += 1;
	}
	else 
	{
		strcat(sql, ";");
		sqllen += 1;
	}

	//pthread_mutex_lock(&affectedLock);
	ExecSql(sql, sqllen/*strlen(sql)*/);
	unsigned long ret = (unsigned long)mysql_affected_rows(mysql);
	//pthread_mutex_unlock(&affectedLock);
	return ret;
} 

bool MysqlHandler::SelectDataByColIT(const char * tableName, const mysqlCol * column, const char * where, const char * order)
{       
	char sql[4096] = "";

	if(tableName == NULL || column == NULL || mysql == NULL)
	{
		//log_error("null pointer error. ---- %s",__FUNCTION__);
		return false;
	}
	if(res != NULL)
	{
		//log_error("resultIT is not null.you must call selectDataByColNext until return FALSE!");
		return false;
	}
	//生成Sql，并执行
	//pthread_mutex_lock(&resultITLock);
	int retsize = FetchSelectSql(tableName, column, where, order, sql);
	if(retsize < 1)
	{               
		//pthread_mutex_unlock(&resultITLock);
		return false;
	}

	//得到结果集
	res = mysql_use_result(mysql);
	if(res == NULL)
	{
		//pthread_mutex_unlock(&resultITLock);
		//log_error(mysql_error(mysql));
		return false;
	}
	else
		return true;
}

bool MysqlHandler::SelectDataByColNext(const mysqlCol * column, unsigned char * data)
{
	//填充返回值
	if(column == NULL || data == NULL)
	{
		//log_error("null pointer error. ---- %s",__FUNCTION__);
		ResetResult();	
		return false;
	}
	if(res == NULL)
	{
		//log_error("You must successfully call selectUnionDataByColIT or selectDataByColIT before calling selectDataByColNext.");
		return false;
	}
	MYSQL_ROW row = mysql_fetch_row(res);
	if (row != NULL)
	{
		unsigned long * lengths = mysql_fetch_lengths(res);
		if(!FullSelectDataByRow(row, lengths, column, data))
		{
			ResetResult();	
			return false;
		}
		return true;
	}
	else
	{
		ResetResult();	
		return false;
	}
}