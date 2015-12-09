//*****************************************************
//
// bibutil.cpp
// Biblioteca de rotinas auxiliares que inclui fun��es
// para realizar as seguintes tarefas:
// - Normalizar um vetor;
// - Calcular o produto vetorial entre dois vetores;
// - Calcular um vetor normal a partir de tr�s v�rtices;
// - Rotacionar um v�rtice ao redor de um eixo (x, y ou z);
// - Ler um modelo de objeto 3D de um arquivo no formato
//		OBJ e armazenar em uma estrutura;
// - Desenhar um objeto 3D recebido por par�metro;
// - Liberar a mem�ria ocupada por um objeto 3D;
// - Calcular o vetor normal de cada face de um objeto 3D;
// - Decodificar e armazenar numa estrutura uma imagem JPG
//		para usar como textura;
// - Armazenar em uma estrutura uma imagem JPG para usar
//		como textura.
//
// Marcelo Cohen e Isabel H. Manssour
// Este c�digo acompanha o livro
// "OpenGL - Uma Abordagem Pr�tica e Objetiva"
//
//*****************************************************
#include <math.h>
#include <string.h>
#include <vector>
#include "bibutil.h"


#define DEBUG

using namespace std;

// Lista de objetos
vector<OBJ*> _objetos(0);

// Lista de materiais
vector<MAT*> _materiais(0);

// Lista de texturas
vector<TEX*> _texturas(0);

// Modo de desenho
char _modo = 't';

// Vari�veis para controlar a taxa de quadros por segundo
int _numquadro =0, _tempo, _tempoAnterior = 0;
float _ultqps = 0;

#ifndef __FREEGLUT_EXT_H__
// Fun��o para desenhar um texto na tela com fonte bitmap
void glutBitmapString(void *fonte,char *texto)
{
	// Percorre todos os caracteres
	while (*texto)
    	glutBitmapCharacter(fonte, *texto++);
}
#endif

// Calcula e retorna a taxa de quadros por segundo
float CalculaQPS(void)
{
	// Incrementa o contador de quadros
	_numquadro++;

	// Obt�m o tempo atual
	_tempo = glutGet(GLUT_ELAPSED_TIME);
	// Verifica se passou mais um segundo
	if (_tempo - _tempoAnterior > 1000)
	{
		// Calcula a taxa atual
		_ultqps = _numquadro*1000.0/(_tempo - _tempoAnterior);
		// Ajusta as vari�veis de tempo e quadro
	 	_tempoAnterior = _tempo;
		_numquadro = 0;
	}
	// Retorna a taxa atual
	return _ultqps;
}

// Escreve uma string na tela, usando uma proje��o ortogr�fica
// normalizada (0..1, 0..1)
/*void Escreve2D(float x, float y, char *str)
{
	glMatrixMode(GL_PROJECTION);
	// Salva proje��o perspectiva corrente
	glPushMatrix();
	glLoadIdentity();
	// E cria uma proje��o ortogr�fica normalizada (0..1,0..1)
	gluOrtho2D(0,1,0,1);

	glMatrixMode(GL_MODELVIEW);
	// Salva modelview corrente
	glPushMatrix();
	glLoadIdentity();

	// Posiciona no canto inferior esquerdo da janela
	glRasterPos2f(x,y);
	glColor3f(0,0,0);
	// "Escreve" a mensagem
	glutBitmapString(GLUT_BITMAP_9_BY_15,str);

	glMatrixMode(GL_PROJECTION);
	// Restaura a matriz de proje��o anterior
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	// ... e a modelview anterior
	glPopMatrix();
}*/

// Normaliza o vetor recebido por par�metro.
void Normaliza(VERT &norm)
{
	float tam = sqrt(norm.x*norm.x
			+norm.y*norm.y
			+norm.z*norm.z);
	if(tam == 0) return;
	norm.x /= tam;
	norm.y /= tam;
	norm.z /= tam;
}

// Rotina que recebe dois vetores e retorna por par�metro,
// atrav�s de vresult, o produto vetorial entre eles.
void ProdutoVetorial (VERT &v1, VERT &v2, VERT &vresult)
{
	vresult.x = v1.y * v2.z - v1.z * v2.y;
	vresult.y = v1.z * v2.x - v1.x * v2.z;
	vresult.z = v1.x * v2.y - v1.y * v2.x;
}

