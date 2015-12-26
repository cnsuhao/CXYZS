/********************************************************************
created:	2014/7/15
author:		LL
desc:		iniÅäÖÃÎÄ¼ş½âÎöÆ÷¡£
*********************************************************************/
#ifndef _INIParser_
#define _INIParser_
#include <string>
#include <map>

typedef std::map <std::string, std::string, std::less<std::string> > strMap;
typedef strMap::iterator strMapIt;

class INIParser
{
public:
	INIParser(void);
	~INIParser(void);
	//
	bool open(const char* pinipath);
	//
	std::string read(const char* psect, const char*pkey);
	std::wstring readW(const char* psect, const char*pkey);
protected:
	bool do_open(const char* pinipath);
	strMap c_inimap;
};
#endif //_INIParser_