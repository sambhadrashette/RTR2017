#include<GL/freeglut.h>

// Global variable declaration
bool bFullScreen = false;

int main(int argc, char** argv) {

	//function prototypes
	void display(void);
	void resize(int, int);
	void keyboard(unsigned char, int, int);
	void mouse(int, int, int, int);
	void initialize(void);
	void uninitialize(void);

	//code
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(800, 600);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("OpenGL First Window : Hello World !!");

	initialize();
	glutDisplayFunc(display);
	glutReshapeFunc(resize);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutCloseFunc(uninitialize);

	glutMainLoop();
}

void display(void) {
	//to clear all buffer bit
	glClear(GL_COLOR_BUFFER_BIT);

	//to process buffered opengl routines
	glFlush();
}

void initialize() {
	// code
	//to select clearing (background) clear
	glClearColor(0.0f, 0.0f, 1.0f, 0.0f);
}

void keyboard(unsigned char key, int x, int y) {

	switch (key) {
	case 27 : // Escape
		glutLeaveMainLoop();
	case 'F':
	case 'f':
		if (bFullScreen == false) {
			glutFullScreen();
			bFullScreen = true;
		}
		else {
			glutLeaveFullScreen();
			bFullScreen = false;
		}
		break;
	default:
		break;
	}
	
}

void mouse(int button, int state, int x, int y) {
	switch (button) {
	case GLUT_LEFT_BUTTON : 
		break;
	default:
		break;
	}
}


void resize(int width, int height) {
	//TODO	
}

void uninitialize(void) {
	//TODO
}












