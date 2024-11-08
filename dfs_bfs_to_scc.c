#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>

#define BRANCO 'b'
#define PRETO 'p'
#define CINZA 'c'


typedef struct No {
    int vertice;
    struct No *proximo_no;
} No;

typedef struct Vertice {
    char cor;
    int td, tf, d;
    No *lista_adjacencia;
    struct Vertice *pai;
} Vertice;

typedef struct Grafo {
    int qtd_vertices;
    Vertice *grafo;
} Grafo;


No *criar_no_adjacente(int vertice) {  // "vertice" é a posição do vértice vizinho no array
    No *novo_no = (No*) malloc(sizeof(No));

    if (novo_no) {
        novo_no->vertice = vertice;
        novo_no->proximo_no = NULL;
        return novo_no;
    }
    return NULL;
}

Grafo *criar_grafo(int qtd_vertices) {
    Grafo *g = (Grafo*) malloc(sizeof(Grafo));

    if (g) {
        g->grafo = (Vertice*) malloc(qtd_vertices * sizeof(Vertice));

        if (g->grafo) {
            g->qtd_vertices = qtd_vertices;

            for (int i = 0; i < qtd_vertices; i++) 
                g->grafo[i].lista_adjacencia = NULL;
            
            return g;
        }
    }

    if (g)
        free(g);

    return NULL;
}

void desalocar_grafo(Grafo *g) {
    if (g) {
        No *no_atual = NULL, *prox = NULL;

        for (int i = 0; i < g->qtd_vertices; i++) {
            no_atual = g->grafo[i].lista_adjacencia;

            while (no_atual) {
                prox = no_atual->proximo_no;
                free(no_atual);
                no_atual = prox;
            }
        }

        free(g->grafo);
        free(g);
    }
}

void exibir_grafo(Grafo *g) {

    if (g) {
        No *no_atual = NULL;

        for (int i = 0; i < g->qtd_vertices; i++) {
            no_atual = g->grafo[i].lista_adjacencia;
            printf("\n [%d]", i);

            while (no_atual) {
                printf(" ~> %d", no_atual->vertice);
                no_atual = no_atual->proximo_no;
            }
        }
        printf("\n");
    }
}

int aresta_existe(Grafo *g, int u, int v) {

    No *no_atual = g->grafo[u].lista_adjacencia;
    
    // Percorre a lista de adjacência de u para verificar se v já está nela
    while (no_atual) {
        if (no_atual->vertice == v)
            return 1; // Aresta incidente de u para v já existe   
        no_atual = no_atual->proximo_no;
    }

    return 0; // Aresta incidente de u para v não existe
}

void inserir_aresta(Grafo *g, int eh_grafo_dirigido, int u, int v) {
    // Adiciona a aresta (u, v), se ela não existir

    if (!aresta_existe(g, u, v)) {
        No *novo_no = criar_no_adjacente(v);

        if (novo_no) {
            novo_no->proximo_no = g->grafo[u].lista_adjacencia;
            g->grafo[u].lista_adjacencia = novo_no;

            if (!eh_grafo_dirigido) {
                if (u != v && !aresta_existe(g, v, u)) { // !aresta_existe(g, v, u)
                    // Adiciona a aresta (v, u) também para o caso particular de grafo simples
                    novo_no = criar_no_adjacente(u);

                    if (novo_no) {
                        novo_no->proximo_no = g->grafo[v].lista_adjacencia;
                        g->grafo[v].lista_adjacencia = novo_no;
                    }
                }
            }
        }
    }
}

void visita_dfs(Grafo *g, int u, int *tempo) {
    printf("O vértice %d tem cor '%c'\n", u, g->grafo[u].cor);

    (*tempo)++;
    g->grafo[u].td = *tempo;
    g->grafo[u].cor = CINZA;
    
    printf("Descobrindo o vértice %d, agora de cor '%c', sendo seu tempo de descoberta td = %d\n", 
    u, g->grafo[u].cor, g->grafo[u].td);

    No *v = g->grafo[u].lista_adjacencia;

    printf("A exploração dos vértices vizinhos do vértice %d será iniciada.\n", u);

    while (v) {
        if (g->grafo[v->vertice].cor == BRANCO) {
            g->grafo[v->vertice].pai = &(g->grafo[u]);
            visita_dfs(g, v->vertice, tempo);
        }
        v = v->proximo_no;
    }

    g->grafo[u].cor = PRETO;
    (*tempo)++;
    g->grafo[u].tf = *tempo;

    printf("O vértice %d foi totalmente processado, tendo tempo de finalização tf = %d e agora cor '%c'\n",
    u, g->grafo[u].tf, g->grafo[u].cor);
}

