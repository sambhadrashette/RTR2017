#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<memory.h>

#include<X11/Xlib.h>
#include<X11/Xutil.h>
#include<X11/XKBlib.h>
#include<X11/keysym.h>
#include<SOIL/SOIL.h>

#include<GL/gl.h>
#include<GL/glu.h>
#include<GL/glx.h>

#define WIN_WIDTH 800
#define WIN_HEIGHT 600

//namespace
using namespace std;

GLuint	Texture_Stone; 
GLuint	Texture_Kundali;
float angleTrangle = 0.0f;
float angleRect = 360.0f;

//global variables
bool bgFullscreen=false;
Display *gpDisplay=NULL;
XVisualInfo *gpXVisualInfo=NULL;
Colormap gColorMap;
Window gWindow;
GLXContext gGLXContext;

//Enrty Point function
int main(void) {
	//function prototype
	void createWindow(void);
	void ToggleFullscreen(void);
	void uninitialize();
	void resize(int, int);
	void display();
	void initialize();
	void rotate(void);
	
	//variable declaration
	int winWidth=WIN_WIDTH;
	int winHeight=WIN_HEIGHT;

	//code
	createWindow();

	initialize();

	//MessageLoop
	XEvent event;
	KeySym keysym;
	bool bDone = false;

	while(bDone == false) {
		while(XPending(gpDisplay)) {
			XNextEvent(gpDisplay, &event); //Similer to getMessage()
			switch(event.type)
			{
				case MapNotify:
					break;
				case KeyPress:
					keysym=XkbKeycodeToKeysym(gpDisplay, event.xkey.keycode, 0, 0);
					switch(keysym){
						case XK_Escape:
							bDone = true;
							break;
						case XK_F:
						case XK_f:
							if(bgFullscreen==false){
								ToggleFullscreen();
								bgFullscreen=true;
							}
							else { 
								ToggleFullscreen();
								bgFullscreen=false;
							}
							break;
						default:
							break;
					}
					break;
				case ButtonPress://mouse press
					switch(event.xbutton.button){
						case 1:	//left mouse button press
							break;
						case 2: //middle mouse button press
							break;
						case 3: // Right mouse button press
							break;
						case 4: //mouse wheen up
							break;
						case 5:	//mouse wheen down
							break;
						default: 
							break;
					}
					break;
				case MotionNotify:
					break;
				case ConfigureNotify:
					winWidth=event.xconfigure.width;
					winHeight=event.xconfigure.height;
					resize(winWidth, winHeight);
					break;
				case Expose: 
					break;
				case DestroyNotify:
					break;
				case 33:
					uninitialize();
					exit(0);
				default:
					break;
			}
		}
		display();
		rotate();
		
	}
	//below code used in OpneGL code
	uninitialize();
	return(0);
}