// Esta rotina recebe tr�s v�rtices coplanares e calcula
// o vetor normal a eles (n).
void VetorNormal(VERT vert1, VERT vert2, VERT vert3, VERT &n)
{
    VERT vet1, vet2;

    vet1.x = vert1.x - vert2.x;
    vet1.y = vert1.y - vert2.y;
    vet1.z = vert1.z - vert2.z;
    vet2.x = vert3.x - vert2.x;
    vet2.y = vert3.y - vert2.y;
    vet2.z = vert3.z - vert2.z;
    ProdutoVetorial(vet2, vet1, n);
    Normaliza(n);
}

// Rotaciona um v�rtice em torno de Z por <ang> graus
void RotaZ(VERT &in, VERT &out, float ang)
{
	float arad = ang*M_PI/180.0;
	out.x =  in.x * cos(arad) + in.y * sin(arad);
	out.y = -in.x * sin(arad) + in.y * cos(arad);
	out.z =  in.z;
}

// Rotaciona um v�rtice em torno de Y por <ang> graus
void RotaY(VERT &in, VERT &out, float ang)
{
	float arad = ang*M_PI/180.0;
	out.x = in.x * cos(arad) - in.z * sin(arad);
	out.y = in.y;
	out.z = in.x * sin(arad) + in.z * cos(arad);
}

// Rotaciona um v�rtice em torno de X por <ang> graus
void RotaX(VERT &in, VERT &out, float ang)
{
	float arad = ang*M_PI/180.0;
	out.x =  in.x;
	out.y =  in.y * cos(arad) + in.z * sin(arad);
	out.z = -in.y * sin(arad) + in.z * cos(arad);
}

// Fun��o interna, usada por CarregaObjeto para
// interpretar a defini��o de uma face em um
// arquivo .OBJ
//
// Recebe um apontador para a posi��o corrente na
// string e devolve o valor num�rico encontrado
// at� o primeiro / ou -1 caso n�o exista
//
// Tamb�m retorna em sep o separador encontrado
int leNum(char **face, char *sep)
{
	char temp[10];
	int pos = 0;
	while(**face)
	{
		if(**face == '/' || **face==' ') // achamos o separador
		{
			*sep = **face;
			(*face)++;
			break;
		}
		temp[pos++] = **face;
		(*face)++;
	}
	temp[pos]=0;
	if(!pos) // string vazia ?
		return -1;
	return atoi(temp);
}

// Procura um material pelo nome na lista e devolve
// o �ndice onde est�, ou -1 se n�o achar
int _procuraMaterial(char *nome)
{
	unsigned int i;
	for(i=0;i<_materiais.size();++i)
		if(!strcmp(nome,_materiais[i]->nome))
			return i;
	return -1;
}

// Procura um material pelo nome e devolve um
// apontador para ele ou NULL caso n�o ache
MAT *ProcuraMaterial(char *nome)
{
	int pos = _procuraMaterial(nome);
	if(pos == -1) return NULL;
	else return _materiais[pos];
}

