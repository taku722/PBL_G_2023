#include "ecc.cpp"

int main(void){
	Prime=65537;
	Order=65538;
	Constant_B=7;

	E_Point tmp,ans;
	tmp.x=1;
	tmp.y=2;
	tmp.flag=0;

	ans.flag=0;
	//ECA(&ans,tmp,tmp);
	SCM(&ans,5,tmp);
	printf("ans=%lld,%lld,%d\n",ans.x,ans.y,ans.flag);


	//Problem();
}


