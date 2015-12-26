/********************************************************************
created:	2014/12/01
author:		LL
desc:		csv文件解析：用于加载csv文件并解析出各种类型数据。
*********************************************************************/
#ifndef __CSVPARSER_H__
#define __CSVPARSER_H__

#include "GameDef.h"

#define CSV_LINE_MAX_LENGTH	4096	//csv每行的最大长度
/////////////////////////////////////////////////////////////////////////////////
// excel .csv parser.

class  CSVParser{
public:
	CSVParser();
	~CSVParser();
	bool LoadFile(const char* szFileName);	//从硬盘加载csv文件，加载成功返回true，失败返回false。
	bool LoadFileFromMemory(const char* szFileData, int32 size);	//从内存加载csv文件，加载成功返回true，失败返回false。

	bool GetLine(); //取得一行，返回值为内部分配的内存，调用结束需手动释放。
	int32 GetLineNum(char* filename);		//取得行数
	int32 GetColumnNum(char* filename);	//取的列数

	bool IsEnd();	//当前行的数据是否取完
    void NextColumn();  //跳过本列,到下一列
	int32 GetInt();	//取int值
	int32 Get16Int();//取16进制的int值
	char* GetStr();	//取字符串值
	void GetStr(char* szOut);	//取字符串值
	float GetFloat();	//取float值
	double GetDouble();	//取double值
	bool GetBool();	//取bool值
private:	
	char* szFileBuff;	//文件缓存,每次加载文件可能改变其内容、地址及长度。有new操作，需要在结束的时候delete一次。
	char* szFileBuffPOffset;	//文件缓存偏移，取值用，每次加载和取得一行会改变其指向的内容
	char szLineBuff[CSV_LINE_MAX_LENGTH];	//行缓存，每次取得行、取得值都会改变其内容。
	bool bIsUpdateLineBuff;		//是否更新了行缓存,为真则重新使用szLineBuff取值，为假则继续从上次的取值。
	char* szTokBuff;			//strtok取出来的字符串
	int32 LineBuffLength;			//行长度，每次取一行时更新，用于判断是否取完。
};

#endif //__CSVPARSER_H__