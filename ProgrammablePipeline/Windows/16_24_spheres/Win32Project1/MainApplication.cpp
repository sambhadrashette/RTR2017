#include<windows.h>
#include<GL/glew.h>
#include<gl/GL.h>
#include<math.h>
#include<stdio.h>
#include<stdlib.h>

#include "vmath.h"
#include "Sphere.h"

#define WIN_WIDTH 900
#define WIN_HEIGHT 600

#pragma comment(lib,"glew32.lib")
#pragma comment (lib,"opengl32.lib")
#pragma comment(lib,"Sphere.lib")

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

HWND ghwnd = NULL;
HDC ghdc = NULL;
HGLRC ghrc = NULL;

int keyPressed;

DWORD dwStyle;
WINDOWPLACEMENT wPrev = { sizeof(WINDOWPLACEMENT) };


bool gbActiveWindow = false;
bool gbEscapeKeyPressed = false;
bool gbFullScreen = false;

GLfloat gRedious = 50.0f;
GLfloat gAngle = 0.0f;
GLfloat gXPressed = false;
GLfloat gYPressed = false;
GLfloat gZPressed = false;

GLuint gVertexShaderObject;
GLuint gFragmentShaderObject;
GLuint gShaderProgramObject;

GLuint gVao_sphere;
GLuint gVbo_sphere_position;
GLuint gVbo_sphere_normal;
GLuint gVbo_sphere_element;

GLuint gModelMatrixUniform, gViewMatrixUniform, gProjectionMatrixUniform;
GLuint gLaUniform, gLdUniform, gLsUniform, gLightPositionUniform;
GLuint gKaUniform, gKdUniform, gKsUniform, gMaterialShininessniform;

GLuint gLKeyPressedUniform;

GLfloat light_ambient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat light_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f ,1.0f };
GLfloat light_position[] = { 100.0f, 100.0f, 100.0f, 1.0f };

