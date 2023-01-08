//---------------------------------------------------------------------------
// Q-engineering 1997 - 2023
//---------------------------------------------------------------------------
#ifndef NumbersH
#define NumbersH
#include "math.h"
#include "float.h"
//---------------------------------------------------------------------------
struct DPoint
{
	double X, Y;

	DPoint(): X(0),Y(0) {}
	DPoint(const double& a): X(a),Y(a) {}
	DPoint(const double& x,const double& y): X(x),Y(y) {}

	inline void   operator =(const DPoint &a) {	X=a.X; Y=a.Y; }
	inline DPoint operator -(const DPoint &a) { DPoint D; D.X=X-a.X; D.Y=Y-a.Y; return D; }
	inline void   operator -=(const DPoint &a) { X-=a.X; Y-=a.Y; }
	inline void   operator /=(const double &a) { X/=a; Y/=a; }
	inline void   operator /=(const DPoint &a) { X/=a.X; Y/=a.Y; }
	inline double Length(const DPoint &a){	return sqrt((X-a.X)*(X-a.X)+(Y-a.Y)*(Y-a.Y)); }
};
//---------------------------------------------------------------------------
struct TParaLine
{
	DPoint A;
	DPoint B;

	inline void operator =(const TParaLine &a) { A=a.A; B=a.B; }

	inline void Get(const int t,int &X,int &Y){ X=int(A.X*t+B.X+0.5); Y=int(A.Y*t+B.Y+0.5);}

	inline void Get(const double t,double &X,double &Y){ X=A.X*t+B.X; Y=A.Y*t+B.Y;}

	inline void Get(const double t,DPoint &Pnt){ Pnt.X=A.X*t+B.X; Pnt.Y=A.Y*t+B.Y;}

	inline double GetT(const double X)
	{
		double t;

		if(A.X!=0.0) t=(X-B.X)/A.X;
      else         t=DBL_MAX;

		return t;
	}

	inline double GetY(const double X)
	{
		double Y,t;

		if(A.X!=0.0){ t=(X-B.X)/A.X; Y=A.Y*t+B.Y;	}
		else Y=DBL_MAX;

		return Y;
	}

	inline double GetX(const double Y)
	{
		double X,t;

		if(A.Y!=0.0){ t=(Y-B.Y)/A.Y; X=A.X*t+B.X;	}
		else X=DBL_MAX;

		return X;
	}

	inline void MakeLine(const DPoint &P1,const DPoint &P2){ B=P1; A=P1; A-=P2; }

	inline void Orthogonal(void)
	{
		double D;

		D=A.X; A.X=A.Y; A.Y=D;

		if(A.X!=0.0) A.X*=-1.0;
		else 			 A.Y*=-1.0;
	}

	inline void Normalize(void)
	{
		double D;

		D=sqrt(A.X*A.X+A.Y*A.Y);
		if(D>0.0){ A.X/=D; A.Y/=D; }
	}

	inline void Intersection(const TParaLine &a,DPoint &Pnt)  //Pnt is crossing of the paraline with a
	{
		double cx=B.X-a.B.X;
		double cy=B.Y-a.B.Y;
		double t,pr=(A.Y*a.A.X-A.X*a.A.Y);

		if(pr!=0.0){
			t=(cx*a.A.Y-cy*a.A.X)/pr;
			Pnt.X=t*A.X+B.X; Pnt.Y=t*A.Y+B.Y;
		}
		else{ Pnt.X=DBL_MAX; Pnt.Y=DBL_MAX; }//lines are parallel
	}

	inline void Rotate(const double Angle) //+ clockwise angle in °
	{
      double An;

      if(A.X==0.0) An=90.0;
      else         An=180.0*atan(A.Y/A.X)/M_PI;

      An+=Angle;

      if(An==90.0){ A.X=0.0; A.Y=1.0; }
      else{ A.Y=tan(M_PI*(An/180.0)); A.X=1.0; }

      Normalize();
	}
};
//---------------------------------------------------------------------------
struct TLine
{
	double A;
	double B;

	inline TLine(void){ A=B=0.0; }
	inline TLine(const TLine &a) {  operator =(a); }

	inline void operator =(const double &a){A=a;   B=a;   }
	inline void operator =(const TLine &a) {A=a.A; B=a.B; }
	inline double GetY(const double &x){ return A*x+B; }
	inline double GetX(const double &y){ return (y-B)/A; }

	inline void MakeLine(const DPoint &P1,const DPoint &P2)
   {
      double dx=P1.X-P2.X;

      if(dx!=0.0){ A=(P1.Y-P2.Y)/dx; B=P1.Y-A*P1.X; }
      else{ A=DBL_MAX; B=DBL_MAX; }
   }

	inline void Orthogonal(const DPoint &P)      //perpendicular in P
	{
		if(A!=0.0){ A=-1.0/A; B=P.Y-A*P.X; }
		else{ A=DBL_MAX; B=DBL_MAX; }
	}

   inline void MakeParaLine(double X,TParaLine &Pl)
   {
      Pl.A.X=1.0; Pl.A.Y=A;
      Pl.B.X=X;   Pl.B.Y=A*X+B;
      Pl.Normalize();
   }

   inline void MakeOrthoParaLine(double X,TParaLine &Pl)
   {
      Pl.A.Y=-1.0; Pl.A.X=A;
      Pl.B.X=X;   Pl.B.Y=A*X+B;
      Pl.Normalize();
   }
};
//---------------------------------------------------------------------------
struct TQuadratic
{
	double A;
	double B;
	double C;

