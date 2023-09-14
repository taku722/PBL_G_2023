#include"ecc.cpp"
E_Point P,Q;

int main(void){
	INT64 a[2],b[2];
	INT64 dA,dB,ans,inv_dB;

	Order=74929;
	a[0]=49706; b[0]=60644;
	a[1]=16194; b[1]=22935;

	dA=(Order+a[0]-a[1]) % Order;
	dB=(Order+b[1]-b[0]) % Order;

	inv_dB=Invert(dB,Order);
	ans=(dA*inv_dB) % Order;
	printf("%lld\n",ans);
}
