#include<windows.h>
#include<gl/GL.h>
#include<gl/GLU.h>
#include<math.h>

#define PI  3.14159
#define WIN_WIDTH 800
#define WIN_HEIGHT 600

#pragma comment(lib,"glu32.lib")
#pragma comment (lib,"opengl32.lib")

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

HWND ghwnd = NULL;
HDC ghdc = NULL;
HGLRC ghrc = NULL;


DWORD dwStyle;
WINDOWPLACEMENT wPrev = { sizeof(WINDOWPLACEMENT) };


bool gbActiveWindow = false;
bool gbEscapeKeyPressed = false;
bool gbFullScreen = false;


GLfloat animationSpeed = 0.0005f;
GLfloat v1x_axis = -5.0f, v2x_axis = -5.0f; // for horizontal line of A
GLfloat I1_xAxis = -3.5f, N_yAxis = 3.5f, I2_yAxis = -3.5f, A_xAxis = 4.5f; // for I, N, A animation start position
GLfloat v1_red = 0.0f, v1_green = 0.0f, v2_red = 0.0f, v2_green = 0.0f; // for fade in effect of D


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow) {
	void display(void);
	void initialize(void);
	void uninitialize(void);
	void animate();
	WNDCLASSEX wndClass;
	HWND hwnd;
	MSG msg;
	TCHAR szClassName[] = TEXT("VRedLine");
	bool bDone = false;
	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = hInstance;
	wndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wndClass.lpfnWndProc = WndProc;
	wndClass.lpszClassName = szClassName;
	wndClass.lpszMenuName = NULL;

	RegisterClassEx(&wndClass);

	hwnd = CreateWindowEx(WS_EX_APPWINDOW,
		szClassName,
		TEXT("RollNo 74 | Sachin Bhadrashette | Animated India "),
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE,
		100,
		100,
		WIN_WIDTH,
		WIN_HEIGHT,
		NULL,
		NULL,
		hInstance,
		NULL);

	ghwnd = hwnd;

	initialize();

	ShowWindow(hwnd, SW_SHOW);
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);

	while (bDone == false) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				bDone = true;
			else {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else {
			if (gbActiveWindow == true)
			{
				if (gbEscapeKeyPressed == true) {
					bDone = true;
				}
				animate();
				display();

			}
		}
	}

	uninitialize();
	return ((int)msg.wParam);
}

void ToggleFullScreen() {
	MONITORINFO mi;
	dwStyle = GetWindowLong(ghwnd, GWL_STYLE);
	if (gbFullScreen == FALSE) {
		if (dwStyle & WS_OVERLAPPEDWINDOW) {
			wPrev.length = sizeof(WINDOWPLACEMENT);
			BOOL bIsWindowPlacement = GetWindowPlacement(ghwnd, &wPrev);
			HMONITOR hMonitor = MonitorFromWindow(ghwnd, MONITORINFOF_PRIMARY);
			mi.cbSize = sizeof(MONITORINFO);
			BOOL bIsMonitorInfo = GetMonitorInfo(hMonitor, &mi);

			if (bIsWindowPlacement == TRUE && bIsMonitorInfo == TRUE) {
				SetWindowLong(ghwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
				SetWindowPos(ghwnd, HWND_TOP,
					mi.rcMonitor.left,
					mi.rcMonitor.top,
					mi.rcMonitor.right - mi.rcMonitor.left,
					mi.rcMonitor.bottom - mi.rcMonitor.top,
					SWP_NOZORDER | SWP_FRAMECHANGED);
				ShowCursor(false);
			}
		}
	}
	else {
		SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd, &wPrev);
		SetWindowPos(ghwnd, HWND_TOP,
			0,
			0,
			0,
			0,
			SWP_NOZORDER | SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER);
		ShowCursor(true);
	}
}

