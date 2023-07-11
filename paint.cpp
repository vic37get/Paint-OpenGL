/*
 * Computação Gráfica
 * Codigo: Paint em OpenGL
 * Autor: Victor Ribeiro da Silva
 */

// Bibliotecas utilizadas pelo OpenGL
#ifdef __APPLE__
    #define GL_SILENCE_DEPRECATION
    #include <GLUT/glut.h>
    #include <OpenGL/gl.h>
    #include <OpenGL/glu.h>
#else
    #include <GL/glut.h>
    #include <GL/gl.h>
    #include <GL/glu.h>
#endif

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <forward_list>
#include "glut_text.h"

using namespace std;

// DEFINES
#define ESC 27
#define maxVer 10000

//Variáveis para o desenho de polígonos
bool desenha_poligono = false;
int *xPoli;
int *yPoli;
int lados_poligono = 0;

int raio;
//Enumeracao com os tipos de formas geometricas
enum tipo_forma{LIN = 1, TRI, RET, POL, CIR, FLOOD, TRANS, PREEN}; // Linha, Triangulo, Retangulo Poligono, Circulo

//Verifica se foi realizado o primeiro clique do mouse
bool click1 = false, click2 = false, click3 = false;

//Coordenadas da posicao atual do mouse
int m_x, m_y;

//Coordenadas do primeiro clique, segundo clique e terceiro clique do mouse.
int x_1, y_1, x_2, y_2, x_3, y_3;

//Indica o tipo de forma geometrica ativa para desenhar
int modo = LIN;

//Largura e altura da janela
int width = 500, height = 500;

//Cores para o FloodFIll
float novaCor[3] = {1.0, 1.0, 0.0};
float corAntiga[3] = {1.0, 1.0, 1.0};

typedef struct{
	int ymax; //y máximo da aresta
	float xdeymin; //x do y mínimo da aresta
	float m;
}Arestas;

typedef struct{
	int quantArestas; //quantidade de arestas
	Arestas arestas[maxVer];
}tabela;

tabela TA[512], TAA;

struct ponto {
    int x1;
    int y1;
    int x2;
    int y2;
};

forward_list<ponto> pontos;

void pushPonto(int x1, int y1, int x2, int y2) {
    ponto ponto;
    ponto.x1 = x1;
    ponto.y1 = y1;
    ponto.x2 = x2;
    ponto.y2 = y2;
    pontos.push_front(ponto);
}

// Definicao de vertice
struct vertice{
    int x;
    int y;
};

// Definicao das formas geometricas
struct forma{
    int tipo;
    forward_list<vertice> v; //lista encadeada de vertices
};

// Lista encadeada de formas geometricas
forward_list<forma> formas;

// Funcao para armazenar uma forma geometrica na lista de formas
// Armazena sempre no inicio da lista
void pushForma(int tipo){
    forma f;
    f.tipo = tipo;
    formas.push_front(f);
}

// Funcao para armazenar um vertice na forma do inicio da lista de formas geometricas
// Armazena sempre no inicio da lista
void pushVertice(int x, int y){
    vertice v;
    v.x = x;
    v.y = y;
    formas.front().v.push_front(v);
}

void limpaTela(){
	while(!formas.empty()){
		formas.pop_front();	
	}
	glutPostRedisplay();
}

//Fucao para armazenar uma Linha na lista de formas geometricas
void pushLinha(int x1, int y1, int x2, int y2){
    pushForma(LIN);
    pushVertice(x1, y1);
    pushVertice(x2, y2);
}

void pushTriangulo(int x1, int y1, int x2, int y2, int x3, int y3){
	pushForma(TRI);
	pushVertice(x1, y1);
	pushVertice(x2, y2);
	pushVertice(x3, y3);	
}

void pushQuadrilatero(int x1, int y1, int x2, int y2){
	pushForma(RET);
	pushVertice(x1,y1);
	pushVertice(x2,y2);
}

void pushCircunferencia(int x1, int y1, int x2, int y2, int origemX, int origemY){
	pushForma(CIR);
	pushVertice(x1, y1);
	pushVertice(x2, y2);
}