GLfloat material_ambient[24][4] = { { 0.0215f, 0.1745f, 0.0215f, 1.0f },{ 0.135f, 0.2225f, 0.1575f, 1.0f },{ 0.05375f, 0.05f, 0.06625f, 1.0f },{ 0.25f, 0.20725f, 0.20725f, 1.0f },{ 0.1745f, 0.01175f, 0.01175f, 1.0f },{ 0.1f, 0.18725f, 0.1745f, 1.0f },{ 0.329412f, 0.223529f, 0.027451f, 1.0f },{ 0.2125f, 0.1275f, 0.054f, 1.0f },{ 0.25f, 0.25f, 0.25f, 1.0f },{ 0.19125f, 0.0735f, 0.0225f, 1.0f },{ 0.24725f, 0.1995f, 0.0745f, 1.0f },{ 0.19225f, 0.19225f, 0.19225f, 1.0f },{ 0.0f, 0.0f, 0.0f, 1.0f },{ 0.0f, 0.1f, 0.06f, 1.0f },{ 0.0f, 0.0f, 0.0f, 1.0f },{ 0.0f, 0.0f, 0.0f, 1.0f },{ 0.0f, 0.0f, 0.0f, 1.0f } ,{ 0.0f, 0.0f, 0.0f, 1.0f },{ 0.02f, 0.02f, 0.02f, 1.0f },{ 0.0f, 0.05f, 0.05f, 1.0f },{ 0.0f, 0.05f, 0.0f, 1.0f },{ 0.05f, 0.0f, 0.0f, 1.0f },{ 0.05f, 0.05f, 0.05f, 1.0f },{ 0.05f, 0.05f, 0.0f, 1.0f } };
GLfloat material_diffuse[24][4] = { { 0.07568f, 0.61424f, 0.07568f, 1.0f },{ 0.54f, 0.89f, 0.63f, 1.0f },{ 0.18275f, 0.17f, 0.22525f, 1.0f },{ 1.0f, 0.829f, 0.829f, 1.0f },{ 0.61424f, 0.04136f, 0.04136f, 1.0f },{ 0.396f, 0.74151f, 0.69102f, 1.0f },{ 0.780392f, 0.568627f, 0.113725f, 1.0f },{ 0.714f, 0.4284f, 0.18144f, 1.0f },{ 0.4f, 0.4f, 0.4f, 1.0f },{ 0.7038f, 0.27048f, 0.0828f, 1.0f },{ 0.75164f, 0.60648f, 0.22648f, 1.0f },{ 0.50754f, 0.50754f, 0.50754f, 1.0f },{ 0.01f, 0.01f, 0.01f, 1.0f },{ 0.0f, 0.50980392f, 0.50980392f, 1.0f },{ 0.1f, 0.35f, 0.1f, 1.0f },{ 0.5f, 0.0f, 0.0f, 1.0f },{ 0.55f, 0.55f, 0.55f, 1.0f },{ 0.5f, 0.5f, 0.0f, 1.0f },{ 0.01f, 0.01f, 0.01f, 1.0f },{ 0.4f, 0.5f, 0.5f, 1.0f },{ 0.4f, 0.5f, 0.4f, 1.0f },{ 0.5f, 0.4f, 0.4f, 1.0f },{ 0.5f, 0.5f, 0.5f, 1.0f },{ 0.5f, 0.5f, 0.4f, 1.0f } };
GLfloat material_specular[24][4] = { { 0.633f, 0.727811f, 0.633f, 1.0f },{ 0.316228f, 0.316228f, 0.316228f, 1.0f },{ 0.332741f, 0.328634f, 0.346435f, 1.0f },{ 0.296648f, 0.296648f, 0.296648f, 1.0f },{ 0.727811f, 0.626959f, 0.626959f, 1.0f },{ 0.297254f, 0.30829f, 0.306678f, 1.0f },{ 0.992157f, 0.941176f, 0.807843f, 1.0f },{ 0.393548f, 0.271906f, 0.166721f, 1.0f },{ 0.774597f, 0.774597f, 0.774597f, 1.0f },{ 0.256777f, 0.137622f, 0.086014f, 1.0f },{ 0.628281f, 0.555802f, 0.366065f, 1.0f },{ 0.508273f, 0.508273f, 0.508273f, 1.0f },{ 0.50f, 0.50f, 0.50f, 1.0f },{ 0.50196078f, 0.50196078f, 0.50196078f, 1.0f },{ 0.45f, 0.55f, 0.45f, 1.0f },{ 0.7f, 0.6f, 0.6f, 1.0f },{ 0.70f, 0.70f, 0.70f, 1.0f },{ 0.60f, 0.60f, 0.50f, 1.0f },{ 0.4f, 0.4f, 0.4f, 1.0f },{ 0.04f, 0.7f, 0.7f, 1.0f },{ 0.04f, 0.7f, 0.04f, 1.0f },{ 0.7f, 0.04f, 0.04f, 1.0f },{ 0.7f, 0.7f, 0.7f, 1.0f },{ 0.7f, 0.7f, 0.04f, 1.0f } };
GLfloat material_shininess[24] = { 0.6f * 128, 0.1f * 128, 0.3f * 128, 0.088f * 128, 0.6f * 128, 0.1f * 128, 0.21794872f * 128, 0.2f * 128, 0.6f * 128, 0.1f * 128, 0.4f * 128, 0.4f * 128, 0.25f * 128, 0.25f * 128, 0.25f * 128, 0.25f * 128, 0.25f * 128, 0.25f * 128, 0.078125f * 128, 0.078125f * 128, 0.078125f * 128, 0.078125f * 128, 0.078125f * 128, 0.078125 * 128 };

float gNumVertices = 0;
float gNumElements = 0;

float gSphereVertices[1146];
float gSphereNormals[1146];
float sphere_textures[764];
unsigned short gSphereElements[2280];


bool gbLight = true;

using namespace vmath;

mat4 gPerspectiveProjectionMatrix;

FILE *gpFile = NULL;


