#include <stdio.h>
#include "detect_by_mysql.h"

const std::string HOST="localhost";
const std::string USER="attacker";
const std::string PASS_WORD="0000";
const std::string DBNAME="RhoMethod";
const std::string TABLE_NAME="point_table";

CMysql::CMysql(bool delFlag){
	// MySQLとのコネクションを確立
	connect=mysql_init(NULL);
	ExitMysqlError(connect,"mysql_init",connect);
	ExitMysqlError(mysql_real_connect(connect,HOST.c_str(),USER.c_str(),PASS_WORD.c_str(),DBNAME.c_str(),0,NULL,0),"mysql_real_connect",connect);

	// tableの生成
	if(delFlag){
		std::string query ="DROP TABLE IF EXISTS "+TABLE_NAME;
		int ret=mysql_query(connect,query.c_str());
		ExitMysqlError(ret==0,query.c_str(),connect);
		query = "CREATE TABLE "+ TABLE_NAME + "(X char(20),A text,B text,DATE datetime,UNIQUE INDEX(X))";
		ret=mysql_query(connect,query.c_str());
		ExitMysqlError(ret==0,query,connect);
	}
}

CMysql::~CMysql(){
}

void CMysql::GetData(std::string &a,std::string &b,std::string x){
	std::string query="SELECT A, B FROM "+TABLE_NAME+" WHERE X = '"+x+"'";
	int t=mysql_query(connect,query.c_str());
	ExitMysqlError(t==0,query,connect);
	MYSQL_RES *result=mysql_store_result(connect);
	MYSQL_ROW row=mysql_fetch_row(result);
	if(row!=NULL){
		//xが見つかったら対応するabを返す
		a=std::string(row[0]);
		b=std::string(row[1]);
	}
	mysql_free_result(result);
}

void CMysql::SetData(std::string x,std::string a,std::string b){
	std::string query="INSERT INTO "+TABLE_NAME+"(X,A,B,DATE) values(\""+x+"\",\""+a+"\",\""+b+"\",now())";
	int ret=mysql_query(connect,query.c_str());
	ExitMysqlError(ret==0,query,connect);
}

void CMysql::ExitMysqlError(bool cnd,std::string err_message,MYSQL *con){
	if(!cnd){
		fprintf(stderr,"%s:%s\n",err_message.c_str(),mysql_error(con));
		if(con)
			mysql_close(con);
		exit(1);
	}
}