void pushFloodFill(int x1, int y1){
	pushForma(FLOOD);
	pushVertice(x1,y1);
}

/*
 * Declaracoes antecipadas (forward) das funcoes (assinaturas das funcoes)
 */
void init(void);
void reshape(int w, int h);
void display(void);
void menu_popup(int value);
void keyboard(unsigned char key, int x, int y);
void mouse(int button, int state, int x, int y);
void mousePassiveMotion(int x, int y);
void drawPixel(int x, int y);
// Funcao que percorre a lista de formas geometricas, desenhando-as na tela
void drawFormas();

// Funcao que implementa o Algoritmo de Bresenham para rasterizacao de segmentos de retas.
void linha(int x1, int y1, int x2, int y2);

//Funcao que implementa o Quadrilatero
void quadrilatero(int x1, int y1, int x2, int y2);

//Funcao que implementa o Triangulo
void triangulo(int x1, int y1, int x2, int y2, int x3, int y3);

//Funcao que implementa a Circunferencia
void circunferencia(int x1, int x2, int origemX, int origemY, int raio);

//Funções que implementam a coloração FloodFill
void floodFill(int x, int y, float oldColor[3], float fillColor[3]);

//Função que desenha o polígono.
void poligono(int* x, int* y);

//Funções do algoritmo de preenchimento de Polígonos.
void pushPreenchimento(int* x, int* y);
void initTabela();
void preenchimentoPoligono();
void addTabela (int x1,int y1, int x2, int y2);

/*
 * Inicializa alguns parametros do GLUT
 */
void init(void){
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f); //Limpa a tela com a cor branca;
}

/*
 * Ajusta a projecao para o redesenho da janela
 */
void reshape(int w, int h)
{
	// Muda para o modo de projecao e reinicializa o sistema de coordenadas
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Definindo o Viewport para o tamanho da janela
	glViewport(0, 0, w, h);
	
	width = w;
	height = h;
    glOrtho (0, w, 0, h, -1 ,1);  

   // muda para o modo de desenho
	glMatrixMode(GL_MODELVIEW);
 	glLoadIdentity();

}

/*
 * Controla os desenhos na tela
 */
void display(void){
    glClear(GL_COLOR_BUFFER_BIT); //Limpa o buffer de cores e reinicia a matriz
    glColor3f (0.0, 0.0, 0.0); // Seleciona a cor default como preto
    drawFormas(); // Desenha as formas geometricas da lista
    //Desenha texto com as coordenadas da posicao do mouse
    draw_text_stroke(0, 0, "(" + to_string(m_x) + "," + to_string(m_y) + ")", 0.2);
    glutSwapBuffers(); // manda o OpenGl renderizar as primitivas
}

/*
 * Controla o menu pop-up
 */
void menu_popup(int value){
    if (value == 0) exit(EXIT_SUCCESS);
    modo = value;
}

/*
 * Controle das teclas comuns do teclado
 */
void keyboard(unsigned char key, int x, int y){
    switch (key) { // key - variavel que possui valor ASCII da tecla precionada
    	//Tecla L, desenha Linhas.
    	case 'l':
    		modo = LIN;
    		break;
    	case 'L':
    		modo = LIN;
    		break;
    	//Tecla T, desenha Triangulos.
    	case 't':
        	modo = TRI;
        	break;
        case 'T':
        	modo = TRI;
        	break;
    	//Tecla Q, desenha Quadrilateros.
    	case 'q':
        	modo = RET;
        	break;
        case 'Q':
        	modo = RET;
        	break;
        //Tecla P, desenha Polignos.
    	case 'p':
        	modo = POL;
        	break;
        case 'P':
        	modo = POL;
        	break;
        //Tecla C, desenha Circunferencias.
    	case 'c':
        	modo = CIR;
        	break;
        case 'C':
        	modo = CIR;
        	break;
        //Tecla F, para o FloodFill.
        case 'f':
        	modo = FLOOD;
        	break;
        case 'F':
        	modo = FLOOD;
        	break;
        //Tecla G, para o Preenchimento de Poligonos.
        case 'g':
        	modo = PREEN;
        	break;
        case 'G':
        	modo = PREEN;
        	break;
        // Tecla 0, para limpar a tela.
        case '0':
        	limpaTela();
        	break;
        //Tecla ESC, para sair da aplicação.
        case ESC: exit(EXIT_SUCCESS); 
			break;
    }
}

