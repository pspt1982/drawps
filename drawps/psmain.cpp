#pragma warning( disable: 4996 )  

#include "drawps2.h"

DRAWPS2 drps2, subwin, graph;

// プロトタイプ
void Init();
void Calculate();
void Display();
void gDisplay();

// 変数
double t, r, s, vr, vs, l;
double T, U, L;

// 定数
const double dt = 0.0078125;
const double G  = 1.0;
const double m1 = 1.0;
const double m2 = 5.0;
const double PI = 3.141592653589793;
const double u = m1*m2/(m1+m2);

// 連立微分方程式
double fr(double vr)
{
	return vr;
}
double fs(double vs)
{
	return vs;
}
double fvr(double r, double vs)
{
	return r*vs*vs-G*m1*m2/(u*r*r);
}
double fvs(double r, double vr, double vs)
{
	return -2*vr*vs/r;
}
double V(double r){
	return u*l*l/(2.0*m1*m1*r*r)-G*m1*m2/r;
}

// DrawPS2の設定
void psMain()
{
	drps2.window.Size(1024, 80);
	drps2.window.Position(0, 0);
	drps2.psCreateWindow();

	subwin.window.Size(720, 560);
	subwin.window.Position(0, 80);
	subwin.event.Init(Init);
	subwin.event.Calculate(Calculate);
	subwin.event.Display(Display);
	subwin.window.style = beach;
	subwin.psCreateWindow();
	
	graph.window.Size(300, 300);
	graph.window.Position(724, 80);
	graph.event.Display(gDisplay);
	graph.psCreateWindow();
}

// 変数の初期化
void Init()
{
	// 変数メンバの初期値指定
	t  = 0.0;
	r  = 1.5;
	s  = 0.0;
	vr = 0.0;
	vs = 1.1;
	l  = m1*r*r*vs;
	
	T = 0;
	U = 0;
	L = 0;
}

// 計算関数
void Calculate()
{
	//ルンゲクッタ法
	double kr[4],ks[4],kvr[4], kvs[4];

	kr[0]  = fr(vr)*dt;
	ks[0]  = fs(vs)*dt;
	kvr[0] = fvr(r,vs)*dt;
	kvs[0] = fvs(r, vr, vs)*dt;

	kr[1]  = fr(vr+kvr[0]/2.0)*dt;
	ks[1]  = fs(vs+kvs[0]/2.0)*dt;
	kvr[1] = fvr(r+kr[0]/2.0, vs+kvs[0]/2.0)*dt;
	kvs[1] = fvs(r+kr[0]/2.0, vr+kvr[0]/2.0, vs+kvs[0]/2.0)*dt;

	kr[2]  = fr(vr+kvr[1]/2.0)*dt;
	ks[2]  = fs(vs+kvs[1]/2.0)*dt;
	kvr[2] = fvr(r+kr[1]/2.0, vs+kvs[1]/2.0)*dt;
	kvs[2] = fvs(r+kr[1]/2.0, vr+kvr[1]/2.0, vs+kvs[1]/2.0)*dt;

	kr[3]  = fr(vr+kvr[2])*dt;
	ks[3]  = fs(vs+kvs[2])*dt;
	kvr[3] = fvr(r+kr[2], vs+kvs[2])*dt;
	kvs[3] = fvs(r+kr[2], vr+kvr[2], vs+kvs[2])*dt;

	t  += dt;
	r  += (kr[0]+2.0*kr[1]+2.0*kr[2]+kr[3])/6.0;
	s  += (ks[0]+2.0*ks[1]+2.0*ks[2]+ks[3])/6.0;
	vr += (kvr[0]+2.0*kvr[1]+2.0*kvr[2]+kvr[3])/6.0;
	vs += (kvs[0]+2.0*kvs[1]+2.0*kvs[2]+kvs[3])/6.0;
	
	T = u*vr*vr/2.0;
	U = V(r);
	L = m1*r*r*vs;
}


