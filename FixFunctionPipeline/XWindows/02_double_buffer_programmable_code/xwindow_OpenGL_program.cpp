#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<memory.h>

#include<X11/Xlib.h>
#include<X11/Xutil.h>
#include<X11/XKBlib.h>
#include<X11/keysym.h>

#include<GL/gl.h>
#include<GL/glx.h>

#define WIN_WIDTH 800
#define WIN_HEIGHT 600

//namespace
using namespace std;

//global variables
bool bgFullscreen=false;
Display *gpDisplay=NULL;
XVisualInfo *gpXVisualInfo=NULL;
Colormap gColorMap;
Window gWindow;

typedef GLXContext (*glXCreateContextAttribsARBProc)(Display*, GLXFBConfig, GLXContext, Bool, const int*);
glXCreateContextAttribsARBProc glXCreateContextAttribsARB = NULL;
GLXFBConfig gGLXFBConfig;
GLXContext gGLXContext; //paraller to hglrc


FILE *gpFile;

//Enrty Point function
int main(void) {
	//function prototype
	void createWindow(void);
	void ToggleFullscreen(void);
	void uninitialize();
	void resize(int, int);
	void display();
	void initialize();

	//for logging purpose
	gpFile =  fopen("OGLog.txt", "w" );
	if(gpFile == NULL) {
		printf("Error in opening file.");
		return(EXIT_FAILURE);
	}

	
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

	fprintf(gpFile, "Entering into game loop.\n");

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
		
	}
	fprintf(gpFile, "exit from game loop.\n");
	//below code used in OpneGL code
	uninitialize();
	return(0);
}

int createWindow(void){
	fprintf(gpFile, "Entering into create window.\n");
	//function prototypes
	void uninitialize(void);
	
	//variable declaration
	XSetWindowAttributes winAttribs;
	int defaultScreen;
	int defaultDepth;
	int styleMask;
    GLXFBConfig *pGLXFBConfig = NULL;
    GLXFBConfig bestGLXFBConfig;
    XVisualInfo *pTempVisualInfo=NULL;
    int iNUmFBConfigs=0;
    int i;

	static int frameBufferAttributes[] = {
        GLX_X_RENDERABLE, True,
        GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
        GLX_RENDER_TYPE, GLX_RGBA_BIT,
        GLX_X_VISUAL_TYPE, GLX_TRUE_COLOR,
		GLX_RED_SIZE, 8,
		GLX_GREEN_SIZE, 8,
		GLX_BLUE_SIZE, 8,
        GLX_ALPHA_SIZE, 8,
        GLX_DEPTH_SIZE, 24,
        GLX_STENCIL_SIZE, 8,
		GLX_DOUBLEBUFFER,True,
		None
	};

	//code
	gpDisplay=XOpenDisplay(NULL);
	if(gpDisplay==NULL){
		printf("ERROR:Unable to open X Display.\n Exitting now...\n");
		fprintf(gpFile, "ERROR:Unable to open X Display.\n Exitting now...\n");
		uninitialize();
		exit(EXIT_FAILURE);
	}
	
	//defaultScreen=XDefaultScreen(gpDisplay);
	pGLXFBConfig = glXChooseFBConfig(gpDisplay,DefaultScreen(gpDisplay), frameBufferAttributes, &iNUmFBConfigs);
	if(pGLXFBConfig == NULL) {
		printf("ERROR:Unable to get valid frame buffer config.\n Exitting now...\n");
		fprintf(gpFile, "ERROR:Unable to get valid frame buffer config.\n Exitting now...\n");
		uninitialize();
		exit(EXIT_FAILURE);
	}

	printf("%d Matching FB configs found.\n", iNUmFBConfigs);
	fprintf(gpFile, "%d Matching FB configs found.\n", iNUmFBConfigs);

	int bestFrameBufferCongig = -1, worstFrameBufferConfig=-1,bestNumberOfSamples=-1, worstNumberOfSamples=999;
	for(i=0; i<iNUmFBConfigs ; i++) {
		pTempVisualInfo = glXGetVisualFromFBConfig(gpDisplay, pGLXFBConfig[i]);
		if(pTempVisualInfo) {
			int sampleBuffer, samples;
			glXGetFBConfigAttrib(gpDisplay, pGLXFBConfig[i], GLX_SAMPLE_BUFFERS, &sampleBuffer);
			glXGetFBConfigAttrib(gpDisplay, pGLXFBConfig[i], GLX_SAMPLES, &samples);
			printf("Matching Framebuffer config=%d, VisualID=0x%lu, SAMPLE_BUFFERS=%d, SAMPLES=%d\n", i, pTempVisualInfo -> visualid, sampleBuffer, samples);
			fprintf(gpFile, "Matching Framebuffer config=%d, VisualID=0x%lu, SAMPLE_BUFFERS=%d, SAMPLES=%d\n", i, pTempVisualInfo -> visualid, sampleBuffer, samples);
			if(bestFrameBufferCongig <0 || sampleBuffer && samples > bestFrameBufferCongig) {
				bestFrameBufferCongig = i;
				bestNumberOfSamples = samples;
			} 
			if( worstFrameBufferConfig <0 || !samples || samples > worstFrameBufferConfig) {
				worstFrameBufferConfig = i;
				worstNumberOfSamples = samples;
			}
		}
		XFree(pTempVisualInfo);
	}

	bestGLXFBConfig = pGLXFBConfig[bestFrameBufferCongig];
	
	//set global GLXFBConfig
	gGLXFBConfig = bestGLXFBConfig;

	//free FBConfig list allocated by glXChooseFBConfig()
	XFree(pGLXFBConfig);

	gpXVisualInfo = glXGetVisualFromFBConfig(gpDisplay, bestGLXFBConfig);
	
	printf("Chosen visual ID=0x%lu\n", gpXVisualInfo->visualid);
	fprintf(gpFile, "Chosen visual ID=0x%lu\n", gpXVisualInfo->visualid);

	winAttribs.border_pixmap=0;
	winAttribs.background_pixmap=0;
	winAttribs.border_pixel=0;
	
	winAttribs.colormap=XCreateColormap(gpDisplay, RootWindow(gpDisplay, gpXVisualInfo->screen),gpXVisualInfo->visual, AllocNone);
		
	gColorMap=winAttribs.colormap;
	
	//winAttribs.background_pixel=BlackPixel(gpDisplay,defaultScreen);
	printf("here is the call\n");
	winAttribs.event_mask= ExposureMask | VisibilityChangeMask | ButtonPressMask | KeyPressMask | PointerMotionMask | StructureNotifyMask;
	styleMask= CWBorderPixel | CWBackPixel | CWEventMask | CWColormap;
	
	gWindow=XCreateWindow(gpDisplay, RootWindow(gpDisplay, gpXVisualInfo->screen), 0, 0, WIN_WIDTH, WIN_HEIGHT, 0, gpXVisualInfo->depth, InputOutput, gpXVisualInfo->visual, styleMask, &winAttribs);
	
	if(!gWindow) {
		printf("Error :  Failed to create Main window\n Extting now\n");
		uninitialize();
		return(EXIT_FAILURE);
	}

	XStoreName(gpDisplay, gWindow, "Roll no 74 | Sachin Bhadrashette | double buffer | Best FBConfig selection");
	Atom windowManagerDelete=XInternAtom(gpDisplay, "WM_DELETE_WINDOW", True);
	XSetWMProtocols(gpDisplay,gWindow, &windowManagerDelete, 1);
	XMapWindow(gpDisplay, gWindow);
	printf("Returning from create window.\n");
	fprintf(gpFile, "Returning from create window.\n");
}