/*
 * Controle dos botoes do mouse
 */
void mouse(int button, int state, int x, int y){
    switch (button) {
        case GLUT_LEFT_BUTTON:
                if (modo == LIN){
                    if (state == GLUT_DOWN) {
                        if(click1){
                            x_2 = x;
                            y_2 = height - y - 1;
                            pushLinha(x_1, y_1, x_2, y_2);
                            click1 = false;
                            glutPostRedisplay();
                        }else{
                            click1 = true;
                            x_1 = x;
                            y_1 = height - y - 1;
                        }
                    }
				}
				else if (modo == RET){
                    if (state == GLUT_DOWN) {
                        if(click1){
                            x_2 = x;
                            y_2 = height - y - 1;
                            pushQuadrilatero(x_1, y_1, x_2, y_2);
                            click1 = false;
                            glutPostRedisplay();
                        }else{
                            click1 = true;
                            x_1 = x;
                            y_1 = height - y - 1;
                        }
                    }
				}
            	else if(modo == TRI){
            		if (state == GLUT_DOWN) {
                        if(click2){
                            x_3 = x;
                            y_3 = height - y - 1;
                            click3 = true;
                            pushTriangulo(x_1, y_1, x_2, y_2, x_3, y_3);
                            click2 = false;
                            glutPostRedisplay();
                        }else if(click1){
                            x_2 = x;
                            y_2 = height - y - 1;
                            click2 = true;
                            click1 = false;
                            glutPostRedisplay();
                        }
                        else{
							click1 = true;
                            x_1 = x;
                            y_1 = height - y - 1;
						}
                    }
				}            	
            	else if (modo == CIR){
            		if (state == GLUT_DOWN) {
                        if(click1){
                            x_2 = x;
                            y_2 = height - y - 1;
                            pushCircunferencia(x_1, y_1, x_2, y_2, x_1, y_1);
                            click1 = false;
                            glutPostRedisplay();
                        }else{
                            click1 = true;
                            x_1 = x;
                            y_1 = height - y - 1;
                        }
                    }
				}
				else if (modo == FLOOD){
					if (state == GLUT_DOWN){
					    x_1 = x;
                        y_1 = height - y - 1;
                        floodFill(x_1, y_1, corAntiga, novaCor);
                        glutPostRedisplay();
					}
				}
				else if(modo == POL){
					//implementar o POLIGONO.
					if (state == GLUT_DOWN){
 	 	 	 	          x_1 = x;
                          y_1 = height - y - 1;
	                  		xPoli = (int*)realloc(xPoli, (lados_poligono + 1) * sizeof(int));
	                  		yPoli = (int*)realloc(yPoli, (lados_poligono + 1) * sizeof(int));
	                  		xPoli[lados_poligono] = x_1;
	                  		yPoli[lados_poligono] = y_1;
	                  		lados_poligono++;
                        }

					}
			break;
		case GLUT_RIGHT_BUTTON:
			if (modo == POL){
				if (state == GLUT_DOWN){
					desenha_poligono = true;
					poligono(xPoli, yPoli);
					lados_poligono = 0;
				    desenha_poligono = false;
					xPoli = (int*)realloc(xPoli, 0 * sizeof(int));
    	            yPoli = (int*)realloc(yPoli, 0 * sizeof(int));
    	            free(xPoli);
    	            free(yPoli);
    	            glutPostRedisplay();
					
				}
			}
			else if(modo == PREEN){
				if(state == GLUT_DOWN){
					pushPreenchimento(xPoli, yPoli);
					glutPostRedisplay();
				}
				
			}
			break;
	}
}

/*
 * Controle da posicao do cursor do mouse
 */
void mousePassiveMotion(int x, int y){
    m_x = x; m_y = height - y - 1;
    glutPostRedisplay();
}

/*
 * Funcao para desenhar apenas um pixel na tela
 */
