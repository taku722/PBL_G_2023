#include"ecc.cpp"

// テーブルサイズ
#define Table_Num 100
#define Nthread 8

static pthread_t threads[Nthreads];
static pthread_mutex_t t_mutex;

// 事前計算用の配列
E_Point P,Q;
E_Point pre_R[Table_Num];
INT64 pre_A[Table_Num], pre_B[Table_Num];

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
	E_Point tmp, tmp_P, tmp_Q;
	INT64 A, B;
	int i=0;
	char str[MAX];
	string ret_a, ret_b, ret_x;
	srand((INT64)time(NULL));

	while(i<Table_Num){
		A = (INT64)rand() % Order;
		B = (INT64)rand() % Order;
		SCM(&tmp_P,A,P);
		SCM(&tmp_Q,B,Q);
		ECA(&tmp,tmp_P,tmp_Q);
		pre_A[i]=A;
		pre_B[i]=B;
		pre_R[i].x=tmp.x;
		i++;
	}
	return 1;
}

//ランダムウォーク関数
int Random_Walk(CMysql *con){
	E_Point R, _R, tmp, tmp_P, tmp_Q;
	INT64 A, B, _A, _B;// _X, _A, _Bは一つ前のデータ
	char str[MAX];
	string ret_a, ret_b, ret_x;
	string col_a, col_b;

	srand((INT64)time(NULL));

	_A = (INT64)rand() % Order;
	_B = (INT64)rand() % Order;
	SCM(&tmp_P,A,P);
	SCM(&tmp_Q,B,Q);
	ECA(&tmp,tmp_P,tmp_Q);
	_R.x = tmp.x;  _R.y=tmp.y; _R.flag=tmp.flag;
	sprintf(str,"%lld",_A); ret_a = str;
	sprintf(str,"%lld",_B); ret_b = str;
	sprintf(str,"%lld",_R.x); ret_x = str;
	con -> SetData(ret_x,ret_a,ret_b);
	while(1){
		A = (_A + pre_A[_R.x%Table_Num])%Order;
		B = (_B + pre_B[_R.x%Table_Num])%Order;
		ECA(&R,_R,pre_R[_R.x%Table_Num]);

		sprintf(str,"%lld",A); ret_a=str;
		sprintf(str,"%lld",B); ret_b=str;
		sprintf(str,"%lld",R.x); ret_x=str;
		pthread_mutex_lock(&t_mutex);
		if(FindThenSetData(ret_x,ret_a,ret_b,con)){
			printf("collision occur\n");
			printf("(X,A,B)=(%lld,%lld,%lld)\n",R.x,A,B);
			pthread_mutex_unlock(&t_mutex); 
			break;
		}else{
			printf("(X,A,B)=(%lld,%lld,%lld)\n",R.x,A,B);
			pthread_mutex_unlock(&t_mutex); 
		}
		
		_A = A;
		_B = B;
		_R.x = R.x; _R.y=R.y; _R.flag=R.flag;
	}
	/*=====課題ex6_2 (finish)===============*/
	return 1;
}

int main(int argc,char *argv[]){
	long long thread_num;
	int status;
	void *exit_status;

	pthread_mutex_init(&t_mutex, NULL);

	CMysql mysql=new CMysql(1);
	//Problem(); //問題作成用

	Prime=75223; Order=74929; Constant_B=7;
	P.x=4036; P.y=44304; P.flag=0;
	Q.x=5324; Q.y=7468; Q.flag=0;

	Precomputation(&mysql);
	for(int thread_num=0; thread_num<Nthreads; thread_num++){
		status=pthread_create(&threads[thread_num], NULL, (void*(*)(void*))Random_Walk, &mysql);
		(void)thread_join(threads[thread_num], &exit_status);
	}
}