// L� um arquivo que define materiais para um objeto 3D no
// formato .OBJ
void _leMateriais(char *nomeArquivo)
{
	char aux[256];
	FILE *fp;
	MAT *ptr;
	fp = fopen(nomeArquivo,"r");
	if (fp == NULL){
        printf("Problema:%s\n",nomeArquivo);
	}


	/* Especifica��o do arquivo de materiais (.mtl):
	 *
	 * #,!,$  - coment�rio
	 * newmtl - nome do novo material(sem espa�os)
	 * Ka	  - Cor ambiente, especificada como 3 floats
	 * Kd	  - Cor difusa, idem
	 * Ks	  - Cor especular, idem
	 * Ns	  - Coeficiente de especularidade, entre 0 and 1000
	 *          (convertido para a faixa v�lida em OpenGL (0..128) durante a leitura)
	 * d	  - Transpar�ncia - mesmo significado que o alpha em OpenGL: 0.0 a 1.0 (0.0 = transparente)
	 *
	 * Os demais campos s�o ignorados por este leitor:
	 * Tr	  - Transpar�ncia (0 = opaco, 1 = transparente)
	 * map_Kd - Especifica arquivo com a textura a ser mapeada como material difuso
	 * refl	  - Especifica arquivo com um mapa de reflex�o
	 * Ni	  - �ndice de refra��o, 1.0 ou maior, onde 1.0 representa sem refra��o
	 * bump	  - Especifica arquivo para "bump mapping"
	 * illum  - Modelo de ilumina��o (1 = apenas cores, 2 = textura, 3 = reflex�es e raytracing)
	 * map_Ka - Especifica arquivo com a textura a ser mapeada como cor ambiente
	 * map_Ks - Especifica arquivo com a textura a ser mapeada como cor especular
	 * map_Ns - Especifica arquivo com a textura a ser mapeada como reflex�o especular
	 * map_d  - Especifica arquivo cmo a textura a ser mapeada como transpar�ncia (branco � opaco, preto � transparente)
	 *
	 */


	while(!feof(fp))
	{
		fgets(aux,255,fp);

		if (strlen(aux)>1)
			aux[strlen(aux)-1]=0;	// elimina o \n lido do arquivo
		if(aux[0]=='#') continue;

		if(!strncmp(aux,"newmtl",6)) // Novo material ?
		{
			// Se material j� existe na lista, pula para o
			// pr�ximo
			if(_procuraMaterial(&aux[7])!=-1)
			{
				ptr = NULL;
				continue;
			}
			if((ptr = (MAT *) malloc(sizeof(MAT)))==NULL)
			{
				printf("Sem mem�ria para novo material!");
				exit(1);
			}
			// Adiciona � lista
			_materiais.push_back(ptr);
			// Copia nome do material
			strcpy(ptr->nome,&aux[7]);
			// N�o existe "emission" na defini��o do material
			// mas o valor pode ser setado mais tarde,
			// via SetaEmissaoMaterial(..)
			ptr->ke[0] = ptr->ke[1] = ptr->ke[2] = 0.0;
		}
		if(!strncmp(aux,"Ka ",3)) // Ambiente
		{
			if(ptr==NULL) continue;
			sscanf(aux,"Ka %f %f %f",&ptr->ka[0],&ptr->ka[1],&ptr->ka[2]);
		}
		if(!strncmp(aux,"Kd ",3)) // Difuso
		{
			if(ptr==NULL) continue;
			sscanf(aux,"Kd %f %f %f",&ptr->kd[0],&ptr->kd[1],&ptr->kd[2]);
		}
		if(!strncmp(aux,"Ks ",3)) // Especular
		{
			if(ptr==NULL) continue;
			sscanf(aux,"Ks %f %f %f",&ptr->ks[0],&ptr->ks[1],&ptr->ks[2]);
		}
		if(!strncmp(aux,"Ns ",3)) // Fator de especularidade
		{
			if(ptr==NULL) continue;
			sscanf(aux,"Ns %f",&ptr->spec);
			// Converte da faixa lida (0...1000) para o intervalo
			// v�lido em OpenGL (0...128)
			ptr->spec = ptr->spec/1000.0 * 128;
		}
		if(!strncmp(aux,"d ",2)) // Alpha
		{
			if(ptr==NULL) continue;
			// N�o existe alpha na defini��o de cada componente
			// mas o valor � lido em separado e vale para todos
			float alpha;
			sscanf(aux,"d %f",&alpha);
			ptr->ka[3] = alpha;
			ptr->kd[3] = alpha;
			ptr->ks[3] = alpha;
		}
	}
	fclose(fp);
}