void drawPixel(int x, int y){
    glBegin(GL_POINTS); // Seleciona a primitiva GL_POINTS para desenhar
    	glColor3f(0.0, 0.0, 0.0);
        glVertex2i(x, y);
    glEnd();  // indica o fim do ponto
}

//----------------------------------------- TRANSFORMAÇÕES GEOMÉTRICAS ----------------------------------------------------------//
void transfGeometricas(int key, int x, int y){
		int i = 0;
		int y_aux;
		int x_aux;
		forward_list<forma>::iterator primeiraForma = formas.begin();
		vertice primeiroVertice = *(primeiraForma->v.begin());
		int med_x = 0, med_y = 0, sum_x = 0, sum_y = 0;
		int count = 0;
		for (forward_list<vertice>::iterator v = primeiraForma->v.begin(); v != primeiraForma->v.end(); ++v, ++i) {
			count++;
			sum_x = sum_x + v->x;
			sum_y = sum_y + v->y;
		}
		med_x = sum_x/count;
		med_y = sum_y/count;
		
		if (primeiraForma != formas.end()) {
    		for (forward_list<vertice>::iterator v = primeiraForma->v.begin(); v != primeiraForma->v.end(); ++v, ++i) {
    			switch (key) {
    				//----------Translação----------//
    				//Para cima:
    				case GLUT_KEY_UP:
            			v->y = v->y + 10;
            			glutPostRedisplay();
            			break;
            		//Para baixo:
            		case GLUT_KEY_DOWN:
            			v->y = v->y - 10;
            			glutPostRedisplay();
            			break;
            		//Para a esquerda:
            		case GLUT_KEY_LEFT:
            			v->x = v->x - 10;
            			glutPostRedisplay();
            			break;
            		//Para a direita:
            		case GLUT_KEY_RIGHT:
            			v->x = v->x + 10;
            			glutPostRedisplay();
            			break;
            		//----------Escala----------//
            		//No eixo X:
            		case GLUT_KEY_F1:
            			v->x = v->x - med_x;
            			v->x = v->x * 1.1;
            			v->x = v->x + med_x;
            			glutPostRedisplay();
            			break;
            		//No eixo Y:
            		case GLUT_KEY_F2:
            			v->y = v->y - med_y;
            			v->y = v->y * 1.1;
            			v->y = v->y + med_y;
            			glutPostRedisplay();
            			break;
            		//----------Cisalhamento----------//
            		//No eixo X:
            		case GLUT_KEY_F3:
            			v->y = v->y - med_y;
            			y_aux = v->y;
            			v->x = v->x + (1.05*y_aux);
            			v->y = v->y + med_y;
            			glutPostRedisplay();
            			break;
            		//No eixo Y:
            		case GLUT_KEY_F4:
            			v->x = v->x - med_x;
            			x_aux = v->x;
            			v->y = v-> y + (1.05*x_aux);
            			v->x = v->x + med_x;
            			glutPostRedisplay();
            			break;
            		//----------Reflexão----------//
            		//No eixo X:
            		case GLUT_KEY_F5:
            			v->y = v->y - med_y;
            			v->y = v->y * -1;
            			v->y = v->y + med_y;
            			glutPostRedisplay();
            			break;
            		//No eixo Y:
            		case GLUT_KEY_F6:
            			v->x = v->x - med_x;
            			v->x = v->x * -1;
            			v->x = v->x + med_x;
            			glutPostRedisplay();
            			break;
            		//----------Rotacao----------//
            		case GLUT_KEY_F7:
            			v->x = v->x - med_x;
            			x_aux = v->x;
            			v->y = v->y - med_y;
            			v->x = (v->x*0.7) - (v->y*0.7);
            			v->y = (x_aux*0.7) + (v->y*0.7);
            			v->y = v->y + med_y;
            			v->x = v->x + med_x;
            			glutPostRedisplay();
            			break;		
				}
			}
        }	
        
}

/*
 *Funcao que desenha a lista de formas geometricas
 */
