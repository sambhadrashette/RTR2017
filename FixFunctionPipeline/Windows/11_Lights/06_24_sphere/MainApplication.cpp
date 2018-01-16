#include<windows.h>
#include<gl/GL.h>
#include<gl/GLU.h>

#define WIN_WIDTH 900
#define WIN_HEIGHT 900

#pragma comment(lib,"glu32.lib")
#pragma comment (lib,"opengl32.lib")

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

HWND ghwnd = NULL;
HDC ghdc = NULL;
HGLRC ghrc = NULL;

float angleLight = 0.0f;

DWORD dwStyle;
WINDOWPLACEMENT wPrev = { sizeof(WINDOWPLACEMENT) };

GLUquadric *quadric = NULL;

bool gbActiveWindow = false;
bool gbEscapeKeyPressed = false;
bool gbFullScreen = false;
bool gLightEnabled = false;

GLfloat light_ambient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat light_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat light_position[] = { 0.0f, 0.0f, 1.0f, 1.0f };
GLfloat light_model_ambient[] = {0.2f, 0.2f, 0.2f, 0.0f};
GLfloat light_model_local_viewer = {0.0f};

GLfloat material_ambient11[] = { 0.0215f, 0.1745f, 0.0215f, 1.0f };
GLfloat material_diffuse11[] = { 0.07568f, 0.61424f, 0.07568f, 1.0f };
GLfloat material_specular11[] = { 0.633f, 0.727811f, 0.633f, 1.0f };
GLfloat material_shininess11 =  0.6f * 128;

GLfloat material_ambient21[] = { 0.135f, 0.2225f, 0.1575f, 1.0f };
GLfloat material_diffuse21[] = { 0.54f, 0.89f, 0.63f, 1.0f };
GLfloat material_specular21[] = { 0.316228f, 0.316228f, 0.316228f, 1.0f };
GLfloat material_shininess21 = 0.1f * 128;

GLfloat material_ambient31[] = { 0.05375f, 0.05f, 0.06625f, 1.0f };
GLfloat material_specular31[] = { 0.332741f, 0.328634f, 0.346435f, 1.0f };
GLfloat material_diffuse31[] = { 0.18275f, 0.17f, 0.22525f, 1.0f };
GLfloat material_shininess31 = 0.3f * 128;

GLfloat material_ambient41[] = { 0.25f, 0.20725f, 0.20725f, 1.0f };
GLfloat material_specular41[] = { 0.296648f, 0.296648f, 0.296648f, 1.0f };
GLfloat material_diffuse41[] = { 1.0f, 0.829f, 0.829f, 1.0f };
GLfloat material_shininess41 = 0.088f * 128;

GLfloat material_ambient51[] = { 0.1745f, 0.01175f, 0.01175f, 1.0f };
GLfloat material_specular51[] = { 0.727811f, 0.626959f, 0.626959f, 1.0f };
GLfloat material_diffuse51[] = { 0.61424f, 0.04136f, 0.04136f, 1.0f };
GLfloat material_shininess51 = 0.6f * 128;

GLfloat material_ambient61[] = { 1.0f, 0.18725f, 0.1745f, 1.0f };
GLfloat material_specular61[] = { 0.297254f, 0.30829f, 0.306678f, 1.0f };
GLfloat material_diffuse61[] = { 0.396f, 0.74151f, 0.69102f, 1.0f };
GLfloat material_shininess61 = 0.1f * 128;

GLfloat material_ambient12[] = { 0.329412f, 0.223529f, 0.027451f, 1.0f };
GLfloat material_specular12[] = { 0.992157f, 0.941176f, 0.807843f, 1.0f };
GLfloat material_diffuse12[] = { 0.780392f, 0.568627f, 0.113725f, 1.0f };
GLfloat material_shininess12 = 0.21794872f * 128;

GLfloat material_ambient22[] = { 0.2125f, 0.1275f, 0.054f, 1.0f };
GLfloat material_specular22[] = { 0.393548f, 0.271906f, 0.166721f, 1.0f };
GLfloat material_diffuse22[] = { 0.714f, 0.4284f, 0.18144f, 1.0f };
GLfloat material_shininess22 = 0.2f * 128;

