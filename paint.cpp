/*
 * Computacao Grafica
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

// Variaveis Globais
#define ESC 27

//Enumeracao com os tipos de formas geometricas
enum tipo_forma{LIN = 1, TRI, RET, POL, CIR}; // Linha, Triangulo, Retangulo Poligono, Circulo

//Verifica se foi realizado o primeiro clique do mouse
bool click1 = false, click2 = false, click3 = false;

//Coordenadas da posicao atual do mouse
int m_x, m_y;

//Coordenadas do primeiro clique, segundo clique e terceiro clique do mouse.
int x_1, y_1, x_2, y_2, x_3, y_3;

//Indica o tipo de forma geometrica ativa para desenhar
int modo = LIN;

//Largura e altura da janela
int width = 1024, height = 768;

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

void pushQuadrado(int x1, int y1, int x2, int y2){
	pushForma(RET);
	pushVertice(x1,y1);
	pushVertice(x2,y2);
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

//Funcao que implementa o Triangulo
void triangulo(int x1, int y1, int x2, int y2, int x3, int y3);

//Funcao que implementa o Quadrado
void quadrado(int x1, int y1, int x2, int y2);


/*
 * Funcao principal
 */
int main(int argc, char** argv){
    glutInit(&argc, argv); // Passagens de parametro C para o glut
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB); //Selecao do Modo do Display e do Sistema de cor
    glutInitWindowSize (width, height);  // Tamanho da janela do OpenGL
    glutInitWindowPosition (100, 100); //Posicao inicial da janela do OpenGL
    glutCreateWindow ("Computacao Grafica: Paint com OpenGL"); // Da nome para uma janela OpenGL
    init(); // Chama funcao init();
    glutReshapeFunc(reshape); //funcao callback para redesenhar a tela
    glutKeyboardFunc(keyboard); //funcao callback do teclado
    glutMouseFunc(mouse); //funcao callback do mouse
    glutPassiveMotionFunc(mousePassiveMotion); //fucao callback do movimento passivo do mouse
    glutDisplayFunc(display); //funcao callback de desenho
    
    // Define o menu pop-up
    glutCreateMenu(menu_popup);
    glutAddMenuEntry("Linha", LIN);
//    glutAddMenuEntry("Retangulo", RET);
    glutAddMenuEntry("Sair", 0);
    glutAttachMenu(GLUT_RIGHT_BUTTON);

    
    glutMainLoop(); // executa o loop do OpenGL
    return EXIT_SUCCESS; // retorna 0 para o tipo inteiro da funcao main();
}

/*
 * Inicializa alguns parametros do GLUT
 */
