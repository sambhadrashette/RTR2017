#include<GL/freeglut.h>

// Global variable declaration
bool bFullScreen = false;

struct Color {
	GLfloat red;
	GLfloat green;
	GLfloat blue;
};

struct Vertex {
	GLfloat x;
	GLfloat y;
	GLfloat z;
};

struct Color colors[] = {
	{ 0.0f, 0.0f, 0.0f },
	{ 1.0f, 0.0f, 0.0f },
	{ 0.0f, 1.0f, 0.0f },
	{ 0.0f, 0.0f, 1.0f },
	{ 1.0f, 1.0f, 0.0f },
	{ 1.0f, 0.0f, 1.0f },
	{ 0.0f, 1.0f, 1.0f },
	{ 0.75f, 0.75f, 0.75f },
	{ 1.0f, 165.0f/255.0f, 1.0f },
	{ 0.50f, 0.50f, 0.50f }
};


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
	glutCreateWindow("Rectangles !");

	initialize();
	glutDisplayFunc(display);
	glutReshapeFunc(resize);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutCloseFunc(uninitialize);

	glutMainLoop();
}

void display(void) {
	void drawRectangleWithLines(Color, GLfloat);
	//to clear all buffer bit
	glClear(GL_COLOR_BUFFER_BIT);
	GLfloat reduction = 0.09f;
	for (int i = 0; i < 10; i++) {
		drawRectangleWithLines(colors[i], reduction);
		reduction = reduction + 0.09;
	}
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

void drawRectangleWithLines(Color color, GLfloat reducBy) {
	glBegin(GL_LINES);
	glColor3f(color.red, color.green, color.blue);
	glVertex3f(-(1.0f - reducBy), 1.0f-reducBy, 0.0f - reducBy);
	glVertex3f(-(1.0f - reducBy), -(1.0f - reducBy), 0.0f - reducBy);

	glVertex3f(-(1.0f - reducBy), -(1.0f - reducBy), 0.0f - reducBy);
	glVertex3f((1.0f - reducBy), -(1.0f - reducBy), 0.0f - reducBy);

	glVertex3f((1.0f - reducBy), -(1.0f - reducBy), 0.0f - reducBy);
	glVertex3f(1.0f - reducBy, 1.0f - reducBy, 0.0f - reducBy);

	glVertex3f(1.0f - reducBy, 1.0f - reducBy, 0.0f - reducBy);
	glVertex3f(-(1.0f - reducBy), 1.0f - reducBy, 0.0f - reducBy);
	
	glEnd();
}












 