void ToggleFullscreen(void){
	fprintf(gpFile, "Entering : ToggleFullscreen.\n");
	
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
	fprintf(gpFile, "Returning : ToggleFullscreen.\n");

}

void uninitialize(){
	fprintf(gpFile, "Entering : uninitialize.\n");
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
	fprintf(gpFile, "Closing log file.\n");
	if(fopen) {
		fclose(gpFile);
		gpFile = NULL;
	}
}   

void initialize(){

	void uninitialize(void);
	void resize(int, int);

	//create new gGL Contet 4.5 for rendering

	glXCreateContextAttribsARB = (glXCreateContextAttribsARBProc)glXGetProcAddressARB((GLubyte *)"glXCreateContextAttribsARB");

	GLint attribs[] = {
		GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
		GLX_CONTEXT_MINOR_VERSION_ARB, 1,
		GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB,
		0
	};

	gGLXContext = glXCreateContextAttribsARB(gpDisplay, gGLXFBConfig, 0, True, attribs);

	if(gGLXContext == NULL) { //Fallback to safe old 2.X context
		//When a context vesrion below 3.0 is requested, implementation will return the newest contet version xompatible with OpenGL versions less than version 3.0
		GLint attribs[] = {
			GLX_CONTEXT_MAJOR_VERSION_ARB, 1,
			GLX_CONTEXT_MINOR_VERSION_ARB, 0,
			0
		};
		printf("Failed to create GLX 4.5 context, hence using old -style GLX Context\n");
		fprintf(gpFile, "Failed to create GLX 4.5 context, hence using old -style GLX Context\n");
		gGLXContext = glXCreateContextAttribsARB(gpDisplay, gGLXFBConfig, 0, True, attribs);
	}
	else {
		printf("OpenGL Context 4.5 is created.\n");
		fprintf(gpFile, "OpenGL Context 4.5 is created.\n");
	}

	if(!glXIsDirect(gpDisplay, gGLXContext)) {
		printf("Indirect GLX rendering context obtained\n");
		fprintf(gpFile, "Indirect GLX rendering context obtained\n");
	} else {
		printf("Direct GLX rendering context obtained\n");
		fprintf(gpFile, "Direct GLX rendering context obtained\n");
	}

	glXMakeCurrent(gpDisplay, gWindow, gGLXContext);

	//code
	glShadeModel(GL_SMOOTH);

	//setup depth buffer 
	glClearDepth(1.0f);

	//eable depth testing
	glEnable(GL_DEPTH_TEST);

	//depth test to do
	glDepthFunc(GL_LEQUAL);

	//set really nice perspective calculations
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	//We will always cull back face for better performance
	glEnable(GL_CULL_FACE);

	//set background clearing color
	glClearColor(0.0f, 0.0f, 1.0f, 0.0f);

	resize(WIN_WIDTH, WIN_HEIGHT);
}

void display() {
	//fprintf(gpFile, "Entering : display function.\n");
	//code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glXSwapBuffers(gpDisplay,gWindow);
	//fprintf(gpFile, "returning : display function.\n");
}

void resize(int width, int height) {
	fprintf(gpFile, "Entering : resize.\n");
	if(height == 0)
		height = 1;
	glViewport(0,0,(GLsizei)width, (GLsizei)height);
	fprintf(gpFile, "returning : resize.\n");
}

