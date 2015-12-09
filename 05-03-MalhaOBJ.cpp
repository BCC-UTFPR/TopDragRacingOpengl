#include "bibutil.h"
#include <stdio.h>
#include <stdlib.h>
#include <GL/freeglut.h>

//#define DEBUG 1
// Vari�veis para controles de navega��o
GLfloat angle, fAspect;
GLfloat rotX, rotY, rotX_ini, rotY_ini;
GLfloat obsX, obsY, obsZ, obsX_ini, obsY_ini, obsZ_ini;

int x_ini,y_ini,bot;
int a1,a2;
int rotaciona=0;

int carro1X = 1.2;
int carro1Y = -1.4;
int carro1Z = 1.0;

int carro2X = 1.2;
int carro2Y = -1.4;
int carro2Z = 1.0;

int parada = 35;
int setarColor = 1;

int viraAngulo = 180.0f;

// Apontador para objeto
OBJ *carro1;
OBJ *carro2;
OBJ *trofeu;
OBJ *cone;
OBJ *seta;
OBJ *poste;
TEX *torcida;
OBJ *bandeira;

//OBJ *champanhe;

// Fun��o respons�vel pela especifica��o dos par�metros de ilumina��o
void DefineIluminacao (void)
{
	GLfloat luzAmbiente[4]={10.2,1.2,1.2,1.0};
	//GLfloat luzAmbiente[4]={1.0,1.0,1.0,1.0};

	GLfloat luzDifusa[4]={5.0,2.0,2.0,2.0};
	//GLfloat luzDifusa[4]={1.0,1.0,1.0,1.0};	   	// "cor"

	GLfloat luzEspecular[4]={20.0, 0.0, 1.0, 0.0};
	//GLfloat luzEspecular[4]={1.0,1.0,1.0,1.0};	// "brilho"

	GLfloat posicaoLuz[4]={1.0, 1.0, 1.0, 1.0};
	//GLfloat posicaoLuz[4]={1.0,1.0,1.0,1.0};

	// Capacidade de brilho do material
	GLfloat especularidade[4]={1.0,1.0,1.0,1.0};
	GLint especMaterial = 60;


	// Define a reflet�ncia do material
	glMaterialfv(GL_FRONT,GL_SPECULAR, especularidade);
	// Define a concentra��o do brilho
	glMateriali(GL_FRONT,GL_SHININESS,especMaterial);

	// Ativa o uso da luz ambiente
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, luzAmbiente);

	// Define os par�metros da luz de n�mero 0
	glLightfv(GL_LIGHT0, GL_AMBIENT, luzAmbiente);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, luzDifusa );
	glLightfv(GL_LIGHT0, GL_SPECULAR, luzEspecular );
	glLightfv(GL_LIGHT0, GL_POSITION, posicaoLuz );
}

void rotAmbiente(double a, double b, double c){
	//glPushMatrix();
    glTranslated(a,b,c);
    //glRotatef(rotX,1,0,0);
	//glRotatef(rotY,0,1,0);
	glTranslated(-a,-b,-c);

}

