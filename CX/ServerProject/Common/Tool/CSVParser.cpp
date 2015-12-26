#include "CSVParser.h"
#include <fstream>
#include <string.h>
#include <stdlib.h>
#ifdef GAME_CLIENT
    #include "ccMacros.h"
#else
    #include <cassert>
    #define CCASSERT(con, msg) assert(con)
#endif // GAME_CLIENT
using namespace std;

const char* csvDefaultStr = "NULL";
const int32 csvDefualtStrLenght = strlen(csvDefaultStr);


CSVParser::CSVParser():szFileBuff(NULL),bIsUpdateLineBuff(false)
{
}

CSVParser::~CSVParser()
{
	SAFE_DELETE(szFileBuff);
}

bool CSVParser::LoadFile(const char* szFileName)
{	

	//std::string filedata = FileUtils::getInstance()->getStringFromFile(szFileName);
	//
	//if (filedata.size() > 0)
	//{
	//	ssize_t FileLength = filedata.size();	//加载的文件长度
	//	if (szFileBuff == NULL)
	//	{
	//		szFileBuff = new char[FileLength + 1];
	//		memset(szFileBuff, '\0', FileLength + 1);
	//	}
	//	else 
	//	{
	//		if (strlen(szFileBuff) <= FileLength)
	//		{
	//			SAFE_DELETE(szFileBuff);
	//			szFileBuff = new char[FileLength + 1];				
	//		}
	//		memset(szFileBuff, '\0', FileLength + 1);
	//	}
	//	memcpy(szFileBuff, filedata.c_str(), FileLength);
	//	szFileBuff[FileLength] = '\0';
	//	szFileBuffPOffset = szFileBuff;
	//	return true;
	//}
    SAFE_DELETE(szFileBuff);

	unsigned long FileLength = 0;	//加载的文件长度	
	ifstream file(szFileName);
	if(!file)
	{
		return false;
	}

	file.seekg(0,ios_base::end);
	FileLength = file.tellg();
	if (FileLength == 0)
	{
		return false;
	}
	file.seekg(0,ios_base::beg);
	if (FileLength > 0)
	{
		if (szFileBuff == NULL)
		{
			szFileBuff = new char[FileLength + 1];
			memset(szFileBuff, '\0', FileLength + 1);
		}
		else 
		{
			if (strlen(szFileBuff) <= FileLength)
			{
				SAFE_DELETE(szFileBuff);
				szFileBuff = new char[FileLength + 1];				
			}
			memset(szFileBuff, '\0', FileLength + 1);
		}

		file.read(szFileBuff, FileLength);
		szFileBuff[FileLength] = '\0';
		szFileBuffPOffset = szFileBuff;	//重置偏移
		file.close();
		return true;
	}
	file.close();
	return false;
}

bool CSVParser::LoadFileFromMemory(const char *szFileData, int32 size)
{
	if (szFileData && size > 0)
	{
		if (szFileBuff == NULL)
		{
			szFileBuff = new char[size + 1];
			memset(szFileBuff, '\0', size + 1);
		}
		else 
		{
			if ((int32)strlen(szFileBuff) <= size)
			{
				SAFE_DELETE(szFileBuff);
				szFileBuff = new char[size + 1];
			}
			memset(szFileBuff, '\0', size + 1);	
		}
		memcpy(szFileBuff, szFileData, size);
		szFileBuffPOffset = szFileBuff;	//重置偏移		
		return true;
	}
	return false;
}

bool CSVParser::GetLine()
{
	//是否到结尾
	if (*szFileBuffPOffset == '\0')
	{		
		return false;
	}

	// 忽略上次的\r\n 
	while( *szFileBuffPOffset == '\r' || *szFileBuffPOffset == '\n' )
	{
		szFileBuffPOffset++;
	}
	if (*szFileBuffPOffset == '\0')
	{		
		return false;
	}
	char* pStart = szFileBuffPOffset;
	while( *szFileBuffPOffset && *szFileBuffPOffset != '\r' && *szFileBuffPOffset != '\n' )
	{
		szFileBuffPOffset++;
	}

	char* pEnd = szFileBuffPOffset;
	if (*pEnd == '\0')
	{		
		return false;	
	}
	int32 CurrentLength = pEnd - pStart;
	if (CurrentLength >= CSV_LINE_MAX_LENGTH)
	{		
		return false;
		//超出定义长度，要报错！！！
	}
	int32 Num = 0;
	int32 Modify = 0; // 修正偏移 
	for (int32 i = 0; i < CurrentLength; i++)
	{
		if (*(pStart + i) == '\\' && i+1 < CurrentLength)
		{
			if (*(pStart + i + 1) == 'n')
			{
				szLineBuff[i - Num + Modify] = 10;
				Num++;
				i++;
				continue;
			}
		}

		szLineBuff[i - Num + Modify] = *(pStart + i);

		//判断
		if (*(pStart + i) == ',' &&  i+1 < CurrentLength)
		{
			if (*(pStart + i + 1) == ',')
			{
				int32 offset = 0;
				for (int32 c = 0; c < csvDefualtStrLenght; c++)
				{
					++Modify;
					offset =  i - Num + Modify;
					if (offset < CSV_LINE_MAX_LENGTH)
					{
						szLineBuff[offset] = csvDefaultStr[c];
					}
					else
					{
						CCASSERT(0,"空值插入NULL时，超出缓冲区");
					}
				}
			}
		}
		else
		{
			//如果最后一个也为空
			if (*(pStart + i) == ',' &&  i+1 == CurrentLength)
			{
				int32 offset = 0;
				for (int32 c = 0; c < csvDefualtStrLenght; c++)
				{
					++Modify;
					offset =  i - Num + Modify;
					if (offset < CSV_LINE_MAX_LENGTH)
					{
						szLineBuff[offset] = csvDefaultStr[c];
					}
					else
					{
						CCASSERT(0,"空值插入NULL时，超出缓冲区");
					}
				}
			}
		}
	}
	CurrentLength = CurrentLength - Num + Modify;
	szLineBuff[CurrentLength] = 0;	
	LineBuffLength = strlen(szLineBuff);
	bIsUpdateLineBuff = true;
	return true;
}

