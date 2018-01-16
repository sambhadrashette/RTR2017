#define _CRT_SECURE_NO_WARNINGS
#include<windows.h>
#include<gl/GL.h>
#include<gl/GLU.h>
#include<vector>

#define WIN_WIDTH	800
#define WIN_HEIGHT	600

#define NR_POINT_COORD		3
#define NR_TEXTURE_COORD	2
#define NR_NORMAL_COORD		3 
#define NR_FACE_TOKENS		3

float angle = 0.0f;

GLUquadric *quadric = NULL;

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
bool gLightEnabled = false;

GLboolean gbPyramid = GL_TRUE;
GLboolean gbCube = GL_FALSE;
GLboolean gbSphere = GL_FALSE;

std::vector<std::vector<float>>   g_vertices;
std::vector<std::vector<float>>   g_textures;
std::vector<std::vector<float>>   g_normals;
FILE* g_fp_meshfile = NULL;

std::vector<std::vector<int>> g_face_triangle, g_face_texture, g_face_normal;

char line[256];

GLfloat red_light_ambient[] = { 0.5f, 0.5f, 0.5f, 1.0f };
GLfloat red_light_diffuse[] = { 1.0f, 0.0f, 0.0f, 0.0f };
GLfloat red_light_specular[] = { 1.0f, 0.0f, 0.0f, 0.0f };
GLfloat red_light_position[] = { 2.0f, 1.0f, 1.0f, 0.0f };

GLfloat blue_light_ambient[] = { 0.5f, 0.5f, 0.5f, 1.0f };
GLfloat blue_light_diffuse[] = { 0.0f, 0.0f, 1.0f, 0.0f };
GLfloat blue_light_specular[] = { 0.0f, 0.0f, 1.0f, 0.0f };
GLfloat blue_light_position[] = { -2.0f, 1.0f, 1.0f, 0.0f };

GLfloat material_ambient[] = { 0.0f, 0.0f, 0.0f, 0.0f };
GLfloat material_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat material_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat material_shininess[] = { 50.0f, 50.0f, 50.0f, 50.0f };


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
		TEXT("RollNo 74 | Sachin Bhadrashette | Obj Loading | Monkey Head "),
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
	void LoadMeshdata();
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
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glShadeModel(GL_SMOOTH);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glLightfv(GL_LIGHT0, GL_AMBIENT, red_light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, red_light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, red_light_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, red_light_position);
	glEnable(GL_LIGHT0);

	glLightfv(GL_LIGHT1, GL_AMBIENT, blue_light_ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, blue_light_diffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, blue_light_specular);
	glLightfv(GL_LIGHT1, GL_POSITION, blue_light_position);
	glEnable(GL_LIGHT1);

	/*glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, material_shininess*/
	glEnable(GL_LIGHTING);

	LoadMeshdata();

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

		default:
			break;
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
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, -5.0f);
	glRotatef(angle, 0.0f, 1.0f, 0.0f);
	glScalef(1.5f, 1.5f, 1.5f);
	glFrontFace(GL_CCW);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	for (int i = 0; i != g_face_triangle.size(); i++) {
		glBegin(GL_TRIANGLES);

		for (int j = 0; j != g_face_triangle[i].size(); j++) {
			int vi = g_face_triangle[i][j] - 1;
			glVertex3f(g_vertices[vi][0], g_vertices[vi][1], g_vertices[vi][2]);
		}
		glEnd();
	}


	SwapBuffers(ghdc);
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
	angle = angle + 0.9f;
	if (angle >= 360.0f)
		angle = 0.0f;
}

void LoadMeshdata(void) {
	void uninitialise(void);
	g_fp_meshfile = fopen("MonkeyHead.OBJ", "r");

	if (!g_fp_meshfile) {
		uninitialize();
	}


	char *sep_space = " ";
	char *sep_fslash = "/";
	char *firstToken = NULL;

	char *token = NULL;

	char *face_tokens[3];

	int nr_tokes;

	char *token_vertex_index = NULL;
	char *token_texture_index = NULL;
	char *token_normal_index = NULL;

	while (fgets(line, 256, g_fp_meshfile) != NULL) {
	
		firstToken = strtok(line, sep_space);

		if (strcmp(firstToken, "v") == 0) {
			std::vector<float> vec_point_coord(NR_POINT_COORD);

			for (int i = 0; i != NR_POINT_COORD; i++) {
				vec_point_coord[i] = atof(strtok(NULL, sep_space));
			}
			g_vertices.push_back(vec_point_coord);
		}
		else if (strcmp(firstToken, "vt") == 0) {

			std::vector<float> vec_texture_coord(NR_TEXTURE_COORD);

			for (int i = 0; i != NR_TEXTURE_COORD; i++) {
				vec_texture_coord[i] = atof(strtok(NULL, sep_space));
			}
			g_textures.push_back(vec_texture_coord);
		
		}
		else if (strcmp(firstToken, "vn") == 0) {
			std::vector<float> vec_normal_coord(NR_NORMAL_COORD);

			for (int i = 0; i != NR_NORMAL_COORD; i++) {
				vec_normal_coord[i] = atof(strtok(NULL, sep_space));
			}
			g_normals.push_back(vec_normal_coord);
		}
		else if (strcmp(firstToken, "f") == 0) {
			std::vector<int> triangle_vertices_indices(3), triangle_texture_indices(3), triangle_normal_indices(3);

			memset((void*)face_tokens, 0, NR_FACE_TOKENS);

			nr_tokes = 0;
			while (token = strtok(NULL, sep_space))
			{
				if (strlen(token) < 3) {
					break;
				}
				face_tokens[nr_tokes] = token;
				nr_tokes++;
			}

			for (int i = 0; i != NR_FACE_TOKENS; i++) {
				token_vertex_index = strtok(face_tokens[i], sep_fslash);
				token_texture_index = strtok(NULL, sep_fslash);
				token_normal_index = strtok(NULL, sep_fslash);

				triangle_vertices_indices[i] = atoi(token_vertex_index);
				triangle_texture_indices[i] = atoi(token_texture_index);
				triangle_normal_indices[i] = atoi(token_normal_index);

			}
			g_face_triangle.push_back(triangle_vertices_indices);
			g_face_texture.push_back(triangle_texture_indices);
			g_face_normal.push_back(triangle_normal_indices);
			
		}
		memset((void*)line, (int)"\0", 256);
	}

	fclose(g_fp_meshfile);
	g_fp_meshfile = NULL;


}