// Cria e carrega um objeto 3D que esteja armazenado em um
// arquivo no formato OBJ, cujo nome � passado por par�metro.
// � feita a leitura do arquivo para preencher as estruturas
// de v�rtices e faces, que s�o retornadas atrav�s de um OBJ.
//
// O par�metro mipmap indica se deve-se gerar mipmaps a partir
// das texturas (se houver)
OBJ *CarregaObjeto(char *nomeArquivo, bool mipmap)
{
	int i;
	int vcont,ncont,fcont,tcont;
	int material, texid;
	char aux[256];
	TEX *ptr;
	FILE *fp;
	OBJ *obj;

	fp = fopen(nomeArquivo, "r");  // abre arquivo texto para leitura

#ifdef DEBUG
	printf("*** Lendo Objeto: %s\n",nomeArquivo);
#endif
	/* Especifica��o do formato .obj para objetos poligonais:
	 *
	 * # - comment�rio
	 * v x y z - v�rtice
	 * vn x y z - normal
	 * vt x y z - texcoord
	 * f a1 a2 ... - face com apenas �ndices de v�rtices
	 * f a1/b1/c1 a2/b2/c2 ... - face com �ndices de v�rtices, normais e texcoords
	 * f a1//c1 a2//c2 ... - face com �ndices de v�rtices e normais (sem texcoords)
	 *
	 * Campos ignorados:
	 * g nomegrupo1 nomegrupo2... - especifica que o objeto � parte de um ou mais grupos
	 * s numerogrupo ou off - especifica um grupo de suaviza��o ("smoothing")
	 * o nomeobjeto: define um nome para o objeto
	 *
	 * Biblioteca de materiais e textura:
	 * mtllib - especifica o arquivo contendo a biblioteca de materiais
	 * usemtl - seleciona um material da biblioteca
	 * usemat - especifica o arquivo contendo a textura a ser mapeada nas pr�ximas faces
	 *
	 */

	if(fp == NULL)
          return NULL;

	if ( ( obj = (OBJ *) malloc(sizeof(OBJ)) ) == NULL)
		return NULL;

	// Inicializa contadores do objeto
	obj->numVertices  = 0;
	obj->numFaces     = 0;
	obj->numNormais   = 0;
	obj->numTexcoords = 0;
	// A princ�pio n�o temos normais por v�rtice...
	obj->normais_por_vertice = false;
	// E tamb�m n�o temos materiais...
	obj->tem_materiais = false;
	obj->textura = -1;	// sem textura associada
	obj->dlist = -1;	// sem display list

	obj->vertices = NULL;
	obj->faces = NULL;
	obj->normais = NULL;
	obj->texcoords = NULL;

	// A primeira passagem serve apenas para contar quantos
	// elementos existem no arquivo - necess�rio para
	// alocar mem�ria depois
	while(!feof(fp))
	{
		fgets(aux,255,fp);
		if(!strncmp(aux,"v ",2)) // encontramos um v�rtice
			obj->numVertices++;
		if(!strncmp(aux,"f ",2)) // encontramos uma face
			obj->numFaces++;
		if(!strncmp(aux,"vn ",3)) // encontramos uma normal
			obj->numNormais++;
		if(!strncmp(aux,"vt ",3)) // encontramos uma texcoord
			obj->numTexcoords++;
	}
	// Agora voltamos ao in�cio do arquivo para ler os elementos
	rewind(fp);

#ifdef DEBUG
	printf("Vertices: %d\n",obj->numVertices);
	printf("Faces:    %d\n",obj->numFaces);
	printf("Normais:  %d\n",obj->numNormais);
	printf("Texcoords:%d\n",obj->numTexcoords);
#endif

	// Aloca os v�rtices
	if ( ( obj->vertices = (VERT *) malloc((sizeof(VERT)) * obj->numVertices) ) == NULL )
		return NULL;

	// Aloca as faces
	if ( ( obj->faces = (FACE *) malloc((sizeof(FACE)) * obj->numFaces) ) == NULL )
		return NULL;

	// Aloca as normais
	if(obj->numNormais)
		if ( ( obj->normais = (VERT *) malloc((sizeof(VERT)) * obj->numNormais) ) == NULL )
			return NULL;

	// Aloca as texcoords
	if(obj->numTexcoords)
		if ( ( obj->texcoords = (TEXCOORD *) malloc((sizeof(TEXCOORD)) * obj->numTexcoords) ) == NULL )
			return NULL;

	// A segunda passagem � para ler efetivamente os
	// elementos do arquivo, j� que sabemos quantos
	// tem de cada tipo
	vcont = 0;
	ncont = 0;
	tcont = 0;
	fcont = 0;
	// Material corrente = nenhum
	material = -1;
	// Textura corrente = nenhuma
	texid = -1;

	// Utilizadas para determinar os limites do objeto
	// em x,y e z
	float minx,miny,minz;
	float maxx,maxy,maxz;


	while(!feof(fp))
	{
		fgets(aux,255,fp);
		aux[strlen(aux)-1]=0;	// elimina o \n lido do arquivo
		// Pula coment�rios
		if(aux[0]=='#') continue;


		// Defini��o da biblioteca de materiais ?
		if(!strncmp(aux,"mtllib",6))
		{
				// Chama fun��o para ler e interpretar o arquivo
				// que define os materiais
				///_leMateriais(&aux[7]);
				char *matFile = new char[strlen(aux)-6];
                strcpy(matFile, &aux[7]);

                _leMateriais(matFile);

				// Indica que o objeto possui materiais
				obj->tem_materiais = true;
		}

		// Sele��o de material ?

		if(!strncmp(aux,"usemtl",6))
		{
			// Procura pelo nome e salva o �ndice para associar
			// �s pr�ximas faces
			material = _procuraMaterial(&aux[7]);
			texid = -1;
		}

		// Sele��o de uma textura (.jpg)
		if(!strncmp(aux,"usemat",6))
		{
			// �s vezes, � especificado o valor (null) como textura
			// (especialmente em se tratando do m�dulo de exporta��o
			// do Blender)
			if(!strcmp(&aux[7],"(null)"))
			{
				texid = -1;
				continue;
			}
			// Tenta carregar a textura
			//ptr = CarregaTextura(&aux[7],mipmap);
			texid = ptr->texid;
		}
		// V�rtice ?
		if(!strncmp(aux,"v ",2))
		{
			sscanf(aux,"v %f %f %f",&obj->vertices[vcont].x,
					&obj->vertices[vcont].y,
					&obj->vertices[vcont].z);
			if(!vcont)
			{
				minx = maxx = obj->vertices[vcont].x;
				miny = maxy = obj->vertices[vcont].y;
				minz = maxz = obj->vertices[vcont].z;
			}
			else
			{
				if(obj->vertices[vcont].x < minx) minx = obj->vertices[vcont].x;
				if(obj->vertices[vcont].y < miny) miny = obj->vertices[vcont].y;
				if(obj->vertices[vcont].z < minz) minz = obj->vertices[vcont].z;
				if(obj->vertices[vcont].x > maxx) maxx = obj->vertices[vcont].x;
				if(obj->vertices[vcont].y > maxy) maxy = obj->vertices[vcont].y;
				if(obj->vertices[vcont].z > maxz) maxz = obj->vertices[vcont].z;
			}
			vcont++;
		}
		// Normal ?
		if(!strncmp(aux,"vn ",3))
		{
			sscanf(aux,"vn %f %f %f",&obj->normais[ncont].x,
					&obj->normais[ncont].y,
					&obj->normais[ncont].z);
			ncont++;
			// Registra que o arquivo possui defini��o de normais por
			// v�rtice
			obj->normais_por_vertice = true;
		}

		// Texcoord ?
		if(!strncmp(aux,"vt ",3))
		{
			sscanf(aux,"vt %f %f %f",&obj->texcoords[tcont].s,
					&obj->texcoords[tcont].t,
					&obj->texcoords[tcont].r);
			tcont++;
		}

		// Face ?

		if(!strncmp(aux,"f ",2))
		{

			// Pula "f "
			char *ptr = aux+2;
			// Associa � face o �ndice do material corrente, ou -1 se
			// n�o houver
			obj->faces[fcont].mat = material;
			// Associa � face o texid da textura seleciona ou -1 se
			// n�o ouver
			obj->faces[fcont].texid = texid;
			// Tempor�rios para armazenar os �ndices desta face
			int vi[10],ti[10],ni[10];
			// Separador encontrado
			char sep;
			obj->faces[fcont].nv = 0;
			int nv = 0;
			bool tem_t = false;
			bool tem_n = false;

			// Interpreta a descri��o da face
			while(*ptr)
			{
				// L� �ndice do v�rtice
				vi[nv] = leNum(&ptr,&sep);
				// Se o separador for espa�o, significa que
				// n�o temos texcoord nem normais
				if(sep==' ')
				{
					// Nesse caso, incrementamos o contador
					// de v�rtices e continuamos
					nv++;
					continue;
				}
				// L� �ndice da texcoord
				int aux = leNum(&ptr,&sep);
				// Se o valor for != -1, significa
				// que existe um �ndice de texcoord
				if(aux!=-1)
				{
					ti[nv] = aux;
					tem_t = true;
				}
				// Mas se o separador for " ", significa
				// que n�o existe um �ndice de normal
				if(sep==' ')
				{
					// E nesse caso, pulamos para
					// o pr�ximo v�rtice
					nv++;
					continue;
				}
				// L� o �ndice da normal
				aux = leNum(&ptr,&sep);
				if(aux!=-1)
				{
					ni[nv] = aux;
					tem_n = true;
				}
				// Prepara para pr�ximo v�rtice
				nv++;
			}



			// Fim da face, aloca mem�ria para estruturas e preenche com
			// os valores lidos
			obj->faces[fcont].nv = nv;
			obj->faces[fcont].vert = (int *) malloc(sizeof(int)*nv);
			// S� aloca mem�ria para normais e texcoords se for necess�rio
			if(tem_n) obj->faces[fcont].norm = (int *) malloc(sizeof(int)*nv);
				else obj->faces[fcont].norm = NULL;
			if(tem_t) obj->faces[fcont].tex  = (int *) malloc(sizeof(int)*nv);
				else obj->faces[fcont].tex = NULL;
			// Copia os �ndices dos arrays tempor�rios para a face
			for(i=0;i<nv;++i)
			{
				// Subtra�mos -1 dos �ndices porque o formato OBJ come�a
				// a contar a partir de 1, n�o 0
				obj->faces[fcont].vert[i] = vi[i]-1;
				if(tem_n) obj->faces[fcont].norm[i] = ni[i]-1;
				if(tem_t) obj->faces[fcont].tex[i]  = ti[i]-1;
			}

			// Prepara para pr�xima face
			fcont++;
		}
	}
#ifdef DEBUG
	printf("Limites: %f %f %f - %f %f %f\n",minx,miny,minz,maxx,maxy,maxz);
#endif
	// Fim, fecha arquivo e retorna apontador para objeto
	fclose(fp);
	// Adiciona na lista
	_objetos.push_back(obj);
	return obj;
}