void drawFormas(){
	int i = 0;
    //Apos o primeiro clique, desenha a reta com a posicao atual do mouse
	//Desenha o segmento de reta apos dois cliques
	if (click1 && (modo == LIN || modo == RET) ){
	   	linha(x_1, y_1, m_x, m_y);
	}
    //Percorre a lista de formas geometricas para desenhar
    for(forward_list<forma>::iterator f = formas.begin(); f != formas.end(); f++){
        switch (f->tipo) {
        	//Feita pegando dois pontos, após o segundo clique é desenhada a linha.
            case LIN:
            	i = 0;
				int x[2];
				int y[2];
                //Percorre a lista de vertices da forma linha para desenhar
                for(forward_list<vertice>::iterator v = f->v.begin(); v != f->v.end(); v++, i++){
                    x[i] = v->x;
                    y[i] = v->y;
                }
                linha(x[0], y[0], x[1], y[1]);
                break;
            
            //Feito pegando dois pontos diagonais, após o segundo clique, é desenhado o quadrilátero.
            case RET:
      			i = 0;
	   	   	   	int xq[2];
				int yq[2];
	            for(forward_list<vertice>::iterator v = f->v.begin(); v != f->v.end(); v++, i++){
					xq[i] = v->x;
					yq[i] = v->y;
 	 	        }
	            quadrilatero(xq[0], yq[0], xq[1], yq[1]);
	            break;
	        
	        //Feita pegando três pontos, após o terceiro clique é desenhado o triângulo.
    		case TRI:
            	i = 0;
				int xt[3];
				int yt[3];
                for(forward_list<vertice>::iterator v = f->v.begin(); v != f->v.end(); v++, i++){
                    xt[i] = v->x;
                    yt[i] = v->y;
                }
                triangulo(xt[0], yt[0], xt[1], yt[1], xt[2], yt[2]);
                break;
            
            //Feita pegando dois pontos como o raio, após o segundo clique é desenhada a circunferência.
            case CIR:
            	i = 0;
				int xc[2];
				int yc[2];
				int origemX, origemY;
				
                for(forward_list<vertice>::iterator v = f->v.begin(); v != f->v.end(); v++, i++){
                    xc[i] = v->x;
                    yc[i] = v->y;
                }
                raio = (std::sqrt(std::pow((xc[1] - xc[0]), 2) + std::pow((yc[1] - yc[0]), 2)))/2;
                origemX = (xc[1] + xc[0])/2;
                origemY = (yc[1] + yc[0])/2;
                circunferencia(xc[0], yc[0], origemX, origemY, raio);
                break;
            
			 case FLOOD:
			 	i = 0;
				int xf;
				int yf;
				glBegin(GL_POINTS);
					glColor3f(novaCor[0], novaCor[1], novaCor[2]);
	                for(forward_list<vertice>::iterator v = f->v.begin(); v != f->v.end(); v++, i++){
	                    xf = v->x;
	                    yf = v->y;
						glVertex2i(xf, yf);
	                }
 				glEnd();
				glFlush();		
				break;
			
			case POL:
			 	i = 0;
			 	int xp[2];
				int yp[2];
                for (std::forward_list<vertice>::iterator v = f->v.begin(); v != f->v.end(); v++, i++) {
				    xp[i%2] = v->x;
				    yp[i%2] = v->y;
				
				    std::forward_list<vertice>::iterator proximo = std::next(v);
				    if (proximo != f->v.end()) {
				        xp[(i + 1)%2] = proximo->x;
				        yp[(i + 1)%2] = proximo->y;
				        linha(xp[i%2], yp[i%2], xp[(i + 1)%2], yp[(i + 1)%2]);
				        if (modo == PREEN && f==formas.begin()){
							addTabela(xp[i%2], yp[i%2], xp[(i + 1)%2], yp[(i + 1)%2]);
						}
				    }
				}
				if (modo == PREEN && f==formas.begin()){
					preenchimentoPoligono();
					
				}
				glutPostRedisplay();
				break;
			
			case PREEN:
				int j = 0;
			 	int xpr[2];
				int ypr[2];
				initTabela();
				glBegin(GL_LINES);
					glColor3f(1.0,0.0,0.0);
                	for (std::forward_list<vertice>::iterator v = f->v.begin(); v != f->v.end(); v++, j++) {
				    	xpr[j] = v->x;
				    	ypr[j] = v->y;	    
    	        	}
    				glVertex2i(xpr[0],ypr[0]);
					glVertex2i(xpr[1],ypr[1]);
					glEnd();
				glFlush();
				break;			
    	}
	}

}

