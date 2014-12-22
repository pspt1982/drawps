//== ================================================
//　　　　　　   DrawPS2 - drawps2.cpp
//        Copyright(C)2003-2009 計算物理研究会
//==================================================
#include "drawps2.h"
#include <map>

extern DRAWPS2 drps2;
extern void psMain();

// staticメンバ、関数
int DRAWPS2::success;
int DRAWPS2::gnumber;
int DRAWPS2::gWinNumber;
HWND DRAWPS2::hWnd[MAXWIN];
void (*DRAWPS2::vpInitFunc[MAXWIN])();
void (*DRAWPS2::vpCalculateFunc[MAXWIN])();
void (*DRAWPS2::vpDisplayFunc[MAXWIN])();
void (*DRAWPS2::vpMouseFunc[MAXWIN])(UINT state, int x, int y);
void (*DRAWPS2::vpKeyboardFunc[MAXWIN])(UINT state, WPARAM wp, LPARAM lp);
void (*DRAWPS2::vpControlFunc[MAXWIN])(int ID, int code, HWND handle);
HANDLE DRAWPS2::hThread;
MENUITEMINFO DRAWPS2::hmenuInfo;
HMENU DRAWPS2::hmenu;
HFONT DRAWPS2::hFont;

// エラー用メッセージボックス
void psError(HWND hwnd, char *str)
{
	MessageBox(hwnd, str, "エラー", MB_OK | MB_ICONERROR);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	MSG msg;
	BOOL bRet;

	if(drps2.InitDrawPS2(hInstance)) return -1;

	psMain();

	if(drps2.CheckDrawPS2()) return -1;

	while((bRet=GetMessage(&msg, NULL, 0, 0)) != 0){
		if(bRet == -1){
			MessageBox(NULL, "GetMessageエラー\n\n対策：DrawPS2内部エラーです。適当な修正をしてください。", "エラー", MB_OK);
			break;
		}
		else{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}


////////////////////////////////////////////////////////////////////////////////
/// DrawPS2クラス コンストラクタ
DRAWPS2::DRAWPS2()
{
	GLdouble x1,y1,z1;

	// ウィンドウサイズ
	window.width      = 640;
	window.height     = 480;
	window.position_x = CW_USEDEFAULT;
	window.position_y = CW_USEDEFAULT;
	window.menu       = true;
	window.maximize   = true;

	// マルチメディア変数
	window.style = DRAW_PS;
	movie.uElapse = 10;
	movie.mode = SUSPEND;
	event.wnumber = gWinNumber++;

	// win32API関連
	pen_called   = false;
	brush_called = false;

	x1 = 1.0;	// カメラ座標
	y1 = 1.0;
	z1 = 1.0;

	Focus.x = 0.0;	// 目標
	Focus.y = 0.0;
	Focus.z = 0.0;

	Direction.x = 0.0;
	Direction.y = 1.0;
	Direction.z = 0.0;

	// カメラ座標変換（直交座標->極座標）
	vpoint.r     = sqrt(pow(x1, 2) + pow(y1, 2) + pow(z1, 2));
	vpoint.phi   = atan2(z1, x1);
	vpoint.theta = atan2(sqrt(pow(x1, 2) + pow(z1, 2)), y1);

	fovy  = 30.0;	// 画角
	zNear = 1.0;
	zFar  = 100.0;
}


////////////////////////////////////////////////////////////////////////////////
// ウィンドウクラス登録とイベント関数ポインタ初期化
BOOL DRAWPS2::InitDrawPS2(HINSTANCE hInst)
{
	int i;
	WNDCLASS winc, wins;

	winc.style         = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	winc.lpfnWndProc   = InitWndProc;
	winc.cbClsExtra    = 0;
	winc.cbWndExtra    = 0;
	winc.hInstance     = hInst;
	winc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
	winc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	winc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	winc.lpszMenuName  = NULL;
	winc.lpszClassName = "DRAWPS2";

	if(!RegisterClass(&winc)) return TRUE;

	wins.style         = 0;
	wins.lpfnWndProc   = InitWndProc;
	wins.cbClsExtra    = 0;
	wins.cbWndExtra    = 0;
	wins.hInstance     = hInst;
	wins.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
	wins.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wins.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wins.lpszMenuName  = NULL;
	wins.lpszClassName = "DRAWPS2sub";

	if(!RegisterClass(&wins)) return TRUE;

	for(i=0;i<MAXWIN;i++){
		vpInitFunc[i]      = NULL;
		vpCalculateFunc[i] = NULL;
		vpDisplayFunc[i]   = NULL;
		vpMouseFunc[i]     = NULL;
		vpKeyboardFunc[i]  = NULL;
		vpControlFunc[i]   = NULL;
	}

	hInstance = hInst;
	success = 0;
	gnumber = 0;

	return FALSE;
}

BOOL DRAWPS2::CheckDrawPS2()
{
	if(success==gWinNumber){
		return false;
	}
	else{
		psError(hWnd[0], "オブジェクト数とpsCreateWindow()呼び出し数が一致しません\n\n"
		"対策：オブジェクトを削除またはpsCreateWindow()を呼び出す");
		return true;
	}
}


////////////////////////////////////////////////////////////////////////////////
/// ウィンドウ作成
HWND DRAWPS2::psCreateWindow()
{
	LPCTSTR lpClassName = "DRAWPS2";
	LPCTSTR lpWindowName = "DrawPS2";
	DWORD dwStyle = window.maximize==true ? WS_OVERLAPPEDWINDOW | WS_VISIBLE : WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU | WS_VISIBLE;
	HMENU hMenu   = window.menu==true ? LoadMenu(hInstance, "PSMENU") : NULL;

	if(event.wnumber==0){
		hWnd[event.wnumber] = CreateWindow(
			lpClassName,
			lpWindowName,
			dwStyle,
			window.position_x,
			window.position_y,
			window.width,
			window.height,
			NULL,
			hMenu,
			hInstance,
			this);
	}
	else{
		hWnd[event.wnumber] = CreateWindow(
			lpClassName,
			lpWindowName,
			WS_CAPTION | WS_VISIBLE,
			window.position_x,
			window.position_y,
			window.width,
			window.height,
			hWnd[0],
			NULL,
			hInstance,
			this);
	}

	gnumber++;

	if(hWnd[event.wnumber]!=NULL) success++;

	return hWnd[event.wnumber];
}


////////////////////////////////////////////////////////////////////////////////
/// プロシージャ
BOOL CALLBACK DialogProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp)
{
	switch (msg) {
	case WM_CLOSE:
			EndDialog(hDlg, IDOK);
			return true;
	case WM_COMMAND:
		switch(LOWORD(wp)){
		case IDOK:
			EndDialog(hDlg, IDOK);
			return true;
		}
		return false;
	}
	return false;
}

// 共通プロシージャ
LRESULT CALLBACK DRAWPS2::InitWndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	DRAWPS2* this_ = NULL;
	static DWORD dwID;
	static std::map<HWND, DRAWPS2*> mapwin;

	int  tewnumber;
	void (*tInitFunc)();
	void (*tDisplayFunc)();
	void (*tMouseFunc)(UINT, int, int);
	void (*tKeyboardFunc)(UINT, WPARAM, LPARAM);
	void (*tControlFunc)(int, int, HWND);

	if(msg==WM_CREATE){
		// ウィンドウハンドルマップ
		this_ = (DRAWPS2*)((CREATESTRUCT*)lp)->lpCreateParams;
		mapwin[hwnd] = this_;

		tewnumber = this_->event.wnumber;
		tInitFunc = this_->vpInitFunc[tewnumber];

		// スレッド作成
		if(tewnumber==0){
			this_->hThread = CreateThread(NULL, 0, ThreadFunc, (LPVOID)this_, CREATE_SUSPENDED, &dwID);
			// メニュー作成
			hmenu = GetMenu(hwnd);
			hmenuInfo.cbSize = sizeof(MENUITEMINFO);
			hmenuInfo.fMask = MIIM_STATE;
			hmenuInfo.fState = MFS_DISABLED;
			SetMenuItemInfo(hmenu, IDM_SUSPEND, FALSE, &hmenuInfo);
			SetMenuItemInfo(hmenu, IDM_STOP, FALSE, &hmenuInfo);
			if(this_->movie.mode==START){
				this_->movie.start();
			}
		}
		// 初期化
		if(tInitFunc!=NULL) (*tInitFunc)();
	}
	else if(mapwin.count(hwnd)){
		this_ = mapwin[hwnd];
	}

	if(this_!=NULL){
		tewnumber     = this_->event.wnumber;
		tInitFunc     = this_->vpInitFunc[tewnumber];
		tDisplayFunc  = this_->vpDisplayFunc[tewnumber];
		tMouseFunc    = this_->vpMouseFunc[tewnumber];
		tKeyboardFunc = this_->vpKeyboardFunc[tewnumber];
		tControlFunc  = this_->vpControlFunc[tewnumber];

		// 共通動作
		switch(msg){
		// リサイズイベント
		case WM_SIZE:
			this_->window.width  = LOWORD(lp);
			this_->window.height = HIWORD(lp);
			return 0;
		// マウスイベント
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_LBUTTONDBLCLK:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
		case WM_RBUTTONDBLCLK:
		case WM_MBUTTONDOWN:
		case WM_MBUTTONUP:
		case WM_MOUSEWHEEL:
		case WM_MOUSEMOVE:
		case WM_NCMOUSEMOVE:
			if(tMouseFunc!=NULL) (*tMouseFunc)(msg, LOWORD(lp), HIWORD(lp));
			if(tKeyboardFunc==NULL && this_->window.style==beach) this_->defaultMouse(msg, LOWORD(lp), HIWORD(lp),GET_WHEEL_DELTA_WPARAM(wp));
			return 0;
		// キーボードイベント
		case WM_KEYUP:
		case WM_KEYDOWN:
		case WM_CHAR:
			if(tKeyboardFunc!=NULL) (*tKeyboardFunc)(msg, wp, lp);
			return 0;
		//case WM_SETFOCUS:
			//for(i=0;i<gnumber;i++) ShowWindow(hWnd[i], SW_SHOWNA);
			//break;
		// 定義イベント
		case WM_COMMAND:
			switch (LOWORD(wp)){
			case IDM_VERSION:
				DialogBox(this_->hInstance, "PSVERDLG", hwnd, DialogProc);
				return 0;
			case IDM_END:
				SendMessage(hwnd, WM_CLOSE, 0, 0);
				return 0;
			case IDM_START:
				this_->movie.start();
				return 0;
			case IDM_SUSPEND:
				 this_->movie.suspend();
				return 0;
			case IDM_STOP:
				this_->movie.stop();
				return 0;
			case IDM_NEXTFRAME:
				this_->movie.nextFrame();
				return 0;
			}
			// コントロールイベント
			if(tControlFunc!=NULL) (*tControlFunc)(LOWORD(wp), HIWORD(wp), (HWND)lp);
		}
		// タイプ別プロシージャへ振り分け
		switch(this_->window.style){
		case DRAW_PS : return this_->WndProc_DRAW_PS(hwnd, msg, wp, lp);
		case beach   : return this_->WndProc_beach(hwnd, msg, wp, lp);
		}
	}
	else{
		return DefWindowProc(hwnd, msg, wp, lp);
	}
	return 0;
}