void busca_em_profundidade(Grafo *g) {
    printf("Grafo de entrada:\n");
    exibir_grafo(g);

    printf("\n");
    printf("INICIANDO A BUSCA EM PROFUNDIDADE\n\n");

    for (int i = 0; i < g->qtd_vertices; i++) {
        g->grafo[i].cor = BRANCO;
        g->grafo[i].pai = NULL;
        printf("O vértice %d recebeu cor '%c' e tem pai desconhecido\n", i, g->grafo[i].cor);
    }

    int tempo = 0;

    printf("\nComeçando a vista de vértices brancos\n");
    for (int i = 0; i < g->qtd_vertices; i++)
        if (g->grafo[i].cor == BRANCO)
            visita_dfs(g, i, &tempo);

    printf("\nBUSCA EM PROFUNDIDADE ENCERRADA\n\n\n");

    printf("Resultado final do algoritmo de busca em profundidade:\n\n");

    for (int i = 0; i < g->qtd_vertices; i++) {
        printf("O vértice %d terminou com cor '%c', tendo td = %d, tf = %d e vértice pai %d\n", 
        i, g->grafo[i].cor, g->grafo[i].td, g->grafo[i].tf, g->grafo[i].pai ? (int)(g->grafo[i].pai - g->grafo) : -1);
    }

    printf("\n\n");
}

void inserir_no_final(No **cabeca, No *novo_no) {
    if (cabeca && novo_no) {

        // Lista vazia
        if (!*cabeca) {
            *cabeca = novo_no;
        } else { // Lista não vazia
            No *temp = *cabeca;

            while (temp->proximo_no)
                temp = temp->proximo_no; // Procurando o último nó
        
            temp->proximo_no = novo_no; // Inserindo no final da lista
        }
    }
}

void inserir_no_inicio(No **cabeca, No *novo_no) {
    if (cabeca && novo_no) {

        // Lista vazia
        if (!*cabeca) {
            *cabeca = novo_no;
        } else { // Lista não vazia
            novo_no->proximo_no = *cabeca;
            *cabeca = novo_no;
        }
    }
}

int remover_no_inicio(No **cabeca) {
    if (!*cabeca || !cabeca)
        return -1; // Tentativa de remoção de fila vazia ou o ponteiro é inválido

    No *temp = *cabeca;

    int no_removido = temp->vertice;

    *cabeca = temp->proximo_no; // Atualiza a cabeça da lista para o próximo nó
    free(temp);                // Desaloca o nó removido

    return no_removido; // Retorna o nó removido
}


