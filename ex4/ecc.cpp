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
#define TBNUM 1024				//テーブルのインデックス it should be a power of 2
#define THREADNUM 1				//スレッド数		確認用コマンド：　ls /proc/`pgrep [実行ファイル名]`/task
#define ARG_ERROR "\n***Error*** Invalid arguments to main()\n1 -> Problem\n2 -> Attack\n\n"
#define F_ERROR "\n***ERROR*** Cannot open the file\n"
#define FNAME "Parameter.txt"
#define INT64 long long int

//楕円曲線上の点
typedef struct {
	INT64 x,y;
	int flag;
} E_Point;

//ランダムウォークテーブル
struct RW {
	INT64 x, y, a, b;
};

//グローバル変数
INT64 Prime;       //素体Fpの素数p
INT64 Order;       //点位数
INT64 Constant_B;  //y^2=x^3+CONSTANT_B /Fp

RW table[TBNUM];
E_Point p, q;
int solve_flag=0;
unsigned long long StoreDataNum=0;
unsigned long long Random_WalkNum=0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

//String型からlong long int型に変換する関数　C++11のmystoll()の代用
INT64 mystoll(std::string str){
	const char* ch = str.c_str();
	return atoll(ch);
}

//有理点PとQをテキスト入力する関数
//読み込み失敗時は-1を返す
int Readf(){

	char str[MAX];

	std::ifstream ifs(FNAME);

	if(ifs.fail()){
      	std::cerr << F_ERROR << std::endl;
    	return -1;
  	}

	ifs.getline(str, MAX-1);	Prime = atoll(str);
	ifs.getline(str, MAX-1);	Order = atoll(str);
	ifs.getline(str, MAX-1);	Constant_B = atoll(str);
	ifs.getline(str, MAX-1);	p.x = atoll(str);
	ifs.getline(str, MAX-1);	p.y = atoll(str);
	ifs.getline(str, MAX-1);	q.x = atoll(str);
	ifs.getline(str, MAX-1);	q.y = atoll(str);

	return 1;
}

//有理点PとQをテキスト出力する関数（秘密スカラーdはテキスト出力しない）
//書き込み失敗時は-1を返す
int Writef(){
	fstream ofs;
	ofs.open(FNAME, ios::out);

	if(!ofs.is_open()) {
		std::cerr << F_ERROR << std::endl;
		return -1;
	}
	ofs << Prime << endl << Order << endl << Constant_B << endl << p.x << endl << p.y << endl << q.x << endl << q.y << endl;
	ofs.close();

	return 1;
}

//剰余関数（負数の剰余にも対応している)
INT64 Mod(INT64 a, INT64 b){
	return (a%b+b) % b;
}

//べき剰余関数（バイナリ法ver）
INT64 Bin_Power_Mod(INT64 x, INT64 n, INT64 p) {

	int i;
	int bin[100];
	INT64 mul = x;

	if(n == 0) return 1;

	//10進数から2進数に変換
	for(i = 0; n > 0; i++){
		bin[i] = n%2;
		n = n/2;
	}

	for(i -= 1; i > 0; i--){
		mul = (mul*mul) % p;
		if(bin[i-1] == 1) mul = (mul*x) % p;
	}

	return mul;
}

//逆元を返す関数^M
INT64 Invert(INT64 a, INT64 p){
	return Bin_Power_Mod(a, p-2, p);
}

//ルジャンドル記号を返す関数
int Legendre_Symbol(INT64 x, INT64 y){

	int L=1;
	INT64 tmp;

	if(Mod(x, y) == 0) return 0;
	while(1){
		x = Mod(x, y);
		if(x > y/2){
			x = y-x;
			if(Mod(y, 4) == 3) L = -L;
		}

		while(Mod(x, 4) == 0) x = x/4;

		if(Mod(x, 2) == 0){
			x = x/2;
			if(Mod(y, 8) == 8-3 || Mod(y, 8) == 3) L = -L;
		}

		if(x == 1) return L;

		if(Mod(x, 4) == 3 && Mod(y, 4) == 3)  L = -L;

		tmp = x;
		x = y;
		y = tmp;
	}
}

//平方剰余関数
INT64 Square_Root_Modulo(INT64 a, INT64 p){

  	INT64 x=0, y=0, q=p-1, b=0, t=0;
  	int e=0, n=0, r=0, m=0, i=0;

	//(n/p)=-1を満たすような任意のnを選択する（ここでは最小のものを採用）
	for(n=1; Legendre_Symbol(n,p) >= 0; n++){}

  	//p-1=q*2^e
	while(Mod(q, 2) == 0) {
    	q /= 2;
    	e++;
	}

 	x = Bin_Power_Mod(a, (q-1)/2, p);
 	y = Bin_Power_Mod(n, q, p);
	r = e;
 	b = Mod(a*Bin_Power_Mod(x, 2, p), p);
	x = Mod(a*x, p);

  	while (Mod(b, p) != 1) {
    	//Mod(b^(2^m),p)==1を満たす最小のmを見つける
    	for(m=1 ; ; m++){
			INT64 tmp_b = Bin_Power_Mod(b, 2, p);
			for(i=1; i<m; i++) tmp_b = Bin_Power_Mod(tmp_b, 2, p);
				if(Mod(tmp_b, p) == 1) break;
    	}
    	t = (r-m-1 == 0) ?  t = Mod(y,p) : Bin_Power_Mod(y, Bin_Power_Mod(2, r-m-1, p), p);
    	r = m;
    	y = Mod(t*t, p);
    	x = Mod(x*t, p);
    	b = Mod(b*y, p);
	}

	return x;
}