int createWindow(void){
	//function prototypes
	void uninitialize(void);
	
	//variable declaration
	XSetWindowAttributes winAttribs;
	int defaultScreen;
	int defaultDepth;
	int styleMask;

	static int frameBufferAttribute[] = {
		GLX_RGBA,
		GLX_RED_SIZE, 8,
		GLX_GREEN_SIZE, 8,
		GLX_BLUE_SIZE, 8,
		GLX_ALPHA_SIZE, 8,
		GLX_DOUBLEBUFFER,True,
		None
	};

	//code
	gpDisplay=XOpenDisplay(NULL);
	if(gpDisplay==NULL){
		printf("ERROR:Unable to open X Display.\n Exitting now...\n");
		uninitialize();
		exit(EXIT_FAILURE);
	}
	
	defaultScreen=XDefaultScreen(gpDisplay);
	
	gpXVisualInfo = glXChooseVisual(gpDisplay, defaultScreen, frameBufferAttribute);
	//defaultDepth=DefaultDepth(gpDisplay, defaultScreen);
	
	//gpXVisualInfo=(XVisualInfo*) malloc(sizeof(XVisualInfo));
	
	if(gpXVisualInfo==NULL){
		printf("ERROR: Unable to get memory for visual info\n Exitting now\n");
		uninitialize();
		return(EXIT_FAILURE);
	}
	
	/*if(XMatchVisualInfo(gpDisplay,defaultScreen,defaultDepth,TrueColor,gpXVisualInfo)==0)
	{
		printf("ERROR: Unable to get Visual\n Exitting now\n");
		uninitialize();
		return(EXIT_FAILURE);
	}*/
	winAttribs.border_pixmap=0;
	winAttribs.background_pixmap=0;
	winAttribs.border_pixel=0;
	winAttribs.colormap=XCreateColormap(gpDisplay, RootWindow(gpDisplay, gpXVisualInfo->screen),gpXVisualInfo->visual, AllocNone);
	gColorMap=winAttribs.colormap;
	winAttribs.background_pixel=BlackPixel(gpDisplay,defaultScreen);
	winAttribs.event_mask= ExposureMask | VisibilityChangeMask | ButtonPressMask | KeyPressMask | PointerMotionMask | StructureNotifyMask;
	styleMask= CWBorderPixel | CWBackPixel | CWEventMask | CWColormap;
	
	gWindow=XCreateWindow(gpDisplay, RootWindow(gpDisplay, gpXVisualInfo->screen), 0, 0, WIN_WIDTH, WIN_HEIGHT, 0, gpXVisualInfo->depth, InputOutput, gpXVisualInfo->visual, styleMask, &winAttribs);
	
	if(!gWindow) {
		printf("Error :  Failed to create Main window\n Extting now\n");
		uninitialize();
		return(EXIT_FAILURE);
	}

	XStoreName(gpDisplay, gWindow, "Roll no 74 | Sachin Bhadrashette | Texture Smiley");
	Atom windowManagerDelete=XInternAtom(gpDisplay, "WM_DELETE_WINDOW", True);
	XSetWMProtocols(gpDisplay,gWindow, &windowManagerDelete, 1);
	XMapWindow(gpDisplay, gWindow);
}

void ToggleFullscreen(void){
	
	// variable declaration
	Atom wm_state;
	Atom fullscreen;
	XEvent xev={0};

	//code
	wm_state=XInternAtom(gpDisplay, "_NET_WM_STATE", False);
	memset(&xev, 0, sizeof(xev));
	xev.type=ClientMessage;
	xev.xclient.window=gWindow;
	xev.xclient.message_type=wm_state;
	xev.xclient.format=32;
	xev.xclient.data.l[0]=bgFullscreen ? 0 : 1;
	
	fullscreen=XInternAtom(gpDisplay, "_NET_WM_STATE_FULLSCREEN", False);
	xev.xclient.data.l[1]=fullscreen;

	XSendEvent(gpDisplay, RootWindow(gpDisplay, gpXVisualInfo->screen), False, StructureNotifyMask, &xev);

}

void uninitialize(){
	GLXContext currentContext;
	currentContext = glXGetCurrentContext();
	if(currentContext != NULL && currentContext == gGLXContext) {
		glXMakeCurrent(gpDisplay, 0, 0);
	}
	if(gGLXContext){
		glXDestroyContext(gpDisplay, gGLXContext);
	}

	if(gWindow) {
		XDestroyWindow(gpDisplay, gWindow);
	}
	if(gColorMap) {
		XFreeColormap(gpDisplay, gColorMap);
	}
	if(gpXVisualInfo){
		free(gpXVisualInfo);
		gpXVisualInfo=NULL;
	}
	if(gpDisplay){
		XCloseDisplay(gpDisplay);
		gpDisplay=NULL;
	}
}

void initialize(){
	void resize(int, int);
	void LoadGLTextures(GLuint *texture, char *path);

	gGLXContext = glXCreateContext(gpDisplay, gpXVisualInfo, NULL, GL_TRUE);

	glXMakeCurrent(gpDisplay, gWindow, gGLXContext);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glShadeModel(GL_SMOOTH);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glEnable(GL_TEXTURE_2D);
	LoadGLTextures(&Texture_Kundali, "Vijay_Kundali.bmp");
	LoadGLTextures(&Texture_Stone, "Stone.bmp");
	resize(WIN_WIDTH, WIN_HEIGHT);
}


