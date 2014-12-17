//==================================================
//　　　　　　   DrawPS2 - drawps2.h
//        Copyright(C)2003-2009 計算物理研究会
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

	// カメラ変数
	GLdouble fovy, zNear, zFar;                                    // カメラ設定
	struct{GLdouble x, y, z;}Focus,FocusPre,Click,Direction,init_Focus,init_vp;  // 焦点座標など
	struct{GLdouble r, phi, theta;}vpoint,pre;                   // 視点座標など

public:
	class WINDOWCLASS;
	class EVENTCLASS;
	class MOVIECLASS;

	BOOL InitDrawPS2(HINSTANCE hInstance);
	BOOL CheckDrawPS2();

	// コンストラクタ
	DRAWPS2();

	HINSTANCE hInstance;

	static int gnumber;
	static HWND hWnd[MAXWIN];
	static HMENU hmenu;
	static MENUITEMINFO hmenuInfo;

	// イベント関数ポインタ
	static void (*vpInitFunc[MAXWIN])();
	static void (*vpCalculateFunc[MAXWIN])();
	static void (*vpDisplayFunc[MAXWIN])();
	static void (*vpMouseFunc[MAXWIN])(UINT state, int x, int y);
	static void (*vpKeyboardFunc[MAXWIN])(UINT msg, WPARAM wp, LPARAM lp);
	static void (*vpControlFunc[MAXWIN])(int ID, int code, HWND handle);

	// プロシージャ
	static LRESULT CALLBACK InitWndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
	LRESULT CALLBACK WndProc_DRAW_PS(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
	LRESULT CALLBACK WndProc_beach(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);

	// 制御スレッド
	static HANDLE hThread;
	static DWORD WINAPI ThreadFunc(LPVOID vdParam);

	// windowクラス
	class WINDOWCLASS {
	public:
		int width;
		int height;
		int position_x;
		int position_y;
		enum SIM style;			// シミュレーションスタイル
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

	// イベントクラス
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

	// ムービークラス
	class MOVIECLASS {
	public:
		enum MOVIE mode;				// アニメーション状態
		int  uElapse;					// 更新時間(msec)
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

	// ウィンドウ登録と作成
	HWND psCreateWindow();

	// OpenGL描画関数（小文字から）
	void beachIPM(double x1, double y1, double z1, double x2, double y2, double z2);// beach標準（初期化、プロジェクション、モデルビュー）
	void defaultMouse(UINT state, int x, int y,int w);

	// win32API描画関数（大文字から）
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