// Fun��o callback de redesenho da janela de visualiza��o
void Desenha(void)
{
	// Limpa a janela de visualiza��o com a cor
	// de fundo definida previamente
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);

        //DefineIluminacao();
	// Altera a cor do desenho para rosa
	//glColor3f(1.0f, 0.0f, 1.0f);

	/***********************************/
    /**  ###### Paredes ###### **/

    /**Direita**/
    glPushMatrix();

    rotAmbiente(0.0,0.0,0.0);
    glTranslated(10.0,0.0,0.0);
    glRotated(90,0,1,0);
    glColor3f(0.0f, 1.0f, 0.0f);
   // glRotated(rotaciona,0,0,1);
    glEnable(GL_TEXTURE_2D);
   // glNormal3f(-1,0,0);
   //DefineIluminacao();
    glBegin(GL_POLYGON);
        glVertex2i(-20,-20);
        glVertex2i(-20,20);
        // Especifica que a cor corrente � azul
        glVertex2i(200,20);
        glVertex2i(200,-20);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();

    glPushMatrix();

    glTranslated(9.9,0.0,-10.1);
    glRotated(90,0,1,0);
    glScaled(0.15,0.19,0.5);

    glEnable(GL_TEXTURE_2D);
    glNormal3f(0,-1,0);
    glBegin(GL_POLYGON);
        glColor3f(0.2f, 0.2f, 1.0f);
        glVertex2i(-10,-10);
        glVertex2i(-10,10);
        // Especifica que a cor corrente � azul
    glEnd();

    glDisable(GL_TEXTURE_2D);

    glPopMatrix();


    /**Esquerda**/

    glPushMatrix();
    rotAmbiente(0.0,0.0,0.0);

    glTranslated(-10.0,0.0,0.0);
    glRotated(90,0,1,0);
    glRotated(180,1,0,0);
    glColor3f(0.0f, 1.0f, 0.0f);
   // glRotated(rotaciona,0,0,1);
    glEnable(GL_TEXTURE_2D);
    glBegin(GL_POLYGON);
      //  glNormal3f(-1,0,0);
        glVertex2i(-20,-20);
        glVertex2i(-20,20);
        // Especifica que a cor corrente � azul
        glVertex2i(200,20);
        glVertex2i(200,-20);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();

    glPushMatrix();


    glTranslated(-9.9,0.0,-10.1);
    glRotated(180,0,1,0);

    glRotated(90,0,1,0);
    glScaled(0.15,0.19,0.5);

    glEnable(GL_TEXTURE_2D);
    glNormal3f(0,-1,0);
    glBegin(GL_POLYGON);
        glColor3f(0.2f, 0.2f, 1.0f);
        glVertex2i(-10,-10);
        glVertex2i(-10,10);
        // Especifica que a cor corrente � azul
        glVertex2i(12,10);
        glVertex2i(12,-10);
    glEnd();

    glDisable(GL_TEXTURE_2D);


    glPopMatrix();

	   /**Ambaixo**/

    glPushMatrix();
    rotAmbiente(0.0,0.0,0.0);

    glTranslated(0.0,-5.0,15.0);
    glRotated(90,0,1,0);

    glRotated(90,1,0,0);

    glColor3f(0.0f, 0.0f, 0.0f);
   // glRotated(rotaciona,0,0,1);
    glEnable(GL_TEXTURE_2D);
    glNormal3f(0,1,0);
    glBegin(GL_POLYGON);
        glVertex2i(-10,-10);
        glVertex2i(-10,10);
        // Especifica que a cor corrente � azul
        glVertex2i(200,10);
        glVertex2i(200,-10);
        glEnd();
    glDisable(GL_TEXTURE_2D);

    glPopMatrix();

         /**Cima**/

    //DefineIluminacao();
    glPushMatrix();
    rotAmbiente(0.0,0.0,0.0);
    DefineIluminacao();
    glTranslated(0.0,5.0,15.0);
    glRotated(90,0,1,0);
    glRotated(180,1,0,0);
    glRotated(90,1,0,0);
    glColor3f(1.0f, 1.0f, 0.0f);
   // glRotated(rotaciona,0,0,1);
    glEnable(GL_TEXTURE_2D);
    glNormal3f(0,-1,0);
    glBegin(GL_POLYGON);
        glVertex2i(-10,-10);
        glVertex2i(-10,10);
        // Especifica que a cor corrente � azul
        glVertex2i(200,10);
        glVertex2i(200,-10);
    glEnd();
    glDisable(GL_TEXTURE_2D);


    glPopMatrix();
    /***********************************/
    /**  ###### Carro1###### **/
    glPushMatrix();

    rotAmbiente(0.0,-2.0,0.0);
    glColor3f(0.0f, 255.0f, 0.0f);
    glScaled(3.0,3.0,3.0);
    glRotated(viraAngulo,0.0f,1.0f,0.0f);
    glTranslated(carro1X,carro1Y,carro1Z);
    DesenhaObjeto(carro1);
    glPopMatrix();

    /**  ###### Carro2 ###### **/
    glPushMatrix();


    rotAmbiente(0.0,-2.0,0.0);
    glColor3f(0.0f, 0.0f, 255.0f);
    glScaled(-2.9,3.0,3.0);
    glRotated(180.0f,0.0f,1.0f,0.0f);
    glTranslated(carro2X,carro2Y,carro2Z);
	DesenhaObjeto(carro2);
    glPopMatrix();

    /**  ###### Trofeu###### **/
    glPushMatrix();
    rotAmbiente(0.0,-2.0,0.0);
    glTranslated(1.2,-4.0,1.0);
    if(setarColor == 1){
        glColor3f(255.0f, 0.0f, 0.0f);
    } else if(setarColor == 2){
        glColor3f(0.0f, 255.0f, 0.0f);
    } else if(setarColor == 3){
        glColor3f(0.0f, 0.0f, 255.0f);
    }

    glRotated(180.0f,0.0f,1.0f,0.0f);
    glScaled(0.0030,0.0030,0.0030);
	DesenhaObjeto(trofeu);
    glPopMatrix();


     /**  ###### Bandeira###### **/
    glPushMatrix();
    rotAmbiente(0.0,-2.0,0.0);
    glTranslated(0.6,-4.0,-150.0);
   // glRotated(90.0f,0.0f,1.0f,0.0f);
    if(setarColor == 1){
        //vermelho
        glColor3f(255.0f, 0.0f, 0.0f);
        glRotated(90.0f,0.0f,1.0f,0.0f);
    } else if(setarColor == 2){
        glColor3f(0.0f, 255.0f, 0.0f);
        glRotated(160.0f,0.0f,1.0f,0.0f);
    } else if(setarColor == 3){
        //azul
        glColor3f(0.0f, 0.0f, 255.0f);
        glRotated(-360.0f,0.0f,1.0f,0.0f);
    }
    glScaled(0.25,0.25,0.25);
	DesenhaObjeto(bandeira);
    glPopMatrix();


    /**  ###### seta###### **/
    glPushMatrix();
    rotAmbiente(0.0,-2.0,0.0);
    glTranslated(-7.50,-2.20,1.47);
    glColor3f(255.0f, 0.0f, 0.0f);
    glRotated(90.0f,0.0f,1.0f,0.0f);
    glScaled(0.50,0.50,0.50);
	DesenhaObjeto(seta);
    glPopMatrix();

