#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>


#define GL_SILENCE_DEPRECATION

#if defined(__APPLE__) || defined(MACOSX)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "glm.h"

/**************************************
************* CONSTANTE PI ************
**************************************/

#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795
#endif

/**************************************
* AUXILIARES CONVERSÃO GRAUS-RADIANOS *
**************************************/

#define RAD(x)          (M_PI*(x)/180)
#define GRAUS(x)        (180*(x)/M_PI)

#define DEBUG 1

#define	GAP					              25

#define	OBJETO_ALTURA		        0.4
#define OBJETO_VELOCIDADE	      10
#define OBJETO_ROTACAO		      0.1
#define OBJETO_RAIO		          0.12
#define EYE_ROTACAO			        0.1

#define LIMITE_ESQUERDA		      -2.7f
#define LIMITE_DIREITA		      3.69f

#define NOME_TEXTURA_CHAO     "C:/Github/Multimedia1/template_projeto/data/Tileable-Road-Texture.ppm"
// "C:\\Users\\User\\Desktop\\UFP\\Git-Hub\\Multimedia1\\template_projeto\\data\\Tileable-Road-Texture.ppm"
// C:/Github/Multimedia1/template_projeto/data/Tileable-Road-Texture.ppm    
#define MARIO "C:/Github/Multimedia1/template_projeto/data/textures/mariotex.ppm"
// C:\Users\User\Desktop\UFP\Git-Hub\Multimedia1\template_projeto\data\textures\mariotex.ppm
// C:/Github/Multimedia1/template_projeto/data/textures/mariotex.ppm

#define CAIXA "C:/Github/Multimedia1/template_projeto/data/caixa.ppm"
// C:\Users\User\Desktop\UFP\Git-Hub\Multimedia1\template_projeto\data\textures\mariotex.ppm
// C:/Github/Multimedia1/template_projeto/data/textures/mariotex.ppm



#define NUM_TEXTURAS              3
#define ID_TEXTURA_CHAO           0
#define ID_TEXTURA_MARIO          1
#define ID_TEXTURA_CAIXA          2

#define	CHAO_DIMENSAO		          50

#define NUM_JANELAS               2
#define JANELA_TOP                0
#define JANELA_NAVIGATE           1

#define STEP                      1

#define qntyCubos                 50

/**************************************
********** VARIÁVEIS GLOBAIS **********
**************************************/

typedef struct {
  GLboolean   up,down,left,right,a,d,s;
} Teclas;

typedef struct {
    GLfloat    x,y,z;
} Posicao;

typedef struct {
    Posicao   pos;  
    GLfloat   dir; 
    GLfloat   vel; 
} Objeto;

typedef struct {
    Posicao  eye;  
    GLfloat  dir_long;  // longitude olhar (esq-dir)
    GLfloat  dir_lat;   // latitude olhar	(cima-baixo)
    GLfloat  fov;
} Camera;

typedef struct {
    Camera      camera;
    GLint         timer;
    GLint         mainWindow,topSubwindow,navigateSubwindow;
    Teclas        teclas;
    GLboolean     localViewer;
    GLuint        vista[NUM_JANELAS];
} Estado;

typedef struct {
    GLuint        texID[NUM_JANELAS][NUM_TEXTURAS];
    GLuint        mapa[NUM_JANELAS];
    GLuint        chao[NUM_JANELAS];
    Objeto	      objeto;
    GLuint        xMouse;
    GLuint        yMouse;
    GLMmodel*     modelo;
    GLboolean     andar;
    GLuint        prev;
} Modelo;

Estado estado;
Modelo modelo;
GLMmodel* pmodel = NULL;
GLint countM=1,tamChao=CHAO_DIMENSAO,dimensaoChao=0;

GLint posCuboZ[qntyCubos], posCubos[qntyCubos], incrementador=0,mudaXBola=0;

/**************************************
******* ILUMINAÇÃO E MATERIAIS ********
**************************************/

void setLight()
{
  GLfloat light_pos[4] = {-5.0, 20.0, -8.0, 0.0};
  GLfloat light_ambient[] = {0.2f, 0.2f, 0.2f, 1.0f};
  GLfloat light_diffuse[] = {0.8f, 0.8f, 0.8f, 1.0f};
  GLfloat light_specular[] = {0.5f, 0.5f, 0.5f, 1.0f};

  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
  glLightfv(GL_LIGHT0, GL_POSITION, light_pos);

  glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, estado.localViewer);
}

void setMaterial()
{
  GLfloat mat_specular[] = {0.8f, 0.8f, 0.8f, 1.0f};
  GLfloat mat_shininess = 104;

  // Criação automática das componentes Ambiente e Difusa do material a partir das cores
  glEnable(GL_COLOR_MATERIAL);
  glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

  // Definir de outros parâmetros dos materiais estáticamente
  glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
  glMaterialf(GL_FRONT, GL_SHININESS, mat_shininess);
}

/**************************************
*** INICIALIZAÇÃO DO AMBIENTE OPENGL **
**************************************/

void init(void)
{
  GLfloat amb[] = {0.3f, 0.3f, 0.3f, 1.0f};

  estado.timer = 100;

  estado.camera.eye.x = 0;
  estado.camera.eye.y = OBJETO_ALTURA * 2;
  estado.camera.eye.z = 0;
  estado.camera.dir_long = 0;
  estado.camera.dir_lat = 0;
  estado.camera.fov = 75;

  estado.localViewer = 1;
  estado.vista[JANELA_TOP] = 0;
  estado.vista[JANELA_NAVIGATE] = 0;
  modelo.objeto.pos.x =700;
  // tamChao-(CHAO_DIMENSAO*0.9)
  modelo.objeto.pos.y = OBJETO_ALTURA * 2;
  modelo.objeto.pos.z = 0;
  modelo.objeto.dir = 0;
  modelo.objeto.vel = OBJETO_VELOCIDADE;

  modelo.xMouse = modelo.yMouse = -1;
  modelo.andar = GL_FALSE;
int previousRandom = 0;

for (int i = 0; i < 50; i++)
{
  // int random = (rand() % 6) + 10;
  if(i==0){
    posCubos[i]=15;
  }
  else{
  posCubos[i] = posCubos[i-1]+ 15;
  posCuboZ[i] = (rand() % 5) - 2;

  }
  
}

// Resto do código...

// for (int i = 0; i < qntyCubos; i++)
// {
//   posCuboZ[i] = (rand() % 5) - 2;
//   if(i==0){
//     posCubos[i] += rand() % 6 + 10;
//   }else{
//     posCubos[i] += rand() % posCubos[i-1] + 10;
//   }
//   printf("teste %d : %d\n",i,posCubos[i]);
// }

  glEnable(GL_DEPTH_TEST);
  glShadeModel(GL_SMOOTH);
  glEnable(GL_POINT_SMOOTH);
  glEnable(GL_LINE_SMOOTH);
  glEnable(GL_POLYGON_SMOOTH);
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_NORMALIZE); 

  if (glutGetWindow() == estado.mainWindow)
    glClearColor(0.8f, 0.8f, 0.8f, 0.0f);
  else
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, amb);
}

