//---------------------------------------------------------------------------
// Q-engineering 1997 - 2023
//---------------------------------------------------------------------------
#ifndef RegressionH
#define RegressionH
#include "Numbers.h"
//---------------------------------------------------------------------------
struct TLinRegression;
struct TLinRegressionStore;
struct TSquareRegression;
struct TCubicRegression;
struct TPolyRegression;
struct TGaussRegression;
bool   GaussianElimination(double **M,double *V,double *X,int Dim);
//---------------------------------------------------------------------------
struct TAverMean
{
private:
	int Cnt;
	double Mean2;
public:
	double Min;
	double Max;
	double Mean;
	double Variance;
	double StdDev;

	TAverMean(void);

	void Reset(void);
	void Add(const double X);		//after every Add, Mean, Variance and StdDev are updated
};
//---------------------------------------------------------------------------
struct TLinRegression
{
private:
	double sum,xsum,ysum,x2sum,xysum,y2sum;
public:
	TLinRegression(void);

   void operator =(const TLinRegression &a);
	void Reset(void);
   void Sub(const DPoint Pnt);      //used in moving average line
   void Add(const DPoint Pnt);
	void Add(const double X,const double Y,const double Weight=1.0);
	inline double GetSum(void){ return sum; }
	bool Execute(TLine &Le);
   bool Execute(TLine &Le,const DPoint &Anchor);
	bool Execute(double &A,double &B);
	bool Execute(double &A,double &B,double &R);
	//Y = A*X + B
};
//---------------------------------------------------------------------------
struct TSquareRegression
{
private:
	double sum,xsum,ysum,x2sum,x3sum;
	double x4sum,xysum,x2ysum;
public:
   TSquareRegression(void);

   void operator =(const TSquareRegression &a);
   void Reset(void);
   void Sub(const DPoint Pnt);      //used in walking lines
   void Add(const DPoint Pnt);
	void Add(const double X,const double Y,const double Weight=1.0);
	bool Execute(TQuadratic &Qr);
	bool Execute(double &A,double &B,double &C);
	//Y = A*X^2 + B*X + C
	bool ExecuteTop(double Xtop,double &A,double &B,double &C);
	//Y = A*X^2 + B*X + C with given top position on X-axis
};
//---------------------------------------------------------------------------
struct TSquareZeroRegression
{
private:
	double xsum,ysum,x2sum,x3sum;
	double x4sum,xysum,x2ysum;
public:
	TSquareZeroRegression(void);

	void Reset(void);
	void Add(const double X,const double Y,const double Weight=1.0);
	bool Execute(double &A,double &B);
	//Y = A*X^2 + B*X   X=0->Y=0
};
//---------------------------------------------------------------------------
struct TCubicRegression
{
private:
	double sum,xsum,ysum,x2sum,x3sum;
	double x4sum,x5sum,x6sum,xysum,x2ysum,x3ysum;
public:
	TCubicRegression(void);

   void operator =(const TCubicRegression &a);
	void Reset(void);
   void Add(const DPoint Pnt);
   void Sub(const DPoint Pnt);
	void Add(const double X,const double Y,const double Weight=1.0);
	bool Execute(TCubic &Cb);
	bool Execute(double &A,double &B,double &C,double &D);
	//Y = A*X^3 + B*X^2 + C*X + D
};
//---------------------------------------------------------------------------
struct TCubicZeroRegression
{
private:
	double x2sum,x3sum,x4sum,x5sum;
	double x6sum,xysum,x2ysum,x3ysum;
public:
	TCubicZeroRegression(void);

	void Reset(void);
	void Add(const double X,const double Y,const double Weight=1.0);
	bool Execute(double &A,double &B,double &C);
	//Y = A*X^3 + B*X^2 + C*X  X=0->Y=0
};
//---------------------------------------------------------------------------
struct TPolyRegression
{
private:
   bool Zero;
   int Dim;
   double **A;
   double *V;
public:
   //above Order>10 use another solver (QRSolve) due to gaussian elimination
   TPolyRegression(int Order);
   ~TPolyRegression(void);

   void Reset(void);
   void Add(const double X,const double Y,const double Weight=1.0);
   bool Execute(double *Z);
   //Y = Z[0]*X^n ... Z[n-2]*X^2 + Z[n-1]*X^1 + Z[n]*X^0
};
//---------------------------------------------------------------------------
struct TGaussRegression:TSquareRegression
{
private:
public:
   //be aware: internal use of exp() without checks, so don't feed extreme small or large numbers
   //also feed no outliers due to the propagation of errors
   //save X's and Y's : Y>=0.25*Ymax (Ymax = top = A)
   //standard deviation of the peak height = 1.73*noise/sqrt(N), N=samples
   //standard deviation of the peak position = noise/sqrt(N),
   //standard deviation of the peak width = 3.62*noise/sqrt(N),
	void Add(const double X,const double Y,const double Weight=1.0);
   bool Execute(double &A,double &Mu,double &Sigma);
   //Y=A*exp(-0.5*((X-Mu)/Sigma)^2 )
   bool ExecuteTop(double Xtop,double &A,double &Mu,double &Sigma);
   //Y=A*exp(-0.5*((X-Mu)/Sigma)^2 ) with given top position on X-axis
};
//---------------------------------------------------------------------------
struct TGaussEstimation
{
private:
   int    Cnt;
   double Mean;
   double Var;
public:
   //if the distribution is gaussian the outcome will a VERY good estimation,
   //even with a few entries. (<10)
   TGaussEstimation(void);

   void Reset(void);
   void Add(const double X);
   bool Execute(double &A,double &Mu,double &Sigma);
   //Y=A*exp(-0.5*((X-Mu)/Sigma)^2 )
};
//---------------------------------------------------------------------------
struct TWalkingLine:TLinRegression,TLine
{
private:
	int Size;
	int Cnt;
	DPoint *Data;
public:
	TWalkingLine(void);           //simple initialization with 10 points for derived classes
	TWalkingLine(int WalkSize);
	~TWalkingLine(void);

   void operator =(const TWalkingLine &a);
	void Reset(void);
   bool Add(const DPoint Pnt);   //true when Cnt>=Size and LinRegression is true
};
//---------------------------------------------------------------------------
struct TWalkingSquare:TSquareRegression,TQuadratic
{
private:
	int Size;
	int Cnt;
	DPoint *Data;
public:
	TWalkingSquare(int WalkSize);
	~TWalkingSquare(void);

   void operator =(const TWalkingSquare &a);
	void Reset(void);
   bool Add(const DPoint Pnt);   //true when Cnt>=Size and SquareRegression is true
};
//---------------------------------------------------------------------------
struct TWalkingCubic:TCubicRegression,TCubic
{
private:
	int Size;
	int Cnt;
	DPoint *Data;
public:
	TWalkingCubic(int WalkSize);
	~TWalkingCubic(void);

   void operator =(const TWalkingCubic &a);
	void Reset(void);
   bool Add(const DPoint Pnt);   //true when Cnt>=Size and CubicRegression is true
};
//---------------------------------------------------------------------------
struct TWalkingMean
{
private:
	int Size;
	int Cnt;
	double *Data;
public:
	double Mean;

	TWalkingMean(int WalkSize);
   ~TWalkingMean(void);

	void operator =(const TWalkingMean &a);
	void Reset(void);
	void Add(const double X);		//after every Add, Mean, Variance and StdDev are updated
};
//---------------------------------------------------------------------------

#endif
