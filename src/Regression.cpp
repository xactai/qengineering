//---------------------------------------------------------------------------
// Q-engineering 1997 - 2023
//---------------------------------------------------------------------------
#include "Regression.h"
//---------------------------------------------------------------------------
// TAverMean
//---------------------------------------------------------------------------
TAverMean::TAverMean(void)
{
	Reset();
}
//---------------------------------------------------------------------------
void TAverMean::Reset(void)
{
	Cnt=0; Min=DBL_MAX; Max=-DBL_MAX; Mean=Mean2=Variance=StdDev=0.0;
}
//---------------------------------------------------------------------------
void TAverMean::Add(const double X)
{
	double Delta=X-Mean;

	if(X<Min) Min=X;
	if(X>Max) Max=X;

	Mean  += Delta/++Cnt;
	Mean2 += Delta*(X-Mean);

	Variance=Mean2/Cnt;
	StdDev=sqrt(Variance);
}
//---------------------------------------------------------------------------
// GaussianElimination
//---------------------------------------------------------------------------
bool GaussianElimination(double **M,double *V,double *X,int Dim)
{
   int p,q,r;
   int i,j,k;
   double t,m;
   bool Singularity=false;
   //elimination
   for(i=0;i<Dim;i++){
		for(p=i,r=i+1;r<Dim;r++) if(fabs(M[r][i])>fabs(M[p][i])) p=r;
      if(M[p][i]==0.0){ Singularity=true; break; }
      else{
         if(i!=p){   //swap
            for(q=i;q<Dim;q++){t=M[i][q]; M[i][q]=M[p][q]; M[p][q]=t;}
            t=V[i]; V[i]=V[p]; V[p]=t;
         }
         for(k=i+1;k<Dim;k++){
            m=M[k][i]/M[i][i];
            for(j=i+1;j<Dim;j++) M[k][j]-=m*M[i][j];
            V[k]-=m*V[i];
            M[k][i]=0.0;
         }
      }
   }
   //backsubstitution
   if(!Singularity){
      for(i=Dim-1;i>=0;i--){
         t=V[i];
         for(j=i+1;j<Dim;j++) t-=M[i][j]*X[j];
         X[i]=t/M[i][i];
      }
   }
   return !Singularity;
}
//---------------------------------------------------------------------------
// TLinRegression
//---------------------------------------------------------------------------
TLinRegression::TLinRegression(void)
{
   Reset();
}
//---------------------------------------------------------------------------
void TLinRegression::Reset(void)
{
   sum=xsum=x2sum=ysum=y2sum=xysum=0.0;
}
//---------------------------------------------------------------------------
void TLinRegression::operator =(const TLinRegression &a)
{
	sum=a.sum;
   xsum=a.xsum;
   ysum=a.ysum;
   x2sum=a.x2sum;
   y2sum=a.y2sum;
   xysum=a.xysum;
}
//---------------------------------------------------------------------------
void TLinRegression::Add(const DPoint Pnt)
{
	sum   += 1.0;
	xsum  += Pnt.X;
	x2sum += Pnt.X*Pnt.X;
	ysum  += Pnt.Y;
	y2sum += Pnt.Y*Pnt.Y;
	xysum += Pnt.X*Pnt.Y;
}
//---------------------------------------------------------------------------
void TLinRegression::Sub(const DPoint Pnt)
{
	sum   -= 1.0;
	xsum  -= Pnt.X;
	x2sum -= Pnt.X*Pnt.X;
	ysum  -= Pnt.Y;
	y2sum -= Pnt.Y*Pnt.Y;
	xysum -= Pnt.X*Pnt.Y;
}
//---------------------------------------------------------------------------
void TLinRegression::Add(const double X,const double Y,const double Weight)
{
	sum   += Weight;
	xsum  += Weight*X;
	x2sum += Weight*X*X;
	ysum  += Weight*Y;
	y2sum += Weight*Y*Y;
	xysum += Weight*X*Y;
}
//---------------------------------------------------------------------------
bool TLinRegression::Execute(TLine &Le)
{
	return Execute(Le.A,Le.B);
}
//---------------------------------------------------------------------------
bool TLinRegression::Execute(double &A,double &B)
{
   bool Succes=false;
   double m1,m2;

   if(sum>0.0){
      m1=(sum*x2sum-xsum*xsum);
      m2=(sum*xysum-xsum*ysum);
      if(m1!=0.0){
         A=m2/m1;
         B=(ysum-A*xsum)/sum;
         Succes=true;
      }
   }

   return Succes;
}
//---------------------------------------------------------------------------
bool TLinRegression::Execute(double &A,double &B,double &R)
//R = sample correlation (0 worst .. 1 perfect fit)
//be aware R not R² is calculated !
{
   bool Succes=false;
   double m1,m2,m3;

   if(sum>0.0){
      m1=(sum*x2sum-xsum*xsum);
      m2=(sum*xysum-xsum*ysum);
      m3=(sum*y2sum-ysum*ysum);
      if(m1>0.0 && m3>0.0){
         A=m2/m1;
         B=(ysum-A*xsum)/sum;
         R=fabs(m2/(sqrt(m1)*sqrt(m3)));
         Succes=true;
      }
   }

   return Succes;
}
//---------------------------------------------------------------------------
bool TLinRegression::Execute(TLine &Le,const DPoint &Anchor)
{
   bool Succes=false;
   double dx,dy;

   if(sum>0.0){
      dx=xsum-sum*Anchor.X;
      dy=ysum-sum*Anchor.Y;
      if(dx!=0.0){
         Le.A=dy/dx;
         Le.B=Anchor.Y-Le.A*Anchor.X;
         Succes=true;
      }
   }

   return Succes;
}
//---------------------------------------------------------------------------
// TSquareRegression
//---------------------------------------------------------------------------
TSquareRegression::TSquareRegression(void)
{
   Reset();
}
//---------------------------------------------------------------------------
void TSquareRegression::Reset(void)
{
   sum=xsum=x2sum=x3sum=x4sum=xysum=x2ysum=ysum=0.0;
}
//---------------------------------------------------------------------------
void TSquareRegression::operator =(const TSquareRegression &a)
{
	sum=a.sum;
   xsum=a.xsum;
   ysum=a.ysum;
   x2sum=a.x2sum;
   x3sum=a.x3sum;
   x4sum=a.x4sum;
   xysum=a.xysum;
   x2ysum=a.x2ysum;
}
//---------------------------------------------------------------------------
void TSquareRegression::Sub(const DPoint Pnt)
{
   double x=Pnt.X;

	sum   -= 1.0;
	ysum  -= Pnt.Y;
	xsum  -= x;
	xysum -= x*Pnt.Y; x*=Pnt.X;
	x2sum -= x;
	x2ysum-= x*Pnt.Y; x*=Pnt.X;
	x3sum -= x;       x*=Pnt.X;
	x4sum -= x;
}
//---------------------------------------------------------------------------
void TSquareRegression::Add(const DPoint Pnt)
{
   double x=Pnt.X;

	sum   += 1.0;
	ysum  += Pnt.Y;
	xsum  += x;
	xysum += x*Pnt.Y; x*=Pnt.X;
	x2sum += x;
	x2ysum+= x*Pnt.Y; x*=Pnt.X;
	x3sum += x;       x*=Pnt.X;
	x4sum += x;
}
//---------------------------------------------------------------------------
void TSquareRegression::Add(const double X,const double Y,const double Weight)
{
   double x=X;

	sum   += Weight;
	ysum  += Weight*Y;
	xsum  += Weight*x;
	xysum += Weight*x*Y; x*=X;
	x2sum += Weight*x;
	x2ysum+= Weight*x*Y; x*=X;
	x3sum += Weight*x;   x*=X;
	x4sum += Weight*x;
}
//---------------------------------------------------------------------------
bool TSquareRegression::Execute(TQuadratic &Qr)
{
	return Execute(Qr.A,Qr.B,Qr.C);
}
//---------------------------------------------------------------------------
bool TSquareRegression::Execute(double &A,double &B,double &C)
{
   bool Succes=false;
   double m1,m2,m3,m4,m5,m6,m7;

   if(x2sum!=0.0){
      m1=(sum*x3sum-x2sum*xsum);
      m2=(xsum*x3sum-x2sum*x2sum);
      m3=(ysum*x3sum-xysum*x2sum);
      m4=(xsum*x4sum-x2sum*x3sum);
      m5=(x2sum*x4sum-x3sum*x3sum);
      m6=(xysum*x4sum-x2ysum*x3sum);
      m7=(m1*m5-m2*m4);

      if(m7!=0.0 && m5!=0.0){
         C=(m3*m5-m6*m2)/m7;
         B=(m6-C*m4)/m5;
         A=(ysum-B*xsum-C*sum)/x2sum;
         Succes=true;
      }
   }

   return Succes;
}
//---------------------------------------------------------------------------
bool TSquareRegression::ExecuteTop(double Xtop,double &A,double &B,double &C)
{
   bool Succes=false;
   double m1,m2,m3,m4;

   if(x2sum!=0.0){
      m1=(xsum*x2sum-x3sum*sum);
      m2=(x2sum*x2sum-x4sum*sum);
      m3=(ysum*x2sum-x2ysum*sum);
      m4=(2*Xtop*m1-m2);
      if(m4!=0.0){
         A=-m3/m4;
         B=(m3-A*m2)/m1;
         C=(ysum-A*x2sum-B*xsum)/sum;
         Succes=true;
      }
   }

   return Succes;
}
//---------------------------------------------------------------------------
// TSquareZeroRegression
//---------------------------------------------------------------------------
TSquareZeroRegression::TSquareZeroRegression(void)
{
   Reset();
}
//---------------------------------------------------------------------------
void TSquareZeroRegression::Reset(void)
{
   xsum=x2sum=x3sum=x4sum=xysum=x2ysum=ysum=0.0;
}
//---------------------------------------------------------------------------
void TSquareZeroRegression::Add(const double X,const double Y,const double Weight)
{
   double x=X;

	ysum  += Weight*Y;
	xsum  += Weight*x;
	xysum += Weight*x*Y; x*=X;
	x2sum += Weight*x;
	x2ysum+= Weight*x*Y; x*=X;
	x3sum += Weight*x;   x*=X;
   x4sum += Weight*x;
}
//---------------------------------------------------------------------------
bool TSquareZeroRegression::Execute(double &A,double &B)
{
   bool Succes=false;
   double m5,m6;

   if(x2sum!=0.0){
      m5=(x2sum*x4sum-x3sum*x3sum);
      m6=(xysum*x4sum-x2ysum*x3sum);
      if(m5!=0.0){
         B=m6/m5;
         A=(ysum-B*xsum)/x2sum;
         Succes=true;
      }
   }

   return Succes;
}
//---------------------------------------------------------------------------
// TCubicRegression
//---------------------------------------------------------------------------
TCubicRegression::TCubicRegression(void)
{
   Reset();
}
//---------------------------------------------------------------------------
void TCubicRegression::Reset(void)
{
   sum=xsum=x2sum=x3sum=x4sum=x5sum=x6sum=xysum=x2ysum=x3ysum=ysum=0.0;
}
//---------------------------------------------------------------------------
void TCubicRegression::operator =(const TCubicRegression &a)
{
	sum=a.sum;
   xsum=a.xsum;
   ysum=a.ysum;
   x2sum=a.x2sum;
   x3sum=a.x3sum;
   x4sum=a.x4sum;
   x5sum=a.x5sum;
   x6sum=a.x6sum;
   xysum=a.xysum;
   x2ysum=a.x2ysum;
   x3ysum=a.x3ysum;
}
//---------------------------------------------------------------------------
void TCubicRegression::Add(const DPoint Pnt)
{
   double x=Pnt.X;

	sum   += 1.0;
	ysum  += Pnt.Y;
	xsum  += x;
	xysum += x*Pnt.Y; x*=Pnt.X;
	x2sum += x;
	x2ysum+= x*Pnt.Y; x*=Pnt.X;
	x3sum += x;
	x3ysum+= x*Pnt.Y; x*=Pnt.X;
	x4sum += x;       x*=Pnt.X;
	x5sum += x;       x*=Pnt.X;
	x6sum += x;
}
//---------------------------------------------------------------------------
void TCubicRegression::Sub(const DPoint Pnt)
{
   double x=Pnt.X;

	sum   -= 1.0;
	ysum  -= Pnt.Y;
	xsum  -= x;
	xysum -= x*Pnt.Y; x*=Pnt.X;
	x2sum -= x;
	x2ysum-= x*Pnt.Y; x*=Pnt.X;
	x3sum -= x;
	x3ysum-= x*Pnt.Y; x*=Pnt.X;
	x4sum -= x;       x*=Pnt.X;
	x5sum -= x;       x*=Pnt.X;
	x6sum -= x;
}
//---------------------------------------------------------------------------
void TCubicRegression::Add(const double X,const double Y,const double Weight)
{
   double x=X;

	sum   += Weight;
	ysum  += Weight*Y;
	xsum  += Weight*x;
	xysum += Weight*x*Y; x*=X;
	x2sum += Weight*x;
	x2ysum+= Weight*x*Y; x*=X;
	x3sum += Weight*x;
	x3ysum+= Weight*x*Y; x*=X;
	x4sum += Weight*x;   x*=X;
	x5sum += Weight*x;   x*=X;
	x6sum += Weight*x;
}
//---------------------------------------------------------------------------
bool TCubicRegression::Execute(TCubic &Cb)
{
	return Execute(Cb.A,Cb.B,Cb.C,Cb.D);
}
//---------------------------------------------------------------------------
bool TCubicRegression::Execute(double &A,double &B,double &C,double &D)
{
   bool Succes=false;
   double m1,m2,m3,m4,m5,m6,m7,m8,m9,m10;
   double m11,m12,n1,n2,n3,n4,n5,n6,n7,n8,n9;

   //    cubic regression
   //    formula y=a*x^3+b*x^2 + c*x + d
   //let S(a,b,c,d)=SUM[1..i](d+c*xi+b*xi^2+a*xi^3-yi)^2

   //minimize S(a,b,c,d) then
   //dS/da=0 and dS/db=0 and dS/dc=0 and dS/dd=0 are at any point

   //dS/da=SUM[1..i](2*xi^3*(d+c*xi+b*xi^2+a*xi^3-yi))
   //dS/db=SUM[1..i](2*xi^2*(d+c*xi+b*xi^2+a*xi^3-yi))
   //dS/dc=SUM[1..i](2*xi*  (d+c*xi+b*xi^2+a*xi^3-yi))
   //dS/dd=SUM[1..i](2*1*   (d+c*xi+b*xi^2+a*xi^3-yi))

   //which gives
   //d*SUM[1..i](xi^3)+c*SUM[1..i](xi^4)+b*SUM[1..i](xi^5)+a*SUM[1..i](xi^6)-SUM[1..i](xi^3*yi)=0
   //d*SUM[1..i](xi^2)+c*SUM[1..i](xi^3)+b*SUM[1..i](xi^4)+a*SUM[1..i](xi^5)-SUM[1..i](xi^2*yi)=0
   //d*SUM[1..i](xi)  +c*SUM[1..i](xi^2)+b*SUM[1..i](xi^3)+a*SUM[1..i](xi^4)-SUM[1..i](xi*yi)=0
   //d*SUM[1..i](1)   +c*SUM[1..i](xi)  +b*SUM[1..i](xi^2)+a*SUM[1..i](xi^3)-SUM[1..i](yi)=0

   //or
   //d*x3sum+c*x4sum+b*x5sum+a*x6sum=x3ysum
   //d*x2sum+c*x3sum+b*x4sum+a*x5sum=x2ysum
   //d*xsum +c*x2sum+b*x3sum+a*x4sum=xysum
   //d*sum  +c*xsum +b*x2sum+a*x3sum=ysum

   if(x3sum!=0.0){
      m1=(x4sum*x2sum-x3sum*x3sum);
      m2=(x5sum*x2sum-x3sum*x4sum);
      m3=(x6sum*x2sum-x3sum*x5sum);
      m4=(x3ysum*x2sum-x2ysum*x3sum);
      m5=(x3sum*xsum-x2sum*x2sum);
      m6=(x4sum*xsum-x3sum*x2sum);
      m7=(x5sum*xsum-x4sum*x2sum);
      m8=(x2ysum*xsum-xysum*x2sum);
      m9 =(x2sum*sum-xsum*xsum);
      m10=(x3sum*sum-x2sum*xsum);
      m11=(x4sum*sum-x3sum*xsum);
      m12=(xysum*sum-ysum*xsum);
      n1=(m2*m5-m6*m1);
      n2=(m3*m5-m7*m1);
      n3=(m4*m5-m8*m1);
      n4=(m6*m9-m10*m5);
      n5=(m7*m9-m11*m5);
      n6=(m8*m9-m12*m5);
      n7=(n2*n4-n1*n5);
      n8=(n1*n4);
      n9=(m5*m9);

      if(n7!=0.0 && n8!=0.0 && n9!=0.0){
         A=(n3*n4-n6*n1)/n7;
         B=(n3*n4-A*n2*n4)/n8;
         C=(m8*m9-A*m7*m9-B*m6*m9)/n9;
         D=(x3ysum-C*x4sum-B*x5sum-A*x6sum)/x3sum;
         Succes=true;
      }
      else A=B=C=D=0.0;
   }
   else A=B=C=D=0.0;

   return Succes;
}
//---------------------------------------------------------------------------
// TCubicZeroRegression
//---------------------------------------------------------------------------
TCubicZeroRegression::TCubicZeroRegression(void)
{
   Reset();
}
//---------------------------------------------------------------------------
void TCubicZeroRegression::Reset(void)
{
   x2sum=x3sum=x4sum=x5sum=x6sum=xysum=x2ysum=x3ysum=0.0;
}
//---------------------------------------------------------------------------
void TCubicZeroRegression::Add(const double X,const double Y,const double Weight)
{
   double x=X;

   xysum += Weight*x*Y; x*=X;
   x2sum += Weight*x;
   x2ysum+= Weight*x*Y; x*=X;
   x3sum += Weight*x;
   x3ysum+= Weight*x*Y; x*=X;
   x4sum += Weight*x;   x*=X;
   x5sum += Weight*x;   x*=X;
   x6sum += Weight*x;
}
//---------------------------------------------------------------------------
bool TCubicZeroRegression::Execute(double &A,double &B,double &C)
{
   bool Succes=false;
   double E,F,H,I,K,S,J;

   E=(x3sum*x3sum-x4sum*x2sum);
   F=(x4sum*x3sum-x5sum*x2sum);
   H=(x4sum*x4sum-x5sum*x3sum);
   I=(x5sum*x4sum-x6sum*x3sum);
   K=( F   * H   -I    * E   );
   S=(xysum*x3sum-x2ysum*x2sum);
   J=(x2ysum*x4sum-x3ysum*x3sum);

   if(K!=0.0 && E!=0.0){
      A=(S*H-J*E)/K;
      B=(S-A*F)/E;
      C=(xysum-B*x3sum-A*x4sum)/x2sum;
      Succes=true;
   }
   else A=B=C=0.0;

   return Succes;
}
//---------------------------------------------------------------------------
// TPolyRegression
//---------------------------------------------------------------------------
TPolyRegression::TPolyRegression(int Order)
{
   Dim=Order;

   V=new double[Dim];
   A=new double*[Dim];
   for(int i=0;i<Dim;i++) A[i]=new double[Dim];

   Reset();
}
//---------------------------------------------------------------------------
TPolyRegression::~TPolyRegression(void)
{
   for(int i=0;i<Dim;i++) delete[] A[i];
   delete[] A;
   delete[] V;
}
//---------------------------------------------------------------------------
void TPolyRegression::Reset(void)
{
   for(int i=0;i<Dim;i++){
      for(int j=0;j<Dim;j++) A[i][j]=0.0;
      V[i]=0.0;
   }
}
//---------------------------------------------------------------------------
void TPolyRegression::Add(const double X,const double Y,const double Weight)
{
   int i,j;
   double t,s;

   for(s=Weight*Y,i=0;i<Dim;i++,s*=X) V[i]+=s;
   for(t=Weight,i=0;i<Dim;i++,t*=X){
      for(s=t,j=0;j<Dim;j++,s*=X) A[i][j]+=s;
   }
}
//---------------------------------------------------------------------------
bool TPolyRegression::Execute(double *Z)
{
   bool Succes;
   double t;
   int i,j;

   Succes=GaussianElimination(A,V,Z,Dim);

   //swap *X so you can use Numbers.h (this.Execute((double*)&Number);
   for(j=Dim-1, i=0;i<Dim/2;i++, j--){ t=Z[i]; Z[i]=Z[j]; Z[j]=t; }

   return Succes;
}
//---------------------------------------------------------------------------
// GaussRegression
//---------------------------------------------------------------------------
void TGaussRegression::Add(const double X,const double Y,const double Weight)
{
   TSquareRegression::Add(X,log(Y),Weight);
}
//---------------------------------------------------------------------------
bool TGaussRegression::Execute(double &A,double &Mu,double &Sigma)
{
   bool Succes;
   double a,b,c,p;

   Sigma=0.0; Mu=0.0; A=0.0;

   Succes=TSquareRegression::Execute(a,b,c);
   if(a>=-8.8E-5) Succes=false;
   if(Succes){
      Sigma=sqrt(-1.0/(2.0*a));
      Mu=b*Sigma*Sigma;
      p=c+(Mu*Mu)/(2.0*Sigma*Sigma);
      if(p<87.0) A=exp(p);
      else Succes=false;
   }

   return Succes;
}
//---------------------------------------------------------------------------
bool TGaussRegression::ExecuteTop(double Xtop,double &A,double &Mu,double &Sigma)
{
   bool Succes;
   double a,b,c,p;

   Succes=TSquareRegression::ExecuteTop(Xtop,a,b,c);
   if(a>=-8.8E-5) Succes=false;
   if(Succes){
      Sigma=sqrt(-1.0/(2.0*a));
      Mu=b*Sigma*Sigma;
      p=c+(Mu*Mu)/(2.0*Sigma*Sigma);
      if(p<87.0) A=exp(p);
      else{
         Sigma=0.0; Mu=0.0; Succes=false;
      }
   }

   return Succes;
}
//---------------------------------------------------------------------------
// GaussEstimation
//---------------------------------------------------------------------------
TGaussEstimation::TGaussEstimation(void)
{
   Reset();
}
//---------------------------------------------------------------------------
void TGaussEstimation::Reset(void)
{
   Cnt=0;
   Mean=Var=0.0;
}
//---------------------------------------------------------------------------
void TGaussEstimation::Add(const double X)
{
   double Delta=X-Mean;

   Cnt++;
   Mean+=Delta/Cnt;
	Var +=Delta*(X-Mean);
}
//---------------------------------------------------------------------------
bool TGaussEstimation::Execute(double &A,double &Mu,double &Sigma)
{
	if(Cnt==0) return false;

	Mu   =Mean;
	Sigma=sqrt(Var/Cnt); if(Sigma<1E-15) Sigma=1E-15;
	A    =Cnt/(Sigma*2.506628274631000502415765284811); // is M_SQRT2PI

	return true;
}
//---------------------------------------------------------------------------
// TWalkingLine
//---------------------------------------------------------------------------
TWalkingLine::TWalkingLine(void)  //simple initialization with 10 points
{
   Cnt=0;
	Size=10;
	Data=new DPoint[Size];

   Reset();
}
//---------------------------------------------------------------------------
TWalkingLine::TWalkingLine(int WalkSize)
{
   Cnt=0;
	Size=WalkSize;
	Data=new DPoint[Size];

   Reset();
}
//---------------------------------------------------------------------------
TWalkingLine::~TWalkingLine(void)
{
   if(Size>0){ delete[] Data; Size=0; }
}
//---------------------------------------------------------------------------
void TWalkingLine::operator =(const TWalkingLine &a)
{
   if(Size!=a.Size){
      delete[] Data;
      Size=a.Size;
   	Data=new DPoint[Size];
   }
   for(int i=0;i<Size;i++) Data[i]=a.Data[i];
   Cnt=a.Cnt;
   TLine::A=a.A;
   TLine::B=a.B;
   TLinRegression::operator =(a);
}
//---------------------------------------------------------------------------
void TWalkingLine::Reset(void)
{
   for(int i=0;i<Size;i++){ Data[i].X=0.0;  Data[i].Y=0.0; }
   TLinRegression::Reset();
   Cnt=0;
}
//---------------------------------------------------------------------------
bool TWalkingLine::Add(const DPoint Pnt)
{
   bool Succes;

   Data[Cnt%Size]=Pnt;

   TLinRegression::Add(Pnt);
   Succes=TLinRegression::Execute(TLine::A,TLine::B);
   if(Cnt>=(Size-1)){
      TLinRegression::Sub(Data[(Cnt+1)%Size]);
      Cnt++; if(Cnt==4*Size) Cnt=Size;
   }
   else Cnt++;

   return Succes;
}
//---------------------------------------------------------------------------
// TWalkingSquare
//---------------------------------------------------------------------------
TWalkingSquare::TWalkingSquare(int WalkSize)
{
   Cnt=0;
	Size=WalkSize;
	Data=new DPoint[Size];

   Reset();
}
//---------------------------------------------------------------------------
TWalkingSquare::~TWalkingSquare(void)
{
   if(Size>0){ delete[] Data; Size=0; }
}
//---------------------------------------------------------------------------
void TWalkingSquare::operator =(const TWalkingSquare &a)
{
   if(Size!=a.Size){
      delete[] Data;
      Size=a.Size;
   	Data=new DPoint[Size];
   }
   for(int i=0;i<Size;i++) Data[i]=a.Data[i];
   Cnt=a.Cnt;
   TQuadratic::A=a.A;
   TQuadratic::B=a.B;
   TQuadratic::C=a.C;
   TSquareRegression::operator =(a);
}
//---------------------------------------------------------------------------
void TWalkingSquare::Reset(void)
{
   for(int i=0;i<Size;i++){ Data[i].X=0.0;  Data[i].Y=0.0; }
   TSquareRegression::Reset();
   Cnt=0;
}
//---------------------------------------------------------------------------
bool TWalkingSquare::Add(const DPoint Pnt)
{
   bool Succes;

   Data[Cnt%Size]=Pnt;

   TSquareRegression::Add(Pnt);
   Succes=TSquareRegression::Execute(TQuadratic::A,TQuadratic::B,TQuadratic::C);
   if(Cnt>=(Size-1)){
      TSquareRegression::Sub(Data[(Cnt+1)%Size]);
      Cnt++; if(Cnt==4*Size) Cnt=Size;
   }
   else Cnt++;

   return Succes;
}
//---------------------------------------------------------------------------
// TWalkingCubic
//---------------------------------------------------------------------------
TWalkingCubic::TWalkingCubic(int WalkSize)
{
   Cnt=0;
	Size=WalkSize;
	Data=new DPoint[Size];

   Reset();
}
//---------------------------------------------------------------------------
TWalkingCubic::~TWalkingCubic(void)
{
   if(Size>0){ delete[] Data; Size=0; }
}
//---------------------------------------------------------------------------
void TWalkingCubic::operator =(const TWalkingCubic &a)
{
   if(Size!=a.Size){
      delete[] Data;
      Size=a.Size;
   	Data=new DPoint[Size];
   }
   for(int i=0;i<Size;i++) Data[i]=a.Data[i];
   Cnt=a.Cnt;
   TCubic::A=a.A;
   TCubic::B=a.B;
   TCubic::C=a.C;
   TCubic::D=a.D;
   TCubicRegression::operator =(a);
}
//---------------------------------------------------------------------------
void TWalkingCubic::Reset(void)
{
   for(int i=0;i<Size;i++){ Data[i].X=0.0;  Data[i].Y=0.0; }
   TCubicRegression::Reset();
   Cnt=0;
}
//---------------------------------------------------------------------------
bool TWalkingCubic::Add(const DPoint Pnt)
{
   bool Succes;

   Data[Cnt%Size]=Pnt;

   TCubicRegression::Add(Pnt);
   Succes=TCubicRegression::Execute(TCubic::A,TCubic::B,TCubic::C,TCubic::D);
   if(Cnt>=(Size-1)){
      TCubicRegression::Sub(Data[(Cnt+1)%Size]);
      Cnt++; if(Cnt==4*Size) Cnt=Size;
   }
   else Cnt++;


   return Succes;
}
//---------------------------------------------------------------------------
// TWalkingMean
//---------------------------------------------------------------------------
TWalkingMean::TWalkingMean(int WalkSize)
{
   Cnt=0;
   Mean=0.0;
	Size=WalkSize;
	Data=new double[Size];
}
//---------------------------------------------------------------------------
TWalkingMean::~TWalkingMean(void)
{
   if(Size>0){ delete[] Data; Size=0; }
}
//---------------------------------------------------------------------------
void TWalkingMean::Reset(void)
{
   Mean=0.0; Cnt=0;
}
//---------------------------------------------------------------------------
void TWalkingMean::operator =(const TWalkingMean &a)
{
   if(Size!=a.Size){
      delete[] Data;
      Size=a.Size;
   	Data=new double[Size];
   }
   for(int i=0;i<Size;i++) Data[i]=a.Data[i];
   Cnt=a.Cnt;
   Mean=a.Mean;
}
//---------------------------------------------------------------------------
void TWalkingMean::Add(const double X)
{
   if(Cnt>=Size){
      Mean-=Data[Cnt%Size];
      Data[Cnt%Size]=X/Size;
      Mean+=Data[Cnt%Size];
      Cnt++; if(Cnt==4*Size) Cnt=Size;
   }
   else{
      Data[Cnt%Size]=X/Size;
      Cnt++; Mean=0.0;
      for(int i=0;i<Cnt;i++) Mean+=Data[i];
      Mean*=Size; Mean/=Cnt;
   }
}
//---------------------------------------------------------------------------
#pragma package(smart_init)