void busca_em_largura(Grafo *g, int s) {
    printf("Grafo de entrada:\n");
    exibir_grafo(g);

    printf("\n");

    printf("INICIANDO A BUSCA EM LARGURA\n\n");
    // "Para cada vértice não fonte do grafo"
    for (int i = 0; i < g->qtd_vertices; i++) {
        if (i != s) {
            g->grafo[i].cor = BRANCO;    // Os vértices começam sem ser visitados
            g->grafo[i].d = -1;            //  Representação da distância infinita (inalcançável) do livr
            g->grafo[i].pai = NULL;       //   Cada vértice é um "senpai" (PS: é um trocadilho, ok?)
            printf("O vértice %d recebeu cor '%c', possuindo distância infinita do vértice fonte e pai desconhecido.\n", 
            i, g->grafo[i].cor);
        }
    }

    g->grafo[s].cor = CINZA;          // Indica que o vértice fonte está em processamento (acaba de ser "descoberto")
    g->grafo[s].d = 0;               //  Distância do nó fonte até si
    g->grafo[s].pai = NULL;         //   O vértice fonte também é um "senpai"
    printf("O vértice fonte recebeu cor '%c', possuindo distância %d de si e pai desconhecido.\n\n", 
    g->grafo[s].cor, g->grafo[s].d);


    No *Q = NULL;

    printf("CRIANDO A FILA DE VÉRTICES DESCOBERTOS NÃO PROCESSADOS E INSERINDO O VÉRTICE FONTE\n\n");
    Q = criar_no_adjacente(s); //!\\ Reusei essa função para não ter que criar outra que aloca e retorna um endereço de memória do tipo No

    while (Q) {
        // Tira o primeiro vértice que está da fila
        int u = remover_no_inicio(&Q);
        printf("Retirando o vértice %d de cor '%c' para processamento.\n", u, g->grafo[u].cor);

        // Preparativo para explorar a lista de adjacência de u
        No *v = g->grafo[u].lista_adjacencia;

        while (v) {
            if (g->grafo[v->vertice].cor == BRANCO) {           // Vizinho ainda não visitado
                g->grafo[v->vertice].cor = CINZA;              // Descoberto, mas ainda não completamente explorado
                g->grafo[v->vertice].d = g->grafo[u].d + 1;   // Distância da fonte até vizinho é a distância até u + 1
                g->grafo[v->vertice].pai = &g->grafo[u];     // O pai do vizinho é u

                printf("Adicionando na fila o vértice vizinho %d, cuja cor agora é '%c', está a uma distância %d do vértice fonte e possui como pai o vértice %d\n", 
                v->vertice, g->grafo[v->vertice].cor, g->grafo[v->vertice].d, u);
                // Adicionar o vizinho na fila para processar depois
                inserir_no_final(&Q, criar_no_adjacente(v->vertice));
            }
            v = v->proximo_no;
        }

        // O vértice u neste ponto fica 100% explorado 
        g->grafo[u].cor = PRETO;
        printf("O vértice %d foi finalizado e agora tem cor '%c'\n", u, g->grafo[u].cor);
    }

    printf("\nBUSCA EM LARGURA ENCERRADA\n\n\n");

    // Mostrando o estado final de todos os vértices
    printf("Resultado final do algoritmo de busca em largura:\n\n");
    for (int i = 0; i < g->qtd_vertices; i++) {
        printf("O vértice %d terminou com cor '%c', distância do vértice fonte de %d e vértice pai %d\n", 
        i, g->grafo[i].cor, g->grafo[i].d, g->grafo[i].pai ? (int)(g->grafo[i].pai - g->grafo) : -1);
    }

    printf("\n\n");
}

Grafo *transpor_grafo(Grafo *g) {
    Grafo *g_transposto = criar_grafo(g->qtd_vertices);

    if (g_transposto) {
        No *v = NULL;

        for (int u = 0; u < g->qtd_vertices; u++) {
            v = g->grafo[u].lista_adjacencia;

            while (v) {
                inserir_aresta(g_transposto, 1, v->vertice, u);  // Inverte a direção da aresta
                v = v->proximo_no;
            }
        }
        return g_transposto;
    }
    return NULL;
}

void exibir_ordenacao_topologica(No *lista_ordenada) {
    printf("Resultado final da ordenação topológica:\n\n");
    
    No *atual = lista_ordenada;

    printf("[L]");
    while (atual) {
        printf(" ~> %d", atual->vertice);
        atual = atual->proximo_no;
    }

    printf("\n");
}

void visita_dfs_topologica(Grafo *g, int u, int *tempo, No **lista_ordenada) {
    printf("O vértice %d tem cor '%c'\n", u, g->grafo[u].cor);

    (*tempo)++;
    g->grafo[u].td = *tempo;
    g->grafo[u].cor = CINZA;
    
    printf("Descobrindo o vértice %d, agora de cor '%c', sendo seu tempo de descoberta td = %d\n", 
    u, g->grafo[u].cor, g->grafo[u].td);

    No *v = g->grafo[u].lista_adjacencia;

    printf("A exploração dos vértices vizinhos do vértice %d será iniciada.\n", u);

    while (v) {
        if (g->grafo[v->vertice].cor == BRANCO) {
            g->grafo[v->vertice].pai = &(g->grafo[u]);
            visita_dfs_topologica(g, v->vertice, tempo, lista_ordenada);
        }
        v = v->proximo_no;
    }

    g->grafo[u].cor = PRETO;
    (*tempo)++;
    g->grafo[u].tf = *tempo;

    printf("O vértice %d foi totalmente processado, tendo tempo de finalização tf = %d e agora cor '%c'\n",
    u, g->grafo[u].tf, g->grafo[u].cor);


    // Coloca o vértice u no início da lista topológica (ordena em ordem inversa de finalzação)
    printf("Inserindo o vértice %d no início da lista topológica\n", u);
    No *novo_no = criar_no_adjacente(u);
    inserir_no_inicio(lista_ordenada, novo_no);
}