// DRAW_PSプロシージャ
LRESULT CALLBACK DRAWPS2::WndProc_DRAW_PS(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	PAINTSTRUCT ps;
	HDC hdc;

	const double PI = 3.141592653589793;

	switch(msg){
	case WM_DESTROY:
		DeleteDC(hBuffer);
		DeleteObject(hBitmap);
		DeleteObject(hFont);
		if(pen_called){
			DeleteObject(hpen);
			DeleteObject(hpen_temp);
		}
		if(brush_called){
			DeleteObject(hbrush);
			DeleteObject(hbrush_temp);
		}
		PostQuitMessage(0);
		return 0;
	case WM_CREATE:
		hdc = GetDC(hwnd);
		hBuffer = CreateCompatibleDC(hdc);
		hBitmap = CreateCompatibleBitmap(hdc, window.width, window.height);
		SelectObject(hBuffer, hBitmap);
		PatBlt(hBuffer, 0, 0, window.width, window.height, WHITENESS);
		ReleaseDC(hwnd, hdc);
		// フォント作成
		hFont = CreateFont(
			16, 0, 0, 0,
			FW_DONTCARE,
			false, false, false,
			SHIFTJIS_CHARSET,
			OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS,
			DEFAULT_QUALITY,
			DEFAULT_PITCH | FF_DONTCARE,
			"MS UI Gothic"
		);
		if(vpDisplayFunc[event.wnumber]!=NULL) (vpDisplayFunc[event.wnumber])();
		return 0;
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		BitBlt(hdc, 0, 0, window.width, window.height, hBuffer, 0, 0, SRCCOPY);
		EndPaint(hwnd, &ps);
		return 0;
	}
	return DefWindowProc(hwnd, msg, wp, lp);
}

