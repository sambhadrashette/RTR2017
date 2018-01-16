#include<windows.h>
#include<gl/GL.h>
#include<gl/GLU.h>
#include<math.h>

#define WIN_WIDTH 800
#define WIN_HEIGHT 600
#define PI 3.14159

#pragma comment(lib,"glu32.lib")
#pragma comment (lib,"opengl32.lib")

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

HWND ghwnd = NULL;
HDC ghdc = NULL;
HGLRC ghrc = NULL;

float angleTrangle = 0.0f;
float angleRect = 360.0f;

DWORD dwStyle;
WINDOWPLACEMENT wPrev = { sizeof(WINDOWPLACEMENT) };

GLfloat identityMatrix[16];
GLfloat translationMatrix[16];
GLfloat scaleMatrix[16];
GLfloat x_RotationMatrix[16];
GLfloat y_RotationMatrix[16];
GLfloat z_RotationMatrix[16];



bool gbActiveWindow = false;
bool gbEscapeKeyPressed = false;
bool gbFullScreen = false;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow) {
	void display(void);
	void initialize(void);
	void uninitialize(void);
	void rotate(void);

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
		TEXT("RollNo 74 | Sachin Bhadrashette | Multicoloured Cube "),
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
				display();
				rotate();
			}
		}
	}

	uninitialize();
	return ((int)msg.wParam);
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
	case WM_LBUTTONDOWN:
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		break;
	}
	return (DefWindowProc(hwnd, iMsg, wParam, lParam));
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
	void initializeIdentityMatrix(void);
	void initializeTranslationMatrix(GLfloat x, GLfloat y, GLfloat z);
	void initializeScaleMatrix(GLfloat x, GLfloat y, GLfloat z);

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

	initializeIdentityMatrix();
	initializeTranslationMatrix(0.0f, 0.0f, -6.0f);
	initializeScaleMatrix(0.75f, 0.75f, 0.75f);
	
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glShadeModel(GL_SMOOTH);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	resize(WIN_WIDTH, WIN_HEIGHT);
}

void display() {
	void drawCube();
	void initializeRotationMatrix(GLfloat angle, GLfloat x, GLfloat y, GLfloat z);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	//glLoadIdentity();
	glLoadMatrixf(identityMatrix);

	//glTranslatef(0.0f, 0.0f, -6.0f);
	glMultMatrixf(translationMatrix);

	//glScalef(0.75f, 0.75f, 0.75f);
	glMultMatrixf(scaleMatrix);

	//glRotatef(angleRect, 1.0f, 1.0f, 1.0f);

	initializeRotationMatrix(angleRect, 1.0f, 1.0f, 1.0f);
	glMultMatrixf(x_RotationMatrix);
	glMultMatrixf(y_RotationMatrix);
	glMultMatrixf(z_RotationMatrix);

	drawCube();
	SwapBuffers(ghdc);
	
}

void drawCube() {
	glBegin(GL_QUADS);
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(-1.0f, 1.0f, 1.0f);
	glVertex3f(-1.0f, -1.0f, 1.0f);
	glVertex3f(1.0f, -1.0f, 1.0f);
	glVertex3f(1.0f, 1.0f, 1.0f);
	
	
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(-1.0f, 1.0f, -1.0f);
	glVertex3f(-1.0f, -1.0f, -1.0f);
	glVertex3f(1.0f, -1.0f, -1.0f);
	glVertex3f(1.0f, 1.0f, -1.0f);
	

	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(1.0f, 1.0f, -1.0f);
	glVertex3f(1.0f, -1.0f, -1.0f);
	glVertex3f(1.0f, -1.0f, 1.0f);
	glVertex3f(1.0f, 1.0f, 1.0f);

	glColor3f(1.0f, 0.0f, 1.0f);
	glVertex3f(-1.0f, 1.0f, -1.0f);
	glVertex3f(-1.0f, -1.0f, -1.0f);
	glVertex3f(-1.0f, -1.0f, 1.0f);
	glVertex3f(-1.0f, 1.0f, 1.0f);

	glColor3f(1.0f, 1.0f, 0.0f);
	glVertex3f(1.0f, 1.0f, -1.0f);
	glVertex3f(-1.0f, 1.0f, -1.0f);
	glVertex3f(-1.0f, 1.0f, 1.0f);
	glVertex3f(1.0f, 1.0f, 1.0f);

	glColor3f(0.0f, 1.0f, 1.0f);
	glVertex3f(1.0f, -1.0f, -1.0f);
	glVertex3f(-1.0f, -1.0f, -1.0f);
	glVertex3f(-1.0f, -1.0f, 1.0f);
	glVertex3f(1.0f, -1.0f, 1.0f);
	
	glEnd();

}

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

void rotate(void)
{
	
	angleRect = angleRect - 0.09f;
	if (angleRect <= 0.0f)
		angleRect = 360.0f;
}