No *ordenacao_topologica(Grafo *g) {
    printf("Grafo de entrada:\n");
    exibir_grafo(g);
    printf("\n");

    printf("INICIANDO A BUSCA EM PROFUNDIDADE\n\n");
    for (int i = 0; i < g->qtd_vertices; i++) {
        g->grafo[i].cor = BRANCO;
        g->grafo[i].pai = NULL;
        printf("O vértice %d recebeu cor '%c' e tem pai desconhecido\n", i, g->grafo[i].cor);
    }

    No *lista_ordenada = NULL;
    int tempo = 0;

    printf("\nComeçando a vista de vértices brancos\n");
    for (int i = 0; i < g->qtd_vertices; i++)
        if (g->grafo[i].cor == BRANCO)
            visita_dfs_topologica(g, i, &tempo, &lista_ordenada);

    printf("\nBUSCA EM PROFUNDIDADE ENCERRADA\n\n\n");


    printf("Resultado final do algoritmo de busca em profundidade:\n\n");

    for (int i = 0; i < g->qtd_vertices; i++) {
        printf("O vértice %d terminou com cor '%c', tendo td = %d, tf = %d e vértice pai %d\n", 
        i, g->grafo[i].cor, g->grafo[i].td, g->grafo[i].tf, g->grafo[i].pai ? (int)(g->grafo[i].pai - g->grafo) : -1);
    }

    printf("\n\n");

    exibir_ordenacao_topologica(lista_ordenada);

    return lista_ordenada;
}

void desalocar_lista(No *lista_ordenada) {
    while (lista_ordenada) {
        No *temp = lista_ordenada;

        lista_ordenada = lista_ordenada->proximo_no;
        free(temp);
    }
}

void visita_dfs_componente(Grafo *g, int u, No **componente) {
    g->grafo[u].cor = CINZA;

    No *v = g->grafo[u].lista_adjacencia;
    while (v) {
        if (g->grafo[v->vertice].cor == BRANCO) {
            visita_dfs_componente(g, v->vertice, componente);
        }
        v = v->proximo_no;
    }

    No *novo_no = criar_no_adjacente(u);
    inserir_no_inicio(componente, novo_no);
}

void cfcs(Grafo *g) {
    // Primeira DFS para calcular o tf de cada vértice e já ordenar g topologicamente
    No *lista_ordenada = ordenacao_topologica(g);

    Grafo *g_transposto = transpor_grafo(g);    

    printf("Grafo transposto:\n");
    exibir_grafo(g_transposto);

    for (int i = 0; i < g_transposto->qtd_vertices; i++) {
        g_transposto->grafo[i].cor = BRANCO;
        g_transposto->grafo[i].pai = NULL;
    }

    No *temp = lista_ordenada;

    printf("\nAs componentes fortemente conexas são:\n\n");

    while (temp) {
        int vertice = temp->vertice;
        temp = temp->proximo_no;
        
        if (g_transposto->grafo[vertice].cor == BRANCO) {
            No *componente = NULL;

            visita_dfs_componente(g_transposto, vertice, &componente);

            printf("Componente: [ ");

            No *c = componente;
            while (c) {
                printf("%d ", c->vertice);
                No *proximo = c->proximo_no;
                free(c);
                c = proximo;
            }
            printf("]\n");
        }
    }

    desalocar_grafo(g_transposto);
    desalocar_lista(lista_ordenada);
}

int main(void) {

    int eh_grafo_direcionado, qtd_vertices, u, v;


    if (fscanf(stdin, "%d %d", &eh_grafo_direcionado, &qtd_vertices) == 2) {
        // Criando o grafo
        Grafo *g = criar_grafo(qtd_vertices);

        if (g) {
            // Inserindo as arestas do grafo
            while (fscanf(stdin, "%d %d", &u, &v) == 2  
                   &&  u < qtd_vertices
                   &&  v < qtd_vertices)
                    inserir_aresta(g, eh_grafo_direcionado, u, v);


            // busca_em_largura(g, 0); // (G, s)
            // busca_em_profundidade(g); // (G)

            
            //if (eh_grafo_direcionado) {
                //No *lista_ordenada = ordenacao_topologica(g);
                //desalocar_lista(lista_ordenada);
                //cfcs(g);
            //} 
            

            desalocar_grafo(g);

            return EXIT_SUCCESS;
        }
    }

    return EXIT_FAILURE;
}