void initialize() {
	void resize(int, int);
	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormatIndex;

	ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));

	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cRedBits = 8;
	pfd.cGreenBits = 8;
	pfd.cBlueBits = 8;
	pfd.cAlphaBits = 8;
	pfd.cDepthBits = 32;

	ghdc = GetDC(ghwnd);
	iPixelFormatIndex = ChoosePixelFormat(ghdc, &pfd);

	if (iPixelFormatIndex == 0) {
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	if (SetPixelFormat(ghdc, iPixelFormatIndex, &pfd) == FALSE) {
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	ghrc = wglCreateContext(ghdc);
	if (ghrc == NULL) {
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}
	if (wglMakeCurrent(ghdc, ghrc) == FALSE) {
		wglDeleteContext(ghrc);
		ghrc = NULL;
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glShadeModel(GL_SMOOTH);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	resize(WIN_WIDTH, WIN_HEIGHT);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam) {

	void resize(int, int);
	void ToggleFullScreen(void);
	void uninitialize(void);

	switch (iMsg) {
	case WM_ACTIVATE:
		if (HIWORD(wParam) == 0)
			gbActiveWindow = true;
		else
			gbActiveWindow = false;
		break;
	case WM_SIZE:
		resize(LOWORD(lParam), HIWORD(lParam));
		break;
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			gbEscapeKeyPressed = true;
			break;
		case 0x46:
			if (gbFullScreen == false) {
				ToggleFullScreen();
				gbFullScreen = true;
			}
			else {
				ToggleFullScreen();
				gbFullScreen = false;
			}
			break;
		default: break;
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		break;
	}
	return (DefWindowProc(hwnd, iMsg, wParam, lParam));
}



void display() {
	void drawAlphabet_I();
	void drawAlphabet_N();
	void drawAlphabet_D();
	void drawAlphabet_A();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(I1_xAxis, 0.0f, -5.0f);
	glLineWidth(4.0f);
	drawAlphabet_I();
	glLoadIdentity();
	glTranslatef(-1.30f, N_yAxis, -5.0f);
	drawAlphabet_N();
	glLoadIdentity();
	glTranslatef(0.00f, 0.0f, -5.0f);
	drawAlphabet_D();
	glLoadIdentity();
	glTranslatef(0.8f, I2_yAxis, -5.0f);
	drawAlphabet_I();
	glLoadIdentity();
	glTranslatef(A_xAxis, 0.0f, -5.0f);
	drawAlphabet_A();
	
	SwapBuffers(ghdc);
}

void drawAlphabet_I() {
	glBegin(GL_LINES);
	glColor3f(1.0f, (69.0f/255.0f), 0.0f);
	glVertex3f(0.0f, 1.0f, 0.0f);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(0.0f, -1.0f, 0.0f);
	glEnd();
}

void drawAlphabet_N() {
	
	glBegin(GL_LINES);
	glColor3f(1.0f, (69.0f / 255.0f), 0.0f);
	glVertex3f(-0.50f, 1.0f, 0.0f);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(-0.50f, -1.0f, 0.0f);

	glColor3f(1.0f, (69.0f / 255.0f), 0.0f);
	glVertex3f(-0.50f, 1.0f, 0.0f);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(0.50f, -1.0f, 0.0f);


	glColor3f(1.0f, (69.0f / 255.0f), 0.0f);
	glVertex3f(0.50f, 1.0f, 0.0f);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(0.50f, -1.0f, 0.0f);

	glEnd();
	
}

void drawAlphabet_D() {

	glBegin(GL_LINES);
	glColor3f(v1_red, v1_green, 0.0f);
	glVertex3f(-0.60f, 1.0f, 0.0f);
	glVertex3f(0.50f, 1.0f, 0.0f);

	glColor3f(v1_red, v1_green, 0.0f); //(69.0f / 255.0f)
	glVertex3f(-0.50f, 1.0f, 0.0f);
	glColor3f(0.0f, v2_green, 0.0f);
	glVertex3f(-0.50f, -1.0f, 0.0f);

	glColor3f(v1_red, v1_green, 0.0f);
	glVertex3f(0.50f, 1.0f, 0.0f);
	glColor3f(0.0f, v2_green, 0.0f);
	glVertex3f(0.50f, -1.0f, 0.0f);

	glColor3f(0.0f, v2_green, 0.0f);
	glVertex3f(-0.60f, -1.0f, 0.0f);
	glVertex3f(0.50f, -1.0f, 0.0f);

	glEnd();

}

void drawAlphabet_A() {

	glBegin(GL_LINES);
	glColor3f(1.0f, (69.0f / 255.0f), 0.0f);
	glVertex3f(0.0f, 1.0f, 0.0f);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(-0.50f, -1.0f, 0.0f);

	glColor3f(1.0f, (69.0f / 255.0f), 0.0f);
	glVertex3f(0.0f, 1.0f, 0.0f);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(0.50f, -1.0f, 0.0f);

	glColor3f(1.0f, (69.0f / 255.0f), 0.0f);
	glVertex3f(v2x_axis, 0.02f, 0.0f);
	glVertex3f(v1x_axis, 0.02f, 0.0f);

	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(v2x_axis, 0.0f, 0.0f);
	glVertex3f(v1x_axis, 0.0f, 0.0f);

	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(v2x_axis, -0.02f, 0.0f);
	glVertex3f(v1x_axis, -0.02f, 0.0f);

	glEnd();

}

/*void drawCircle() {
	glBegin(GL_POINTS);
	glColor3f(0.0f, 0.0f, 1.0f);
	for (GLfloat angle = 0.0f; angle < 2.0f * PI; angle = angle + 0.001f) {
		glVertex3f(cos(angle)*0.3, sin(angle)*0.3, 0.0f);
	}
	glEnd();
	glBegin(GL_LINES);
	glVertex3f(0.0f, -0.3, 0.0f);
	glVertex3f(0.0f, 0.3, 0.0f);

	glVertex3f(0.1f, -0.3, 0.0f);
	glVertex3f(-0.1f, 0.3, 0.0f);

	glVertex3f(0.2f, -0.24, 0.0f);
	glVertex3f(-0.2f, 0.24, 0.0f);

	glVertex3f(0.21f, -0.18f, 0.0f);
	glVertex3f(-0.21f, 0.18f, 0.0f);

	glVertex3f(0.35f, -0.16f, 0.0f);
	glVertex3f(-0.35f, 0.16f, 0.0f);

	glEnd();
	
}*/

void resize(int width, int height) {
	if (height == 0) {
		height = 1;
	}
	if (width == 0) {
		width = 1;
	}
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
}

void uninitialize(void) {
	if (gbFullScreen == true) {
		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);
		SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd, &wPrev);
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);
	}

	wglMakeCurrent(NULL, NULL);

	wglDeleteContext(ghrc);

	ghrc = NULL;

	ReleaseDC(ghwnd, ghdc);
	ghdc = NULL;

	DestroyWindow(ghwnd);
	ghwnd = NULL;

}

void animate() {
	if (I1_xAxis <= -2.0f) {
		I1_xAxis = I1_xAxis + animationSpeed;  //Animate I
	}
	else if (N_yAxis >= 0.0f) {
		N_yAxis  = N_yAxis  - animationSpeed; //Animate N
	}
	else if (v2_green < 1.0f) { // fade in effect of letter 'D'
		v2_green = v2_green + 0.001f;
		v1_red = v1_red + 0.001f;
		if(v1_green < (69.0f / 255.0f))
			v1_green = v1_green + 0.001f;
	}
	else if (I2_yAxis <= 0.0) {
		I2_yAxis = I2_yAxis + animationSpeed; //Animate I
	}
	else if (A_xAxis >= 1.5) {
		A_xAxis = A_xAxis - animationSpeed; //Animate A
	}
	else if (v1x_axis <= 0.25) {
		v1x_axis = v1x_axis + animationSpeed; //Animate A horizontal line
	}
	else if (v2x_axis <= -0.25) { 
		v2x_axis = v2x_axis + animationSpeed; //Animate A horizontal line
	}

}





