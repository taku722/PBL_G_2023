#pragma once

#include <string>
#include <mysql.h>
//#include <my_global.h>

class CMysql{
	MYSQL *connect;

	void ExitMysqlError(bool cnd,std::string,MYSQL *con);
public:
	CMysql(bool delFlag);
	~CMysql();

	void GetData(std::string &a, std::string &b, std::string x);
	void SetData(std::string x,std::string a,std::string b);
};