enum {
	VDG_ATTRIBUTE_VERTEX = 0,
	VDG_ATTRIBUTE_COLOR,
	VDG_ATTRIBUTE_NORMAL,
	VDG_ATTRIBUTE_TEXTURE0
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow) {
	void display(void);
	void initialize(void);
	void uninitialize(void);
	void update(void);
	WNDCLASSEX wndClass;
	HWND hwnd;
	MSG msg;
	TCHAR szClassName[] = TEXT("FirstShader");
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
		TEXT("RollNo 74 | Sachin Bhadrashette | Lights | 24 sphere"),
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
		case 0x41:
			break;
		case 0x4C:
			gbLight = !gbLight;
			break;
		case 0x58:// 'X' or 'x'
			gXPressed = true;
			gYPressed = false;
			gZPressed = false;
			break;

		case 0x59:// 'Y' or 'y'
			gXPressed = false;
			gYPressed = true;
			gZPressed = false;
			break;

		case 0x5A:// 'Z' or 'z'
			gXPressed = false;
			gYPressed = false;
			gZPressed = true;
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

void initialize() {
	void resize(int, int);
	void printErrorLog(GLuint object, char* message);
	void uninitialize(void);
	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormatIndex;
	ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));

	fopen_s(&gpFile, "OGLog.txt", "w");
	if (gpFile == NULL) {
		//printf("Error in opening file.");
	}

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

	GLenum glew_error = glewInit();
	if (glew_error != GLEW_OK) {
		wglDeleteContext(ghrc);
		ghrc = NULL;
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}
	GLint num;
	glGetIntegerv(GL_NUM_EXTENSIONS, &num);
	fprintf(gpFile, "===================================== Extension Details ======================================= \n");
	fprintf(gpFile, "Total extension supported : %d \n", num);
	fprintf(gpFile, "===================================== Extension Details ======================================= \n");


	for (int i = 0; i < num; i++) {
		//fprintf(gpFile, "%s \n", (const char*) glGetStringi(GL_EXTENSIONS, i));
	}

	// *** VERTEXT SHADEER ****
	gVertexShaderObject = glCreateShader(GL_VERTEX_SHADER);

	const GLchar *vertexShaderSourceCode =
		"#version 440 core"\
		"\n"\
		"in vec4 vPosition;"\
		"in vec3 vNormal;"\
		"uniform mat4 u_model_matrix;"\
		"uniform mat4 u_view_matrix;"\
		"uniform mat4 u_projection_matrix;"\
		"uniform int u_lighting_enabled;"\
		"uniform vec3 u_La;"\
		"uniform vec3 u_Ld;"\
		"uniform vec3 u_Ls;"\
		"uniform vec4 u_light_position;"\
		"uniform vec3 u_Ka;"\
		"uniform vec3 u_Kd;"\
		"uniform vec3 u_Ks;"\
		"uniform float u_material_shininess;"\
		"out vec3 phong_ads_color;"\
		"void main(void)"\
		"{"\
		"if(u_lighting_enabled == 1)"\
		"{"\
		"vec4 eye_coordinates = u_view_matrix * u_model_matrix * vPosition;"\
		"vec3 transformed_normals = normalize(mat3(u_view_matrix * u_model_matrix) * vNormal);"\
		"vec3 light_direction = normalize(vec3(u_light_position) - eye_coordinates.xyz);"\
		"float tn_dot_ld = max(dot(transformed_normals, light_direction), 0.0);"\
		"vec3 ambient = u_La * u_Kd;"\
		"vec3 diffuse = u_Ld * u_Kd * tn_dot_ld;"\
		"vec3 reflection_vector = reflect(-light_direction, transformed_normals);"\
		"vec3 viewer_vector = normalize(-eye_coordinates.xyz);"\
		"vec3 specular = u_Ls * u_Ks * pow(max(dot(reflection_vector, viewer_vector), 0.0), u_material_shininess);"\
		"phong_ads_color = ambient + diffuse + specular;"\
		"}"\
		"else"\
		"{"\
		"phong_ads_color = vec3(1.0, 1.0, 1.0);"\
		"}"\
		"gl_Position = u_projection_matrix * u_view_matrix * u_model_matrix * vPosition;"\
		"}";

	glShaderSource(gVertexShaderObject, 1, (const GLchar **)&vertexShaderSourceCode, NULL);


	//compile shader
	glCompileShader(gVertexShaderObject);
	GLint iShaderCompiledStatus = 0;
	glGetShaderiv(gVertexShaderObject, GL_COMPILE_STATUS, &iShaderCompiledStatus);
	fprintf(gpFile, "Vertex shader compilation status : %d\n", iShaderCompiledStatus);

	if (iShaderCompiledStatus == GL_FALSE) {
		//printErrorLog(gVertexShaderObject, "Vertex shader compilation failure");

		GLint iInfoLogLength;
		char *szInfoLog = NULL;
		glGetShaderiv(gVertexShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0) {
			szInfoLog = (char*)malloc(iInfoLogLength);
			if (szInfoLog) {
				GLsizei written;
				glGetShaderInfoLog(gVertexShaderObject, iInfoLogLength, &written, szInfoLog);
				fprintf(gpFile, "Error : %s : %s", "Vertex shader compilation failure ::: ", szInfoLog);
				free(szInfoLog);
				uninitialize();
				exit(0);
			}
		}
	}

	// *** FRAGMENT SHADEER ****
	gFragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);

	const char *fragmentShaderSourceCode =
		"#version 440 core"\
		"\n"\
		"in vec3 phong_ads_color;"\
		"out vec4 FragColor;"\
		"void main(void)"\
		"{"\
		"FragColor = vec4(phong_ads_color, 1.0);"\
		"}";

	glShaderSource(gFragmentShaderObject, 1, (const GLchar**)&fragmentShaderSourceCode, NULL);

	//compile shader
	glCompileShader(gFragmentShaderObject);

	iShaderCompiledStatus = 0;
	glGetShaderiv(gFragmentShaderObject, GL_COMPILE_STATUS, &iShaderCompiledStatus);
	fprintf(gpFile, "Fragment shader compilation status : %d\n", iShaderCompiledStatus);

	if (iShaderCompiledStatus == GL_FALSE) {
		GLint iInfoLogLength;
		char *szInfoLog = NULL;
		glGetShaderiv(gFragmentShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0) {
			szInfoLog = (char*)malloc(iInfoLogLength);
			if (szInfoLog) {
				GLsizei written;
				glGetShaderInfoLog(gFragmentShaderObject, iInfoLogLength, &written, szInfoLog);
				fprintf(gpFile, "Error : %s : %s", "Fragment shader compilation failure", szInfoLog);
				free(szInfoLog);
				uninitialize();
				exit(0);
			}
		}
	}

	//*** SHADER PROGRAM ***
	gShaderProgramObject = glCreateProgram();

	glAttachShader(gShaderProgramObject, gVertexShaderObject);
	glAttachShader(gShaderProgramObject, gFragmentShaderObject);

	glBindAttribLocation(gShaderProgramObject, VDG_ATTRIBUTE_VERTEX, "vPosition");
	glBindAttribLocation(gShaderProgramObject, VDG_ATTRIBUTE_NORMAL, "vNormal");

	glLinkProgram(gShaderProgramObject);
	GLint iShaderLinkingStatus = 0;
	glGetShaderiv(gShaderProgramObject, GL_COMPILE_STATUS, &iShaderLinkingStatus);
	fprintf(gpFile, "Shader Linking status : %d\n", iShaderCompiledStatus);

	if (iShaderCompiledStatus == GL_FALSE) {
		//printErrorLog(gShaderProgramObject, " shader linking failure");
		GLint iInfoLogLength;
		char *szInfoLog = NULL;
		glGetShaderiv(gShaderProgramObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0) {
			szInfoLog = (char*)malloc(iInfoLogLength);
			if (szInfoLog) {
				GLsizei written;
				glGetShaderInfoLog(gShaderProgramObject, iInfoLogLength, &written, szInfoLog);
				fprintf(gpFile, "Error : %s : %s", "Shader Linking failure", szInfoLog);
				free(szInfoLog);
				uninitialize();
				exit(0);
			}
		}
	}

	gModelMatrixUniform = glGetUniformLocation(gShaderProgramObject, "u_model_matrix");
	gViewMatrixUniform = glGetUniformLocation(gShaderProgramObject, "u_view_matrix");
	gProjectionMatrixUniform = glGetUniformLocation(gShaderProgramObject, "u_projection_matrix");

	gLKeyPressedUniform = glGetUniformLocation(gShaderProgramObject, "u_lighting_enabled");

	gLaUniform = glGetUniformLocation(gShaderProgramObject, "u_La");
	gLdUniform = glGetUniformLocation(gShaderProgramObject, "u_Ld");
	gLsUniform = glGetUniformLocation(gShaderProgramObject, "u_Ls");
	gLightPositionUniform = glGetUniformLocation(gShaderProgramObject, "u_light_position");

	gKaUniform = glGetUniformLocation(gShaderProgramObject, "u_Ka");
	gKdUniform = glGetUniformLocation(gShaderProgramObject, "u_Kd");
	gKsUniform = glGetUniformLocation(gShaderProgramObject, "u_Ks");
	gMaterialShininessniform = glGetUniformLocation(gShaderProgramObject, "u_material_shininess");

	getSphereVertexData(gSphereVertices, gSphereNormals, sphere_textures, gSphereElements);
	gNumVertices = getNumberOfSphereVertices();
	gNumElements = getNumberOfSphereElements();


	glGenVertexArrays(1, &gVao_sphere);
	glBindVertexArray(gVao_sphere);

	glGenBuffers(1, &gVbo_sphere_position);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_sphere_position);
	glBufferData(GL_ARRAY_BUFFER, sizeof(gSphereVertices), gSphereVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(VDG_ATTRIBUTE_VERTEX, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(VDG_ATTRIBUTE_VERTEX);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &gVbo_sphere_normal);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_sphere_normal);
	glBufferData(GL_ARRAY_BUFFER, sizeof(gSphereNormals), gSphereNormals, GL_STATIC_DRAW);
	glVertexAttribPointer(VDG_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(VDG_ATTRIBUTE_NORMAL);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &gVbo_sphere_element);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(gSphereElements), gSphereElements, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glBindVertexArray(0);


	glClearColor(0.25f, 0.25f, 0.25f, 0.2f);

	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glShadeModel(GL_SMOOTH);
	glCullFace(GL_TRUE);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	gPerspectiveProjectionMatrix = mat4::identity();
	resize(WIN_WIDTH, WIN_HEIGHT);
}