// beachプロシージャ
LRESULT CALLBACK DRAWPS2::WndProc_beach(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	HDC hDC=0;
	PAINTSTRUCT ps;
	static HGLRC hRC;

	int PixelFormat;
	PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW |
		PFD_SUPPORT_OPENGL |
		PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA,
		24,
		0, 0, 0, 0, 0, 0,
		0,
		0,
		0,
		0, 0, 0, 0,
		32,
		0,
		0,
		PFD_MAIN_PLANE,
		0,
		0, 0, 0
	};

	switch(msg){
	case WM_DESTROY:
		wglMakeCurrent(hDC, 0);
		wglDeleteContext(hRC);
		PostQuitMessage(0);
		return 0;
	case WM_CREATE:
		hDC = GetDC(hwnd);
		PixelFormat = ChoosePixelFormat(hDC, &pfd);
		SetPixelFormat(hDC, PixelFormat, &pfd);
		hRC = wglCreateContext(hDC);
		ReleaseDC(hwnd, hDC);
		return 0;
	case WM_PAINT:
		// GL描画
		hDC = BeginPaint(hwnd , &ps);
		wglMakeCurrent(hDC, hRC);
		if(vpDisplayFunc[event.wnumber]!=NULL) (*vpDisplayFunc[event.wnumber])();
		SwapBuffers(hDC);
		wglMakeCurrent(0, 0);
		EndPaint(hwnd, &ps);
		return 0;
	}
	return DefWindowProc(hwnd, msg, wp, lp);
}


