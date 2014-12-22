#define _CRT_SECURE_NO_WARNINGS
#include "drawps2.h"

#define GN 160

DRAWPS2 drps2, graph;

// 変数
double t, x, v;
const double g  = 9.8;
const double l  = 1.0;
const double dt = 0.01;
const double m  = 1.0;
double T, U, E;
double graphT[GN], graphU[GN];

// プロトタイプ宣言
void Init();
void Calculate();
void Display();
void subDisplay();

// 連立微分方程式
double fx(double v){ return v;}
double fv(double x){ return -g/l*sin(x);}

void psMain()
{
	drps2.window.Size(640, 480);
	drps2.window.Position(50, 100);
	drps2.event.Init(Init);
	drps2.event.Calculate(Calculate);
	drps2.event.Display(Display);
	drps2.psCreateWindow();

	graph.window.Size(320, 240);
	graph.window.Position(680, 100);
	graph.event.Display(subDisplay);
	graph.psCreateWindow();
}

void Init()
{
	int i;

	t = 0.0;
	x = 1.0;
	v = 0.0;

	T = 0.0;
	U = 0.0;
	E = 0.0;

	for(i=0;i<GN;i++){
		graphT[i] = 0.0;
		graphU[i] = 0.0;
	}
}

void Calculate()
{
	int i;
	double kx[4],kv[4];

	kx[0] = fx(v)*dt;
	kv[0] = fv(x)*dt;
	kx[1] = fx(v+kv[0]/2.0)*dt;
	kv[1] = fv(x+kx[0]/2.0)*dt;
	kx[2] = fx(v+kv[1]/2.0)*dt;
	kv[2] = fv(x+kx[1]/2.0)*dt;
	kx[3] = fx(v+kv[2])*dt;
	kv[3] = fv(x+kx[2])*dt;

	t += dt;
	x += (kx[0]+2.0*kx[1]+2.0*kx[2]+kx[3])/6.0;
	v += (kv[0]+2.0*kv[1]+2.0*kv[2]+kv[3])/6.0;

	T = m*pow((l*v),2.0)/2.0;
	U = g/l*(1.0-cos(x));
	E = T + U;

	for(i=0;i<GN-1;i++){
		graphT[i] = graphT[i+1];
		graphU[i] = graphU[i+1];
	}
	graphT[GN-1] = T;
	graphU[GN-1] = U;
}

void Display()
{
	drps2.EraseOrbit();
	drps2.SetPen(PS_SOLID, 1, RGB(0, 0, 0));
	drps2.DrawLine(
		320.0,
		100.0,
		320.0+l*sin(x)*200.0,
		100.0+l*cos(x)*200.0
	);
	drps2.DrawEllipse(
		320.0+l*sin(x)*200.0-10.0,
		100.0+l*cos(x)*200.0-10.0,
		320.0+l*sin(x)*200.0+10.0,
		100.0+l*cos(x)*200.0+10.0
	);
}

void subDisplay()
{
	int i;
	char str[64];

	graph.EraseOrbit();

	sprintf(str, "x : %f", (l*sin(x)));
	graph.DrawText(2, 2, str, RGB(0, 0, 0));
	sprintf(str, "y : %f", (l*cos(x)));
	graph.DrawText(2, 22, str, RGB(0, 0, 0));

	sprintf(str, "T : %f", T);
	graph.DrawText(120, 2, str, RGB(0, 0, 255));
	sprintf(str, "U : %f", U);
	graph.DrawText(120, 22, str, RGB(0, 255, 0));
	sprintf(str, "E : %f", E);
	graph.DrawText(120, 42, str, RGB(255, 0, 0));

	graph.SetPen(PS_SOLID, 1, RGB(0, 0, 255));
	for(i=0;i<GN-1;i++){
		graph.DrawLine(
			(double)(i*2),
			200.0-graphT[i]*30.0,
			(double)((i+1)*2),
			200.0-graphT[i+1]*30.0
		);
	}
	graph.SetPen(PS_SOLID, 1, RGB(0, 255, 0));
	for(i=0;i<GN-1;i++){
		graph.DrawLine(
			(double)(i*2),
			200.0-graphU[i]*30.0,
			(double)((i+1)*2),
			200.0-graphU[i+1]*30.0
		);
	}
	graph.SetPen(PS_SOLID, 1, RGB(255, 0, 0));
	for(i=0;i<GN-1;i++){
		graph.DrawLine(
			(double)(i*2),
			200.0-(graphT[i]+graphU[i])*30.0,
			(double)((i+1)*2),
			200.0-(graphT[i]+graphU[i])*30.0
		);
	}
}