/**************************************
***** CALL BACKS DE JANELA/DESENHO ****
**************************************/

void redisplayTopSubwindow(int width, int height)
{
	// glViewport(botom, left, width, height)
	// Define parte da janela a ser utilizada pelo OpenGL
	glViewport(0, 0, (GLint) width, (GLint) height);
	
  // Matriz Projeção
	// Matriz onde se define como o mundo e apresentado na janela
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();	
	gluPerspective(60,(GLfloat)width/height,.5,100);
	
  // Matriz Modelview
	// Matriz onde são realizadas as transformações dos modelos desenhados
	glMatrixMode(GL_MODELVIEW);

}


void reshapeNavigateSubwindow(int width, int height)
{
	// glViewport(botom, left, width, height)
	// Define parte da janela a ser utilizada pelo OpenGL
	glViewport(0, 0, (GLint) width, (GLint) height);
	
  // Matriz Projeção
	// Matriz onde se define como o mundo e apresentado na janela
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();	
	gluPerspective(estado.camera.fov,(GLfloat)width/height,0.1,50);
	
  // Matriz Modelview
	// Matriz onde são realizadas as transformações dos modelos desenhados
	glMatrixMode(GL_MODELVIEW);
}

void reshapeMainWindow(int width, int height)
{
  GLint w, h;
  w = (width - GAP * 3) * .5;
  h = (height - GAP * 2);
  glutSetWindow(estado.topSubwindow);
  glutPositionWindow(GAP, GAP);
  glutReshapeWindow(w, h);
  glutSetWindow(estado.navigateSubwindow);
  glutPositionWindow(GAP + w + GAP, GAP);
  glutReshapeWindow(w, h);
}

/**************************************
** ESPAÇO PARA DEFINIÇÃO DAS ROTINAS **
****** AUXILIARES DE DESENHO ... ******
**************************************/

void strokeCenterString(char *str,double x, double y, double z, double s)
{
	int i,n;
	
	n = strlen(str);
	glPushMatrix();
	  glTranslated(x-glutStrokeLength(GLUT_STROKE_ROMAN,(const unsigned char*)str)*0.5*s,y-119.05*0.5*s,z);
	  glScaled(s,s,s);
	  for(i=0;i<n;i++)
		  glutStrokeCharacter(GLUT_STROKE_ROMAN,(int)str[i]);
	glPopMatrix();

}

void desenhaAngVisao(Camera *cam)
{
    GLfloat ratio;
    ratio=(GLfloat)glutGet(GLUT_WINDOW_WIDTH)/glutGet(GLUT_WINDOW_HEIGHT);  
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE);

    glPushMatrix();
        glTranslatef(cam->eye.x,OBJETO_ALTURA,cam->eye.z);
        glColor4f(0,0,1,0.2);
        glRotatef(GRAUS(cam->dir_long),0,1,0);

        glBegin(GL_TRIANGLES);
            glVertex3f(0,0,0);
            glVertex3f(5*cos(RAD(cam->fov*ratio*0.5)),0,-5*sin(RAD(cam->fov*ratio*0.5)));
            glVertex3f(5*cos(RAD(cam->fov*ratio*0.5)),0,5*sin(RAD(cam->fov*ratio*0.5)));
        glEnd();
    glPopMatrix();

    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
}