// Seta o modo de desenho a ser utilizado para os objetos
// 'w' - wireframe
// 's' - s�lido
// 't' - s�lido + textura
void SetaModoDesenho(char modo)
{
	if(modo!='w' && modo!='s' && modo!='t') return;
	_modo = modo;
}

// Desenha um objeto 3D passado como par�metro.
void DesenhaObjeto(OBJ *obj)
{
	int i;	// contador
	GLint ult_texid, texid;	// �ltima/atual textura
	GLenum prim = GL_POLYGON;	// tipo de primitiva
	GLfloat branco[4] = { 1.0, 1.0, 1.0, 1.0 };	// constante para cor branca

	// Gera nova display list se for o caso
	if(obj->dlist >= 1000)
		glNewList(obj->dlist-1000,GL_COMPILE_AND_EXECUTE);
	// Ou chama a display list j� associada...
	else if(obj->dlist > -1)
	{
		glCallList(obj->dlist);
		return;
	}

	// Seleciona GL_LINE_LOOP se o objetivo
	// for desenhar o objeto em wireframe
	if(_modo=='w') prim = GL_LINE_LOOP;

	// Salva atributos de ilumina��o e materiais
	glPushAttrib(GL_LIGHTING_BIT);
	glDisable(GL_TEXTURE_2D);
	// Se objeto possui materiais associados a ele,
	// desabilita COLOR_MATERIAL - caso contr�rio,
	// mant�m estado atual, pois usu�rio pode estar
	// utilizando o recurso para colorizar o objeto
	if(obj->tem_materiais)
		glDisable(GL_COLOR_MATERIAL);

	// Armazena id da �ltima textura utilizada
	// (por enquanto, nenhuma)
	ult_texid = -1;
	// Varre todas as faces do objeto
	for(i=0; i<obj->numFaces; i++)
	{
		// Usa normais calculadas por face (flat shading) se
		// o objeto n�o possui normais por v�rtice
		if(!obj->normais_por_vertice)
			glNormal3f(obj->normais[i].x,obj->normais[i].y,obj->normais[i].z);

		// Existe um material associado � face ?
		if(obj->faces[i].mat != -1)
		{
			// Sim, envia par�metros para OpenGL
			int mat = obj->faces[i].mat;
			glMaterialfv(GL_FRONT,GL_AMBIENT,_materiais[mat]->ka);
			// Se a face tem textura, ignora a cor difusa do material
			// (caso contr�rio, a textura � colorizada em GL_MODULATE)
			if(obj->faces[i].texid != -1 && _modo=='t')
				glMaterialfv(GL_FRONT,GL_DIFFUSE,branco);
			else
				glMaterialfv(GL_FRONT,GL_DIFFUSE,_materiais[mat]->kd);
			glMaterialfv(GL_FRONT,GL_SPECULAR,_materiais[mat]->ks);
			glMaterialfv(GL_FRONT,GL_EMISSION,_materiais[mat]->ke);
			glMaterialf(GL_FRONT,GL_SHININESS,_materiais[mat]->spec);
		}

		// Se o objeto possui uma textura associada, utiliza
		// o seu texid ao inv�s da informa��o em cada face
		if(obj->textura != -1)
			texid = obj->textura;
		else
			// L� o texid associado � face (-1 se n�o houver)
			texid = obj->faces[i].texid;

		// Se a �ltima face usou textura e esta n�o,
		// desabilita
		if(texid == -1 && ult_texid != -1)
			glDisable(GL_TEXTURE_2D);

		// Ativa texturas 2D se houver necessidade
		if (texid != -1 && texid != ult_texid && _modo=='t')
		{
		       glEnable(GL_TEXTURE_2D);
		       glBindTexture(GL_TEXTURE_2D,texid);
		}

		// Inicia a face
		glBegin(prim);
		// Para todos os v�rtices da face
		for(int vf=0; vf<obj->faces[i].nv;++vf)
		{
			// Se houver normais definidas para cada v�rtice,
			// envia a normal correspondente
			if(obj->normais_por_vertice)
				glNormal3f(obj->normais[obj->faces[i].norm[vf]].x,
				obj->normais[obj->faces[i].norm[vf]].y,
				obj->normais[obj->faces[i].norm[vf]].z);

			// Se houver uma textura associada...
			if(texid!=-1)
				// Envia as coordenadas associadas ao v�rtice
				glTexCoord2f(obj->texcoords[obj->faces[i].tex[vf]].s,
				obj->texcoords[obj->faces[i].tex[vf]].t);
			// Envia o v�rtice em si
			glVertex3f(obj->vertices[obj->faces[i].vert[vf]].x,
		    	obj->vertices[obj->faces[i].vert[vf]].y,
			obj->vertices[obj->faces[i].vert[vf]].z);
		}
		// Finaliza a face
		glEnd();

		// Salva a �ltima texid utilizada
		ult_texid = texid;
	} // fim da varredura de faces

	// Finalmente, desabilita as texturas
	glDisable(GL_TEXTURE_2D);
	// Restaura os atributos de ilumina��o e materiais
	glPopAttrib();
	// Se for uma nova display list...
	if(obj->dlist >= 1000)
	{
		// Finaliza a display list
		glEndList();
		// E armazena a identifica��o correta
		obj->dlist-=1000;
	}
}

