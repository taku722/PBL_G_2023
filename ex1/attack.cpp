// Linux network関係
#include <sys/types.h>
#include <errno.h>

#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <map>
#include "detect_by_mysql.h"

using namespace std;

#define MAX 256
#define INT64 long long int

INT64 q=65537;
INT64 r=65538;

//データベースを検索し、無ければ挿入する関数
bool FindThenSetData(std::string x,std::string a,std::string b,CMysql *mysql){
	string ret_a, ret_b;
	mysql->GetData(ret_a,ret_b,x);

	if(ret_a.empty() || ret_b.empty()){
		mysql->SetData(x,a,b);	// 見つからなかったらデータを登録
		return false;		// 検索できず
	}else return true;		// すでに登録されているなら攻撃成功
}

//ランダムウォーク関数
int Random_Walk(CMysql *con){

	INT64 X, A, B;
	int i=0;
	char str[MAX];
	string ret_a, ret_b, ret_x;

	srand((INT64)time(NULL));
	while(1){
		A = (INT64)rand() % r;
		B = (INT64)rand() % r;
		X = (INT64)rand() % r;

		sprintf(str,"%lld",A); ret_a=str;
		sprintf(str,"%lld",B); ret_b=str;
		sprintf(str,"%lld",X); ret_x=str;

		if(FindThenSetData(ret_x,ret_a,ret_b,con)){
			printf("collision occur\n");
			printf("(X,A,B)=(%lld,%lld,%lld)\n",X,A,B);
			break;
		}else{
			printf("(X,A,B)=(%lld,%lld,%lld)\n",X,A,B);
		}
	}
	return 1;
}

int main(int argc,char *argv[]){
	CMysql mysql=new CMysql(1);
	Random_Walk(&mysql);      //ランダムウォーク
}