void desenhaModelo(GLuint texID)
{
    glEnable(GL_LIGHTING); // Habilita o uso de cor no material
    glPushMatrix();
   GLfloat brown_mat[] = {0.5, 0.2, 0.0, 1.0};  // Castanho claro (R: 0.5, G: 0.2, B: 0.0)
GLfloat white_mat[] = {1.0, 1.0, 1.0, 1.0};  // Branco puro (R: 1.0, G: 1.0, B: 1.0)
   GLfloat low_shininess[] = {25.0};
    GLfloat no_mat[] = {0.5, 0.2, 0.0, 1.0};
glMaterialfv(GL_FRONT, GL_AMBIENT, brown_mat);
glMaterialfv(GL_FRONT, GL_DIFFUSE, brown_mat);
glMaterialfv(GL_FRONT, GL_SPECULAR, white_mat);
glMaterialfv(GL_FRONT, GL_SHININESS, low_shininess);
glMaterialfv(GL_FRONT, GL_EMISSION, no_mat);

    // glTranslatef(0, OBJETO_ALTURA * 0.75, 0);
    glScalef(15, 15, 15);
    // glBindTexture(GL_TEXTURE_2D, texID);
    glutSolidCube(OBJETO_ALTURA * 0.5);
    glPopMatrix();
    glDisable(GL_LIGHTING); // Habilita o uso de cor no material
}
void desenhaArvore(){
    glEnable(GL_LIGHTING);
  glPushMatrix();
    glPushMatrix();
        GLfloat dark_brown_mat[] = {0.36, 0.25, 0.20, 1.0};  // Castanho escuro (R: 0.36, G: 0.25, B: 0.20)
        GLfloat white_mat[] = {1.0, 1.0, 1.0, 1.0};  // Branco puro (R: 1.0, G: 1.0, B: 1.0)
        GLfloat low_shininess[] = {25.0};
        GLfloat no_mat[] = {0.0, 0.0, 0.0, 0.0};
        glMaterialfv(GL_FRONT, GL_AMBIENT, dark_brown_mat);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, dark_brown_mat);
        glMaterialfv(GL_FRONT, GL_SPECULAR, white_mat);
        glMaterialfv(GL_FRONT, GL_SHININESS, low_shininess);
        glMaterialfv(GL_FRONT, GL_EMISSION, no_mat);
      glTranslatef(5.0f, 0.0f, -6.0f);
      glScalef(5,20,5);
      // glColor3f(1.0f, 0.0f, 0.0f);
      glutSolidCube(OBJETO_ALTURA * 0.5);
    glPopMatrix();

    glPushMatrix();
      glPushMatrix();
        GLUquadricObj* quadric = gluNewQuadric();
        gluQuadricDrawStyle(quadric, GLU_FILL);  // Preencher a esfera
        gluQuadricNormals(quadric, GLU_SMOOTH);  // Utilizar normais suaves
      glPopMatrix();
      
      glPushMatrix();
          GLfloat green_mat[] = {0.0, 1.0, 0.0, 1.0};  // Verde claro (R: 0.0, G: 1.0, B: 0.0)
          GLfloat dark_green_mat[] = {0.0, 0.2, 0.0, 1.0};  // Sombra verde escura (R: 0.0, G: 0.2, B: 0.0)
          glMaterialfv(GL_FRONT, GL_AMBIENT, green_mat);
          glMaterialfv(GL_FRONT, GL_DIFFUSE, green_mat);
          glMaterialfv(GL_FRONT, GL_SPECULAR, dark_green_mat);
          glMaterialfv(GL_FRONT, GL_SHININESS, low_shininess);
          glMaterialfv(GL_FRONT, GL_EMISSION, no_mat);
        glPushMatrix();
          glTranslatef(5.0f, 3.0f, -6.0f);
          // glColor3f(0.0f, 1.0f, 0.0f);
          gluSphere(quadric, 1.5, 20, 20);  // Desenhar uma esfera 
        glPopMatrix();
        glPushMatrix();
          glTranslatef(5.0f, 5.0f, -6.0f);
          // glColor3f(0.0f, 1.0f, 0.0f);
          gluSphere(quadric, 1.25, 20, 20);  // Desenhar uma esfera 
        glPopMatrix();
      glPopMatrix();  

  glPopMatrix();
    glDisable(GL_LIGHTING);
}
void desenhaArvore2(){
  glEnable(GL_LIGHTING);
  glPushMatrix();
    glPushMatrix();
        GLfloat dark_brown_mat[] = {0.36, 0.25, 0.20, 1.0};  // Castanho escuro (R: 0.36, G: 0.25, B: 0.20)
        GLfloat white_mat[] = {1.0, 1.0, 1.0, 1.0};  // Branco puro (R: 1.0, G: 1.0, B: 1.0)
        GLfloat low_shininess[] = {25.0};
        GLfloat no_mat[] = {0.0, 0.0, 0.0, 0.0};
        glMaterialfv(GL_FRONT, GL_AMBIENT, dark_brown_mat);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, dark_brown_mat);
        glMaterialfv(GL_FRONT, GL_SPECULAR, white_mat);
        glMaterialfv(GL_FRONT, GL_SHININESS, low_shininess);
        glMaterialfv(GL_FRONT, GL_EMISSION, no_mat);
      glTranslatef(5.0f, 0.0f, 7.0f);
      glScalef(5,20,5);
      // glColor3f(1.0f, 0.0f, 0.0f);
      glutSolidCube(OBJETO_ALTURA * 0.5);
    glPopMatrix();

    glPushMatrix();
      glPushMatrix();
        GLUquadricObj* quadric = gluNewQuadric();
        gluQuadricDrawStyle(quadric, GLU_FILL);  // Preencher a esfera
        gluQuadricNormals(quadric, GLU_SMOOTH);  // Utilizar normais suaves
      glPopMatrix();
      
      glPushMatrix();
          GLfloat green_mat[] = {0.0, 1.0, 0.0, 1.0};  // Verde claro (R: 0.0, G: 1.0, B: 0.0)
          GLfloat dark_green_mat[] = {0.0, 0.2, 0.0, 1.0};  // Sombra verde escura (R: 0.0, G: 0.2, B: 0.0)
          glMaterialfv(GL_FRONT, GL_AMBIENT, green_mat);
          glMaterialfv(GL_FRONT, GL_DIFFUSE, green_mat);
          glMaterialfv(GL_FRONT, GL_SPECULAR, dark_green_mat);
          glMaterialfv(GL_FRONT, GL_SHININESS, low_shininess);
          glMaterialfv(GL_FRONT, GL_EMISSION, no_mat);
        glPushMatrix();
          glTranslatef(5.0f, 3.0f, 7.0f);
          // glColor3f(0.0f, 1.0f, 0.0f);
          gluSphere(quadric, 1.5, 20, 20);  // Desenhar uma esfera 
        glPopMatrix();
        glPushMatrix();
          glTranslatef(5.0f, 5.0f, 7.0f);
          // glColor3f(0.0f, 1.0f, 0.0f);
          gluSphere(quadric, 1.25, 20, 20);  // Desenhar uma esfera 
        glPopMatrix();
      glPopMatrix();  

  glPopMatrix();
    glDisable(GL_LIGHTING);
}

