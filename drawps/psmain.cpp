#pragma warning( disable: 4996 )  

#include "drawps2.h"

DRAWPS2 drps2, subwin, graph;

// �v���g�^�C�v
void Init();
void Calculate();
void Display();
void gDisplay();

// �ϐ�
double t, r, s, vr, vs, l;
double T, U, L;

// �萔
const double dt = 0.0078125;
const double G  = 1.0;
const double m1 = 1.0;
const double m2 = 5.0;
const double PI = 3.141592653589793;
const double u = m1*m2/(m1+m2);

// �A������������
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

// DrawPS2�̐ݒ�
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

// �ϐ��̏�����
void Init()
{
	// �ϐ������o�̏����l�w��
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

// �v�Z�֐�
void Calculate()
{
	//�����Q�N�b�^�@
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


// �`��֐�
void Display()
{
	double rr, ss;

	static GLfloat light0pos[]  = { 0.0, 25.0, 20.0, 1.0 }; //�����̈ʒu
	static GLfloat light0spec[] = { 1.0,  1.0,  1.0, 1.0 }; //���̔��ˋ

	subwin.beachIPM(4.0, 6.0, 10.0, 0.0, 0.0, 0.0);

	glEnable(GL_DEPTH_TEST);                                 //�f�v�X�o�b�t�@(�A�ʏ���)�̗L��

	// �|�e���V����
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

	// ���ʁE�����̐ݒ�
	glEnable(GL_DEPTH_TEST);                                  //�f�v�X�o�b�t�@(�A�ʏ���)�̗L��

	// �����Ɋւ���e��葱��(���ɂ̂݌����̉e����^�������̂ł����ɋL�q)
	glEnable(GL_LIGHTING);                                    //�e�̗L����
	glEnable(GL_LIGHT0);                                      //�����̗L����
	glEnable(GL_COLOR_MATERIAL);                              //������L����(���m�ɂ́AglColor�̐F�ݒ�Ŏ�������������H)
	glLightfv(GL_LIGHT0, GL_POSITION, light0pos);             //�����̃X�e�[�^�X��ݒ�(�����ł͌����̈ʒu)
                                                              //������ݒ肵�Ȃ��ꍇ�ɂ͕K�v�Ȃ�        
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, light0spec); //���ʔ��ˌW��(���˂̂��₷��)��ݒ�
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 40.0);       //�n�C���C�g�̋P����ݒ�

	//  ���̕`��
	glColor3d(1.0, 1.0, 1.0);
	glTranslated((GLdouble)r*cos(s), 0.0, (GLdouble)r*sin(s));              //����\�����������W�Ɍ��_�𕽍s�ړ�
	GLUquadricObj *sphere = gluNewQuadric();                  //�I�u�W�F�N�g�𐶐�
	gluQuadricDrawStyle(sphere, GLU_FILL);                    //�I�u�W�F�N�g�̕`��^�C�v��ݒ�i�ȗ��j
	gluSphere(sphere, 0.1, 38, 16);                           //����`�� ���a1.0�C�܌o���ꂼ��10.0����
	glTranslated(-(GLdouble)r*cos(s), 0.0, -(GLdouble)r*sin(s));            //�R��`�悷��̂ŁA���W�����ɖ߂��Ȃ��Ă͂����Ȃ�

	glDisable(GL_LIGHTING);                                   //�e�̖�����
	glDisable(GL_LIGHT0);                                     //�����̖�����
	glDisable(GL_COLOR_MATERIAL);                             //�����𖳌���
	glDisable(GL_DEPTH_TEST);                                 //�f�v�X�o�b�t�@(�A�ʏ���)�̖���
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