//楕円加算を行う関数（ECDも含む）
//結果が無限遠なら0を無限遠でなければ1を返す
int ECA(E_Point *r, E_Point a, E_Point b){

	INT64 k=0;

	//無限遠になるかどうかの判定
	if(a.flag == 1){
		if(b.flag == 1){
			//aとbが無限遠のとき
			r->flag = 1;
			return 0; //無限遠
		}

		else{
			//aが無限遠のとき
			r->x = b.x;
			r->y = b.y;
			r->flag = b.flag;
			return 1;
		}
	}

	else if(b.flag == 1){
		//bが無限遠のとき
		r->x = a.x;
		r->y = a.y;
		r->flag = a.flag;
		return 1;
	}

	if((a.x == b.x && Mod(a.y+b.y, Prime) == 0)) {
		r->flag = 1;
		return 0; //無限遠
	}

	//無限遠にならない場合
	//k=(3x^2)/2y	(ECD)
	if(a.x==b.x && a.y==b.y)	k = Mod( Mod(3*a.x*a.x, Prime) * Invert(Mod(2*a.y, Prime), Prime), Prime);
	//k=(a.y-b.y)/(a.x-b.x)	(ECA)
	else 	k = Mod((a.y-b.y) * Invert((a.x-b.x), Prime), Prime);

	r->x = Mod((k*k-a.x-b.x), Prime);		//k^2-a.x-b.x
	r->y = Mod((k*(a.x-r->x) - a.y), Prime);	//k(a.x-r.x)-a.y
	r->flag = 0;

	return 1;
}

//ベースポイントとなる有理点Pをランダムに取得する関数
void Random_P(E_Point *a){

	INT64 i, y_2;

	while(1){
		srand((unsigned long)time(NULL));
		a->x = (INT64)rand() % Prime;
		y_2 = Mod(Bin_Power_Mod(a->x, 3, Prime) + Constant_B , Prime);

		if(Legendre_Symbol(y_2, Prime) == 1){
			a->y = Square_Root_Modulo(y_2, Prime);
			a->flag = 0;
			return;
		}
	}
}

//スカラー倍算を行う関数
//結果が無限遠なら0を無限遠でなければ1を返す
int SCM(E_Point *r, INT64 s, E_Point a){
	int i;
    int bin[100];
    E_Point tmp = a;

    //sが0ならば無限遠点に
    if(s==0){
	      r->flag=1;
    }

  	//2進数から10進数へ変換
    for(i=0; s>0; i++){
    	bin[i] = s%2;
        s /= 2;
    }
    for(i-=1; i>0; i--){
    	//ECD
		if(!ECA(&tmp, tmp, tmp)) {
            r->flag = 1;
        }
        if(bin[i-1] == 1){
        	//ECA
            if(!ECA(&tmp, tmp, a)) {
                r->flag = 1;
            }
        }
    }
    r->x = tmp.x;
    r->y = tmp.y;
	r->flag = tmp.flag;
    return 1;
}

//有理点ＰとＱを生成する関数
void Problem(){

	int level;
	cout << "Whitch level?" << endl;
	cout << "[1] -> 16bit" << endl << "[2] -> 20bit" << endl << "[3] -> 26bit" << endl << "[4] -> 27bit" << endl << "[5] -> 28bit" << endl;
	cin >> level;
	switch (level) {
		case 1:{
			//16bit
			Prime=75223;
			Order=74929;
			Constant_B=7;
			break;
		}
		case 2:{
			//20bit
			Prime=1706311;
			Order=1704961;
			Constant_B=3;
			break;
		}
		case 3:{
			//26bit
			Prime=99286339;
			Order=99276253;
			Constant_B=3;
			break;
		}
		case 4:{
			//27bit
			Prime=258220873;
			Order=258204649;
			Constant_B=10;
			break;
		}
		case 5:{
			//28bit
			Prime=323505271;
			Order=323487121;
			Constant_B=3;
			break;
		}
		default:
			cout << "\n***Error*** Invalid number\n\n";
			exit(1);
			break;
	}

	INT64 d = Prime+1;

	Random_P(&p);	sleep(1);	//乱数のシード値が時刻なので1秒sleepしておく
	printf("Prime=%lld, Order=%lld, Constant_B=%lld\n", Prime, Order, Constant_B);
	printf("P(%lld,%lld)\n", p.x, p.y);

	do{
		d = (INT64)rand()% (d-1);
	}while(!SCM(&q,d,p));

	printf("Q(%lld,%lld)\n", q.x, q.y);
	printf("(Secret scalar d is %lld)\n", d);

	Writef();
}

//元の位数(r)を求める
int order(E_Point a){
    INT64 i=1;
	E_Point b;

	if(a.flag==1){
		return 1;
	}

	while(1){
		b.flag=0;
		SCM(&b,i,a);
		if(b.flag==1)return i;
		i++;
	}
	return 0;
}