GLfloat material_ambient32[] = { 0.25f, 0.25f, 0.25f, 1.0f };
GLfloat material_specular32[] = { 0.774597f, 0.774597f, 0.774597f, 1.0f };
GLfloat material_diffuse32[] = { 0.4f, 0.4f, 0.4f, 1.0f };
GLfloat material_shininess32 = 0.6f * 128;

GLfloat material_ambient42[] = { 0.19125f, 0.0735f, 0.0225f, 1.0f };
GLfloat material_specular42[] = { 0.256777f, 0.137622f, 0.086014f, 1.0f };
GLfloat material_diffuse42[] = { 0.7038f, 0.27048f, 0.0828f, 1.0f };
GLfloat material_shininess42 = 0.1f * 128;

GLfloat material_ambient52[] = { 0.24725f, 0.1995f, 0.0745f, 1.0f };
GLfloat material_specular52[] = { 0.628281f, 0.555802f, 0.366065f, 1.0f };
GLfloat material_diffuse52[] = { 0.75164f, 0.60648f, 0.22648f, 1.0f };
GLfloat material_shininess52 = 0.4f * 128;

GLfloat material_ambient62[] = { 0.19225f, 0.19225f, 0.19225f, 1.0f };
GLfloat material_specular62[] = { 0.508273f, 0.508273f, 0.508273f, 1.0f };
GLfloat material_diffuse62[] = { 0.50754f, 0.50754f, 0.50754f, 1.0f };
GLfloat material_shininess62 = 0.4f * 128;