void desenhaMoeda(){

  static GLfloat rotation = 0.0f;  // Angle of rotation for the camera direction

  glEnable(GL_LIGHTING);
  glPushMatrix();
    GLfloat gold_mat[] = {0.8, 0.6, 0.0, 1.0};  // Gold color (R: 0.8, G: 0.6, B: 0.0)
    GLfloat white_mat[] = {1.0, 1.0, 1.0, 1.0};  // White color (R: 1.0, G: 1.0, B: 1.0)
    GLfloat low_shininess[] = {25.0};
    GLfloat no_mat[] = {0.0, 0.0, 0.0, 0.0};
    glMaterialfv(GL_FRONT, GL_AMBIENT, gold_mat);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, gold_mat);
    glMaterialfv(GL_FRONT, GL_SPECULAR, white_mat);
    glMaterialfv(GL_FRONT, GL_SHININESS, low_shininess);
    glMaterialfv(GL_FRONT, GL_EMISSION, no_mat);
    
    GLUquadricObj* quadric = gluNewQuadric();
    gluQuadricDrawStyle(quadric, GLU_FILL);  // Fill the sphere
    gluQuadricNormals(quadric, GLU_SMOOTH);  // Use smooth normals
    
    glPushMatrix();
      // glTranslatef(9.0f, 3.0f, 0.0f);  // Set the position of the coin
        glRotatef(rotation, 1.0f, 0.0f, 1.0f); // Rotate around the y-axis

      gluSphere(quadric, 1.0, 10, 10);  // Draw a sphere as the coin
    glPopMatrix();
  glPopMatrix();


  rotation += 5.0f; // Increase the rotation angle
  if (rotation >= 360.0f) {
    rotation -= 360.0f; // Reset the rotation angle if it exceeds 360 degrees
  }
  glDisable(GL_LIGHTING);

}
void drawmodel(GLuint texID)
{
    if (!pmodel) {
        pmodel = glmReadOBJ("C:/Github/Multimedia1/template_projeto/data/mario.obj");
        //C:/Users/User/Desktop/UFP/Git-Hub/Multimedia1/template_projeto/data/mario.obj
        //C:/Github/Multimedia1/template_projeto/data/mario.obj
        if (!pmodel) exit(0);
        glmUnitize(pmodel);
        glmFacetNormals(pmodel);
        glmVertexNormals(pmodel, 90.0);
    }//nao muda para preto
    glEnable(GL_LIGHTING);
    glPushMatrix();
        GLfloat brown_mat[] = {0.0, 1.0, 0.0, 1.0};  // Castanho claro (R: 0.5, G: 0.2, B: 0.0)
GLfloat white_mat[] = {0.0, 1.0, 0.0, 1.0};  // Branco puro (R: 1.0, G: 1.0, B: 1.0)
   GLfloat low_shininess[] = {25.0};
    GLfloat no_mat[] = {0.0, 1.0, 0.0, 1.0};
glMaterialfv(GL_FRONT, GL_AMBIENT, brown_mat);
glMaterialfv(GL_FRONT, GL_DIFFUSE, brown_mat);
glMaterialfv(GL_FRONT, GL_SPECULAR, white_mat);
glMaterialfv(GL_FRONT, GL_SHININESS, low_shininess);
glMaterialfv(GL_FRONT, GL_EMISSION, no_mat);
    glBindTexture(GL_TEXTURE_2D, texID);
    glmDraw(pmodel, GLM_SMOOTH | GLM_MATERIAL | GLM_TEXTURE);
    glPopMatrix();
    glDisable(GL_LIGHTING);
}

void desenhaLimitesColisao()
{

  // Define a cor da linha como branco
  // glColor3f(1.0f, 0.0f, 0.0f);
  glLineWidth(5.0f);

  glBegin(GL_LINES);
    GLfloat no_mat[] = {0.0, 0.0, 0.0, 0.0};
GLfloat mat_ambient[] = {0.0, 0.0, 0.0, 0.0};      // Vermelho escuro para a cor ambiente
GLfloat mat_diffuse[] = {0.0, 0.0, 0.0, 0.0};      // Vermelho médio para a cor difusa
GLfloat mat_specular[] = {1.0, 0.0, 0.0, 1.0};     // Vermelho brilhante para a cor especular
GLfloat no_shininess[] = {0.0};
GLfloat low_shininess[] = {5.0};
GLfloat high_shininess[] = {100.0};
GLfloat mat_emission[] = {0.3, 0.2, 0.2, 0.0};
           glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR, no_mat);
        glMaterialfv(GL_FRONT, GL_SHININESS, no_shininess);
        glMaterialfv(GL_FRONT, GL_EMISSION, no_mat);
    // Linha da esquerda
    glVertex3f( dimensaoChao-(CHAO_DIMENSAO*countM)-(CHAO_DIMENSAO), 1.0f, LIMITE_ESQUERDA);
    glVertex3f( dimensaoChao, 1.0f, LIMITE_ESQUERDA);

    // Linha da direita
    glVertex3f(dimensaoChao-(CHAO_DIMENSAO*countM)-(CHAO_DIMENSAO), 1.0f,LIMITE_DIREITA);
    glVertex3f(dimensaoChao, 1.0f,LIMITE_DIREITA);
  glEnd();
}

void desenhaChao(GLfloat dimensao, GLuint texID)
{
    GLfloat i,j;
    glBindTexture(GL_TEXTURE_2D, texID);

    glColor3f(0.5f,0.5f,0.5f);
    for(i=dimensao-(CHAO_DIMENSAO*countM)-(CHAO_DIMENSAO);i<=dimensao;i+=STEP) //(CHAO_DIMENSAO/5) quanto mener o valor dividido melhor fica o chao adicionado
    {
      for(j=-10;j<=10;j+=STEP)
      {
          glBegin(GL_POLYGON);
              glNormal3f(0,1,0);
              glTexCoord2f(1,1);
              glVertex3f(i+STEP,0,j+STEP);
              glTexCoord2f(0,1);
              glVertex3f(i,0,j+STEP);
              glTexCoord2f(0,0);
              glVertex3f(i,0,j);
              glTexCoord2f(1,0);
              glVertex3f(i+STEP,0,j);
          glEnd();
      }
    }
    
}

void output(GLfloat x, GLfloat y, char *format, ...)
{
  va_list args;
  char buffer[200], *p;

  va_start(args, format);
  vsprintf(buffer, format, args);
  va_end(args);
  glPushMatrix();
    glTranslatef(x, y, 0);
    for (p = buffer; *p; p++)
      glutStrokeCharacter(GLUT_STROKE_ROMAN, *p);
  glPopMatrix();
}