////////////////////////////////////////////////////////////////////////////////
/// 制御スレッド
DWORD WINAPI DRAWPS2::ThreadFunc(LPVOID vdParam)
{
	DRAWPS2* this_ = (DRAWPS2*)vdParam;
	DWORD dwCheck = timeGetTime() + this_->movie.uElapse;
	int i;

	timeBeginPeriod(1);
	while(true){
		for(i=0;i<this_->gnumber;i++){
			if((this_->vpDisplayFunc[i])!=NULL && this_->window.style==DRAW_PS) (*this_->vpDisplayFunc[i])();
			if((this_->vpCalculateFunc[i])!=NULL) (*this_->vpCalculateFunc[i])();
			InvalidateRect(hWnd[i], NULL, FALSE);
		}
		while(dwCheck>timeGetTime()) Sleep(1);
		dwCheck = timeGetTime() + this_->movie.uElapse;
	}
	timeEndPeriod(1);
}


////////////////////////////////////////////////////////////////////////////////
/// イベント登録
// 初期化
void DRAWPS2::EVENTCLASS::Init(void (*vpFunc)())
{
	vpInitFunc[wnumber] = vpFunc;
}
// 計算
void DRAWPS2::EVENTCLASS::Calculate(void (*vpFunc)())
{
	vpCalculateFunc[wnumber] = vpFunc;
}
// 描画
void DRAWPS2::EVENTCLASS::Display(void (*vpFunc)())
{
	vpDisplayFunc[wnumber] = vpFunc;
}
// マウス
void DRAWPS2::EVENTCLASS::Mouse(void (*vpFunc)(UINT state, int x, int y))
{
	vpMouseFunc[wnumber] = vpFunc;
}
// キーボード
void DRAWPS2::EVENTCLASS::Keyboard(void (*vpFunc)(UINT msg, WPARAM wp, LPARAM lp))
{
	vpKeyboardFunc[wnumber] = vpFunc;
}
// コントロール
void DRAWPS2::EVENTCLASS::Control(void (*vpFunc)(int ID, int code, HWND handle))
{
	vpControlFunc[wnumber] = vpFunc;
}