void display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(gShaderProgramObject);

	if (gbLight == true) {
		glUniform1i(gLKeyPressedUniform, 1);

		glUniform3fv(gLaUniform, 1, light_ambient);
		glUniform3fv(gLdUniform, 1, light_diffuse);
		glUniform3fv(gLsUniform, 1, light_specular);
		glUniform4fv(gLightPositionUniform, 1, light_position);

		
	}
	else {
		glUniform1i(gLKeyPressedUniform, 0);
	}
	int row =0, col =0 , count = 0;
	for (int index = 0; index < 24; index++) {
		mat4 modelMatrix = mat4::identity();
		mat4 viewMatrix = mat4::identity();

		modelMatrix = vmath::translate(-5.0f + col*2, 3.0f - row*2, -10.0f);

		glUniformMatrix4fv(gModelMatrixUniform, 1, GL_FALSE, modelMatrix);
		glUniformMatrix4fv(gViewMatrixUniform, 1, GL_FALSE, viewMatrix);
		glUniformMatrix4fv(gProjectionMatrixUniform, 1, GL_FALSE, gPerspectiveProjectionMatrix);

		glBindVertexArray(gVao_sphere);
		glUniform3fv(gKaUniform, 1, material_ambient[index]);
		glUniform3fv(gKdUniform, 1, material_diffuse[index]);
		glUniform3fv(gKsUniform, 1, material_specular[index]);
		glUniform1f(gMaterialShininessniform, material_shininess[index]);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
		glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
		glBindVertexArray(0);

		if (col < 5) {
			col++;
		}
		else {
			col = 0;
			row++;
		}
	}
	glUseProgram(0);

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

	if (width <= height) {
		gPerspectiveProjectionMatrix = perspective(45.0f, (GLfloat)height / (GLfloat)width, 0.01f, 100.0f);
	}
	else {
		gPerspectiveProjectionMatrix = perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.01f, 100.0f);
	}
}