void createDisplayLists(int janelaID)
{
	modelo.mapa[janelaID]=glGenLists(1);

	modelo.chao[janelaID]=modelo.mapa[janelaID];
	glNewList(modelo.chao[janelaID], GL_COMPILE);
		glPushAttrib(GL_COLOR_BUFFER_BIT | GL_CURRENT_BIT | GL_ENABLE_BIT );
		  // desenhaChao(tamChao,modelo.texID[janelaID][ID_TEXTURA_CHAO]);
		glPopAttrib();
	glEndList();
}

/////////////////////////////////////
//navigateSubwindow

void setNavigateSubwindowCamera(Camera *cam, Objeto obj)
{
  Posicao center;
  /*
    if(estado.vista[JANELA_NAVIGATE])
    {
  */
    // cam->eye.x=obj.pos.x-2;
    // cam->eye.y=obj.pos.y+.9;
    // cam->eye.z=obj.pos.z;
    // center.x=obj.pos.x;
    // center.y=obj.pos.y+.9;
    // center.z=obj.pos.z;

    // Ajustar a distância da câmera diretamente no cálculo
    GLfloat distanceFromObject = 3.0f;
    cam->eye.x = obj.pos.x - distanceFromObject * cos(cam->dir_long );
    cam->eye.y = obj.pos.y + 1.5f;
    cam->eye.z = obj.pos.z - distanceFromObject * sin(cam->dir_long - M_PI);

    // Ajustar o ponto de vista da câmera (olhar para o objeto)
    center.x = obj.pos.x;
    center.y = obj.pos.y + 1.5f;
    center.z = obj.pos.z;

  /*
    }
    else
    {

    }
  */
  gluLookAt(cam->eye.x,cam->eye.y,cam->eye.z,center.x,center.y,center.z,0,1,0);
}


void displayNavigateSubwindow()
{


   glClearColor(0.0f, 0.4f, 1.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glLoadIdentity();

	setNavigateSubwindowCamera(&estado.camera, modelo.objeto);
  setLight();
  dimensaoChao = tamChao;
  desenhaChao(tamChao,modelo.texID[0][ID_TEXTURA_CHAO]);
	glCallList(modelo.mapa[JANELA_NAVIGATE]);
	glCallList(modelo.chao[JANELA_NAVIGATE]);

  desenhaLimitesColisao();
      
	if(!estado.vista[JANELA_NAVIGATE])
  {
  
    // GLfloat pos = tamChao-(CHAO_DIMENSAO*countM)-(CHAO_DIMENSAO) + 10;
    // for (int i = 0; i < 10; i++)
    // {

    //          glPushMatrix();
    //         glTranslatef(modelo.objeto.pos.x+2,modelo.objeto.pos.y+0.3,modelo.objeto.pos.z);
    //         glRotatef(GRAUS(modelo.objeto.dir),0,1,0);
    //         glRotatef(90,0,1,0);
    //       glEnable(GL_LIGHTING);

    //       GLfloat light_pos2[] = { 0.0, 2.0, -1.0, 0.0 };
    //       glLightfv(GL_LIGHT0, GL_POSITION, light_pos2);

    //           glTranslatef(pos, posicoes[0], 0.0);

             
    //       desenhaModelo(pos,posicoes[i]);
    //           // printf("posicao %d\n",posicoes[i]);

    //       pos += 10;
    //        printf("posicao %f\n",pos);
          
    //       glDisable(GL_LIGHTING);
    // glPopMatrix();
    // }
          
          glPushMatrix();
            glTranslatef(780.0f+mudaXBola,1.5f,0.0f);
            desenhaMoeda();
          glPopMatrix();

        
          GLfloat light_pos[] = { 0.0, 2.0, -1.0, 0.0 };
          glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
          for (int i = 0; i < qntyCubos; i++){
               glPushMatrix();
              glTranslatef(posCubos[i],0,posCuboZ[i]);
              desenhaModelo(modelo.texID[1][ID_TEXTURA_CAIXA]);
              glPopMatrix();  
          }

        
          for (int i = 0; i < qntyCubos; i++)
          {
            glPushMatrix();
              glTranslatef(posCubos[i]+5,0,posCuboZ[i]);
              desenhaArvore();
            glPopMatrix();   
            glPushMatrix();
              glTranslatef(posCubos[i]+5,0,posCuboZ[i]);
              desenhaArvore2();
            glPopMatrix();
          }


    glPushMatrix();
        glTranslatef(modelo.objeto.pos.x,modelo.objeto.pos.y+0.3,modelo.objeto.pos.z);
        glRotatef(GRAUS(modelo.objeto.dir),0,1,0);
        glRotatef(90, 1, 0, 0);
        
        glEnable(GL_LIGHTING);

  

        glPushMatrix();
          glRotatef(-180, 1, 0, 0);

          // GLfloat light_pos[] = { 0.0, 2.0, -1.0, 0.0 };
          // glMaterialfv(GL_FRONT, GL_AMBIENT, no_mat);
          // glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
          // glMaterialfv(GL_FRONT, GL_SPECULAR, no_mat);
          // glMaterialfv(GL_FRONT, GL_SHININESS, no_shininess);
          // glMaterialfv(GL_FRONT, GL_EMISSION, no_mat); 
          drawmodel(modelo.texID[0][ID_TEXTURA_MARIO]);
          // drawmodel();

        glPopMatrix();



        glDisable(GL_LIGHTING);


        
        
    glPopMatrix();
     


  }



// glPushAttrib(GL_ENABLE_BIT);
// glDisable(GL_DEPTH_TEST);
// glDisable(GL_LIGHTING);

// glMatrixMode(GL_PROJECTION);
// glPushMatrix(); // Salva o estado atual da matriz de projeção
// glLoadIdentity();
// gluOrtho2D(0, 3000, 0, 3000);

// glMatrixMode(GL_MODELVIEW);
// glPushMatrix(); // Salva o estado atual da matriz de visualização
// glLoadIdentity();
// output(2000, 2800, "Pontuacao: ");

// glPopMatrix(); // Restaura a matriz de visualização para o estado anterior
// glMatrixMode(GL_PROJECTION);
// glPopMatrix(); // Restaura a matriz de projeção para o estado anterior

// glPopAttrib();

	glutSwapBuffers();
}


/////////////////////////////////////
//topSubwindow

void setTopSubwindowCamera(Camera *cam, Objeto obj)
{
	cam->eye.x=obj.pos.x;
	cam->eye.z=obj.pos.z;
	if(estado.vista[JANELA_TOP])
		gluLookAt(obj.pos.x,20*.2,obj.pos.z,obj.pos.x,obj.pos.y,obj.pos.z,0,0,-1);
	else
		gluLookAt(obj.pos.x,20*2,obj.pos.z,obj.pos.x,obj.pos.y,obj.pos.z,0,0,-1);
}

void displayTopSubwindow()
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
    glLoadIdentity();
    setTopSubwindowCamera(&estado.camera,modelo.objeto);
    setLight();
     desenhaChao(tamChao,modelo.texID[0][ID_TEXTURA_CHAO]);
	glCallList(modelo.mapa[JANELA_TOP]);
	glCallList(modelo.chao[JANELA_TOP]);
	
    glPushMatrix();		
        glTranslatef(modelo.objeto.pos.x,modelo.objeto.pos.y,modelo.objeto.pos.z);
        glRotatef(GRAUS(modelo.objeto.dir),0,1,0);
        glRotatef(90,0,1,0);
        	// glCallList(modelo.mapa[JANELA_TOP]);
	        // glCallList(modelo.chao[JANELA_TOP]);
    glPopMatrix();

	desenhaAngVisao(&estado.camera);
	
	glutSwapBuffers();
}