////////////////////////////////////////////////////////////////////////////////
/// win32API 描画関数
void DRAWPS2::SetPen(int fnPenStyle, int nWidth, COLORREF crColor)
{
	if(pen_called){					//既に論理ペンが生成されているかどうかチェック
		// 論理ペンの生成(既に他の論理ペンが生成されているならば破棄)
		SelectObject(hBuffer, hpen_temp);
		DeleteObject(hpen);
	}
	hpen = CreatePen(fnPenStyle, nWidth, crColor);
	hpen_temp = (HPEN)SelectObject(hBuffer, hpen);
	pen_called=true;
}

// 論理ブラシの生成(既に他の論理ブラシが生成されているならば破棄)
void DRAWPS2::SetBrush(int fnStyle, COLORREF crColor)
{
	if(brush_called){				//既に論理ブラシが生成されているかどうかチェック
		SelectObject(hBuffer, hbrush_temp);
		DeleteObject(hbrush);
	}
	if(fnStyle==BS_SOLID)		hbrush = CreateSolidBrush(crColor);
	else if(fnStyle==BS_NULL)	hbrush = (HBRUSH)GetStockObject(NULL_BRUSH);
	else                        hbrush = CreateHatchBrush(fnStyle, crColor);
	hbrush_temp = (HBRUSH)SelectObject(hBuffer, hbrush);
	brush_called=true;
}

// 点を描画
void DRAWPS2::DrawPixel(double x, double y, COLORREF crColor)
{
	SetPixel(hBuffer, x, y, crColor);
}

// 直線を描画
void DRAWPS2::DrawLine(double x1, double y1, double x2, double y2)
{
	MoveToEx(hBuffer, x1, y1, NULL);
	LineTo(hBuffer, x2, y2);
}

// ベクトルを描画 
void DRAWPS2::DrawVector(double x1, double y1, double x2, double y2)
{
	const double PI=3.141592653589793;
	double C1, S1, C2, S2;

	C1 = x2!=0.0 ? x2/sqrt(x2*x2+y2*y2) : 0.0;
	S1 = y2!=0.0 ? y2/sqrt(x2*x2+y2*y2) : 0.0;
	C2 = cos(5.0*PI/6.0);
	S2 = sin(5.0*PI/6.0);

	MoveToEx(hBuffer, x1, y1, NULL);
	LineTo(hBuffer, x1+x2, y1+y2);
 	LineTo(hBuffer, x1+x2+5.0*(C2*C1-S2*S1), y1+y2+5.0*(C2*S1+S2*C1));
	MoveToEx(hBuffer, x1+x2, y1+y2, NULL);
 	LineTo(hBuffer, x1+x2+5.0*(C2*C1+S2*S1), y1+y2+5.0*(C2*S1-S2*C1));
}

// 四角形を描画
void DRAWPS2::DrawRectangle(double x1, double y1, double x2, double y2)
{
	Rectangle(hBuffer, x1, y1, x2, y2);
}

// 楕円を描画
void DRAWPS2::DrawEllipse(double x1, double y1, double x2, double y2)
{
	Ellipse(hBuffer, x1, y1, x2, y2);
}

// テキスト出力
void DRAWPS2::DrawText(int x, int y, LPCTSTR lptstr, COLORREF crColor)
{
	SetTextColor(hBuffer, crColor);
	SelectObject(hBuffer, hFont);
	TextOut(hBuffer, x, y, lptstr, lstrlen(lptstr));
}

