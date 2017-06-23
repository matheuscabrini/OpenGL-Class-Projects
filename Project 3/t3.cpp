#include <GL/glut.h>
#include <cmath>
#include <ctype.h>

#define PI 3.141592653589793
#define WIDTH_WINDOW 600
#define HEIGHT_WINDOW 600
#define CAMERA_ZOOM_FACTOR 2.5f // fator de zoom para a camera
#define TORUS_SCALE_FACTOR 1.2f // fator de escala para o torus
#define SPHERE_ROT_ANGLE PI/2 // angulo de rotaçao para a esfera
#define TEAPOT_TRANSLATE_FACTOR 2.0f // fator de translaçao para o teapot

/* Variáveis globais para operaçoes na camera (zoom in/out e movimento) */
GLfloat fNear = 10.0f; // distancia de proximidade do frustum
bool mouseDown = false; // guarda se o mouse está pressionado
float xRot = 0.0f, yRot = 0.0f; // angulos em x e y da rotacao da camera
float xDiff = 0.0f, yDiff = 0.0f; // auxiliares para rotacionar a camera ao arrastar mouse

/* Variáveis globais para transformaçoes nos 3 objetos */
GLfloat torusScale = 1.0f; // armazena a escala atual do torus
GLfloat sphereRotX = .0f, sphereRotY = .0f, sphereRotZ = .0f; // armazenam os angulos atuais de rotaçao da esfera
GLfloat teapotTranslateX = 15.0f, teapotTranslateY = .0f, teapotTranslateZ = .0f ; // armazenam as atuais translaçoes do teapot
GLfloat sphereTranslateX = .0f, sphereTranslateY = .0f, sphereTranslateZ = 15.0f ; // armazenam as atuais translaçoes do torus
GLfloat torusTranslateX = .0f, torusTranslateY = 10.0f, torusTranslateZ = .0f ; // armazenam as atuais translaçoes da esfera

void Initialize()
{
  // Define a cor de fundo da janela de visualização como branca
  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);  

  // Habilita a definição da cor do material a partir da cor corrente
  glEnable(GL_COLOR_MATERIAL);

  // Habilita o uso de iluminação, e as luzes de numero 0 até 2
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_LIGHT1);
  glEnable(GL_LIGHT2);

  // Habilita o depth-buffering
  glEnable(GL_DEPTH_TEST);

  // Habilita o modelo de colorização de Gouraud
  glShadeModel(GL_SMOOTH);

  // Modo de normalizaçao nas operaçoes de iluminaçao, a fim de impedir que
  // as transformaçoes de escala no torus falhem o cálculo de cores
  glEnable(GL_NORMALIZE);
}

void DefMaterial(GLint especMaterial) {
  // Capacidade de brilho do material
  GLfloat especularidade[4] = { 1.0, 1.0, 1.0, 1.0 };

  // Define a refletância do material 
  glMaterialfv(GL_FRONT, GL_SPECULAR, especularidade);

  // Define a concentração do brilho
  glMateriali(GL_FRONT, GL_SHININESS, especMaterial);
}

void DefIluminacao0(void)
{
  GLfloat luzAmbiente[4] = { 0.2, 0.2, 0.2, 1.0 };
  GLfloat luzDifusa[4] = { 0.0, 0.0, 1.0, 1.0 };
  GLfloat luzEspecular[4] = { 1.0, 1.0, 1.0, 1.0 };
  GLfloat posicaoLuz[4] = { 0.0, 0.0, 250.0, 1.0 };

  // Ativa o uso da luz ambiente 
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, luzAmbiente);

  // Define os parâmetros da luz de número 0
  glLightfv(GL_LIGHT0, GL_AMBIENT, luzAmbiente);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, luzDifusa);
  glLightfv(GL_LIGHT0, GL_SPECULAR, luzEspecular);
  glLightfv(GL_LIGHT0, GL_POSITION, posicaoLuz);
}

void DefIluminacao1(void)
{
  GLfloat luzAmbiente[4] = { 0.2, 0.2, 0.2, 1.0 };
  GLfloat luzDifusa[4] = { 0.0, 1.0, 0.0, 1.0 };
  GLfloat luzEspecular[4] = { 1.0, 1.0, 1.0, 1.0 };
  GLfloat posicaoLuz[4] = { 250.0, 0.0, 0.0, 1.0 };

  // Ativa o uso da luz ambiente 
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, luzAmbiente);

  // Define os parâmetros da luz de número 1
  glLightfv(GL_LIGHT1, GL_AMBIENT, luzAmbiente);
  glLightfv(GL_LIGHT1, GL_DIFFUSE, luzDifusa);
  glLightfv(GL_LIGHT1, GL_SPECULAR, luzEspecular);
  glLightfv(GL_LIGHT1, GL_POSITION, posicaoLuz);
}

