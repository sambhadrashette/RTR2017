#include<windows.h>
#include<GL/glew.h>
#include<gl/GL.h>
#include<math.h>
#include<stdio.h>
#include<stdlib.h>

#include "icon.h"
#include "vmath.h"

#define WIN_WIDTH 800
#define WIN_HEIGHT 600

#pragma comment(lib,"glew32.lib")
#pragma comment (lib,"opengl32.lib")

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

GLuint gVertexShaderObject;
GLuint gFragmentShaderObject;
GLuint gShaderProgramObject;

GLuint gVao_smiley;
GLuint gVbo_smiley_position;
GLuint gVbo_smiley_texture;
GLuint gMVPUniform;

GLuint gTexture_sampler_uniform;
GLuint gTexture_Smiley;
GLuint giDigitPressed;
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
		TEXT("RollNo 74 | Sachin Bhadrashette | Texture Smiley"),
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
	void printErrorLog(GLuint object, char* message);
	void uninitialize(void);
	int loadGLTextures(GLuint *texture, TCHAR imageResourceId[]);
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
	//Create Shader 
	gVertexShaderObject = glCreateShader(GL_VERTEX_SHADER);

	const GLchar *vertexShaderSourceCode =
		"#version 440 core"\
		"\n"\
		"in vec4 vPosition;"\
		"in vec2 vTexture0_Coord;"\
		"out vec2 out_texture0_coord;"\
		"uniform mat4 u_mvp_matrix;"\
		"void main(void)"\
		"{"\
		"gl_Position = u_mvp_matrix * vPosition;"\
		"out_texture0_coord = vTexture0_Coord;"\
		"}";
	glShaderSource(gVertexShaderObject, 1, (const GLchar **)&vertexShaderSourceCode, NULL);

	//Compile Shader
	glCompileShader(gVertexShaderObject);
	GLint iInfoLogLength = 0;
	GLint iShaderCompiledStatus = 0;
	char *szInfoLog = NULL;

	glGetShaderiv(gVertexShaderObject, GL_COMPILE_STATUS, &iShaderCompiledStatus);
	fprintf(gpFile, "Vertext Shader Program status: %i\n", iShaderCompiledStatus);
	if (iShaderCompiledStatus == GL_FALSE)
	{
		glGetShaderiv(gVertexShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (char*)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gVertexShaderObject, iInfoLogLength, &written, szInfoLog);
				fprintf(gpFile, "Vertex shader compilation log: %s\n", szInfoLog);
				free(szInfoLog);
				uninitialize();
				exit(0);
			}
		}
	}

	/**********Fragment Shader*************/
	//Create Shader 
	gFragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);

	//provide source code to shader
	const GLchar *fragmentShaderSourceCode =
		"#version 440 core"\
		"\n"\
		"in vec2 out_texture0_coord;"\
		"out vec4 FragColor;"\
		"uniform sampler2D u_texture0_sampler;"\
		"void main(void)"\
		"{"\
		"FragColor = texture(u_texture0_sampler, out_texture0_coord);"\
		"}";
	glShaderSource(gFragmentShaderObject, 1, (const GLchar **)&fragmentShaderSourceCode, NULL);

	//compile shader
	glCompileShader(gFragmentShaderObject);
	glGetShaderiv(gFragmentShaderObject, GL_COMPILE_STATUS, &iShaderCompiledStatus);
	fprintf(gpFile, "Fragment Shader Program status: %i\n", iShaderCompiledStatus);

	if (iShaderCompiledStatus == GL_FALSE)
	{
		glGetShaderiv(gFragmentShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (char*)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gFragmentShaderObject, iInfoLogLength, &written, szInfoLog);
				fprintf(gpFile, "Fragment Shader Compilation Log: %s\n", szInfoLog);
				free(szInfoLog);
				uninitialize();
				exit(0);
			}
		}
	}

	/*************Shader Program************/
	//Create
	gShaderProgramObject = glCreateProgram();

	//Attach vertex shader to shader program
	glAttachShader(gShaderProgramObject, gVertexShaderObject);

	// attach fragment shader to shader program
	glAttachShader(gShaderProgramObject, gFragmentShaderObject);

	// pre-link binding of shader program object with vertex shader position attribute
	glBindAttribLocation(gShaderProgramObject, VDG_ATTRIBUTE_VERTEX, "vPosition");
	glBindAttribLocation(gShaderProgramObject, VDG_ATTRIBUTE_TEXTURE0, "vTexture0_Coord");

	// link shader
	glLinkProgram(gShaderProgramObject);
	GLint iShaderProgramLinkStatus = 0;
	glGetProgramiv(gShaderProgramObject, GL_LINK_STATUS, &iShaderProgramLinkStatus);
	fprintf(gpFile, " Shader Program Linking status: %i\n", iShaderProgramLinkStatus);

	if (iShaderProgramLinkStatus == GL_FALSE)
	{
		glGetProgramiv(gShaderProgramObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (char*)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetProgramInfoLog(gShaderProgramObject, iInfoLogLength, &written, szInfoLog);
				fprintf(gpFile, "Shader Program Link Log: %s\n", szInfoLog);
				free(szInfoLog);
				uninitialize();
				exit(0);
			}
		}
	}

	//Get MVP uniform location
	gMVPUniform = glGetUniformLocation(gShaderProgramObject, "u_mvp_matrix");
	gTexture_sampler_uniform = glGetUniformLocation(gShaderProgramObject, "u_texture0_sampler");

	/*vertices initialization*/
	//Square values
	const GLfloat squareVertices[] =
	{
		1.0f, 1.0f, 0.0f, //RU
		1.0f, -1.0f, 0.0f,//LU
		-1.0f, -1.0f, 0.0f,//LB
		-1.0f, 1.0f, 0.0f//RB
	};

	const GLfloat textureCoords[] =
	{
		0.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
	};

	//Bind Square VAO
	glGenVertexArrays(1, &gVao_smiley);
	glBindVertexArray(gVao_smiley);

	//VBO for Square vertices
	glGenBuffers(1, &gVbo_smiley_position);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_smiley_position);
	glBufferData(GL_ARRAY_BUFFER, sizeof(squareVertices), squareVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(VDG_ATTRIBUTE_VERTEX, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(VDG_ATTRIBUTE_VERTEX);

	glBindBuffer(GL_ARRAY_BUFFER, 0);//Unbind vertices VBO

									 //VBO for Texture
	glGenBuffers(1, &gVbo_smiley_texture);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_smiley_texture);
	glBufferData(GL_ARRAY_BUFFER, sizeof(textureCoords), textureCoords, GL_STATIC_DRAW);
	glVertexAttribPointer(VDG_ATTRIBUTE_TEXTURE0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(VDG_ATTRIBUTE_TEXTURE0);
	glBindBuffer(GL_ARRAY_BUFFER, 0); //Unbind Texture VBO

									  //Unbind Square VAO
	glBindVertexArray(0);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glShadeModel(GL_SMOOTH);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	loadGLTextures(&gTexture_Smiley, MAKEINTRESOURCE(IDBITMAP_SMILEY));

	glEnable(GL_TEXTURE_2D);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	gPerspectiveProjectionMatrix = mat4::identity();
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
		case 0x31:
			giDigitPressed = 1;
			break;

		case 0x32:
			giDigitPressed = 2;
			break;

		case 0x33:
			giDigitPressed = 3;
			break;

		case 0x34:
			giDigitPressed = 4;
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

void printErrorLog(GLuint object, char* message) {
	void uninitialize(void);
	GLint iInfoLogLength;
	char *szInfoLog = NULL;
	glGetShaderiv(object, GL_COMPILE_STATUS, &iInfoLogLength);
	if (iInfoLogLength > 0) {
		szInfoLog = (char*)malloc(iInfoLogLength);
		if (szInfoLog) {
			GLsizei written;
			glGetShaderInfoLog(object, iInfoLogLength, &written, szInfoLog);
			fprintf(gpFile, "Error : %s : %s", message, szInfoLog);
			free(szInfoLog);
			uninitialize();
			exit(0);
		}
	}
}

void display() {
	GLfloat textureCoords[8] = {
		0.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
	};

	if (giDigitPressed == 1)
	{
		textureCoords[0] = 0.0f;
		textureCoords[1] = 0.5f;
		textureCoords[2] = 0.0f;
		textureCoords[3] = 0.0f;
		textureCoords[4] = 0.5f;
		textureCoords[5] = 0.0f;
		textureCoords[6] = 0.5f;
		textureCoords[7] = 0.5f;

	}
	else if (giDigitPressed == 2)
	{
		textureCoords[0] = 0.0f;
		textureCoords[1] = 1.0f;
		textureCoords[2] = 0.0f;
		textureCoords[3] = 0.0f;
		textureCoords[4] = 1.0f;
		textureCoords[5] = 0.0f;
		textureCoords[6] = 1.0f;
		textureCoords[7] = 1.0f;

	}
	else if (giDigitPressed == 3)
	{
		textureCoords[0] = 0.0f;
		textureCoords[1] = 2.0f;
		textureCoords[2] = 0.0f;
		textureCoords[3] = 0.0f;
		textureCoords[4] = 2.0f;
		textureCoords[5] = 0.0f;
		textureCoords[6] = 2.0f;
		textureCoords[7] = 2.0f;
	}
	else if (giDigitPressed == 4)
	{
		textureCoords[0] = 0.5f;
		textureCoords[1] = 0.5f;
		textureCoords[2] = 0.5f;
		textureCoords[3] = 0.5f;
		textureCoords[4] = 0.5f;
		textureCoords[5] = 0.5f;
		textureCoords[6] = 0.5f;
		textureCoords[7] = 0.5f;
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glUseProgram(gShaderProgramObject);

	mat4 modelViewMatrix = mat4::identity();
	mat4 modelViewProjectionMatrix = mat4::identity();

	/*******Square Block***********/
	modelViewMatrix = translate(0.0f, 0.0f, -3.0f);

	modelViewProjectionMatrix = gPerspectiveProjectionMatrix * modelViewMatrix;

	glUniformMatrix4fv(gMVPUniform, 1, GL_FALSE, modelViewProjectionMatrix);

	glBindVertexArray(gVao_smiley);

	glBindBuffer(GL_ARRAY_BUFFER, gVbo_smiley_texture);
	glBufferData(GL_ARRAY_BUFFER, sizeof(textureCoords), textureCoords, GL_STATIC_DRAW);
	glVertexAttribPointer(VDG_ATTRIBUTE_TEXTURE0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(VDG_ATTRIBUTE_TEXTURE0);
	glBindBuffer(GL_ARRAY_BUFFER, 0); 

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gTexture_Smiley);
	glUniform1i(gTexture_sampler_uniform, 0);

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	glBindVertexArray(0);

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

	if (gVao_smiley) {
		glDeleteVertexArrays(1, &gVao_smiley);
		gVao_smiley = 0;
	}

	if (gVbo_smiley_position) {
		glDeleteBuffers(1, &gVbo_smiley_position);
		gVbo_smiley_position = 0;
	}

	if (gVbo_smiley_texture) {
		glDeleteBuffers(1, &gVbo_smiley_texture);
		gVbo_smiley_texture = 0;
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


int loadGLTextures(GLuint *texture, TCHAR imageResourceId[]) {
	//variable declarations
	HBITMAP hBitmap;
	BITMAP bmp;
	int iStatus = FALSE;

	//code 
	glGenTextures(1, texture); //1 image
	hBitmap = (HBITMAP)LoadImage(GetModuleHandle(NULL), imageResourceId, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
	if (hBitmap) //if bitmap exists ( means hBitmap is not null )
	{
		iStatus = TRUE;
		GetObject(hBitmap, sizeof(bmp), &bmp);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4); //pixel storage mode( word alignment/4 bytes ).
		glBindTexture(GL_TEXTURE_2D, *texture); //bind texture
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		//generate mipmapped texture ( 3 bytes, width, height & data from bmp)
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RGB,
			bmp.bmWidth,
			bmp.bmHeight,
			0,
			GL_BGR_EXT,
			GL_UNSIGNED_BYTE,
			bmp.bmBits);

		glGenerateMipmap(GL_TEXTURE_2D);

		DeleteObject(hBitmap); //delete bitmap handle
	}
	return(iStatus);
}