void DRAWPS2::DrawText(int x, int y, LPCTSTR lptstr, COLORREF crColor, COLORREF bgColor)
{
	SetTextColor(hBuffer, crColor);
	SetBkColor(hBuffer , bgColor);
	SelectObject(hBuffer, hFont);
	TextOut(hBuffer, x, y, lptstr, lstrlen(lptstr));
}

// 白(背景色)で塗りつぶす
void DRAWPS2::EraseOrbit(void)
{
	PatBlt(hBuffer, 0, 0, window.width, window.height, WHITENESS);
}

// 無効領域発効
void DRAWPS2::Update()
{
	if((vpDisplayFunc[event.wnumber])!=NULL) (*vpDisplayFunc[event.wnumber])();
	InvalidateRect(hWnd[event.wnumber], NULL, FALSE);
}


////////////////////////////////////////////////////////////////////////////////
/// OpenGL関連
// beach標準　プロジェクション・モデルビュー変換
void DRAWPS2::beachIPM(double x1, double y1, double z1, double x2, double y2, double z2)
{
	static bool flag = false;
	if(!flag){
		init_vp.x = x1;
		init_vp.y = y1;
		init_vp.z = z1;
		init_Focus.x = x2;
		Focus.x = x2;
		init_Focus.y = y2;
		Focus.y = y2;
		init_Focus.z = z2;
		Focus.z = z2;
		vpoint.r     = sqrt( pow((x1-x2), 2) + pow((y1-y2), 2) + pow((z1-z2), 2) );
		vpoint.phi   = atan2(z1-z2, x1-x2);
		vpoint.theta = atan2( sqrt(pow(x1-x2, 2) + pow(z1-z2, 2)), y1-y2);
 		flag = true;
	}
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, window.width, window.height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fovy, (GLdouble)window.width/(GLdouble)window.height, zNear, zFar);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(
		Focus.x + vpoint.r * sin(vpoint.theta) * cos(vpoint.phi),
		Focus.y + vpoint.r * cos(vpoint.theta),
		Focus.z + vpoint.r * sin(vpoint.theta) * sin(vpoint.phi),
		Focus.x,
		Focus.y,
		Focus.z,
		Direction.x,
		Direction.y,
		Direction.z
	);
}