// Fun��o interna para liberar a mem�ria ocupada
// por um objeto
void _liberaObjeto(OBJ *obj)
{
	// Libera arrays
	if (obj->vertices != NULL)  free(obj->vertices);
	if (obj->normais != NULL)   free(obj->normais);
	if (obj->texcoords != NULL) free(obj->texcoords);
	// Para cada face...
	for(int i=0; i<obj->numFaces;++i)
	{
		// Libera as listas de v�rtices da face
		if (obj->faces[i].vert != NULL) free(obj->faces[i].vert);
		// Libera as listas de normais da face
		if (obj->faces[i].norm != NULL) free(obj->faces[i].norm);
		// Libera as listas de texcoords da face
		if (obj->faces[i].tex  != NULL) free(obj->faces[i].tex);
	}
	// Libera array de faces
	if (obj->faces != NULL) free(obj->faces);
	// Finalmente, libera a estrutura principal
	free(obj);
}

// Libera mem�ria ocupada por um objeto 3D
void LiberaObjeto(OBJ *obj)
{
	unsigned int o;
	if(obj==NULL)	// se for NULL, libera todos os objetos
	{
		for(o=0;o<_objetos.size();++o)
			_liberaObjeto(_objetos[o]);
		_objetos.clear();
	}
	else
	{
		// Procura pelo objeto no vector
		vector<OBJ*>::iterator it = _objetos.begin();
		for(it = _objetos.begin(); it<_objetos.end(); ++it)
			// e sai do loop quando encontrar
			if(*it == obj)
				break;
		// Apaga do vector
		_objetos.erase(it);
		// E libera as estruturas internas
		_liberaObjeto(obj);
	}
}