/**  ###### cone ###### **/
    int aumentaz=0;
    int i = 0;
    for(i = 0; i < 200; i+=10){
        glPushMatrix();
        rotAmbiente(0.0,-2.0,0.0);
        glTranslated(-0.89,-4.0,-aumentaz);
        glColor3f(255.0f, 0.0f, 0.0f);
        glRotated(180.0f,0.0f,1.0f,0.0f);
        glScaled(2.20,2.20,2.20);
        DesenhaObjeto(cone);
        glPopMatrix();
        aumentaz = i;
    }
    /**  ###### poste ###### **/
    int aumentazP=0;
    int j = 0;
    for(j = 0; j < 200; j+=30){
        glPushMatrix();
        rotAmbiente(0.0,-2.0,0.0);
        glTranslated(7.89,-4.0,-aumentazP);
        glColor3f(0.0f, 0.0f, 255.0f);
        glRotated(90.0f,0.0f,1.0f,0.0f);
        glScaled(0.60,0.60,0.60);
        DesenhaObjeto(poste);
        glPopMatrix();
        aumentazP = j;
    }

    int aumentazPz=0;
    int x = 0;
    for(x = 0; x < 200; x+=30){
        glPushMatrix();
        rotAmbiente(0.0,-2.0,0.0);
        glTranslated(-7.89,-4.0,-aumentazPz);
        glColor3f(0.0f, 0.0f, 255.0f);
        glRotated(-90.0f,0.0f,1.0f,0.0f);
        glScaled(0.60,0.60,0.60);
        DesenhaObjeto(poste);
        glPopMatrix();
        aumentazPz = x;
    }

	// Executa os comandos OpenGL
	glutSwapBuffers();
}

// Fun��o usada para especificar a posi��o do observador virtual
void PosicionaObservador(void)
{
    DefineIluminacao();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();//carrega a matriz de identidade
    gluLookAt(0.0,1.5,35.0,//posi��o da c�mera
              rotX,rotY,-40.0,//para onde a c�mera aponta
              0.0,1.0,0.0);//vetor view-up//
}

// Fun��o usada para especificar o volume de visualiza��o
void EspecificaParametrosVisualizacao(void)
{

	// Especifica sistema de coordenadas de proje��o
	glMatrixMode(GL_PROJECTION);
	// Inicializa sistema de coordenadas de proje��o
	glLoadIdentity();

	// Especifica a proje��o perspectiva(angulo,aspecto,zMin,zMax)
	gluPerspective(angle,fAspect,0.01,1200);

	// Chama a fun��o que especifica os par�metros de ilumina��o
	PosicionaObservador();
	DefineIluminacao();
	//DefineIluminacao();
}

