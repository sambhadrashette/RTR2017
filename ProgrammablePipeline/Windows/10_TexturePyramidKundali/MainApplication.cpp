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

GLuint gVao_pyramid;
GLuint gVao_cube;
GLuint gVbo_pyramid_position;
GLuint gVbo_cube_position;
GLuint gVbo_pyramid_texture;
GLuint gVbo_cube_texture;
GLuint gMVPUniform;

GLfloat gAnglePyramid = 0.0;
GLfloat gAngleCube = 0.1f;

GLuint gTexture_sampler_uniform;
GLuint	gTexture_Kundali;
GLuint	gTexture_Stone;

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
		TEXT("RollNo 74 | Sachin Bhadrashette | Stone and Kundali texture 3D rotation"),
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
	gVertexShaderObject = glCreateShader(GL_VERTEX_SHADER);

	const char *vertextShaderSourceCode =
		"#version 130" \
		"\n" \
		"in vec4 vPosition;" \
		"in vec2 vTexture0_Coord;" \
		"out vec2 out_texture0_Coord;" \
		"uniform mat4 u_mvp_matrix;" \
		"void main(void) " \
		"{" \
		"gl_Position = u_mvp_matrix * vPosition;" \
		"out_texture0_Coord = vTexture0_Coord;" \
		"}";
	glShaderSource(gVertexShaderObject, 1, (const GLchar**)&vertextShaderSourceCode, NULL);

	//compile shader
	glCompileShader(gVertexShaderObject);
	GLint iShaderCompiledStatus = 0;
	glGetShaderiv(gVertexShaderObject, GL_COMPILE_STATUS, &iShaderCompiledStatus);
	if (iShaderCompiledStatus == GL_FALSE) {
		//printErrorLog(gVertexShaderObject, "Vertex shader compilation failure");

		GLint iInfoLogLength;
		char *szInfoLog = NULL;
		glGetShaderiv(gVertexShaderObject, GL_COMPILE_STATUS, &iInfoLogLength);
		if (iInfoLogLength > 0) {
			szInfoLog = (char*)malloc(iInfoLogLength);
			if (szInfoLog) {
				GLsizei written;
				glGetShaderInfoLog(gVertexShaderObject, iInfoLogLength, &written, szInfoLog);
				fprintf(gpFile, "Error : %s : %s", "Vertex shader compilation failure", szInfoLog);
				free(szInfoLog);
				uninitialize();
				exit(0);
			}
		}
	}

	// *** FRAGMENT SHADEER ****
	gFragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);

	const char *fragmentShaderSourceCode =
		"#version 130" \
		"\n" \
		"out vec4 FragColor;" \
		"in vec2 out_texture0_Coord;"\
		"uniform sampler2D u_texture0_sampler;" \
		"void main(void) " \
		"{" \
		"FragColor = texture(u_texture0_sampler, out_texture0_Coord);" \
		"}";

	glShaderSource(gFragmentShaderObject, 1, (const GLchar**)&fragmentShaderSourceCode, NULL);

	//compile shader
	glCompileShader(gFragmentShaderObject);

	iShaderCompiledStatus = 0;
	glGetShaderiv(gFragmentShaderObject, GL_COMPILE_STATUS, &iShaderCompiledStatus);
	if (iShaderCompiledStatus == GL_FALSE) {
		//printErrorLog(gFragmentShaderObject, "Fragment shader compilation failure");
		GLint iInfoLogLength;
		char *szInfoLog = NULL;
		glGetShaderiv(gFragmentShaderObject, GL_COMPILE_STATUS, &iInfoLogLength);
		if (iInfoLogLength > 0) {
			szInfoLog = (char*)malloc(iInfoLogLength);
			if (szInfoLog) {
				GLsizei written;
				glGetShaderInfoLog(gFragmentShaderObject, iInfoLogLength, &written, szInfoLog);
				fprintf(gpFile, "Error : %s : %s", "Vertex shader compilation failure", szInfoLog);
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

	glBindAttribLocation(gShaderProgramObject, VDG_ATTRIBUTE_TEXTURE0, "vTexture0_Coord");


	glLinkProgram(gShaderProgramObject);
	GLint iShaderLinkingStatus = 0;
	glGetShaderiv(gShaderProgramObject, GL_COMPILE_STATUS, &iShaderLinkingStatus);
	if (iShaderCompiledStatus == GL_FALSE) {
		//printErrorLog(gShaderProgramObject, " shader linking failure");
		GLint iInfoLogLength;
		char *szInfoLog = NULL;
		glGetShaderiv(gShaderProgramObject, GL_COMPILE_STATUS, &iInfoLogLength);
		if (iInfoLogLength > 0) {
			szInfoLog = (char*)malloc(iInfoLogLength);
			if (szInfoLog) {
				GLsizei written;
				glGetShaderInfoLog(gShaderProgramObject, iInfoLogLength, &written, szInfoLog);
				fprintf(gpFile, "Error : %s : %s", "Vertex shader compilation failure", szInfoLog);
				free(szInfoLog);
				uninitialize();
				exit(0);
			}
		}
	}

	gMVPUniform = glGetUniformLocation(gShaderProgramObject, "u_mvp_matrix");
	gTexture_sampler_uniform = glGetUniformLocation(gShaderProgramObject, "u_texture0_sampler");

	/*vertices,shader attribs,vbo,vao initialization*/
	//Triangle values
	const GLfloat pyramidVertices[] =
	{
		//front face
		0.0f, 1.0f, 0.0f, // apex
		-1.0f, -1.0f, 1.0f, // left-corner
		1.0f, -1.0f, 1.0f, // right-corner

						   //Right face
		0.0f, 1.0f, 0.0f, //Apex
		1.0f, -1.0f, 1.0f, //Left-corner
		1.0f, -1.0f, -1.0f, //right-corner

							//Back face
		0.0f, 1.0f, 0.0f, //Apex
		1.0f, -1.0f, -1.0f, //left-corner
		-1.0f, -1.0f, -1.0f, //right-corner

							 //Left_face
		0.0f, 1.0f, 0.0f, //Apex
		-1.0f, -1.0f, -1.0f, //left-corner
		-1.0f, -1.0f, 1.0f //Right-corner
	};

	//Triangle color values
	const GLfloat pyramidTexcoords[] =
	{
		0.5f, 1.0f, // front-top
		0.0f, 0.0f, // front-left
		1.0f, 0.0f, // front-right

		0.5f, 1.0f, // right-top
		1.0f, 0.0f, // right-left
		0.0f, 0.0f, // right-right

		0.5f, 1.0f, // back-top
		1.0f, 0.0f, // back-left
		0.0f, 0.0f, // back-right

		0.5f, 1.0f, // left-top
		0.0f, 0.0f, // left-left
		1.0f, 0.0f, // left-right
	};

	//Cube values
	GLfloat cubeVertices[] =
	{
		// top surface
		1.0f, 1.0f, -1.0f,  // top-right of top
		-1.0f, 1.0f, -1.0f, // top-left of top
		-1.0f, 1.0f, 1.0f, // bottom-left of top
		1.0f, 1.0f, 1.0f,  // bottom-right of top

						   // bottom surface
		1.0f, -1.0f, 1.0f,  // top-right of bottom
		-1.0f, -1.0f, 1.0f, // top-left of bottom
		-1.0f, -1.0f, -1.0f, // bottom-left of bottom
		1.0f, -1.0f, -1.0f,  // bottom-right of bottom

							 // front surface
		1.0f, 1.0f, 1.0f,  // top-right of front
		-1.0f, 1.0f, 1.0f, // top-left of front
		-1.0f, -1.0f, 1.0f, // bottom-left of front
		1.0f, -1.0f, 1.0f,  // bottom-right of front

							// back surface
		1.0f, -1.0f, -1.0f,  // top-right of back
		-1.0f, -1.0f, -1.0f, // top-left of back
		-1.0f, 1.0f, -1.0f, // bottom-left of back
		1.0f, 1.0f, -1.0f,  // bottom-right of back

							// left surface
		-1.0f, 1.0f, 1.0f, // top-right of left
		-1.0f, 1.0f, -1.0f, // top-left of left
		-1.0f, -1.0f, -1.0f, // bottom-left of left
		-1.0f, -1.0f, 1.0f, // bottom-right of left

							// right surface
		1.0f, 1.0f, -1.0f,  // top-right of right
		1.0f, 1.0f, 1.0f,  // top-left of right
		1.0f, -1.0f, 1.0f,  // bottom-left of right
		1.0f, -1.0f, -1.0f,  // bottom-right of right
	};


	//Cube color values
	const GLfloat cubeTexcoords[] =
	{
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,

		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,

		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,

		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,

		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,

		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f
	};

	glGenVertexArrays(1, &gVao_pyramid);
	glBindVertexArray(gVao_pyramid);

	//for pyramid position
	glGenBuffers(1, &gVbo_pyramid_position);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_pyramid_position);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidVertices), pyramidVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(VDG_ATTRIBUTE_VERTEX, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(VDG_ATTRIBUTE_VERTEX);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//for pyramid texture 
	glGenBuffers(1, &gVbo_pyramid_texture);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_pyramid_texture);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidTexcoords), pyramidTexcoords, GL_STATIC_DRAW);
	glVertexAttribPointer(VDG_ATTRIBUTE_TEXTURE0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(VDG_ATTRIBUTE_TEXTURE0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//for cube position
	glGenVertexArrays(1, &gVao_cube);
	glBindVertexArray(gVao_cube);

	glGenBuffers(1, &gVbo_cube_position);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_cube_position);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(VDG_ATTRIBUTE_VERTEX, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(VDG_ATTRIBUTE_VERTEX);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//for cube cube 
	glGenBuffers(1, &gVbo_cube_texture);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_cube_texture);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeTexcoords), cubeTexcoords, GL_STATIC_DRAW);
	glVertexAttribPointer(VDG_ATTRIBUTE_TEXTURE0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(VDG_ATTRIBUTE_TEXTURE0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glShadeModel(GL_SMOOTH);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glEnable(GL_CULL_FACE);

	loadGLTextures(&gTexture_Kundali, MAKEINTRESOURCE(IDBITMAP_KUNDALI));
	loadGLTextures(&gTexture_Stone, MAKEINTRESOURCE(IDBITMAP_STONE));

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
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(gShaderProgramObject);


	mat4 modelViewMatrix = mat4::identity();
	mat4 rotationMatrix = mat4::identity();
	//mat4 scaleMatrix = mat4::identity();
	mat4 modelViewProjectionMatrix = mat4::identity();

	//Pyramid
	modelViewMatrix = vmath::translate(-1.5f, 0.0f, -6.0f);
	rotationMatrix = vmath::rotate(gAnglePyramid, 0.0f, 1.0f, 0.0f);
	modelViewProjectionMatrix = gPerspectiveProjectionMatrix * (modelViewMatrix * rotationMatrix);
	glUniformMatrix4fv(gMVPUniform, 1, GL_FALSE, modelViewProjectionMatrix);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gTexture_Stone);
	glUniform1i(gTexture_sampler_uniform, 0);

	glBindVertexArray(gVao_pyramid);
	glDrawArrays(GL_TRIANGLES, 0, 12);
	glBindVertexArray(0);

	// Cube
	modelViewMatrix = mat4::identity();
	rotationMatrix = mat4::identity();
	//scaleMatrix = mat4::identity();
	modelViewProjectionMatrix = mat4::identity();

	modelViewMatrix = vmath::translate(1.5f, 0.0f, -6.0f);
	//scaleMatrix = vmath::scale(0.75f, 0.75f, 0.75f);
	rotationMatrix = vmath::rotate(gAngleCube, gAngleCube, gAngleCube);
	modelViewProjectionMatrix = gPerspectiveProjectionMatrix * (modelViewMatrix * rotationMatrix);
	glUniformMatrix4fv(gMVPUniform, 1, GL_FALSE, modelViewProjectionMatrix);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gTexture_Kundali);
	glUniform1i(gTexture_sampler_uniform, 0);

	glBindVertexArray(gVao_cube);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 16, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 20, 4);
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

	if (gVao_pyramid) {
		glDeleteVertexArrays(1, &gVao_pyramid);
		gVao_pyramid = 0;
	}

	if (gVao_cube) {
		glDeleteVertexArrays(1, &gVao_cube);
		gVao_cube = 0;
	}

	if (gVbo_pyramid_position) {
		glDeleteBuffers(1, &gVbo_pyramid_position);
		gVbo_pyramid_position = 0;
	}

	if (gVbo_pyramid_texture) {
		glDeleteBuffers(1, &gVbo_pyramid_texture);
		gVbo_pyramid_texture = 0;
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


void update(void) {

	gAnglePyramid = gAnglePyramid + 0.1f;
	if (gAnglePyramid >= 360) {
		gAnglePyramid = 0.1f;
	}

	gAngleCube = gAngleCube + 0.1f;
	if (gAngleCube >= 360) {
		gAngleCube = 0.0f;
	}

}

int loadGLTextures(GLuint *texture, TCHAR imageResourceId[]) {
	HBITMAP hBitmap;
	BITMAP bmp;
	int iStatus;

	glGenTextures(1, texture);

	hBitmap = (HBITMAP)LoadImage(GetModuleHandle(NULL), imageResourceId, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);

	if (hBitmap) {
		iStatus = TRUE;
		GetObject(hBitmap, sizeof(bmp), &bmp);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glBindTexture(GL_TEXTURE_2D, *texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, bmp.bmWidth, bmp.bmHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, bmp.bmBits);

		glGenerateMipmap(GL_TEXTURE_2D);
		DeleteObject(hBitmap);
	}
	return(iStatus);
}