//----------------------------------------- PREENCHIMENTO DE POLIGONOS ----------------------------------------------------------//
void pushPreenchimento(int* x, int* y){
	pushForma(PREEN);
	for(int i=0; i<lados_poligono; i++){
		pushVertice(x[i], y[i]);
		pushVertice(x[(i+1)%lados_poligono], y[(i+1)%lados_poligono]);
	}
}

void initTabela(){
	int i = 0;
	for(i = 0; i < 512; i++){
		TA[i].quantArestas = 0;
	}
	TAA.quantArestas = 0;
}

void ordenaTabela(tabela *ta){
	Arestas aux;
	int i,j;
	
	for(i = 1; i < ta -> quantArestas; i++){
		aux.ymax = ta->arestas[i].ymax;
		aux.xdeymin = ta->arestas[i].xdeymin;
		aux.m = ta->arestas[i].m;
		j = i - 1;
		
		while((aux.xdeymin < ta->arestas[j].xdeymin) && (j >= 0)){
			ta->arestas[j + 1].ymax = ta->arestas[j].ymax;
			ta->arestas[j + 1].xdeymin = ta->arestas[j].xdeymin;
			ta->arestas[j + 1].m = ta->arestas[j].m;
			j = j - 1;	
		}
		ta->arestas[j + 1].ymax = aux.ymax;
		ta->arestas[j + 1].xdeymin = aux.xdeymin;
		ta->arestas[j + 1].m = aux.m;
	}
	
}

void formarTuplas (tabela *recebe,int ym,int xm,float m){
	(recebe->arestas[(recebe)->quantArestas]).ymax = ym;
	(recebe->arestas[(recebe)->quantArestas]).xdeymin = (float)xm;
	(recebe->arestas[(recebe)->quantArestas]).m = m;
			
	ordenaTabela(recebe);
		
	(recebe->quantArestas)++;	
}

void addTabela (int x1,int y1, int x2, int y2){
	float m,minv;
	int yMaxTabela,xDeyMinTabela, scanline;
	
	if (x2==x1){
		minv=0.000000;
	}
	else{
		m = ((float)(y2-y1))/((float)(x2-x1));
		//Arestas horizontais
		if (y2==y1)
			return;
		minv = (float)1.0/m;
	}
	
	if (y1>y2){
		scanline=y2;
		yMaxTabela=y1;
		xDeyMinTabela=x2;
	}
	
	else{
		scanline=y1;
		yMaxTabela=y2;
		xDeyMinTabela=x1;	
	}
	formarTuplas(&TA[scanline],yMaxTabela,xDeyMinTabela,minv);	
}

void remove(tabela *ta, int yy){
	int i,j;
	for (i=0; i< ta->quantArestas; i++){
		if (ta->arestas[i].ymax == yy){		
			for ( j = i ; j < ta->quantArestas -1 ; j++ ){
				ta->arestas[j].ymax = ta->arestas[j+1].ymax;
				ta->arestas[j].xdeymin = ta->arestas[j+1].xdeymin;
				ta->arestas[j].m = ta->arestas[j+1].m;
		}
				ta->quantArestas--;
			i--;
		}
	}
}

void atualizax(tabela *ta){
	int i;
	
	for (i=0; i<ta->quantArestas; i++){
		(ta->arestas[i]).xdeymin = (ta->arestas[i]).xdeymin + (ta->arestas[i]).m;
	}
}

