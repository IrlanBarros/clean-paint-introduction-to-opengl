# 🎨 CleanPaint: Laboratório 2D com OpenGL

![C](https://img.shields.io/badge/C-00599C?style=for-the-badge&logo=c&logoColor=white)
![OpenGL](https://img.shields.io/badge/OpenGL-FFFFFF?style=for-the-badge&logo=opengl)
![FreeGLUT](https://img.shields.io/badge/FreeGLUT-F34B7D?style=for-the-badge)
![Status](https://img.shields.io/badge/Status-Est%C3%A1vel-success?style=for-the-badge)

**CleanPaint** é um motor gráfico bidimensional e uma ferramenta de desenho interativa desenvolvida inteiramente em linguagem C com OpenGL. Mais do que um simples software de ilustração, o projeto foi arquitetado para ser um **laboratório de geometria computacional e álgebra linear**, demonstrando na prática como matrizes de transformação, algoritmos de colisão e estruturas de dados dinâmicas operam sob o capô das engines gráficas.

## 🚀 Principais Funcionalidades

* **Desenho de Primitivas:** Construção interativa de pontos, linhas e polígonos complexos utilizando listas encadeadas dinâmicas.
* **Transformações Geométricas Matrizes:** Aplicação de translação, rotação, escala, reflexão e cisalhamento (em relação à origem ou ao próprio centroide do objeto) utilizando operações matemáticas com matrizes homogêneas 3x3.
* **Geometria Computacional Avançada:** * Seleção inteligente de polígonos através do algoritmo de **Ray-Casting** (Regra Ímpar-Par).
  * Determinação do **Fecho Convexo (Convex Hull)** dinâmico utilizando os algoritmos de Graham Scan, Jarvis March e Chan's Algorithm.
* **Sistema de Animação e Colisão (AABB):** Um minigame / screensaver integrado onde um polígono animado navega pela tela, colidindo com as bordas da janela e com outras formas instanciadas, engolindo pontos para crescer e devorando linhas para encolher.
* **Persistência de Dados:** Capacidade de salvar o estado completo da tela (vértices e formas) em formato `.txt` e recarregá-lo perfeitamente em sessões futuras.

---

## 🏗️ Arquitetura e Engenharia de Software

O código foi estruturado com foco na manutenção, escalabilidade e na separação clara de responsabilidades, seguindo metodologias sólidas de desenvolvimento:

* **Separação de Contextos (Modularidade):** O projeto divide rigorosamente o roteamento de eventos de I/O (`events/`), as estruturas de dados e estado global (`structures/`) e a lógica matemática (`transformations/`).
* **Gerenciamento de Memória:** Implementação cuidadosa de alocação e liberação de matrizes bi-dimensionais e listas encadeadas, evitando *memory leaks* de forma determinística.
* **Desacoplamento Visual/Lógico:** A renderização visual das primitivas (`GL_LINE_LOOP`, `GL_LINES`) funciona de forma estritamente dependente dos cálculos matemáticos executados nas camadas de serviço, garantindo flexibilidade na interface gráfica.

---

## ⌨️ Controles e Atalhos

A interface é guiada inteiramente por teclado e mouse, projetada para um fluxo de trabalho rápido de desenho e edição matemática:

### Modos e Ferramentas
| Tecla | Ação | Descrição |
| :---: | :--- | :--- |
| **`D`** | Modo Desenhar | Ativa a criação de novos objetos no canvas. |
| **`S`** | Modo Selecionar | Permite clicar nas formas para manipulá-las. |
| **`1`** | Instanciar Ponto | Seleciona a ferramenta de Ponto (Modo Desenhar). |
| **`2`** | Instanciar Linha | Seleciona a ferramenta de Linha (2 cliques). |
| **`3`** | Instanciar Polígono | Ferramenta de Polígono (Botão esquerdo cria vértice, Botão direito finaliza). |

### Transformações Geométricas
*Para usar, entre no Modo Selecionar (`S`), clique em um objeto e use os atalhos abaixo:*

| Tecla / Mouse | Transformação Matemática | Comportamento |
| :---: | :--- | :--- |
| **`T` + Arrastar** | Translação | Move o objeto acompanhando o cursor do mouse. |
| **`E` + Scroll** | Escala | Amplia ou reduz o objeto a partir de seu centroide. |
| **`R` + Arrastar** | Rotação | Gira o objeto calculando o vetor entre o mouse e o centroide. |
| **`C` + Scroll** | Cisalhamento | Deforma o objeto horizontal/verticalmente. |
| **`I`** | Reflexão | Inverte o objeto em seu próprio eixo (Espelhamento). |
| **`F`** | Fecho Convexo | Converte os vértices do polígono em sua envoltória convexa otimizada. |

### Controles Globais
| Tecla | Ação |
| :---: | :--- |
| **`X`** | Exclui o objeto atualmente selecionado. |
| **`U`** | Salva a composição atual no disco (`projeto_grafico.txt`). |
| **`K`** | Carrega a composição salva no disco. |
| **`A` / `B` / `Z`**| Inicia (`A`), Pausa (`B`) ou Reseta (`Z`) a engine de animação e colisões. |
| **`ESC`** | Encerra a aplicação de forma segura. |

---

## ⚙️ Compilação e Execução

### Pré-requisitos
Para compilar e rodar o projeto, é necessário possuir um compilador C (como o GCC) e as bibliotecas de desenvolvimento do OpenGL e FreeGLUT instaladas no sistema.

**No Ubuntu/Debian:**
```bash
sudo apt update
sudo apt install build-essential freeglut3-dev libgl1-mesa-dev libglu1-mesa-dev
```

### Compilando o Projeto
Abra o terminal na pasta raiz do repositório e execute o GCC apontando para a arquitetura de pastas:

```bash
gcc src/*.c src/events/*.c src/structures/*.c src/transformations/*.c -Iinclude -o app -lm -lGL -lGLU -lglut
```
> (Alternativamente, se houver configurado um Makefile, basta rodar o comando make).

### Rodando a Aplicação
Após a compilação bem-sucedida, inicie a aplicação com o comando:

```bash
./app
```

## 👨‍💻 Autor

*Francisco Irlan de Oliveira Barros:* Estudante de Ciência da Computação, desenvolvedor e um amante de café ☕.

> "A base de um bom software não está apenas no que ele mostra na tela, mas na solidez arquitetural e matemática que o sustenta."