void uninitialize(void) {
	if (gbFullScreen == true) {
		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);
		SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd, &wPrev);
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);
	}

	if (gVao_sphere) {
		glDeleteVertexArrays(1, &gVao_sphere);
		gVao_sphere = 0;
	}

	if (gVbo_sphere_position) {
		glDeleteBuffers(1, &gVbo_sphere_position);
		gVbo_sphere_position = 0;
	}

	if (gVbo_sphere_normal) {
		glDeleteBuffers(1, &gVbo_sphere_normal);
		gVbo_sphere_normal = 0;
	}

	glDetachShader(gShaderProgramObject, gVertexShaderObject);

	glDetachShader(gShaderProgramObject, gFragmentShaderObject);

	glDeleteShader(gVertexShaderObject);

	gVertexShaderObject = 0;

	glDeleteShader(gFragmentShaderObject);

	gFragmentShaderObject = 0;

	glDeleteProgram(gShaderProgramObject);
	gShaderProgramObject = 0;

	glUseProgram(0);

	wglMakeCurrent(NULL, NULL);

	wglDeleteContext(ghrc);

	ghrc = NULL;

	ReleaseDC(ghwnd, ghdc);
	ghdc = NULL;

	DestroyWindow(ghwnd);
	ghwnd = NULL;

	if (gpFile) {
		fclose(gpFile);
		gpFile = NULL;
	}

}


void update()
{
	gAngle = gAngle + 0.002f;
	if (gAngle >= 360.0f)
	{
		gAngle = 0.0f;
	}

	if (gXPressed == true)
	{
		light_position[0] = 0.0f;
		light_position[1] = gRedious * cos(gAngle);
		light_position[2] = gRedious * sin(gAngle);
	}
	else if (gYPressed == true)
	{
		light_position[0] = gRedious * cos(gAngle);
		light_position[1] = 0.0f;
		light_position[2] = gRedious * sin(gAngle);
	}
	else if (gZPressed == true)
	{
		light_position[0] = gRedious * cos(gAngle);
		light_position[1] = gRedious * sin(gAngle);
		light_position[2] = 0.0f;
	}
}