void display() {
	void drawMulticolourPyramid();
	void drawCube();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(-1.5f, 0.0f, -6.0f);
	glRotatef(angleTrangle, 0.0f, 1.0f, 0.0f);
	drawMulticolourPyramid();

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(1.5f, 0.0f, -6.0f);
	glScalef(0.75f, 0.75f, 0.75f);
	glRotatef(angleRect, 1.0f, 1.0f, 1.0f);
	drawCube();
	glXSwapBuffers(gpDisplay,gWindow);
}

void drawMulticolourPyramid() {

	glBindTexture(GL_TEXTURE_2D, Texture_Stone);

	glBegin(GL_TRIANGLES);
	glTexCoord2f(0.5f, 1.0f);
	glVertex3f(0.0f, 1.0f, 0.0f);
	glTexCoord2f(0.0f, 0.0f); 
	glVertex3f(1.0f, -1.0f, 1.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-1.0f, -1.0f, 1.0f);

	glTexCoord2f(0.5f, 1.0f);
	glVertex3f(0.0f, 1.0f, 0.0f);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(1.0f, -1.0f, -1.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.0f, -1.0f, 1.0f);


	glTexCoord2f(0.5f, 1.0f); 
	glVertex3f(0.0f, 1.0f, 0.0f);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(1.0f, -1.0f, -1.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-1.0f, -1.0f, -1.0f);

	glTexCoord2f(0.5f, 1.0f);
	glVertex3f(0.0f, 1.0f, 0.0f);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-1.0f, -1.0f, -1.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-1.0f, -1.0f, 1.0f);
	glEnd();

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-1.0f, -1.0f, -1.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.0f, -1.0f, -1.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(1.0f, -1.0f, 1.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-1.0f, -1.0f, 1.0f);

	glEnd();

}

void drawCube() {
	glBindTexture(GL_TEXTURE_2D, Texture_Kundali);
	glBegin(GL_QUADS);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-1.0f, 1.0f, 1.0f);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-1.0f, -1.0f, 1.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(1.0f, -1.0f, 1.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(1.0f, 1.0f, 1.0f);


	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-1.0f, 1.0f, -1.0f);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-1.0f, -1.0f, -1.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(1.0f, -1.0f, -1.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(1.0f, 1.0f, -1.0f);


	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.0f, 1.0f, -1.0f);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(1.0f, -1.0f, -1.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(1.0f, -1.0f, 1.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(1.0f, 1.0f, 1.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-1.0f, 1.0f, -1.0f);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-1.0f, -1.0f, -1.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-1.0f, -1.0f, 1.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-1.0f, 1.0f, 1.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.0f, 1.0f, -1.0f);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-1.0f, 1.0f, -1.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-1.0f, 1.0f, 1.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(1.0f, 1.0f, 1.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.0f, -1.0f, -1.0f);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-1.0f, -1.0f, -1.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-1.0f, -1.0f, 1.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(1.0f, -1.0f, 1.0f);

	glEnd();

}



void resize(int width, int height) {
	if(height == 0)
		height = 1;
	glViewport(0,0,(GLsizei)width, (GLsizei)height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, (GLfloat)width/(GLfloat)height, 0.1f, 1000.0f);
}

void LoadGLTextures(GLuint *texture, char *path) {
	unsigned char *imageData;
	int width, height;
	glGenTextures(1, texture);
	imageData = SOIL_load_image(path, &width, &height, 0, SOIL_LOAD_RGB);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glBindTexture(GL_TEXTURE_2D, *texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, width, height, GL_BGR_EXT, GL_UNSIGNED_BYTE, imageData);
	SOIL_free_image_data(imageData);
	
}

void rotate(void)
{
	angleTrangle = angleTrangle + 0.09f;
	if (angleTrangle >= 360.0f)
		angleTrangle = 0.0f;

	angleRect = angleRect - 0.09f;
	if (angleRect <= 0.0f)
		angleRect = 360.0f;
}

