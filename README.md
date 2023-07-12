# Paint-OpenGL
🎨 Programa Paint feito em OpenGL

## Descrição
Implementação de um editor gráfico em C++ semelhante ao Paint da Microsoft, utilizando as bibliotecas GLUT/OpenGL.

## Execução ▶
Para executar a aplicação, basta ter instalado as seguintes dependências:
  - [Falcon C++](http://falconcpp.sourceforge.net/)
  - Pacotes OpenGL/Glut
## Comandos ⌨
A aplicação conta com funcionalidades que são executadas a partir do acionamento das mesmas pelo teclado.

### 1. Limpar tela 🧯
Para limpar a tela, ou seja, apagar todos os desenhos feitos anteriormente, basta acionar a tecla `0`.

### 2. Desenho de Linhas 📏
- O desenho de linhas é uma funcionalidade inicializada por padrão com a aplicação, caso seja necessário selecionar essa funcionalidade, basta acionar a tecla `l` ou `L`.
- A linha é desenhada clicando em dois pontos com o `botão esquerdo do mouse`.

### 3. Desenho de Quadriláteros 🔲
- Para selecionar a funcionalidade de desenho de quadriláteros, basta acionar a tecla `q` ou `Q`.
- O desenho do quadrilátero é feito selecionando a coordenada do topo esquerdo e da base direita desse quadrilátero a partir do `botão esquerdo do mouse`, totalizando dois cliques.

### 4. Desenho de Triângulos 🔺
- A funcionalidade de desenho de triângulos é selecionada acionando a tecla `t` ou `T`.
- O triângulo é desenhado a partir da seleção de três coordenadas, que são obtidas a partir do clique com o `botão esquerdo do mouse`.

### 5. Desenho de Polígonos 🔷
- Para selecionar a funcionalidade de desenho de polígonos, basta acionar a tecla `p` ou `P`.
- O Polígono é desenhado da seguinte forma:
  - Pode-se selecionar seus vários pontos a partir do clique com o `botão esquerdo do mouse`.
  - Para finalizar a escolha de pontos e desenhar o polígono de vez, basta clicar com o `botão direito do mouse`.

### 6. Transformações Geométricas ⚙

#### 6.1 Translação ➡
- A translação é feita a partir do acionamento das setas do teclado, que translada o objeto rasterizado em 1 pixel por acionamento da tecla.
  - Transladar para cima: basta acionar a tecla `seta para cima`.
  - Transladar para baixo: basta acionar a tecla `seta para baixo`.
  - Transladar para a esquerda: basta acionar a tecla `seta para a esquerda`.
  - Transladar para a direita: basta acionar a tecla `seta para a direita`.
    
#### 6.2 Escala ↔
- A escala é feita com fator 1.1 sobre o eixo selecionado a partir das teclas:
  - Escala no eixo X: basta acionar a tecla `F1`.
  - Escala no eixo Y: basta acionar a tecla `F2`.
    
#### 6.3 Cisalhamento ↘
- O cisalhamento pode ser feito em ambos os eixos, ou X ou Y. O cisalhamento é feito com um fator de 1.05 a partir das teclas:
  - Cisalhamento no eixo X: basta acionar a tecla `F3`.
  - Cisalhamento no eixo Y: basta acionar a tecla `F4`.
#### 6.4 Reflexão 👁‍🗨
- A reflexão pode ser feita em ambos os eixos, ou X ou Y, para selecionar o eixo a ser aplicado a reflexão, basta acionar as teclas:
  - Reflexão sobre o eixo X: basta acionar a tecla `F5`.
  - Reflexão sobre o eixo Y: basta acionar a tecla `F6`.
  
#### 6.5 Rotação 🔄
- A funcionalidade de rotação é executada acionando a tecla `F7`.

### 7. Desenho de Circunferências 🟠
- A funcionalidade de desenho de Circunferências é selecionada ao acionar a tecla `c` ou `C`.
- O desenho da circunferência é feito selecionando duas coordenadas a partir do `botão esquerdo do mouse`, com isso a circunferência é desenhada, sendo o diâmetro da circunferência a distância entre os dois pontos.

### 8. Preenchimento de Polígonos 🔳
- A funcionalidade de preenchimento de polígonos é selecionada ao acionar a tecla `g` ou `G`.
- O último poligono rasterizado é preenchido com a cor `vermelha`.

### 9. Preenchimento Flood Fill 🖌
- A funcionalidade de preenchimento usando o algorítmo Flood Fill é selecionada ao acionar a tecla `f` ou `F`.
- A forma geométrica rasterizada é preenchida com a cor `amarela` ao clicar com o `botão esquerdo do mouse` sobre ela.