// 描画関数
void Display()
{
	double rr, ss;

	static GLfloat light0pos[]  = { 0.0, 25.0, 20.0, 1.0 }; //光源の位置
	static GLfloat light0spec[] = { 1.0,  1.0,  1.0, 1.0 }; //錘の反射具合

	subwin.beachIPM(4.0, 6.0, 10.0, 0.0, 0.0, 0.0);

	glEnable(GL_DEPTH_TEST);                                 //デプスバッファ(陰面消去)の有効

	// ポテンシャル
	glColor3d(0.5, 0.5, 0.5);
	for(ss=0.0;ss<360.0;ss+=30.0){
		glBegin(GL_LINE_STRIP);
		glNormal3d(0.0, 1.0, 0.0);
		for(rr=0.1;rr<=6.0;rr+=0.1){
			glVertex3d(rr*cos(ss/180*PI), V(rr), rr*sin(ss/180*PI));
	     	}
		glVertex3d(rr*cos(ss/180*PI), V(rr), rr*sin(ss/180*PI));
		glRotatef(ss, 0.0, 1.0, 0.0);
		glEnd();
	}
	for(rr=0.1;rr<=6.1;rr+=0.5){
		glBegin(GL_LINE_STRIP);
		glNormal3d(0.0, 1.0, 0.0);
		for(ss=0.0;ss<360.0;ss+=30.0){
			glVertex3d(rr*cos(ss/180*PI), V(rr), rr*sin(ss/180*PI));
		}
		glVertex3d(rr*cos(ss/180*PI), V(rr), rr*sin(ss/180*PI));
		glRotatef(ss, 0.0, 1.0, 0.0);
		glEnd();
	}

	// 効果・光源の設定
	glEnable(GL_DEPTH_TEST);                                  //デプスバッファ(陰面消去)の有効

	// 光源に関する各種手続き(錘にのみ光源の影響を与えたいのでここに記述)
	glEnable(GL_LIGHTING);                                    //影の有効化
	glEnable(GL_LIGHT0);                                      //光源の有効化
	glEnable(GL_COLOR_MATERIAL);                              //質感を有効化(正確には、glColorの色設定で質感を持たせる？)
	glLightfv(GL_LIGHT0, GL_POSITION, light0pos);             //光源のステータスを設定(ここでは光源の位置)
                                                              //光源を設定しない場合には必要なし        
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, light0spec); //鏡面反射係数(反射のしやすさ)を設定
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 40.0);       //ハイライトの輝きを設定

	//  錘の描画
	glColor3d(1.0, 1.0, 1.0);
	glTranslated((GLdouble)r*cos(s), 0.0, (GLdouble)r*sin(s));              //錘を表示したい座標に原点を平行移動
	GLUquadricObj *sphere = gluNewQuadric();                  //オブジェクトを生成
	gluQuadricDrawStyle(sphere, GLU_FILL);                    //オブジェクトの描画タイプを設定（省略可）
	gluSphere(sphere, 0.1, 38, 16);                           //球を描画 半径1.0，緯経それぞれ10.0分割
	glTranslated(-(GLdouble)r*cos(s), 0.0, -(GLdouble)r*sin(s));            //紐を描画するので、座標を元に戻さなくてはいけない

	glDisable(GL_LIGHTING);                                   //影の無効化
	glDisable(GL_LIGHT0);                                     //光源の無効化
	glDisable(GL_COLOR_MATERIAL);                             //質感を無効化
	glDisable(GL_DEPTH_TEST);                                 //デプスバッファ(陰面消去)の無効
}

void gDisplay()
{
	char str[64];

	sprintf(str, "L : %f", L);
	graph.DrawText(2, 2, str, RGB(0, 0, 0));

	sprintf(str, "E : %f", T+U);
	graph.DrawText(2, 22, str, RGB(0, 0, 0));

	graph.SetPen(PS_SOLID, 1, RGB(0, 0, 255));
	graph.DrawLine(140, 150, 160, 150);
	graph.DrawLine(150, 140, 150, 160);
	graph.DrawPixel(50*r*cos(s)+150, 50*r*sin(s)+150, RGB(255, 0, 0));
}



