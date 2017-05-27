#include <GL/glut.h>
#include <cmath>
#include <ctype.h>

#define PI 3.141592653589793
#define WIDTH_WINDOW 600
#define HEIGHT_WINDOW 600
#define CAMERA_ZOOM_FACTOR 2.5f // fator de zoom para a camera
#define CUBE_SCALE_FACTOR 1.2f // fator de escala para o cubo
#define SPHERE_ROT_ANGLE PI/2 // angulo de rotaçao para a esfera
#define TEAPOT_TRANSLATE_FACTOR 2.0f // fator de translaçao para o teapot

/* Variáveis globais para operaçoes na camera (zoom in/out e movimento) */
GLfloat fNear; // distancia de proximidade do frustum
bool mouseDown = false; // guarda se o mouse está pressionado
float xRot = 0.0f, yRot = 0.0f; // angulos em x e y da rotacao da camera
float xDiff = 0.0f, yDiff = 0.0f; // auxiliares para rotacionar a camera ao arrastar mouse

/* Variáveis globais para transformaçoes nos 3 objetos */
GLfloat cubeScale = 1.0f; // armazena a escala atual do cubo
GLfloat sphereRotX = .0f, sphereRotY = .0f, sphereRotZ = .0f; // armazenam os angulos atuais de rotaçao da esfera
GLfloat teapotTranslateX = .0f, teapotTranslateY = .0f, teapotTranslateZ = .0f ; // armazenam as atuais translaçoes do teapot

void Initialize()
{
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  fNear = 10.0f;
}

void Draw(void)
{
  glClear(GL_COLOR_BUFFER_BIT);

  glPushMatrix(); // iniciando transformaçao no cubo
  glScalef(cubeScale, cubeScale, cubeScale); // mesma escala para todos os eixos
  glColor3f(0.0f, 0.3f, 1.0f);
  glutWireCube(20);
  glPopMatrix(); // encerrando transformaçao no cubo
  
  glPushMatrix(); // iniciando transformaçao na esfera
  glRotatef(sphereRotZ, 0.0, 0.0, 1.0); 
  glRotatef(sphereRotY, 0.0, 1.0, 0.0); 
  glRotatef(sphereRotX, 1.0, 0.0, 0.0); 
  glColor3f(.6f, 0.0f, 1.0f);
  glutWireSphere(8.0f, 25, 25);
  glPopMatrix(); // encerrando transformaçao na esfera

  glPushMatrix(); // iniciando transformaçao no teapot
  glTranslatef(teapotTranslateX, teapotTranslateY, teapotTranslateZ); 
  glColor3f(.8f, 0.3f, .0f);
  glutWireTeapot(4.0f);
  glPopMatrix(); // encerrando transformaçao no teapot

  glutSwapBuffers();
}

void EspecificaParametrosVisualizacao(void)
{
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glFrustum(-1.0f, 1.0f, -1.0f, 1.0f, fNear, 600.0f);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(0, 80, 200, 0, 0, 0, 0, 1, 0);
  glRotatef(xRot, 1.0f, 0.0f, 0.0f);
  glRotatef(yRot, 0.0f, 1.0f, 0.0f);
  Draw(); 
}

// Função callback chamada para gerenciar eventos de teclado
void GerenciaTeclado(unsigned char key, int x, int y)
{
  switch (tolower(key)) {
    /* Zoom da camera */
    case '-': // zoom out
      fNear -= CAMERA_ZOOM_FACTOR;
      break;
    case '+': // zoom in
      fNear += CAMERA_ZOOM_FACTOR;
      break;

    /* Escala do cubo */
    case 't':
        cubeScale /= CUBE_SCALE_FACTOR;  
      break;
    case 'y':
        cubeScale *= CUBE_SCALE_FACTOR;  
      break;

    /* Rotaçao da esfera */
    case 'q':
        sphereRotX -= SPHERE_ROT_ANGLE;  
      break;
    case 'w':
        sphereRotX += SPHERE_ROT_ANGLE;  
      break;
    case 'a':
        sphereRotY -= SPHERE_ROT_ANGLE;  
      break;    
    case 's':
        sphereRotY += SPHERE_ROT_ANGLE;  
      break;
    case 'z':
        sphereRotZ -= SPHERE_ROT_ANGLE;  
      break;
    case 'x':
        sphereRotZ += SPHERE_ROT_ANGLE;  
      break;

    /* Translaçao do teapot */
    case 'o':
        teapotTranslateX -= TEAPOT_TRANSLATE_FACTOR;  
      break;
    case 'p':
        teapotTranslateX += TEAPOT_TRANSLATE_FACTOR;  
      break;
    case 'k':
        teapotTranslateY -= TEAPOT_TRANSLATE_FACTOR;  
      break;    
    case 'l':
        teapotTranslateY += TEAPOT_TRANSLATE_FACTOR;  
      break;
    case 'n':
        teapotTranslateZ -= TEAPOT_TRANSLATE_FACTOR;  
      break;
    case 'm':
        teapotTranslateZ += TEAPOT_TRANSLATE_FACTOR;  
      break;
  }

  EspecificaParametrosVisualizacao();
  glutPostRedisplay();
}

// Função callback chamada para gerenciar eventos de clique do mouse
void mouse(int button, int state, int x, int y)
{
  if (button == GLUT_LEFT_BUTTON & state == GLUT_DOWN)
  {
    mouseDown = true;

    xDiff = x - yRot;
    yDiff = -y + xRot;
  }
  else
    mouseDown = false;
}

// Função callback chamada para gerenciar eventos quando o mouse é arrastado (mover a camera)
void mouseMotion(int x, int y)
{
  if (mouseDown)
  {
    yRot = x - xDiff;
    xRot = y + yDiff;

    glutPostRedisplay();
  }
}

// Funçao callback chamada quando o tamanho da janela é alterado 
void AlteraTamanhoJanela(GLsizei w, GLsizei h)
{
  // Para previnir uma divisao por zero
  if (h == 0) h = 1;

  // Especifica o tamanho da viewport
  glViewport(0, 0, w, h);

  EspecificaParametrosVisualizacao();
}

int main(int argc, char** argv)
{
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
  glutInitWindowSize(WIDTH_WINDOW, HEIGHT_WINDOW);
  glutInitWindowPosition((glutGet(GLUT_SCREEN_WIDTH)-WIDTH_WINDOW)/2, 
    (glutGet(GLUT_SCREEN_HEIGHT)-HEIGHT_WINDOW)/2);  
  glutCreateWindow("Trabalho 2 OpenGL");

  glutDisplayFunc(EspecificaParametrosVisualizacao);
  glutKeyboardFunc(GerenciaTeclado);
  glutMouseFunc(mouse);
  glutMotionFunc(mouseMotion);
  glutReshapeFunc(AlteraTamanhoJanela);

  Initialize();
  glutMainLoop();

  return EXIT_SUCCESS;
}