// Libera mem�ria ocupada pela lista de materiais e texturas
void LiberaMateriais()
{
	unsigned int i;
#ifdef DEBUG
	printf("Total de materiais: %d\n",_materiais.size());
#endif
	// Para cada material
	for(i=0;i<_materiais.size();++i)
	{
#ifdef DEBUG
		printf("%s a: (%f,%f,%f,%f) - d: (%f,%f,%f,%f) - e: (%f,%f,%f,%f - %f)\n",_materiais[i]->nome,
				_materiais[i]->ka[0],_materiais[i]->ka[1],_materiais[i]->ka[2], _materiais[i]->ka[3],
				_materiais[i]->kd[0],_materiais[i]->kd[1],_materiais[i]->kd[2], _materiais[i]->kd[3],
				_materiais[i]->ks[0],_materiais[i]->ks[1],_materiais[i]->ks[2], _materiais[i]->ks[3],
				_materiais[i]->spec);
#endif
		// Libera material
		free(_materiais[i]);
	}
	// Limpa lista
	_materiais.clear();
#ifdef DEBUG
	printf("Total de texturas: %d\n",_texturas.size());
#endif
	// Para cada textura
	for(i=0;i<_texturas.size();++i)
	{
		// Libera textura - n�o � necess�rio liberar a imagem, pois esta j�
		// foi liberada durante a carga da textura - ver CarregaTextura
#ifdef DEBUG
		printf("%s: %d x %d (id: %d)\n",_texturas[i]->nome,_texturas[i]->dimx,
				_texturas[i]->dimy,_texturas[i]->texid);
#endif
		free(_texturas[i]);
	}
	// Limpa lista
	_texturas.clear();
}

