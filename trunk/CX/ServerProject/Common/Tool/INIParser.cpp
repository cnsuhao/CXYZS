#include "INIParser.h"
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>
#include <Windows.h>
#include <WinNls.h>

using namespace std;

const char*const MIDDLESTRING = "_____***_______";

struct analyzeini{
	string strsect;
	strMap *pmap;
	analyzeini(strMap & strmap);
	void operator()(const string &strini);
};

INIParser::INIParser(void)
{
}

INIParser::~INIParser(void)
{
}

bool INIParser::open(const char* pinipath)
{
	return do_open(pinipath);
}

string INIParser::read(const char*psect, const char*pkey)
{
	string mapkey = psect;
	mapkey += MIDDLESTRING;
	mapkey += pkey;
	strMapIt it = c_inimap.find(mapkey);
	if(it == c_inimap.end())
		return "";
	else
		return it->second;
}

wstring INIParser::readW(const char*psect, const char*pkey)
{
	string mapkey = psect;
	mapkey += MIDDLESTRING;
	mapkey += pkey;
	strMapIt it = c_inimap.find(mapkey);
	if(it == c_inimap.end())
		return L"";
	else
	{
		wchar_t buf[1024] = {0};
		int m=MultiByteToWideChar(CP_ACP, 0, it->second.c_str(), -1, 0, 0);
		m=MultiByteToWideChar(CP_ACP, 0, it->second.c_str(), -1, buf, m);
		std::wstring temp = buf;
		return temp;
	}
}

bool INIParser::do_open(const char* pinipath)
{
	ifstream fin(pinipath);
	if(!fin.is_open())
		return false;
	vector<string> strvect;
	while(!fin.eof())
	{
		string inbuf;
		getline(fin, inbuf,'\n');
		strvect.push_back(inbuf);
	}
	if(strvect.empty())
		return false;
	for_each(strvect.begin(), strvect.end(), analyzeini(c_inimap));
	fin.close();
	return !c_inimap.empty();
}

analyzeini::analyzeini(strMap & strmap):pmap(&strmap){
}

void analyzeini::operator()( const string & strini)
{
	size_t first =strini.find('[');
	size_t last = strini.rfind(']');
	if( first != string::npos && last != string::npos && first != last+1)
	{
		string strtmp = strini.substr(first+1,last-first-1);
		while(strtmp.begin() != strtmp.end() && *strtmp.begin() == ' ')
		{
			strtmp.erase(strtmp.begin());
		}

		while(strtmp.size() != 0 && strtmp[strtmp.size() - 1] == ' ')
		{
			strtmp.erase(strtmp.size() - 1);
		}
		strsect = strtmp;
		return ;
	}
	if(strsect.empty())
		return ;
	if((first=strini.find('='))== string::npos)
		return ;
	string strtmp1= strini.substr(0,first);
	string strtmp2=strini.substr(first+1, string::npos);
	first= strtmp1.find_first_not_of(" \t");
	last = strtmp1.find_last_not_of(" \t");
	if(first == string::npos || last == string::npos)
		return ;
	string strkey = strtmp1.substr(first, last-first+1);
	first = strtmp2.find_first_not_of(" \t");
	if(((last = strtmp2.find("\t#", first )) != string::npos) ||
		((last = strtmp2.find(" #", first )) != string::npos) ||
		((last = strtmp2.find("\t//", first )) != string::npos)||
		((last = strtmp2.find(" //", first )) != string::npos))
	{
		strtmp2 = strtmp2.substr(0, last-first);
	}
	last = strtmp2.find_last_not_of(" \t");
	if(first == string::npos || last == string::npos)
		return ;
	string value = strtmp2.substr(first, last-first+1);
	string mapkey = strsect + MIDDLESTRING;
	mapkey += strkey;
	(*pmap)[mapkey]=value;
	return ;
}