	inline TQuadratic(void){ A=B=C=0.0; }
	inline TQuadratic(const TQuadratic &a) {  operator =(a); }

	inline void operator =(const double &a)     {A=a;   B=a;   C=a;   }
	inline void operator =(const TQuadratic &a) {A=a.A; B=a.B; C=a.C; }
	inline double GetY(const double &x){ return x*(A*x+B)+C; }

	inline void MakeLine(const DPoint &P1,const DPoint &P2,const DPoint &P3)
   {
      double x1x=P1.X*P1.X;
      double x2x=P2.X*P2.X;
      double x3x=P3.X*P3.X;
      double dx1=P1.X-P2.X;
      double dy1=P1.Y-P2.Y;
      double dx3=P1.X-P3.X;
      double dy3=P1.Y-P3.Y;
      double d2x1=x1x-x2x;
      double d2x3=x1x-x3x;
      double dyd= dy1*dx3- dy3*dx1;
      double dxd=d2x1*dx3-d2x3*dx1;

      if(dxd!=0.0 && dx1!=0.0){ A=dyd/dxd; B=(dy1-A*d2x1)/dx1; C=P1.Y-x1x*A-P1.X*B; }
      else{ A=DBL_MAX; B=DBL_MAX; C=DBL_MAX; }
   }

   inline void MakeParaLine(double X,TParaLine &Pl)
   {
      Pl.B.X=X;    Pl.B.Y=GetY(X);
      Pl.A.X=-0.1; Pl.A.Y=Pl.B.Y-GetY(X+0.1);
      Pl.Normalize();
   }

   inline void MakeOrthoParaLine(double X,TParaLine &Pl)
   {
      Pl.B.X=X;    Pl.B.Y=GetY(X);
      Pl.A.Y=-0.1; Pl.A.X=GetY(X+0.1)-Pl.B.Y;
      Pl.Normalize();
   }
};
//---------------------------------------------------------------------------
struct TCubic
{
	double A;
	double B;
	double C;
	double D;

	inline TCubic(void){ A=B=C=D=0.0; }
	inline TCubic(const TCubic &a) {  operator =(a); }

	inline void operator =(const double &a) {A=a;   B=a;   C=a;   D=a;   }
	inline void operator =(const TCubic &a) {A=a.A; B=a.B; C=a.C; D=a.D; }
   inline void operator /=(const double &a) {A/=a; B/=a;  C/=a;  D/=a;  }
   inline TCubic operator +(const TCubic &a) const
   {
      TCubic Q;
      Q.A=A+a.A;
      Q.B=B+a.B;
      Q.C=C+a.C;
      Q.D=D+a.D;
      return Q;
   }
   inline TCubic operator -(const TCubic &a) const
   {
      TCubic Q;
      Q.A=A-a.A;
      Q.B=B-a.B;
      Q.C=C-a.C;
      Q.D=D-a.D;
      return Q;
   }

	inline double GetY(const double &x){ return x*(x*(A*x+B)+C)+D; }
   double Secant(const double Seed1,const double Seed2,const double e=1E-6)
   {
      int i=0;
      double y1,y2,x1;
      double x2=Seed1;
      double x3=Seed2;

      i=0; x1=x2; x2=x3;
      y1=x1*(x1*(x1*A+B)+C)+D;
      y2=x2*(x2*(x2*A+B)+C)+D;
      x3=x2-((x2-x1)/(y2-y1))*y2;

      while(fabs(x3-x2)>=e && x3!=0.0 && i<50){
         i++; x1=x2; x2=x3; y1=y2;
         y2=x2*(x2*(x2*A+B)+C)+D;
         x3=x2-((x2-x1)/(y2-y1))*y2;
      };

      return x3;
   };


   inline void MakeParaLine(double X,TParaLine &Pl)
   {
      Pl.B.X=X;    Pl.B.Y=GetY(X);
      Pl.A.X=-0.1; Pl.A.Y=Pl.B.Y-GetY(X+0.1);
      Pl.Normalize();
   }

   inline void MakeOrthoParaLine(double X,TParaLine &Pl)
   {
      Pl.B.X=X;    Pl.B.Y=GetY(X);
      Pl.A.Y=-0.1; Pl.A.X=GetY(X+0.1)-Pl.B.Y;
      Pl.Normalize();
   }

   inline void Intersection(TParaLine &a,DPoint &Pnt,const double Tolerance=1E-6)
   {
      TCubic Cb;
      TLine Ln;

      if(a.A.X==0.0){
         Pnt.X=a.B.X;
         Pnt.Y=GetY(Pnt.X);
      }
      else{
         Ln.A=a.A.Y/a.A.X;
         Ln.B=a.B.Y-Ln.A*a.B.X;
         Cb.A=A;
         Cb.B=B;
         Cb.C=C-Ln.A;
         Cb.D=D-Ln.B;
         Pnt.X=Cb.Secant(a.B.X-20.0,a.B.X+20.0);
         Pnt.Y=a.GetY(Pnt.X);
      }
   }
};
//---------------------------------------------------------------------------
struct TQuad
{
	double A;
	double B;
	double C;
	double D;
	double E;

	inline TQuad(void){ A=B=C=D=E=0.0; }
	inline TQuad(const TQuad &a) {  operator =(a); }

	inline void operator =(const double &a){A=a;   B=a;   C=a;   D=a;   E=a;  }
	inline void operator =(const TQuad &a) {A=a.A; B=a.B; C=a.C; D=a.D; E=a.E;}
	inline double GetY(const double &x){ return x*(x*(x*(A*x+B)+C)+D)+E; }
};
//---------------------------------------------------------------------------
#endif