// Fun��o callback chamada quando o tamanho da janela � alterado
void AlteraTamanhoJanela(GLsizei w, GLsizei h)
{
	// Para previnir uma divis�o por zero
	if ( h == 0 ) h = 1;

	// Especifica as dimens�es da viewport
	glViewport(0, 0, w, h);

	// Calcula a corre��o de aspecto
	fAspect = (GLfloat)w/(GLfloat)h;

	EspecificaParametrosVisualizacao();
}

// Fun��o callback para tratar eventos de teclas especiais
void Teclas (unsigned char tecla, int x, int y)
{

	if(tecla=='m')
    {
        if(glIsEnabled(GL_LIGHTING))
			glDisable(GL_LIGHTING);
		else
            glEnable(GL_LIGHTING);
	}

	// CARRO 1

    if(carro1Z != parada){
        if(tecla == 'w'){
            carro1Z+=1;
            //viraAngulo = 180.0f;
        }
    }else{
        setarColor = 2;
    }


    if(tecla == 's'){
        carro1Z-=1;
        //viraAngulo = 0.0f;
    }

	//PosicionaObservador();
	glutPostRedisplay();
}

void TeclasEspeciais (int tecla, int x, int y)
{
    // CARRO 2

    if(carro2Z != parada){
        if(tecla == GLUT_KEY_UP){
            carro2Z+=1;
            //viraAngulo = 180.0f;
        }
    }else{
        setarColor = 3;
    }

    if(tecla == GLUT_KEY_DOWN){
        carro2Z-=1;
        //viraAngulo = 0.0f;
    }

    if(tecla=='q') // ESC ?
	{
		// Libera mem�ria e finaliza programa
		LiberaObjeto(carro1);
        LiberaObjeto(carro2);
        LiberaObjeto(poste);
        LiberaObjeto(cone);
        LiberaObjeto(seta);
        LiberaObjeto(trofeu);
		exit(0);
	}



	if(tecla=='m')
	{
		if(glIsEnabled(GL_LIGHTING))
			glDisable(GL_LIGHTING);
		else
            glEnable(GL_LIGHTING);
	}
	switch (tecla)
	{
		case GLUT_KEY_HOME:	if(angle>=10)  angle -=5;
							break;
		case GLUT_KEY_END:	if(angle<=150) angle +=5;
							break;
	}
	PosicionaObservador();
	EspecificaParametrosVisualizacao();
	glutPostRedisplay();
}


// Fun��o callback para eventos de bot�es do mouse
void GerenciaMouse(int button, int state, int x, int y)
{
	if(state==GLUT_DOWN)
	{
		// Salva os par�metros atuais
		x_ini = x;
		y_ini = y;
		obsX_ini = obsX;
		obsY_ini = obsY;
		obsZ_ini = obsZ;
		rotX_ini = rotX;
		rotY_ini = rotY;
		bot = button;
	}
	else bot = -1;
}

// Fun��o callback para eventos de movimento do mouse
#define SENS_ROT	5.0
#define SENS_OBS	15.0
#define SENS_TRANSL	20.0
void GerenciaMovim(int x, int y)
{
	// Bot�o esquerdo
	if(bot==GLUT_LEFT_BUTTON)
	{
		// Calcula diferen�as
		int deltax = x_ini - x;
		int deltay = y_ini - y;
		// E modifica �ngulos
		rotY = rotY_ini - deltay/SENS_ROT;
		rotX = rotX_ini - deltax/SENS_ROT;
	}
	// Bot�o direito (zoom-in e zoom-out)
	else if(bot==GLUT_RIGHT_BUTTON){
		// Calcula diferen�a
		int deltaz = y_ini - y;
		// E modifica dist�ncia do observador
		obsZ = obsZ_ini + deltaz/SENS_OBS;
	}
	// Bot�o do meio

	rotaciona+=1%360;
	PosicionaObservador();
	glutPostRedisplay();
}



