#include "ecc.cpp"
E_Point P,Q; // 問題設定用

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
	INT64 A, B;
	E_Point tmp,tmp_P, tmp_Q;
	char str[MAX];
	string ret_a, ret_b, ret_x;
	srand((INT64)time(NULL));

	/*=====課題ex5 (start)===============*/
	while(1){
		//(1)AとBにランダムな数を代入しよう！
		A = (INT64)rand() % Order;
		B = (INT64)rand() % Order;
		//(2)[A]P+[B]Qを求めよう！
		SCM(&tmp_P,A,P);
		SCM(&tmp_Q,B,Q);
		ECA(&tmp,tmp_P,tmp_Q);
		//(3)衝突判定（登録されていなければ、登録）
		//(1)~(3)を衝突するまで繰り返そう
		sprintf(str,"%lld",A); ret_a = str;
		sprintf(str,"%lld",B); ret_b = str;
		sprintf(str,"%lld",tmp.x); ret_x = str;
		if(FindThenSetData(ret_x,ret_a,ret_b,con)){
			printf("collision occur\n");
			printf("(X,A,B)=(%lld,%lld,%lld)\n",X,A,B);
			break;
		}else{
			printf("(X,A,B)=(%lld,%lld,%lld)\n",X,A,B);
		}
	}
	/*=====課題ex5 (finish)===============*/
	return 1;
}

int main(int argc,char *argv[]){
	Problem();

	CMysql mysql=new CMysql(1);

	//問題を代入(変更してみよう！)
	Prime=75223; Order=74929; Constant_B=7;
	P.x=20218; P.y=23166; P.flag=0;
	Q.x=69238; Q.y=16927; Q.flag=0;

	Random_Walk(&mysql);      //ランダムウォーク
}

/*==example===========================
  Prime=75223, Order=74929, Constant_B=7
  P(20218,23166)
  Q(69238,16927)
  (Secret scalar d is 64160)
*/