void preenchimentoPoligono(){
	int i, j, x1, ymax1, x2, ymax2, FillFlag = 0, coordCount;
	
	for (i=0; i<height; i++){
		for (j=0; j<TA[i].quantArestas; j++){
			formarTuplas(&TAA, TA[i].arestas[j].ymax, TA[i].arestas[j].xdeymin, TA[i].arestas[j].m);
		}
		remove(&TAA, i);
		ordenaTabela(&TAA);
		
		j = 0;
		FillFlag = 0;
		coordCount = 0;
		x1 = 0;
		x2 = 0;
		ymax1 = 0;
		ymax2 = 0;
		while (j<TAA.quantArestas){
			if (coordCount%2==0){
				x1 = (int)(TAA.arestas[j].xdeymin);
				ymax1 = TAA.arestas[j].ymax;
				if (x1==x2){
					if (((x1==ymax1)&&(x2!=ymax2))||((x1!=ymax1)&&(x2==ymax2))){
						x2 = x1;
						ymax2 = ymax1;
					}else{
						coordCount++;
					}
				}else{
					coordCount++;
				}
			}else{
				x2 = (int)TAA.arestas[j].xdeymin;
				ymax2 = TAA.arestas[j].ymax;
			
				FillFlag = 0;
				
				if (x1==x2){
					if (((x1==ymax1)&&(x2!=ymax2))||((x1!=ymax1)&&(x2==ymax2))){
						x1 = x2;
						ymax1 = ymax2;
					}else{
						coordCount++;
						FillFlag = 1;
					}
				}else{
					coordCount++;
					FillFlag = 1;
				}
			
			
			if(FillFlag){
				pushForma(PREEN);
				pushVertice(x1, i);
				pushVertice(x2, i);		
			}
		}			
		j++;
	}		
	atualizax(&TAA);
	}	
}

//----------------------------------------- DESENHO DE POLIGONOS ----------------------------------------------------------//
void poligono(int* x, int* y){
	pushForma(POL);
	for(int i=0; i<lados_poligono; i++){
		pushVertice(x[i], y[i]);
		pushVertice(x[(i+1)%lados_poligono], y[(i+1)%lados_poligono]);
	}
}

//----------------------------------------- DESENHO DE LINHAS ----------------------------------------------------------//
void linha(int x1, int y1, int x2, int y2){
	int deltaX, deltaY, d, incE, incNE, xi, yi, aux;
	int pontoX, pontoY, pontoX1, pontoY1, pontoX2, pontoY2;
	bool declive, simetrico, superior;
	
	pontoX1 = x1, pontoY1 = y1, pontoX2 = x2, pontoY2 = y2;
	superior = false;
	declive = false;
	simetrico = false;
	deltaX = x2 - x1;
	deltaY = y2 - y1;
	
	if (deltaX*deltaY < 0){
		y1 = -y1;
		y2 = -y2;
		deltaY = -deltaY;
		simetrico = true;
	}
		
    if (abs(deltaX) < abs(deltaY)){
    	aux = x1;
    	x1 = y1;
    	y1 = aux;
    	aux = x2;
    	x2 = y2;
    	y2 = aux;
    	aux = deltaX;
    	deltaX = deltaY;
    	deltaY = aux;
    	declive = true;
	}
    	
 	if (x1 > x2){
 		aux = x1;
 		x1 = x2;
 		x2 = aux;
 		
 		aux = y1;
 		y1 = y2;
 		y2 = aux;
 		
 		deltaX = -deltaX;
 		deltaY = -deltaY;
 		superior = true;
    }
   
    //Aqui se inicia o algoritmo de Bresenham.
	d = (2*deltaY) - deltaX;
	incE = 2*deltaY;
	incNE = 2*(deltaY - deltaX);
	xi = x1;
	yi = y1;
	
	drawPixel(pontoX1, pontoY1);
	for(xi=xi+1; xi<x2; xi++){
		if (d <= 0){
			d += incE;
		}else{
			d += incNE;
			yi++;
		}
		pontoX = xi;
		pontoY = yi;
		
		if (declive == true){
			aux = pontoX;
			pontoX = pontoY;
			pontoY = aux;
		}
			
        if (simetrico == true){
			pontoY = -pontoY;
		}
		drawPixel(pontoX, pontoY);
	}
	drawPixel(pontoX2, pontoY2);
}
	
//----------------------------------------- DESENHO DE QUADRILATEROS ----------------------------------------------------------//
void quadrilatero(int x1, int y1, int x2, int y2){
	linha(x1, y1, x2,y1);
	linha(x2, y1, x2, y2);
	linha(x2, y2, x1,y2);
	linha(x1, y2, x1, y1);
}

