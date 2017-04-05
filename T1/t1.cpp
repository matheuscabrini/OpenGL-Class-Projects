#include <gl/glut.h>
#include <cmath>
#include <algorithm>

#define PI 3.141592653589793
#define WIDTH_WINDOW 800
#define HEIGHT_WINDOW 600
#define N_OBJECTS 5
#define DEBUG true

using namespace std;

const float RAD_TO_DEG = 180 / PI;
const float DEG_TO_RAD = PI / 180;
const float RADIUS_OBJECTS = 0.5 * min(WIDTH_WINDOW, HEIGHT_WINDOW) / N_OBJECTS;

struct Point {
	GLfloat x;
	GLfloat y;
};

enum Movement { RIGHT, LEFT };

struct MovingObject {
	Point p;
	GLfloat velocity;
	GLfloat angle;
	Movement mov;
};

Point pMouse = { .0f, .0f }; // Stores the coordinates of a mouse click
Point pStartObj[N_OBJECTS]; // Array of starting points for the objects
MovingObject obj[N_OBJECTS]; // Array of objects which will be drawn on screen

/* Draws a filled star with parameters: center point p, radius, number of star endpoints numPoints,
and randColor=true if the fill colors should be randomized each drawing frame.
*/
void DrawStarFilled(Point p, GLfloat radius, int numPoints, bool randColor) {
	GLfloat r = 0.0f, g = 0.0f, b = 1.0f;
	glBegin(GL_TRIANGLE_FAN);
	glColor3f(r, g, b);
	glVertex2f(p.x, p.y);

	int count = 1;
	for (int i = 0; i <= numPoints * 2; i++) {
		float angle = i * 360.0 / (numPoints * 2) * DEG_TO_RAD;
		
		// Determining fill color
		if (randColor == true) {
			r = ((GLfloat)rand() / (RAND_MAX));
			g = ((GLfloat)rand() / (RAND_MAX));
			b = ((GLfloat)rand() / (RAND_MAX));
		}
		else {
			if (b != .0f) { b = .0f; r = 1.0f; }
			else if (r != .0f) { r = .0f; g = 1.0f; }
			else if (g != .0f) { g = .0f; b = 1.0f; }
		}
		glColor3f(r, g, b);

		// Drawing the star points 
		if (count % 2 != 0) glVertex2d(p.x + cos(angle) * radius, p.y + sin(angle) * radius);
		else glVertex2d((p.x + cos(angle) * radius/2), (p.y + sin(angle) * radius/2));

		count++;
	}
	glEnd();
}

/* Draws a line from point p1 to point p2.
*/
void DrawLine(Point p1, Point p2) {
	GLfloat dx = p2.x - p1.x;
	GLfloat dy = p2.y - p1.y;
	GLfloat x1 = p1.x;
	GLfloat y1 = p1.y;
	GLfloat step = 0;

	if (abs(dx) > abs(dy)) { step = abs(dx); }
	else { step = abs(dy); }

	GLfloat xInc = dx / step;
	GLfloat yInc = dy / step;

	glBegin(GL_POINTS);
	glColor3f(1.0f, 1.0f, 1.0f);
	for (float i = 1; i <= step; i++) {
		glVertex2f(x1, y1);
		x1 += xInc;
		y1 += yInc;
	}
	glEnd();
}

/* Draws a filled circle with center point p and a given radius.
*/
void DrawCircleFilled(Point p, GLfloat radius) {
	int triangleAmount = 20; // number of triangles used to draw circle

	glBegin(GL_TRIANGLE_FAN);
	glColor3f(.0f, .0f, 1.0f);
	glVertex2f(p.x, p.y); // center of circle
	for (int i = 0; i <= triangleAmount; i++) {
		glVertex2f(
			p.x + (radius * cos(i * 2.0f * PI / triangleAmount)),
			p.y + (radius * sin(i * 2.0f * PI / triangleAmount))
		);
	}
	glEnd();
}

