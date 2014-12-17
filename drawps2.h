//==================================================
//�@�@�@�@�@�@   DrawPS2 - drawps2.h
//        Copyright(C)2003-2009 �v�Z����������
//==================================================

#ifndef DRAWPS2_H
#define DRAWPS2_H
#include <iostream>
using namespace std;
#include <windows.h>
#include <gl/glut.h>
#include "resource.h"

#define MAXWIN 10


enum SIM {DRAW_PS, beach};
enum MOVIE {STOP, START, SUSPEND};

class DRAWPS2 {
	static int success;
	static int gWinNumber;

	HBITMAP hBitmap;
	HDC hBuffer;
	HPEN hpen, hpen_temp;
	HBRUSH hbrush, hbrush_temp;
	bool pen_called, brush_called;
	static HFONT hFont;

	// �J�����ϐ�
	GLdouble fovy, zNear, zFar;                                    // �J�����ݒ�
	struct{GLdouble x, y, z;}Focus,FocusPre,Click,Direction,init_Focus,init_vp;  // �œ_���W�Ȃ�
	struct{GLdouble r, phi, theta;}vpoint,pre;                   // ���_���W�Ȃ�

public:
	class WINDOWCLASS;
	class EVENTCLASS;
	class MOVIECLASS;

	BOOL InitDrawPS2(HINSTANCE hInstance);
	BOOL CheckDrawPS2();

	// �R���X�g���N�^
	DRAWPS2();

	HINSTANCE hInstance;

	static int gnumber;
	static HWND hWnd[MAXWIN];
	static HMENU hmenu;
	static MENUITEMINFO hmenuInfo;

	// �C�x���g�֐��|�C���^
	static void (*vpInitFunc[MAXWIN])();
	static void (*vpCalculateFunc[MAXWIN])();
	static void (*vpDisplayFunc[MAXWIN])();
	static void (*vpMouseFunc[MAXWIN])(UINT state, int x, int y);
	static void (*vpKeyboardFunc[MAXWIN])(UINT msg, WPARAM wp, LPARAM lp);
	static void (*vpControlFunc[MAXWIN])(int ID, int code, HWND handle);

	// �v���V�[�W��
	static LRESULT CALLBACK InitWndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
	LRESULT CALLBACK WndProc_DRAW_PS(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
	LRESULT CALLBACK WndProc_beach(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);

	// ����X���b�h
	static HANDLE hThread;
	static DWORD WINAPI ThreadFunc(LPVOID vdParam);

	// window�N���X
	class WINDOWCLASS {
	public:
		int width;
		int height;
		int position_x;
		int position_y;
		enum SIM style;			// �V�~�����[�V�����X�^�C��
		bool menu;
		bool maximize;
		void Size(int w, int h){
			width  = w;
			height = h;
		}
		void Position(int x, int y){
			position_x = x;
			position_y = y;
		}
	} window;

	// �C�x���g�N���X
	class EVENTCLASS {
	public:
		int wnumber;
		void Init(void (*vpFunc)());
		void Calculate(void (*vpFunc)());
		void Display(void (*vpFunc)());
		void Mouse(void (*vpFunc)(UINT state, int x, int y));
		void Keyboard(void (*vpFunc)(UINT state, WPARAM wp, LPARAM lp));
	 	void Control(void (*vpFunc)(int ID, int code, HWND handle));
	} event;

	// ���[�r�[�N���X
	class MOVIECLASS {
	public:
		enum MOVIE mode;				// �A�j���[�V�������
		int  uElapse;					// �X�V����(msec)
		void setMoviemode(enum MOVIE m){
			mode = m;
		}
		void start(){
			ResumeThread(hThread);
			hmenuInfo.fMask = MIIM_STATE;
			hmenuInfo.fState = MFS_DISABLED;
			SetMenuItemInfo(hmenu, IDM_START, FALSE, &hmenuInfo);
			SetMenuItemInfo(hmenu, IDM_NEXTFRAME, FALSE, &hmenuInfo);
			hmenuInfo.fState = MFS_ENABLED;
			SetMenuItemInfo(hmenu, IDM_SUSPEND, FALSE, &hmenuInfo);
			SetMenuItemInfo(hmenu, IDM_STOP, FALSE, &hmenuInfo);
			mode=START;
		}
		void suspend(){
			SuspendThread(hThread);
			hmenuInfo.fMask = MIIM_STATE;
			hmenuInfo.fState = MFS_DISABLED;
			SetMenuItemInfo(hmenu, IDM_SUSPEND, FALSE, &hmenuInfo);
			hmenuInfo.fState = MFS_ENABLED;
			SetMenuItemInfo(hmenu, IDM_START, FALSE, &hmenuInfo);
			SetMenuItemInfo(hmenu, IDM_STOP, FALSE, &hmenuInfo);
			SetMenuItemInfo(hmenu, IDM_NEXTFRAME, FALSE, &hmenuInfo);
			mode=SUSPEND;
		}
		void stop(){
			int i;
			if(mode==START) SuspendThread(hThread);
			hmenuInfo.fMask = MIIM_STATE;
			hmenuInfo.fState = MFS_DISABLED;
			SetMenuItemInfo(hmenu, IDM_SUSPEND, FALSE, &hmenuInfo);
			SetMenuItemInfo(hmenu, IDM_STOP, FALSE, &hmenuInfo);
			hmenuInfo.fState = MFS_ENABLED;
			SetMenuItemInfo(hmenu, IDM_START, FALSE, &hmenuInfo);
			SetMenuItemInfo(hmenu, IDM_NEXTFRAME, FALSE, &hmenuInfo);
			mode=STOP;
			for(i=0;i<gnumber;i++){
				if(vpInitFunc[i]!=NULL) (*vpInitFunc[i])();
				if(vpDisplayFunc[i]!=NULL) (*vpDisplayFunc[i])();
				InvalidateRect(hWnd[i], NULL, FALSE);
			}
		}
		void nextFrame(){
			int i;
			for(i=0;i<gnumber;i++){
				if((vpDisplayFunc[i])!=NULL) (*vpDisplayFunc[i])();
				if((vpCalculateFunc[i])!=NULL) (*vpCalculateFunc[i])();
				InvalidateRect(hWnd[i], NULL, FALSE);
			}
		}
	} movie;

	// �E�B���h�E�o�^�ƍ쐬
	HWND psCreateWindow();

	// OpenGL�`��֐��i����������j
	void beachIPM(double x1, double y1, double z1, double x2, double y2, double z2);// beach�W���i�������A�v���W�F�N�V�����A���f���r���[�j
	void defaultMouse(UINT state, int x, int y,int w);

	// win32API�`��֐��i�啶������j
	void SetPen(int fnPenStyle, int nWidth, COLORREF crColor);
	void SetBrush(int fnStyle, COLORREF crColor);
	void DrawPixel(double x, double y, COLORREF crColor);
	void DrawLine(double x1, double y1, double x2, double y2);
	void DrawVector(double x1, double y1, double x2, double y2);
	void DrawRectangle(double x1, double y1, double x2, double y2);
	void DrawEllipse(double x1, double y1, double x2, double y2);
	void DrawText(int x, int y, LPCTSTR lptstr, COLORREF crColor);
	void DrawText(int x, int y, LPCTSTR lptstr, COLORREF crColor, COLORREF bgColor);
	void EraseOrbit(void);

	void Update();
	void CloseWindow(char *str);
};







#endif