// Calcula o vetor normal de cada face de um objeto 3D.
void CalculaNormaisPorFace(OBJ *obj)
{
	int i;
	// Retorna se o objeto j� possui normais por v�rtice
	if(obj->normais_por_vertice) return;
	// Aloca mem�ria para as normais (uma por face)
	if ( ( obj->normais = (VERT *) malloc((sizeof(VERT)) * obj->numFaces) ) == NULL )
			return;
	// Varre as faces e calcula a normal, usando os 3 primeiros v�rtices de
	// cada uma
	for(i=0; i<obj->numFaces; i++)
	VetorNormal(obj->vertices[obj->faces[i].vert[0]],
		obj->vertices[obj->faces[i].vert[1]],
		obj->vertices[obj->faces[i].vert[2]],obj->normais[i]);
}

char *nomes[] = {
		"posx", "negx", "posy", "negy", "posz", "negz" };

// Desabilita a gera��o de uma display list
// para o objeto especificado
void DesabilitaDisplayList(OBJ *ptr)
{
	if(ptr == NULL) return;
	if(ptr->dlist > 0 && ptr->dlist < 1000) // j� existe uma dlist ?
	{
			// Libera dlist existente
			glDeleteLists(ptr->dlist,1);
	}
	// O valor especial -2 indica que n�o queremos
	// gerar dlist para esse objeto
	ptr->dlist = -2;
}

// Fun��o interna para criar uma display list
// (na verdade, a display list em si � gerada durante a
// primeira vez em que o objeto � redesenhado)
void _criaDList(OBJ *ptr)
{
	// Se o objeto n�o possui display list, gera uma identifica��o nova
	if(ptr->dlist == -1) ptr->dlist = glGenLists(1);
	// Adiciona 1000 ao valor, para informar � rotina de desenho
	// que uma nova display list deve ser compilada
	ptr->dlist = ptr->dlist + 1000;
}

// Cria uma display list para o objeto informado
// - se for NULL, cria display lists para TODOS os objetos
// (usada na rotina de desenho, se existir)
void CriaDisplayList(OBJ *ptr)
{
	if(ptr==NULL)
	{
		for(unsigned int i=0;i<_objetos.size();++i)
		{
			ptr = _objetos[i];
			// Pula os objetos que n�o devem usar dlists
			if(ptr->dlist == -2) continue;
			_criaDList(ptr);
		}
	}
	else if(ptr->dlist != -2)_criaDList(ptr);
}