/////////////////////////////////////
//mainWindow

void redisplayAll(void)
{
    glutSetWindow(estado.mainWindow);
    glutPostRedisplay();
    glutSetWindow(estado.topSubwindow);
    glutPostRedisplay();
    glutSetWindow(estado.navigateSubwindow);
    glutPostRedisplay();
}

void displayMainWindow()
{
	glClearColor(0.8f, 0.8f, 0.8f, 0.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	
	glutSwapBuffers();
}


/**************************************
******** CALLBACKS TIME/IDLE **********
**************************************/

/* Callback de temporizador */
void timer(int value)
{
  GLfloat nx=0,nz=0;
  GLboolean andar=GL_FALSE;

  GLuint curr = glutGet(GLUT_ELAPSED_TIME);
  // Calcula velocidade baseado no tempo passado
	float velocidade= modelo.objeto.vel*(curr - modelo.prev )*0.001;

  if((modelo.objeto.pos.x >= 780 && modelo.objeto.pos.x <= 781) && modelo.objeto.pos.z >= -1.8f && modelo.objeto.pos.z <= 1.8f){
    printf("ganhou\n");
    estado.teclas.s=GL_FALSE;
    mudaXBola=100.0f;
    estado.teclas.left=GL_FALSE;
    estado.teclas.right=GL_FALSE;

  }
  else if(modelo.objeto.pos.x >= 799 && modelo.objeto.pos.x <= 800){
    printf("ganhou\n");
    estado.teclas.s=GL_FALSE;
    estado.teclas.left=GL_FALSE;
    estado.teclas.right=GL_FALSE;
  }

  if(modelo.objeto.pos.x + (CHAO_DIMENSAO)>=tamChao){
    countM=2;
    tamChao=tamChao+(CHAO_DIMENSAO*countM);
  }
 printf("modelo.objeto.pos.x: %f\n", modelo.objeto.pos.x);
// printf("posCubos[incrementador]: %f\n", posCubos[incrementador]);
// printf("modelo.objeto.pos.z: %f\n", modelo.objeto.pos.z);
// printf("posCuboZ[incrementador]: %f\n", posCuboZ[incrementador]);
  // printf("inc %f\n",posCuboZ[incrementador]);
  // printf("pos %f\n",modelo.objeto.pos.x);
//    if  ((modelo.objeto.pos.x <= posCubos[incrementador]+1 && modelo.objeto.pos.x >= posCubos[incrementador]-1) &&
//    (( modelo.objeto.pos.z >= posCuboZ[incrementador]-2.0f && modelo.objeto.pos.z <= posCuboZ[incrementador]+2.0f)))
//   {
//        modelo.objeto.pos.z = 0.0f;
//         // printf("continha %d",tamChao-(CHAO_DIMENSAO*countM)+(CHAO_DIMENSAO/5));
//         tamChao=CHAO_DIMENSAO;
//         countM=1;
//         modelo.objeto.pos.x = tamChao-(CHAO_DIMENSAO*0.9);
//         estado.teclas.s=GL_FALSE;
//         incrementador=0;
//         printf("pos %f\n",modelo.objeto.pos.x);
//   }
//   else if(modelo.objeto.pos.x > posCubos[incrementador]){
//     incrementador++;
// //     printf("incrementador %d\n",incrementador);
// //     printf("modelo.objeto.pos.x: %f\n", modelo.objeto.pos.x);
// // printf("posCubos[incrementador]: %d\n", posCubos[incrementador]);
// // printf("modelo.objeto.pos.z: %f\n", modelo.objeto.pos.z);
// // printf("posCuboZ[incrementador]: %d\n", posCuboZ[incrementador]);
//   }

  
  // printf("pos %f\n",modelo.objeto.pos.x);
  glutTimerFunc(estado.timer, timer, 0);
  /* Acções do temporizador ...
     Não colocar aqui primitivas OpenGL de desenho glBegin, glEnd, etc.
   */

  modelo.prev = curr;

  if(estado.teclas.up)
  {
    andar=GL_TRUE;
    modelo.objeto.pos.x+=velocidade*cos(RAD(modelo.objeto.dir));
    modelo.objeto.pos.z-=velocidade*sin(RAD(modelo.objeto.dir));  
	}
	
  if(estado.teclas.down){
    andar=GL_TRUE;
    modelo.objeto.pos.x-=velocidade*cos(RAD(modelo.objeto.dir));
    modelo.objeto.pos.z+=velocidade*sin(RAD(modelo.objeto.dir));
	}
	
   if(estado.teclas.left){
    // rodar camara e objeto
    

      // modelo.objeto.dir += OBJETO_ROTACAO;
      // estado.camera.dir_long += EYE_ROTACAO;
      

    // printf("dir left: %f %f\\", modelo.objeto.dir, estado.camera.dir_long);

    
    // modelo.objeto.pos.x -=0.5;
    modelo.objeto.pos.z -=0.1;
 
  }
    if(estado.teclas.right){
    // rodar camara e 
    
      // modelo.objeto.dir -= OBJETO_ROTACAO;
      // estado.camera.dir_long -= EYE_ROTACAO;
    

    
    // printf("dir rigth: %f %f\n", modelo.objeto.dir, estado.camera.dir_long);

    // modelo.objeto.pos.x +=0.5;
    modelo.objeto.pos.z +=0.1;
    // printf("pos %f %f\n",modelo.objeto.pos.x,modelo.objeto.pos.z);

    }
         if(modelo.objeto.pos.z <= -2.7f || modelo.objeto.pos.z >= 3.69f){
        modelo.objeto.pos.z = 0.0f;
        incrementador=0;
        tamChao=CHAO_DIMENSAO;
        countM=1;
        // printf("continha %d",tamChao-(CHAO_DIMENSAO*countM)+(CHAO_DIMENSAO/5));
        modelo.objeto.pos.x = tamChao-(CHAO_DIMENSAO*0.9);
        estado.teclas.s=GL_FALSE;
      }

  if(estado.teclas.a){
    estado.camera.dir_long += EYE_ROTACAO;
    // modelo.objeto.dir += 2 *OBJETO_ROTACAO;

    printf("dir left: %f %f\n", estado.camera.dir_long, modelo.objeto.dir);

    // estado.camera.eye.x -= 0.1;
    // estado.camera.eye.z -= 0.1;

    
  }

  if(estado.teclas.d){
    estado.camera.dir_long -= EYE_ROTACAO;
          // modelo.objeto.dir -= OBJETO_ROTACAO;

    printf("dir rigth: %f %f\n",  estado.camera.dir_long, modelo.objeto.dir);

    // estado.camera.eye.x += 0.1;
    // estado.camera.eye.z += 0.1;
    
  }


  redisplayAll();
}

/**************************************
*********** FUNÇÃO AJUDA **************
**************************************/

void imprime_ajuda(void)
{
  printf("\n\nProjeto MUL1\n");
  printf("h,H   - Ajuda \n");
  printf("******* Diversos ******* \n");
  printf("l,L   - Alterna o calculo luz entre Z e eye (GL_LIGHT_MODEL_LOCAL_VIEWER)\n");
  printf("w,W   - Wireframe \n");
  printf("f,F   - Fill \n");
  printf("******* Movimento ******* \n");
  printf("UP    - Avança (PARA IMPLEMENTAR) \n");
  printf("DOWN  - Recua (PARA IMPLEMENTAR)\n");
  printf("LEFT  - Vira para a direita (PARA IMPLEMENTAR)\n");
  printf("RIGHT - Vira para a esquerda (PARA IMPLEMENTAR)\n");
  printf("******* Camara ******* \n");
  printf("F1    - Alterna camara da janela da Esquerda \n");
  printf("F2    - Alterna camara da janela da Direita \n");
  printf("PAGE_UP, PAGE_DOWN - Altera abertura da camara \n");
  printf("ESC - Sair\n");
}

/**************************************
********* CALLBACKS TECLADO ***********
**************************************/

/* Callback para interação via teclado (carregar na tecla) */
void key(unsigned char key, int x, int y)
{
  switch (key) {
		case 27:
			exit(1);
			break;
    case 'h' :
    case 'H' :
            imprime_ajuda();
            break;
    case 'l':
    case 'L':
            estado.localViewer=!estado.localViewer;
            break;
    case 'w':
    case 'W':
            glutSetWindow(estado.navigateSubwindow);
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glDisable(GL_TEXTURE_2D);
            glutSetWindow(estado.topSubwindow);
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glDisable(GL_TEXTURE_2D);
            break;
    case 'f':
    case 'F':
            glutSetWindow(estado.navigateSubwindow);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glEnable(GL_TEXTURE_2D);
            glutSetWindow(estado.topSubwindow);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glEnable(GL_TEXTURE_2D);
            break;
    
    case 'a':
    case 'A':
        estado.teclas.a = GL_TRUE;

        break;
    case 'd':
    case 'D':
        estado.teclas.d = GL_TRUE;
        break;     
    case 's':
    case 'S':
        estado.teclas.s = GL_TRUE;
        break;   

    case 'r':
    case 'R':
        tamChao=CHAO_DIMENSAO;
        countM=1;
        modelo.objeto.pos.x = tamChao-(CHAO_DIMENSAO*0.9);
        mudaXBola=0.0f;
        incrementador=0;
        break;   
	}

  if (DEBUG)
    printf("Carregou na tecla %c\n", key);
}

/* Callback para interação via teclado (largar a tecla) */
void keyUp(unsigned char key, int x, int y)
{
  switch (key) {
    case 'a':
    case 'A':
        estado.teclas.a = GL_FALSE;
        break;
    case 'd':
    case 'D':
        estado.teclas.d = GL_FALSE;
        break;        
  }
  if (DEBUG)
    printf("Largou a tecla %c\n", key);
}

/* Callback para interacção via teclas especiais (carregar na tecla) */
void specialKey(int key, int x, int y)
{
  /* Ações sobre outras teclas especiais
      GLUT_KEY_F1 ... GLUT_KEY_F12
      GLUT_KEY_UP
      GLUT_KEY_DOWN
      GLUT_KEY_LEFT
      GLUT_KEY_RIGHT
      GLUT_KEY_PAGE_UP
      GLUT_KEY_PAGE_DOWN
      GLUT_KEY_HOME
      GLUT_KEY_END
      GLUT_KEY_INSERT */

  switch (key) {
		case GLUT_KEY_UP: 
      estado.teclas.up =GL_TRUE;
			break;
		case GLUT_KEY_DOWN: 
      estado.teclas.down =GL_TRUE;
			break;
		case GLUT_KEY_LEFT: 
      estado.teclas.left =GL_TRUE;
			break;
		case GLUT_KEY_RIGHT: 
      estado.teclas.right =GL_TRUE;
			break;
		case GLUT_KEY_F1: 
      estado.vista[JANELA_TOP]=!estado.vista[JANELA_TOP];
			break;
		case GLUT_KEY_F2: 
      estado.vista[JANELA_NAVIGATE]=!estado.vista[JANELA_NAVIGATE];
			break;
		case GLUT_KEY_PAGE_UP: 
      if(estado.camera.fov>20)
      {
        estado.camera.fov--;
        glutSetWindow(estado.navigateSubwindow);
        reshapeNavigateSubwindow(glutGet(GLUT_WINDOW_WIDTH),glutGet(GLUT_WINDOW_HEIGHT));
        redisplayAll();
      }
			break;
		case GLUT_KEY_PAGE_DOWN: 
      if(estado.camera.fov<130)
      {
        estado.camera.fov++;
        glutSetWindow(estado.navigateSubwindow);
        reshapeNavigateSubwindow(glutGet(GLUT_WINDOW_WIDTH),glutGet(GLUT_WINDOW_HEIGHT));
        redisplayAll();
      }
			break;
	}

  if (DEBUG)
    printf("Carregou na tecla especial %d\n", key);
}

/* Callback para interação via teclas especiais (largar a tecla) */
void specialKeyUp(int key, int x, int y)
{
  switch (key) {
    case GLUT_KEY_UP: 
            estado.teclas.up =GL_FALSE;
            break;
    case GLUT_KEY_DOWN: 
            estado.teclas.down =GL_FALSE;
            break;
    case GLUT_KEY_LEFT: 
            estado.teclas.left =GL_FALSE;
            break;
    case GLUT_KEY_RIGHT: 
            estado.teclas.right =GL_FALSE;
            break;
  }

  if (DEBUG)
    printf("Largou a tecla especial %d\n", key);
}


/**************************************
************** TEXTURAS ***************
**************************************/

void createTextures(GLuint texID[])
{
    unsigned char *image = NULL, *image2 = NULL, *image3 = NULL;
    int w, h,w2,h2,w3,h3;

    glGenTextures(NUM_TEXTURAS,texID);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    image = glmReadPPM(NOME_TEXTURA_CHAO, &w, &h);
    if(image)
    {
        glBindTexture(GL_TEXTURE_2D, texID[ID_TEXTURA_CHAO]);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST );
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        gluBuild2DMipmaps(GL_TEXTURE_2D, 3, w, h, GL_RGB, GL_UNSIGNED_BYTE, image);
    }else{
        printf("Textura %s não encontrada \n",NOME_TEXTURA_CHAO);
        exit(0);
    }
     image2 = glmReadPPM(MARIO, &w2, &h2);
    if(image2)
    {
        glBindTexture(GL_TEXTURE_2D, texID[ID_TEXTURA_MARIO]);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST );
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        gluBuild2DMipmaps(GL_TEXTURE_2D, 3, w2, h2, GL_RGB, GL_UNSIGNED_BYTE, image2);
    }else{
        printf("Textura %s não encontrada \n",MARIO);
        exit(0);
    }
      image3 = glmReadPPM(CAIXA, &w3, &h3);
    if(image3)
    {
        glBindTexture(GL_TEXTURE_2D, texID[ID_TEXTURA_CAIXA]);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST );
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        gluBuild2DMipmaps(GL_TEXTURE_2D, 3, w3, h3, GL_RGB, GL_UNSIGNED_BYTE, image3);
    }else{
        printf("Textura %s não encontrada \n",CAIXA);
        exit(0);
    }

}

