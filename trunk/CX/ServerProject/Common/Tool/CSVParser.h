/********************************************************************
created:	2014/12/01
author:		LL
desc:		csv�ļ����������ڼ���csv�ļ��������������������ݡ�
*********************************************************************/
#ifndef __CSVPARSER_H__
#define __CSVPARSER_H__

#include "GameDef.h"

#define CSV_LINE_MAX_LENGTH	4096	//csvÿ�е���󳤶�
/////////////////////////////////////////////////////////////////////////////////
// excel .csv parser.

class  CSVParser{
public:
	CSVParser();
	~CSVParser();
	bool LoadFile(const char* szFileName);	//��Ӳ�̼���csv�ļ������سɹ�����true��ʧ�ܷ���false��
	bool LoadFileFromMemory(const char* szFileData, int32 size);	//���ڴ����csv�ļ������سɹ�����true��ʧ�ܷ���false��

	bool GetLine(); //ȡ��һ�У�����ֵΪ�ڲ�������ڴ棬���ý������ֶ��ͷš�
	int32 GetLineNum(char* filename);		//ȡ������
	int32 GetColumnNum(char* filename);	//ȡ������

	bool IsEnd();	//��ǰ�е������Ƿ�ȡ��
    void NextColumn();  //��������,����һ��
	int32 GetInt();	//ȡintֵ
	int32 Get16Int();//ȡ16���Ƶ�intֵ
	char* GetStr();	//ȡ�ַ���ֵ
	void GetStr(char* szOut);	//ȡ�ַ���ֵ
	float GetFloat();	//ȡfloatֵ
	double GetDouble();	//ȡdoubleֵ
	bool GetBool();	//ȡboolֵ
private:	
	char* szFileBuff;	//�ļ�����,ÿ�μ����ļ����ܸı������ݡ���ַ�����ȡ���new��������Ҫ�ڽ�����ʱ��deleteһ�Ρ�
	char* szFileBuffPOffset;	//�ļ�����ƫ�ƣ�ȡֵ�ã�ÿ�μ��غ�ȡ��һ�л�ı���ָ�������
	char szLineBuff[CSV_LINE_MAX_LENGTH];	//�л��棬ÿ��ȡ���С�ȡ��ֵ����ı������ݡ�
	bool bIsUpdateLineBuff;		//�Ƿ�������л���,Ϊ��������ʹ��szLineBuffȡֵ��Ϊ����������ϴε�ȡֵ��
	char* szTokBuff;			//strtokȡ�������ַ���
	int32 LineBuffLength;			//�г��ȣ�ÿ��ȡһ��ʱ���£������ж��Ƿ�ȡ�ꡣ
};

#endif //__CSVPARSER_H__