/* Initiates the moving objects' radius, positions and velocities.
*/
void initObjects() {
	GLfloat vObj[N_OBJECTS];
	GLfloat wO = RADIUS_OBJECTS, hO = RADIUS_OBJECTS;
	GLfloat wF = WIDTH_WINDOW - RADIUS_OBJECTS;
	GLfloat hF = HEIGHT_WINDOW - RADIUS_OBJECTS;
	GLfloat xStep = (wF - wO) / N_OBJECTS;
	GLfloat yStep = (hF - hO) / N_OBJECTS;

	for (int i = 0; i < N_OBJECTS; i++) {
		pStartObj[i] = { (i+1) * xStep, (i+1) * yStep }; // each object's starting position is different
		vObj[i] = ((GLfloat) i + 1) / N_OBJECTS; // each object's velocity is different
		obj[i] = { pStartObj[i], vObj[i], .0f , RIGHT }; // starting angle is 0, and direction is right
		if (DEBUG) printf("init obj%d: x %f, y %f, v %f\n", i, pStartObj[i].x, pStartObj[i].y, vObj[i]);
	};
}

/* Sets initial visualization configurations 
*/
void initVisualization() {
	glPointSize(1.0f);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0f, (GLfloat) WIDTH_WINDOW, 0.0f, (GLfloat) HEIGHT_WINDOW);
}

/* Callback for glutDisplayFunc. Draws the objects (starting points 
and trajectory lines too, if in debug mode)
*/
void Display(void) {
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glClear(GL_COLOR_BUFFER_BIT);

	for (int i = 0; i < N_OBJECTS; i++) {
		DrawStarFilled(obj[i].p, RADIUS_OBJECTS, 5, true);
		if (DEBUG) {
			DrawCircleFilled(pStartObj[i], RADIUS_OBJECTS/10);
			DrawLine(pStartObj[i], pMouse);
		}
	}
	glFlush();
}

/* Changes the movement direction of an object of index i.
Used when mouse click is detected or object moves off screen.
*/
void UpdateObjectAngle(int i) {
	GLfloat dx = (GLfloat) (pMouse.x - obj[i].p.x);
	GLfloat dy = (GLfloat) (pMouse.y - obj[i].p.y);
	obj[i].angle = (GLfloat) atan(dy/dx);
		
	if (dx >= 0) { obj[i].mov = RIGHT; }
	else { obj[i].mov = LEFT; }

	if (DEBUG) printf("obj%d: angle %f, dx %f, dy %f\n", i, obj[i].angle*RAD_TO_DEG, dx, dy);
}

/* Callback for glutMouseFunc. If a mouse click is detected, we get it's
coordinates and make the objects change trajectory to the direction of the click.
*/
void MouseHandler(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		pMouse.x = x;
		pMouse.y = HEIGHT_WINDOW - y;
		if (DEBUG) printf("pMouse: %f , %f\n", pMouse.x, pMouse.y);

		// Updating objects' trajectory
		for (int i = 0; i < N_OBJECTS; i++) {
			UpdateObjectAngle(i);
		}
	}
}

/*
Checks if an object of index i has disappeared off screen.
*/
bool OutOfScreen(int i) {
	if (obj[i].p.x >= WIDTH_WINDOW + RADIUS_OBJECTS || obj[i].p.x <= - RADIUS_OBJECTS || // out by right or left sides
		obj[i].p.y >= HEIGHT_WINDOW + RADIUS_OBJECTS || obj[i].p.y <= - RADIUS_OBJECTS) // out by top or bottom sides
		return true;
	else return false;
}

/* Callback for glutTimerFunc. Updates the objects' positions for the next drawing frame.  
*/
void TranslateInLine(int step) {
	for (int i = 0; i < N_OBJECTS; i++) {
		// If an object disappears off screen, we put it back to it's 
		// starting position and in direction to the mouse click
		if (OutOfScreen(i)) {
			obj[i].p.x = pStartObj[i].x;
			obj[i].p.y = pStartObj[i].y;
			UpdateObjectAngle(i);
		}
		else {
			obj[i].p.x += (obj[i].mov == RIGHT ? 1 : -1) * obj[i].velocity * cos(obj[i].angle);
			obj[i].p.y += (obj[i].mov == RIGHT ? 1 : -1) * obj[i].velocity * sin(obj[i].angle);
		}
	}

	glutPostRedisplay(); // Makes GLUT redraw scene
	glutTimerFunc(1, TranslateInLine, 1); // Sets the timer again
}

int main(int argc, char **argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowPosition(300, 50);
	glutInitWindowSize(WIDTH_WINDOW, HEIGHT_WINDOW);
	glutCreateWindow("Mouse Func");

	initObjects();
	initVisualization();

	glutDisplayFunc(Display);
	glutMouseFunc(MouseHandler);
	glutTimerFunc(1, TranslateInLine, 100);
	
	glutMainLoop();

	return 0;
}