// Fun��o respons�vel por inicializar par�metros e vari�veis
void Inicializa (void)
{

    //PlaySound("topgear.mp3", NULL, SND_ASYNC|SND_FILENAME|SND_LOOP);
	char nomeArquivo[30];

	// Define a cor de fundo da janela de visualiza��o como branca
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	// Habilita a defini��o da cor do material a partir da cor corrente
	glEnable(GL_COLOR_MATERIAL);
	//Habilita o uso de ilumina��o
	glEnable(GL_LIGHTING);
	// Habilita a luz de n�mero 0
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	// Habilita o depth-buffering
	glEnable(GL_DEPTH_TEST);

    //Habilita o recorte das faces ocultas
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

	// Habilita o modelo de tonaliza��o de Gouraud
	glShadeModel(GL_FLAT);

	// Inicializa a vari�vel que especifica o �ngulo da proje��o
	// perspectiva
	angle=20;

	// Inicializa as vari�veis usadas para alterar a posi��o do
	// observador virtual
	obsX = obsY = 0;
	obsZ = 30;

	// Carrega o objeto 3D
	carro1 = CarregaObjeto("car-parsche-sport-red.obj",true);
	carro2 = CarregaObjeto("car-lamba-sport-yellow.obj",true);
	trofeu = CarregaObjeto("cup.obj", true);
	seta = CarregaObjeto("arrow1.obj", true);
	cone = CarregaObjeto("control.obj", true);
	poste = CarregaObjeto("Light_Pole.obj", true);
	bandeira = CarregaObjeto("flag.obj", true);
	//champanhe = CarregaObjeto("champagne_bottle.obj", true);
	//torcida = CarregaJPG("torcida.jpg", false);
    printf("Objeto carregado!");

	// E calcula o vetor normal em cada face
	if(carro1->normais)
	{
		// Se j� existirem normais no arquivo, apaga elas
		free(carro1->normais);
		carro1->normais_por_vertice = false;
	}
	CalculaNormaisPorFace(carro1);

    if(carro2->normais)
	{
		// Se j� existirem normais no arquivo, apaga elas
		free(carro2->normais);
		carro2->normais_por_vertice = false;
	}
	CalculaNormaisPorFace(carro2);

    if(trofeu->normais)
	{
		// Se j� existirem normais no arquivo, apaga elas
		free(trofeu->normais);
		trofeu->normais_por_vertice = false;
	}
	CalculaNormaisPorFace(trofeu);

	if(seta->normais)
	{
		// Se j� existirem normais no arquivo, apaga elas
		free(seta->normais);
		seta->normais_por_vertice = false;
	}
	CalculaNormaisPorFace(seta);

	if(cone->normais)
	{
		// Se j� existirem normais no arquivo, apaga elas
		free(cone->normais);
		cone->normais_por_vertice = false;
	}
	CalculaNormaisPorFace(cone);


	if(poste->normais)
	{
		// Se j� existirem normais no arquivo, apaga elas
		free(poste->normais);
		poste->normais_por_vertice = false;
	}
	CalculaNormaisPorFace(poste);

	if(bandeira->normais)
	{
		// Se j� existirem normais no arquivo, apaga elas
		free(bandeira->normais);
		bandeira->normais_por_vertice = false;
	}
	CalculaNormaisPorFace(poste);
}

// Programa Principal
int main(int argc, char *argv[])
{
    //system("mpg123 topgear.mp3 &");
    glutInit(&argc, argv);
	// Define do modo de opera��o da GLUT
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	// Especifica a posi��o inicial da janela GLUT
	glutInitWindowPosition(5,5);

	// Especifica o tamanho inicial em pixels da janela GLUT
	glutInitWindowSize(800,600);

	// Cria a janela passando como argumento o t�tulo da mesma
	glutCreateWindow("APS de Computa��o Gr�fica");

	// Registra a fun��o callback de redesenho da janela de visualiza��o
	glutDisplayFunc(Desenha);

	// Registra a fun��o callback de redimensionamento da janela de visualiza��o
	glutReshapeFunc(AlteraTamanhoJanela);

	// Registra a fun��o callback para tratamento das teclas normais
	glutKeyboardFunc (Teclas);

	// Registra a fun��o callback para tratamento das teclas especiais
	glutSpecialFunc (TeclasEspeciais);

	// Registra a fun��o callback para eventos de bot�es do mouse
	glutMouseFunc(GerenciaMouse);

	// Registra a fun��o callback para eventos de movimento do mouse
	glutMotionFunc(GerenciaMovim);

	// Chama a fun��o respons�vel por fazer as inicializa��es
	Inicializa();

	// Inicia o processamento e aguarda intera��es do usu�rio
	glutMainLoop();

	//system("killall mpg123");

	return 0;
}