/**************************************
************ FUNÇÃO MAIN **************
**************************************/

int main(int argc, char **argv)
{
  glutInit(&argc, argv);
  glutInitWindowPosition(10, 10);
  glutInitWindowSize(800 + GAP * 3, 400 + GAP * 2);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  if ((estado.mainWindow = glutCreateWindow("Labirinto")) == GL_FALSE)
    exit(1);

  imprime_ajuda();
      srand(time(NULL));
  

  // Registar callbacks do GLUT da janela principal
  init();
  glutReshapeFunc(reshapeMainWindow);
  glutDisplayFunc(displayMainWindow);

  glutTimerFunc(estado.timer, timer, 0);
  glutKeyboardFunc(key);
  glutSpecialFunc(specialKey);
  glutKeyboardUpFunc(keyUp);
  glutSpecialUpFunc(specialKeyUp);

  // criar a sub window topSubwindow
  estado.topSubwindow = glutCreateSubWindow(estado.mainWindow, GAP, GAP, 400, 400);
  init();
  setLight();
  setMaterial();
  createTextures(modelo.texID[JANELA_TOP]);
  createDisplayLists(JANELA_TOP);

  glutReshapeFunc(redisplayTopSubwindow);
  glutDisplayFunc(displayTopSubwindow);

  glutTimerFunc(estado.timer, timer, 0);
  glutKeyboardFunc(key);
  glutSpecialFunc(specialKey);
  glutKeyboardUpFunc(keyUp);
  glutSpecialUpFunc(specialKeyUp);

  // criar a sub window navigateSubwindow
  estado.navigateSubwindow = glutCreateSubWindow(estado.mainWindow, 400 + GAP, GAP, 400, 800);
  init();
  setLight();
  setMaterial();

  createTextures(modelo.texID[JANELA_NAVIGATE]);
  createDisplayLists(JANELA_NAVIGATE);

  glutReshapeFunc(reshapeNavigateSubwindow);
  glutDisplayFunc(displayNavigateSubwindow);
  //glutMouseFunc(mouseNavigateSubwindow);

  glutTimerFunc(estado.timer, timer, 0);
  glutKeyboardFunc(key);
  glutSpecialFunc(specialKey);
  glutKeyboardUpFunc(keyUp);
  glutSpecialUpFunc(specialKeyUp);

  glutMainLoop();
  return 0;
}