void init(void){
    glClearColor(1.0, 1.0, 1.0, 1.0); //Limpa a tela com a cor branca;
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
	//Todas as teclas são em minúsculo.
    switch (key) { // key - variavel que possui valor ASCII da tecla precionada
    	//Tecla L, desenha Linhas.
    	case 108:
    		modo = LIN;
    		break;
    	//Tecla T, desenha Triangulos.
    	case 116:
        	modo = TRI;
        	break;
    	//Tecla Q, desenha Quadrilateros.
    	case 113:
        	modo = RET;
        	break;
        //Tecla P, desenha Polignos.
    	case 112:
        	modo = POL;
        	break;
        //Tecla C, desenha Circunferencias.
    	case 99:
        	modo = CIR;
        	break;
    	
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
                            printf("Clique 2(%d, %d)\n",x_2,y_2);
                            pushLinha(x_1, y_1, x_2, y_2);
                            click1 = false;
                            glutPostRedisplay();
                        }else{
                            click1 = true;
                            x_1 = x;
                            y_1 = height - y - 1;
                            printf("Clique 1(%d, %d)\n",x_1,y_1);
                        }
                    }
				}
				else if (modo == RET){
                    if (state == GLUT_DOWN) {
                        if(click1){
                            x_2 = x;
                            y_2 = height - y - 1;
                            printf("Clique 2(%d, %d)\n",x_2,y_2);
                            pushQuadrado(x_1, y_1, x_2, y_2);
                            click1 = false;
                            glutPostRedisplay();
                        }else{
                            click1 = true;
                            x_1 = x;
                            y_1 = height - y - 1;
                            printf("Clique 1(%d, %d)\n",x_1,y_1);
                        }
                    }
				}
            	else if(modo == TRI){
            		if (state == GLUT_DOWN) {
                        if(click2){
                            x_3 = x;
                            y_3 = height - y - 1;
                            click3 = true;
                            printf("Clique 3(%d, %d)\n",x_3,y_3);
                            pushTriangulo(x_1, y_2, x_2, y_2, x_3, y_3);
                            click2 = false;
                            glutPostRedisplay();
                        }else if(click1){
                            x_2 = x;
                            y_2 = height - y - 1;
                            click2 = true;
                            printf("Clique 2(%d, %d)\n",x_2,y_2);
                            click1 = false;
                            glutPostRedisplay();
                        }
                        else{
							click1 = true;
                            x_1 = x;
                            y_1 = height - y - 1;
                            printf("Clique 1(%d, %d)\n",x_1,y_1);
						}
                    }
				}            	
            	if (modo == POL){
            		printf("POL!");
				}
            	if (modo == CIR){
            		printf("PEGOU!");
				}
            }

//        case GLUT_MIDDLE_BUTTON:
//            if (state == GLUT_DOWN) {
//                glutPostRedisplay();
//            }
//        break;
//
//        case GLUT_RIGHT_BUTTON:
//            if (state == GLUT_DOWN) {
//                glutPostRedisplay();
//            }
//        break;
            
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
        glVertex2i(x, y);
    glEnd();  // indica o fim do ponto
}

/*
 *Funcao que desenha a lista de formas geometricas
 */
void drawFormas(){
	int i = 0;
    //Apos o primeiro clique, desenha a reta com a posicao atual do mouse

    //Percorre a lista de formas geometricas para desenhar
    for(forward_list<forma>::iterator f = formas.begin(); f != formas.end(); f++){
        switch (f->tipo) {
            case LIN:
            	i = 0;
				int x[2];
				int y[2];
                //Percorre a lista de vertices da forma linha para desenhar
                for(forward_list<vertice>::iterator v = f->v.begin(); v != f->v.end(); v++, i++){
                    x[i] = v->x;
                    y[i] = v->y;
                }
                //Desenha o segmento de reta apos dois cliques
                if (click1 && modo == LIN){
			  		linha(x_1, y_1, m_x, m_y);
				}
                linha(x[0], y[0], x[1], y[1]);
                break;
            
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
           case RET:
      			i = 0;
	   	   	   	int xq[2];
				int yq[2];
	            for(forward_list<vertice>::iterator v = f->v.begin(); v != f->v.end(); v++, i++){
				        xq[i] = v->x;
				        yq[i] = v->y;
 	 	        }
	            quadrado(xq[0], yq[0], xq[1], yq[1]);
	            break;
        }
    }
}

//----------------------------------------- DESENHO DE LINHAS ----------------------------------------------------------//

// Função que implementa a linha usando o Algoritmo de Bresenham com redução ao primeiro octante.

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
	
//----------------------------------------- DESENHO DE TRIÂNGULOS ----------------------------------------------------------//

// Função que implementa os triângulos.
void triangulo(int x1,int y1, int x2, int y2, int x3, int y3){
	linha(x1, y1, x2, y2);
	linha(x2, y2, x3, y3);
	linha(x1, y1, x3, y3);
}


//----------------------------------------- DESENHO DE QUADRILATEROS ----------------------------------------------------------//
void quadrado(int x1, int y1, int x2, int y2){
	linha(x1, y1, x2,y1);
	linha(x2, y1, x2, y2);
	linha(x2, y2, x1,y2);
	linha(x1, y2, x1, y1);
}