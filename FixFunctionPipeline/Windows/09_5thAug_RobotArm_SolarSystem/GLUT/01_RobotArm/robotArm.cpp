#include<GL/freeglut.h>
#include<GL/glu.h>
#include<stdlib.h>

//global variable
bool bFullScreen = false;

static int shoulder = 0, elbow = 0;


//main
int main(int argc, char** argv)
{
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
	glutCreateWindow("Roll No 74 | Sachin BHadrashette | Robot Arm");

	initialize();

	glutDisplayFunc(display);
	glutReshapeFunc(resize);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutCloseFunc(uninitialize);

	glutMainLoop();
}//main

void display(void)
{
	//code
	//to clear all pixel
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(1.0f, 1.0f, .0f);

	glPushMatrix();
		glTranslatef(-1.0f, 0.0f, 0.0f);
		glRotatef((GLfloat)shoulder, 0.0f, 0.0f, 1.0f);
		glTranslatef(1.0f, 0.0f, 0.0f);

		glPushMatrix();
			glScalef(2.0f, 0.4f, 1.0f);
			glutWireCube(1.0f);
		glPopMatrix();

		glTranslatef(1.0f, 0.0f, 0.0f);
		glRotatef((GLfloat)elbow, 0.0f, 0.0f, 1.0f);
		glTranslatef(1.0f, 0.0f, 0.0f);

		glPushMatrix();
			glScalef(2.0f, 0.4f, 1.0f);
			glutWireCube(1.0f);
		glPopMatrix();

	glPopMatrix();

	glutSwapBuffers();
}

void initialize(void)
{
	//code

	//to select clearing (background)
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	glShadeModel(GL_FLAT);
}

void keyboard(unsigned char key, int x, int y)
{
	//code
	switch (key)
	{
	case 27://Escape
		glutLeaveMainLoop();
		break;

	case 'f':
	case 'F':
		if (bFullScreen == false)
		{
			glutFullScreen();
			bFullScreen = true;
		}
		else
		{
			glutLeaveFullScreen();
			bFullScreen = false;
		}
		break;

	case 's': //s key rotate at shoulder
		shoulder = (shoulder + 5) % 360;
		glutPostRedisplay();
		break;

	case 'S': //S key rotate at shoulder
		shoulder = (shoulder - 5) % 360;
		glutPostRedisplay();
		break;

	case 'e': //e key rotate at elbow
		elbow = (elbow + 5) % 360;
		glutPostRedisplay();
		break;

	case 'E': //E key rotate at elbow
		elbow = (elbow - 5) % 360;
		glutPostRedisplay();
		break;


	default:
		break;
	}//outer switch
}//keyboard

void mouse(int button, int state, int x, int y)
{
	//code
	switch (button)
	{
	case GLUT_LEFT_BUTTON:
		break;

	default:
		break;
	}
}

void resize(int width, int height)
{
	//code
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(60.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, -5.0f);
}

void uninitialize(void)
{
	//code
}
