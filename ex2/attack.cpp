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
#define Table_Num 100

// パラメータセッティング
INT64 q=65537;
INT64 r=65538;

// 事前計算用の配列
INT64 pre_X[Table_Num], pre_A[Table_Num], pre_B[Table_Num];



//データベースを検索し、無ければ挿入する関数
bool FindThenSetData(std::string x,std::string a,std::string b,CMysql *mysql){
	string ret_a, ret_b;
	mysql->GetData(ret_a,ret_b,x);

	if(ret_a.empty() || ret_b.empty()){
		mysql->SetData(x,a,b);	// 見つからなかったらデータを登録
		return false;		// 検索できず
	}else return true;		// すでに登録されているなら攻撃成功
}

//事前計算テーブル作成関数
int Precomputation(CMysql *con){
	INT64 X, A, B;
	int i=0;
	char str[MAX];
	string ret_a, ret_b, ret_x;

	srand((INT64)time(NULL));
	while(i<Table_Num){
		/*=====課題ex2_1 (start)===============*/
		//A,B,Xにランダムな数をセットし、配列pre_A[i],pre_B[i],pre_X[i]に格納しよう！　(配列pre_A,pre_B,pre_Xはグローバルで定義済み)
		//HINT A,BとXは法(mod)が異なる！
		A = (INT64)rand()%r;
		B = (INT64)rand()%r;
		X = (INT64)rand()%q;

		pre_A[i] = A;
		pre_B[i] = B;
		pre_X[i] = X;
		/*=====課題ex2_1 (finish)===============*/
		i++;
	}
	return 1;
}

//ランダムウォーク関数
int Random_Walk(CMysql *con){
	// _X, _A, _Bは一つ前のデータ
	INT64 X, A, B, _X, _A, _B;
	char str[MAX];
	string ret_a, ret_b, ret_x;
	int i;

	srand((INT64)time(NULL));

	/*=====課題ex2_2 (start)===============*/
	//(1)初期点用に_A,_B,_Xにランダムな数をセットしよう！
	_A = (INT64)rand()%r;
	_B = (INT64)rand()%r;
	_X = (INT64)rand()%q;
	//(2)生成した座標をmySQLに登録をしよう！HINT ex1を思い出そう
	sprintf(str,"%lld",_A); ret_a=str;
	sprintf(str,"%lld",_B); ret_b=str;
	sprintf(str,"%lld",_X); ret_x=str;
	con -> SetData(ret_x, ret_a, ret_b);
	//(3)衝突するまで_Aとpre_A[]から次のAを求めよう!　同様にB,Xを求めよう！
	while(1){
		A = (_A + pre_A[_X%Table_Num])%r;
		B = (_B + pre_B[_X%Table_Num])%r;
		X = (_X + pre_X[_X%Table_Num])%q;

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
		_A = A;
		_B = B;
		_X = X;
	}



	/*=====課題ex2_2 (finish)===============*/
	return 1;
}

int main(int argc,char *argv[]){
	CMysql mysql=new CMysql(1);
	Precomputation(&mysql);
	Random_Walk(&mysql);      //ランダムウォーク
}