int32 CSVParser::GetLineNum(char* filename)
{
	int32 num = 0;
	while(*filename != '\0')
	{
		if (*filename == '\n')
		{
			num++;
		}
		filename++;
	}
	return num;
}

int32 CSVParser::GetColumnNum(char* filename)
{
	int32 num = 0;
	while(*filename != '\0'){
		if (*filename == '\n'){
			return num+1;
		}
		if (*filename == ','){
			num++;
		}
		filename++;
	}
	return num+1;
}

bool CSVParser::IsEnd()
{	
	int32 num = &*szTokBuff + strlen(szTokBuff) - &*szLineBuff;
	if (LineBuffLength == num)
	{
		return true;
	}
	//忽略行末的,号
	num++;
	char* temp = szLineBuff + num;
	while (*temp == ',')
	{
		temp++;
		num++;
	}
	if (LineBuffLength <= num)
	{
		return true;
	}
	return false;
}

void CSVParser::NextColumn()
{
    if (bIsUpdateLineBuff)
    {
        szTokBuff = strtok(szLineBuff, ",");
        bIsUpdateLineBuff = false;
    }
    else
    {
        szTokBuff = strtok(NULL, ",");
    }
}

int32 CSVParser::GetInt()
{		
	if (bIsUpdateLineBuff)
	{
		szTokBuff = strtok(szLineBuff, ",");
		bIsUpdateLineBuff = false;
	}
	else
	{
		szTokBuff = strtok(NULL, ",");
	}

	if (szTokBuff == NULL || 0 == strcmp(szTokBuff,  csvDefaultStr))
	{
		return 0;
	}

	return atoi(szTokBuff);	
}

int32 CSVParser::Get16Int()
{
	if (bIsUpdateLineBuff)
	{
		szTokBuff = strtok(szLineBuff, ",");
		bIsUpdateLineBuff = false;
	}
	else
	{
		szTokBuff = strtok(NULL, ",");
	}	

	if (szTokBuff == NULL || 0 == strcmp(szTokBuff,  csvDefaultStr))
	{
		return 0;
	}

	return strtoul(szTokBuff,0,16);
	/*int32 d=0;
	while(*szTokBuff)
	{
	if(*szTokBuff>='0' && *szTokBuff<='9') d=d*16+(*szTokBuff-'0');
	else if(*szTokBuff>='a' && *szTokBuff<='f')d=d*16+*szTokBuff-'a'+ 10;
	else if(*szTokBuff>='A' && *szTokBuff<='Z')d=d*16+*szTokBuff-'A'+ 10;
	else return 0;
	szTokBuff++;
	}
	return d;*/
}

char* CSVParser::GetStr()
{
	if (bIsUpdateLineBuff)
	{
		szTokBuff = strtok(szLineBuff, ",");
		bIsUpdateLineBuff = false;
	}
	else
	{
		szTokBuff = strtok(NULL, ",");
	}

	return szTokBuff;
}

void CSVParser::GetStr(char* szOut)
{	
	if (bIsUpdateLineBuff)
	{
		szTokBuff = strtok(szLineBuff, ",");
		bIsUpdateLineBuff = false;
	}
	else
	{
		szTokBuff = strtok(NULL, ",");
	}

	strcpy(szOut, szTokBuff);
}

float CSVParser::GetFloat()
{	
	if (bIsUpdateLineBuff)
	{
		szTokBuff = strtok(szLineBuff, ",");
		bIsUpdateLineBuff = false;
	}
	else
	{
		szTokBuff = strtok(NULL, ",");
	}

	if (szTokBuff == NULL || 0 == strcmp(szTokBuff,  csvDefaultStr))
	{
		return 0.0f;
	}

	return (float)atof(szTokBuff);	
}

double CSVParser::GetDouble()
{	
	if (bIsUpdateLineBuff)
	{
		szTokBuff = strtok(szLineBuff, ",");
		bIsUpdateLineBuff = false;
	}
	else
	{
		szTokBuff = strtok(NULL, ",");
	}

	if (szTokBuff == NULL || 0 == strcmp(szTokBuff,  csvDefaultStr))
	{
		return 0.0;
	}

	return atof(szTokBuff);	
}

bool CSVParser::GetBool()
{
	if (bIsUpdateLineBuff)
	{
		szTokBuff = strtok(szLineBuff, ",");
		bIsUpdateLineBuff = false;
	}
	else
	{
		szTokBuff = strtok(NULL, ",");
	}

	if (szTokBuff == NULL || 0 == strcmp(szTokBuff,  csvDefaultStr))
	{
		return false;
	}

	if (atoi(szTokBuff) <= 0)
	{
		return false;
	}
	return true;	
}