GLfloat material_ambient13[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat material_specular13[] = { 0.50f, 0.50f, 0.50f, 1.0f };
GLfloat material_diffuse13[] = { 0.01f, 0.01f, 0.01f, 1.0f };
GLfloat material_shininess13 = 0.25 * 128;

GLfloat material_ambient23[] = { 0.0f, 0.1f, 0.06f, 1.0f };
GLfloat material_specular23[] = { 0.50196078f, 0.50196078f, 0.50196078f, 1.0f };
GLfloat material_diffuse23[] = { 0.0f, 0.50980392f, 0.50980392f, 1.0f };
GLfloat material_shininess23 = 0.25f * 128;

GLfloat material_ambient33[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat material_specular33[] = { 0.45, 0.55f, 0.45f, 1.0f };
GLfloat material_diffuse33[] = { 0.1f, 0.35f, 0.1f, 1.0f };
GLfloat material_shininess33 = 0.25f * 128;

GLfloat material_ambient43[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat material_specular43[] = { 0.7f, 0.6f, 0.6f, 1.0f };
GLfloat material_diffuse43[] = { 0.5f, 0.0f, 0.0f, 1.0f };
GLfloat material_shininess43 = 0.25f * 128;

GLfloat material_ambient53[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat material_specular53[] = { 0.70f, 0.70f, 0.70f, 1.0f };
GLfloat material_diffuse53[] = { 0.55f, 0.55f, 0.55f, 1.0f };
GLfloat material_shininess53 = 0.25f * 128;

GLfloat material_ambient63[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat material_specular63[] = { 0.60f, 0.60f, 0.50f, 1.0f };
GLfloat material_diffuse63[] = { 0.5f, 0.5f, 0.0f, 1.0f };
GLfloat material_shininess63 = 0.25 * 128;

GLfloat material_ambient14[] = { 0.02f, 0.02f, 0.02f, 1.0f };
GLfloat material_specular14[] = { 0.4f, 0.4f, 0.4f, 1.0f };
GLfloat material_diffuse14[] = { 0.01f, 0.01f, 0.01f, 1.0f };
GLfloat material_shininess14 = 0.078125f * 128;

GLfloat material_ambient24[] = { 0.0f, 0.05f, 0.05f, 1.0f };
GLfloat material_specular24[] = { 0.04f, 0.7f, 0.7f, 1.0f };
GLfloat material_diffuse24[] = { 0.4f, 0.5f, 0.5f, 1.0f };
GLfloat material_shininess24 = 0.078125 * 128;

GLfloat material_ambient34[] = { 0.0f, 0.05, 0.0f, 1.0f };
GLfloat material_specular34[] = { 0.04f, 0.7f, 0.04f, 1.0f };
GLfloat material_diffuse34[] = { 0.4f, 0.5f, 0.4f, 1.0f };
GLfloat material_shininess34 = 0.078125f * 128;

GLfloat material_ambient44[] = { 0.05f, 0.0f, 0.0f, 1.0f };
GLfloat material_specular44[] = { 0.7f, 0.04f, 0.04f, 1.0f };
GLfloat material_diffuse44[] = { 0.5f, 0.4f, 0.4f, 1.0f };
GLfloat material_shininess44 = 0.078125f * 128;

GLfloat material_ambient54[] = { 0.05f, 0.05f, 0.05f, 1.0f };
GLfloat material_specular54[] = { 0.7f, 0.7f, 0.7f, 1.0f };
GLfloat material_diffuse54[] = { 0.5f, 0.5f, 0.5f, 1.0f };
GLfloat material_shininess54 = 0.078125f * 128;

GLfloat material_ambient64[] = { 0.05f, 0.05f, 0.0f, 1.0f };
GLfloat material_specular64[] = { 0.7f, 0.7f, 0.04f, 1.0f };
GLfloat material_diffuse64[] = { 0.5f, 0.5f, 0.4f, 1.0f };
GLfloat material_shininess64 = 0.078125f * 128;

GLboolean xKeyPressed = GL_FALSE;
GLboolean yKeyPressed = GL_FALSE;
GLboolean zKeyPressed = GL_FALSE;



int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow) {
	void display(void);
	void initialize(void);
	void uninitialize(void);
	void update(void);
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
		TEXT("RollNo 74 | Sachin Bhadrashette | Lights | 24 spheres "),
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
				update();
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

	glClearColor(0.25f, 0.25f, 0.25f, 0.25f);
	glClearDepth(1.f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glShadeModel(GL_SMOOTH);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glEnable(GL_AUTO_NORMAL);
	glEnable(GL_NORMALIZE);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, light_model_ambient);
	glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER, light_model_local_viewer);

	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	
	glEnable(GL_LIGHT0);
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
		case 0x4C:
			if (gLightEnabled == false) {
				glEnable(GL_LIGHTING);
				gLightEnabled = true;
			}
			else {
				glDisable(GL_LIGHTING);
				gLightEnabled = false;
			}
		case 0x58:
			xKeyPressed = GL_TRUE;
			yKeyPressed = GL_FALSE;
			zKeyPressed = GL_FALSE;
			
			break;
		case 0x59:
			xKeyPressed = GL_FALSE;
			yKeyPressed = GL_TRUE;
			zKeyPressed = GL_FALSE;
			break;
		case 0x5A:
			xKeyPressed = GL_FALSE;
			yKeyPressed = GL_FALSE;
			zKeyPressed = GL_TRUE;
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
	void drawSphere();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	glPushMatrix();
	if (xKeyPressed == GL_TRUE)
	{
		glRotatef(angleLight, 1.0f, 0.0f, 0.0f);
		light_position[0] = angleLight;
	}
	else if (yKeyPressed == GL_TRUE)
	{
		glRotatef(angleLight, 0.0f, 1.0f, 0.0f);
		light_position[1] = angleLight;
	}
	else if (zKeyPressed == GL_TRUE)
	{
		glRotatef(angleLight, 0.0f, 0.0f, 1.0f);
		light_position[2] = angleLight;
	}
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-2.5f, 2.4f, -8.0f);
	glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient11);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse11);
	glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular11);
	glMaterialf(GL_FRONT, GL_SHININESS, material_shininess11);
	drawSphere();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-1.00f, 2.4f, -8.0f);
	glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient12);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse12);
	glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular12);
	glMaterialf(GL_FRONT, GL_SHININESS, material_shininess12);
	drawSphere();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(1.00f, 2.4f, -8.0f);
	glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient13);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse13);
	glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular13);
	glMaterialf(GL_FRONT, GL_SHININESS, material_shininess13);
	drawSphere();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(2.5f, 2.4f, -8.0f);
	glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient14);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse14);
	glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular14);
	glMaterialf(GL_FRONT, GL_SHININESS, material_shininess14);
	drawSphere();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-2.5f, 1.5f, -8.0f);
	glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient21);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse21);
	glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular21);
	glMaterialf(GL_FRONT, GL_SHININESS, material_shininess21);
	drawSphere();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-1.00f, 1.5f, -8.0f);
	glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient22);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse22);
	glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular22);
	glMaterialf(GL_FRONT, GL_SHININESS, material_shininess22);
	drawSphere();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(1.00f, 1.5f, -8.0f);
	glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient23);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse23);
	glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular23);
	glMaterialf(GL_FRONT, GL_SHININESS, material_shininess23);
	drawSphere();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(2.5f, 1.5f, -8.0f);
	glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient24);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse24);
	glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular24);
	glMaterialf(GL_FRONT, GL_SHININESS, material_shininess24);
	drawSphere();
	glPopMatrix();


	glPushMatrix();
	glTranslatef(-2.5f, 0.5f, -8.0f);
	glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient31);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse31);
	glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular31);
	glMaterialf(GL_FRONT, GL_SHININESS, material_shininess31);
	drawSphere();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-1.00f, 0.5f, -8.0f);
	glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient32);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse32);
	glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular32);
	glMaterialf(GL_FRONT, GL_SHININESS, material_shininess32);
	drawSphere();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(1.00f, 0.5f, -8.0f);
	glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient33);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse33);
	glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular33);
	glMaterialf(GL_FRONT, GL_SHININESS, material_shininess33);
	drawSphere();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(2.5f, 0.5f, -8.0f);
	glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient34);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse34);
	glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular34);
	glMaterialf(GL_FRONT, GL_SHININESS, material_shininess34);
	drawSphere();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-2.5f, -0.5f, -8.0f);
	glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient41);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse41);
	glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular41);
	glMaterialf(GL_FRONT, GL_SHININESS, material_shininess41);
	drawSphere();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-1.00f, -0.5f, -8.0f);
	glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient42);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse42);
	glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular42);
	glMaterialf(GL_FRONT, GL_SHININESS, material_shininess42);
	drawSphere();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(1.00f, -0.5f, -8.0f);
	glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient43);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse43);
	glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular43);
	glMaterialf(GL_FRONT, GL_SHININESS, material_shininess43);
	drawSphere();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(2.5f, -0.5f, -8.0f);
	glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient44);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse44);
	glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular44);
	glMaterialf(GL_FRONT, GL_SHININESS, material_shininess44);
	drawSphere();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-2.5f, -1.5f, -8.0f);
	glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient51);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse51);
	glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular51);
	glMaterialf(GL_FRONT, GL_SHININESS, material_shininess51);
	drawSphere();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-1.00f, -1.5f, -8.0f);
	glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient52);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse52);
	glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular52);
	glMaterialf(GL_FRONT, GL_SHININESS, material_shininess52);
	drawSphere();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(1.00f, -1.5f, -8.0f);
	glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient53);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse53);
	glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular53);
	glMaterialf(GL_FRONT, GL_SHININESS, material_shininess53);
	drawSphere();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(2.5f, -1.5f, -8.0f);
	glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient54);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse54);
	glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular54);
	glMaterialf(GL_FRONT, GL_SHININESS, material_shininess54);
	drawSphere();
	glPopMatrix();
	glPushMatrix();
	glTranslatef(-2.5f, -2.4f, -8.0f);
	glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient61);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse61);
	glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular61);
	glMaterialf(GL_FRONT, GL_SHININESS, material_shininess61);
	drawSphere();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-1.00f, -2.4f, -8.0f);
	glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient62);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse62);
	glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular62);
	glMaterialf(GL_FRONT, GL_SHININESS, material_shininess62);
	drawSphere();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(1.00f, -2.4f, -8.0f);
	glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient63);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse63);
	glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular63);
	glMaterialf(GL_FRONT, GL_SHININESS, material_shininess63);
	drawSphere();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(2.5f, -2.4f, -8.0f);
	glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient64);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse64);
	glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular64);
	glMaterialf(GL_FRONT, GL_SHININESS, material_shininess64);
	drawSphere();
	glPopMatrix();

	SwapBuffers(ghdc);
}

void drawSphere() {
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	quadric = gluNewQuadric();
	gluSphere(quadric, 0.40, 30, 30);
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

void update(void)
{
	angleLight = angleLight + 0.10f;
	if (angleLight >= 360.0f)
		angleLight = 1.0f;
}