//----------------------------------------- DESENHO DE TRIÂNGULOS ----------------------------------------------------------//
void triangulo(int x1,int y1, int x2, int y2, int x3, int y3){
	linha(x1, y1, x2, y2);
	linha(x2, y2, x3, y3);
	linha(x3, y3, x1, y1);
}

//----------------------------------------- DESENHO DE CIRCUNFERÊNCIA ----------------------------------------------------------//
void espelhamento(int x1, int y1, int origemX, int origemY){
	//Primeiro quadrante.
	drawPixel(x1+origemX, y1+origemY);
	drawPixel(y1+origemX, x1+origemY);
	
	//Segundo quadrante.
	drawPixel(-x1+origemX, y1+origemY);
	drawPixel(y1+origemX, -x1+origemY);
	
	//Terceiro quadrnte.
	drawPixel(-x1+origemX, -y1+origemY);
	drawPixel(-y1+origemX, -x1+origemY);
	
	//Quarto quadrante.
	drawPixel(x1+origemX, -y1+origemY);
	drawPixel(-y1+origemX, x1+origemY);
}

void circunferencia(int x1, int y1, int origemX, int origemY, int raio){
	int d, deltaE, deltaSE, xi, yi;
	d = 1 - raio;
	deltaE = 3;
	deltaSE = -(2*raio)+5;
	xi = 0, yi = raio;
	while(yi > xi){
		xi = xi + 1;
		if (d < 0){
			d += deltaE;
			deltaE += 2;
			deltaSE += 2;
		}
		else{
			d += deltaSE;
			deltaE += 2;
			deltaSE += 4;
			yi--;
		}
		espelhamento(xi, yi, origemX, origemY);
	}

}

//----------------------------------------- FLOOD FILL ----------------------------------------------------------//
void floodFill(int x, int y, float oldColor[3], float fillColor[3]) {
    float pixelColor[3];
    glReadPixels(x, y, 1, 1, GL_RGB, GL_FLOAT, pixelColor);

    if (pixelColor[0] == fillColor[0] && pixelColor[1] == fillColor[1] && pixelColor[2] == fillColor[2]) {
		return;
    }
    
    if (pixelColor[0] == 0.0 && pixelColor[1] == 0.0 && pixelColor[2] == 0.0) {
		return;
    }

    if (pixelColor[0] == oldColor[0] && pixelColor[1] == oldColor[1] && pixelColor[2] == oldColor[2]) {
        glBegin(GL_POINTS);
			glColor3f(novaCor[0], novaCor[1], novaCor[2]);
        	pushFloodFill(x, y);
        	glVertex2i(x, y);
        glEnd();
        glFlush();

        floodFill(x + 1, y, corAntiga, novaCor);
        floodFill(x - 1, y, oldColor, fillColor);
        floodFill(x, y + 1, oldColor, fillColor);
        floodFill(x, y - 1, oldColor, fillColor);
    }
}

/*
 * Funcao principal
 */
int main(int argc, char** argv){
    glutInit(&argc, argv); // Passagens de parametro C para o glut
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB); //Selecao do Modo do Display e do Sistema de cor
    glutInitWindowSize (width, height);  // Tamanho da janela do OpenGL
    glutInitWindowPosition (100, 100); //Posicao inicial da janela do OpenGL
    glutCreateWindow ("Computação Gráfica: Paint com OpenGL"); // Da nome para uma janela OpenGL
    init(); // Chama funcao init();
    glutReshapeFunc(reshape); //funcao callback para redesenhar a tela
    glutKeyboardFunc(keyboard); //funcao callback do teclado
    glutSpecialFunc(transfGeometricas);
    glutMouseFunc(mouse); //funcao callback do mouse
    glutPassiveMotionFunc(mousePassiveMotion); //fucao callback do movimento passivo do mouse
    glutDisplayFunc(display); //funcao callback de desenho
    glutMainLoop(); // executa o loop do OpenGL
    return EXIT_SUCCESS; // retorna 0 para o tipo inteiro da funcao main();
}