// beach標準マウス動作
void DRAWPS2::defaultMouse(UINT state, int x, int y,int w)
{
	const double PI = 3.141592653589793;
	static enum MOUSE {NO_MODE=0, R_MODE, L_MODE, RL_MODE, LR_MODE} MOUSE_MODE=NO_MODE; // マウス状態
	int mouse;
	char debug[10]={0};

	// マウスイベント
	switch(state){
	case WM_LBUTTONDOWN:
		Click.x  = (GLdouble)x;
		Click.y  = (GLdouble)y;
		switch(MOUSE_MODE){
		case NO_MODE:
			MOUSE_MODE = L_MODE;
			pre.phi  = vpoint.phi;
			pre.theta= vpoint.theta;
			break;
		case R_MODE:
			MOUSE_MODE = RL_MODE;
			FocusPre.x = Focus.x;
			FocusPre.y = Focus.y;
			FocusPre.z = Focus.z;
			break;
		}
		break;
	case WM_LBUTTONUP:
		switch(MOUSE_MODE){
		case L_MODE:
			MOUSE_MODE = NO_MODE;
			break;
		case RL_MODE:
		case LR_MODE:
			MOUSE_MODE = R_MODE;
			Click.x = (GLdouble)x;
			Click.y = (GLdouble)y;
			FocusPre.x = Focus.x;
			FocusPre.y = Focus.y;
			FocusPre.z = Focus.z;
			break;
		}
		break;
	case WM_LBUTTONDBLCLK:
		// ビューポート値設定
		vpoint.r     = sqrt( pow((init_vp.x-init_Focus.x), 2) + pow((init_vp.y-init_Focus.y), 2) + pow((init_vp.z-init_Focus.z), 2) );
		vpoint.phi   = atan2(init_vp.z-init_Focus.z, init_vp.x-init_Focus.x);
		vpoint.theta = atan2( sqrt(pow(init_vp.x-init_Focus.x, 2) + pow(init_vp.z-init_Focus.z, 2)), init_vp.y-init_Focus.y);
		// フォーカス値設定
		Focus.x = init_Focus.x;
		Focus.y = init_Focus.y;
		Focus.z = init_Focus.z;
		Update();
		break;
	case WM_RBUTTONDOWN:
		switch(MOUSE_MODE){
		case NO_MODE:
			MOUSE_MODE = R_MODE;
			Click.x = (GLdouble)x;
			Click.y = (GLdouble)y;
			FocusPre.x = Focus.x;
			FocusPre.y = Focus.y;
			FocusPre.z = Focus.z;
			break;
		case L_MODE:
			MOUSE_MODE = LR_MODE;
			Click.y = (GLdouble)y;
			pre.r = vpoint.r;
			break;
		}
		break;
	case WM_RBUTTONUP:
		switch(MOUSE_MODE){
		case R_MODE:
			MOUSE_MODE = NO_MODE;
			break;
		case LR_MODE:
		case RL_MODE:
			MOUSE_MODE = L_MODE;
			Click.x  = (GLdouble)x;
			Click.y  = (GLdouble)y;
			pre.phi  = vpoint.phi;
			pre.theta= vpoint.theta;
			break;
		}
		break;
	case WM_MOUSEWHEEL:
		mouse=w;
		OutputDebugString("エラー発生！\n");
		if(mouse>0) vpoint.r+= (GLdouble)-0.5 ;
		if(mouse<0) vpoint.r+= (GLdouble)0.5 ;
		//vpoint.r = (GLdouble)((mouse<0) ? vpoint.r-0.5 : vpoint.r+0.5);
		if(vpoint.r <= 2.0) vpoint.r = 2.0;
		Update();
		break;
	case WM_MOUSEMOVE:
		switch(MOUSE_MODE){
		case L_MODE:
			vpoint.phi   = ((GLdouble)x - Click.x)*0.01 + pre.phi;
			vpoint.theta = (Click.y - (GLdouble)y)*0.01 + pre.theta;
			if(vpoint.theta<=0.0)     vpoint.theta = 0.001;
			else if(vpoint.theta>=PI) vpoint.theta = PI;
			break;
		case R_MODE:
			Focus.x =  ((GLdouble)x - Click.x) * 0.04 * cos(PI/2.0-vpoint.phi) + FocusPre.x;
			Focus.y =  (Click.y - (GLdouble)y) * 0.03 + FocusPre.y;
			Focus.z = -((GLdouble)x - Click.x) * 0.04 * sin(PI/2.0-vpoint.phi) + FocusPre.z;
			break;
		case LR_MODE:
			vpoint.r = ((GLdouble)y - Click.y)*0.1 + pre.r;
			if(vpoint.r <= 2.0) vpoint.r = 2.0;
			break;
		case RL_MODE:
			Focus.x = ((GLdouble)y - Click.y) * sin(vpoint.theta) * cos(vpoint.phi) * 0.1 + FocusPre.x;
			Focus.y = ((GLdouble)y - Click.y) * cos(vpoint.theta) * 0.1 + FocusPre.y;
			Focus.z = ((GLdouble)y - Click.y) * sin(vpoint.theta) * sin(vpoint.phi) * 0.1 + FocusPre.z;
			break;
		}
		if(MOUSE_MODE!=NO_MODE && movie.mode!=START) Update();
		break;
	case WM_NCMOUSEMOVE:
		MOUSE_MODE = NO_MODE;
		break;
	}
}


////////////////////////////////////////////////////////////////////////////////
/// 特殊関数
// 強制終了関数
void DRAWPS2::CloseWindow(char *str)
{
	if(str==NULL){
		SendMessage(hWnd[event.wnumber], WM_CLOSE, 0, 0);
	}
	else{
		MessageBox(hWnd[event.wnumber], str, "終了通知", MB_OK | MB_ICONINFORMATION);
		SendMessage(hWnd[event.wnumber], WM_CLOSE, 0, 0);
	}
}


