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

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(800, 600);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Multicolor Triangle");

	initialize();
	glutDisplayFunc(display);
	glutReshapeFunc(resize);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutCloseFunc(uninitialize);

	glutMainLoop();
}

void display(void) {
	void drawTriangleWithShape(void);
	//to clear all buffer bit
	glClear(GL_COLOR_BUFFER_BIT);
	drawTriangleWithShape();
	//to process buffered opengl routines
	//glFlush();
	glutSwapBuffers();

}

void initialize() {
	// code
	//to select clearing (background) clear
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
}

void keyboard(unsigned char key, int x, int y) {

	switch (key) {
	case 27: // Escape
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
	case GLUT_LEFT_BUTTON:
		break;
	default:
		break;
	}
}


void resize(int width, int height) {
	//TODO	
	if (height == 0)
		height = 1;
	glViewport(0, 0, (GLint)width, (GLint)height);
}

void uninitialize(void) {
	//TODO
}

void drawTriangleWithShape() {
	glBegin(GL_TRIANGLES);
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 1.0f, 0.0f);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(-1.0f, -1.0f, 0.0f);
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(1.0f, -1.0f, 0.0f);
	glEnd();
}