void initializeIdentityMatrix(void) {
	identityMatrix[0] = 1.0f;
	identityMatrix[1] = 0.0f;
	identityMatrix[2] = 0.0f;
	identityMatrix[3] = 0.0f;
	identityMatrix[4] = 0.0f;
	identityMatrix[5] = 1.0f;
	identityMatrix[6] = 0.0f;
	identityMatrix[7] = 0.0f;
	identityMatrix[8] = 0.0f;
	identityMatrix[9] = 0.0f;
	identityMatrix[10] = 1.0f;
	identityMatrix[11] = 0.0f;
	identityMatrix[12] = 0.0f;
	identityMatrix[13] = 0.0f;
	identityMatrix[14] = 0.0f;
	identityMatrix[15] = 1.0f;
}

void initializeTranslationMatrix(GLfloat x, GLfloat y, GLfloat z ) {
	translationMatrix[0] = 1.0f;
	translationMatrix[1] = 0.0f;
	translationMatrix[2] = 0.0f;
	translationMatrix[3] = 0.0f;
	translationMatrix[4] = 0.0f;
	translationMatrix[5] = 1.0f;
	translationMatrix[6] = 0.0f;
	translationMatrix[7] = 0.0f;
	translationMatrix[8] = 0.0f;
	translationMatrix[9] = 0.0f;
	translationMatrix[10] = 1.0f;
	translationMatrix[11] = 0.0f;
	translationMatrix[12] = x;
	translationMatrix[13] = y;
	translationMatrix[14] = z;
	translationMatrix[15] = 1.0f;
}

void initializeScaleMatrix(GLfloat x, GLfloat y, GLfloat z) {
	scaleMatrix[0] = x;
	scaleMatrix[1] = 0.0f;
	scaleMatrix[2] = 0.0f;
	scaleMatrix[3] = 0.0f;
	scaleMatrix[4] = 0.0f;
	scaleMatrix[5] = y;
	scaleMatrix[6] = 0.0f;
	scaleMatrix[7] = 0.0f;
	scaleMatrix[8] = 0.0f;
	scaleMatrix[9] = 0.0f;
	scaleMatrix[10] = z;
	scaleMatrix[11] = 0.0f;
	scaleMatrix[12] = 0.0f;
	scaleMatrix[13] = 0.0f;
	scaleMatrix[14] = 0.0f;
	scaleMatrix[15] = 1.0f;
}

void initializeRotationMatrix(GLfloat angle, GLfloat x, GLfloat y, GLfloat z) {
	GLfloat angleRad = angle*(PI/ 180);
	
	//x axis rotation
	if (x == 1.0f) {
		x_RotationMatrix[0] = 1.0f;
		x_RotationMatrix[1] = 0.0f;
		x_RotationMatrix[2] = 0.0f;
		x_RotationMatrix[3] = 0.0f;
		x_RotationMatrix[4] = 0.0f;
		x_RotationMatrix[5] = cos(angleRad);
		x_RotationMatrix[6] = sin(angleRad);
		x_RotationMatrix[7] = 0.0f;
		x_RotationMatrix[8] = 0.0f;
		x_RotationMatrix[9] = -sin(angleRad);
		x_RotationMatrix[10] = cos(angleRad);
		x_RotationMatrix[11] = 0.0f;
		x_RotationMatrix[12] = 0.0f;
		x_RotationMatrix[13] = 0.0f;
		x_RotationMatrix[14] = 0.0f;
		x_RotationMatrix[15] = 1.0f;
	}

	//y axis rotation
	if (y == 1.0f) {
		y_RotationMatrix[0] = cos(angleRad);
		y_RotationMatrix[1] = 0.0f;
		y_RotationMatrix[2] = -sin(angleRad);
		y_RotationMatrix[3] = 0.0f;
		y_RotationMatrix[4] = 0.0f;
		y_RotationMatrix[5] = 1.0f;
		y_RotationMatrix[6] = 0.0f;
		y_RotationMatrix[7] = 0.0f;
		y_RotationMatrix[8] = sin(angleRad);
		y_RotationMatrix[9] = 0.0f;
		y_RotationMatrix[10] = cos(angleRad);
		y_RotationMatrix[11] = 0.0f;
		y_RotationMatrix[12] = 0.0f;
		y_RotationMatrix[13] = 0.0f;
		y_RotationMatrix[14] = 0.0f;
		y_RotationMatrix[15] = 1.0f;
	}

	//z axis rotation
	if (z == 1.0f) {
		z_RotationMatrix[0] = cos(angleRad);
		z_RotationMatrix[1] = sin(angleRad);
		z_RotationMatrix[2] = 0.0f;
		z_RotationMatrix[3] = 0.0f;
		z_RotationMatrix[4] = -sin(angleRad);
		z_RotationMatrix[5] = cos(angleRad);
		z_RotationMatrix[6] = 0.0f;
		z_RotationMatrix[7] = 0.0f;
		z_RotationMatrix[8] = 0.0f;
		z_RotationMatrix[9] = 0.0f;
		z_RotationMatrix[10] = 1.0f;
		z_RotationMatrix[11] = 0.0f;
		z_RotationMatrix[12] = 0.0f;
		z_RotationMatrix[13] = 0.0f;
		z_RotationMatrix[14] = 0.0f;
		z_RotationMatrix[15] = 1.0f;
	}
}



