#include"ecc.cpp"

int main(void){
	INT64 a[2],b[2];
	INT64 dA,dB,ans,inv_dB;

	Order=74929;
	a[0]=15136; b[0]=54884; //a[0]P+b[0]Q
	a[1]=57541; b[1]=28636; //a[1]P+b[0]Q

	dA=(Order+a[0]-a[1]) % Order;
	dB=(Order+b[1]-b[0]) % Order;

	inv_dB=Invert(dB,Order);
	ans=(dA*inv_dB) % Order;
	printf("%lld\n",ans);
}