void DefIluminacao2(void)
{
  GLfloat luzAmbiente[4] = { 0.2, 0.2, 0.2, 1.0 };
  GLfloat luzDifusa[4] = { 1.0, 0.0, 0.0, 1.0 };
  GLfloat luzEspecular[4] = { 1.0, 1.0, 1.0, 1.0 };
  GLfloat posicaoLuz[4] = { -250.0, 00.0, -250.0, 1.0 };

  // Ativa o uso da luz ambiente 
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, luzAmbiente);

  // Define os parâmetros da luz de número 2
  glLightfv(GL_LIGHT2, GL_AMBIENT, luzAmbiente);
  glLightfv(GL_LIGHT2, GL_DIFFUSE, luzDifusa);
  glLightfv(GL_LIGHT2, GL_SPECULAR, luzEspecular);
  glLightfv(GL_LIGHT2, GL_POSITION, posicaoLuz);
}

void Draw(void)
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Chamando as funçoes que especificam os parâmetros
  // das três fontes de iluminação na cena
  DefIluminacao0();
  DefIluminacao1();
  DefIluminacao2();

  // Desenhando eixos xyz
  glBegin(GL_LINES);
  glColor3d(.0, .0, .0);
  DefMaterial(90);
  glVertex3d(0.0, 0.0, 0.0);
  glVertex3d(200.0, 0.0, 0.0);
  glVertex3d(0.0, 0.0, 0.0);
  glVertex3d(0.0, 200.0, 0.0);
  glVertex3d(0.0, 0.0, 0.0);
  glVertex3d(0.0, 0.0, 200.0);
  glEnd();

  glPushMatrix(); // iniciando transformaçao no torus
  glTranslatef(torusTranslateX, torusTranslateY, torusTranslateZ); 
  glScalef(torusScale, torusScale, torusScale); // mesma escala para todos os eixos
  glColor3f(0.0f, 0.3f, 1.0f);
  DefMaterial(20);
  glutSolidTorus(2.0, 3.5, 20, 40); 
  glPopMatrix(); // encerrando transformaçao no torus
  
  glPushMatrix(); // iniciando transformaçao na esfera
  glTranslatef(sphereTranslateX, sphereTranslateY, sphereTranslateZ);  
  glRotatef(sphereRotZ, 0.0, 0.0, 1.0); 
  glRotatef(sphereRotY, 0.0, 1.0, 0.0); 
  glRotatef(sphereRotX, 1.0, 0.0, 0.0);
  glColor3f(.6f, 0.0f, 1.0f);
  DefMaterial(90);
  glutSolidSphere(5.0f, 25, 25);
  glPopMatrix(); // encerrando transformaçao na esfera

  glPushMatrix(); // iniciando transformaçao no teapot
  glTranslatef(teapotTranslateX, teapotTranslateY, teapotTranslateZ); 
  glColor3f(.8f, 0.3f, .0f);
  DefMaterial(5);
  glutSolidTeapot(4.0f);
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

    /* Mudança de tonalizaçao */
    case 'g': // tonalizacao Gouraud
      glShadeModel(GL_SMOOTH);
      break;
    case 'h': // tonalizaçao Flat
      glShadeModel(GL_FLAT);
      break;

    /* Zoom da camera */
    case '-': // zoom out
      fNear -= CAMERA_ZOOM_FACTOR;
      break;
    case '+': // zoom in
      fNear += CAMERA_ZOOM_FACTOR;
      break;

    /* Escala do torus */
    case 't':
        torusScale /= TORUS_SCALE_FACTOR;  
      break;
    case 'y':
        torusScale *= TORUS_SCALE_FACTOR;  
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
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize(WIDTH_WINDOW, HEIGHT_WINDOW);
  glutInitWindowPosition((glutGet(GLUT_SCREEN_WIDTH)-WIDTH_WINDOW)/2, 
    (glutGet(GLUT_SCREEN_HEIGHT)-HEIGHT_WINDOW)/2);  
  glutCreateWindow("Trabalho 3 OpenGL");

  glutDisplayFunc(EspecificaParametrosVisualizacao);
  glutKeyboardFunc(GerenciaTeclado);
  glutMouseFunc(mouse);
  glutMotionFunc(mouseMotion);
  glutReshapeFunc(AlteraTamanhoJanela);
  Initialize();
  glutMainLoop();

  return EXIT_SUCCESS;
}