#include<GL/freeglut.h>
#include<GL/glu.h>

//global variable
bool bFullScreen = false;

static int year = 0, day = 0;


//main
int main(int argc, char** argv)
{
	void display(void);
	void resize(int, int);
	void keyboard(unsigned char, int, int);
	void mouse(int, int, int, int);
	void initialize(void);
	void uninitialize(void);
	
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(800, 600);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Roll No 74 | Sachin Bhadrashette | SolarSystem");

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
	
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(1.0f, 1.0f, 1.0f);

	glPushMatrix();
	glColor3f(1.0f, 1.0f, 0.0f);
	glutWireSphere(1.0f, 20, 16);//draw sun
	glRotatef((GLfloat)year, 0.0f, 1.0f, 0.0f);
	glTranslatef(2.0f,0.0f, 0.0f);
	glRotatef((GLfloat)day, 1.0f, 0.0f, 0.0f);
	glColor3f(0.0f, 1.0f, 0.0f);
	glutWireSphere(0.3f, 10, 8);
	glPopMatrix();

	glutSwapBuffers();
}

void initialize(void)
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);//blue

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

	case 'd': 
		day = (day + 10) % 360;
		glutPostRedisplay();
		break;

	case 'D': 
		day = (day - 10) % 360;
		glutPostRedisplay();
		break;

	case 'y':
		year = (year + 5) % 360;
		glutPostRedisplay();
		break;

	case 'Y':
		year = (year - 5) % 360;
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

	gluPerspective(60.0f, (GLfloat)width / (GLfloat)height,0.1f,100.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(0.0f, 0.0f, 5.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
}

void uninitialize(void)
{
	//code
}
