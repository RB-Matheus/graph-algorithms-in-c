#include <math.h>
#include <stdio.h>
#include <float.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>

#define parent(i) ((i == 0) ? -1 : (int) floor((i - 1) / 2))
#define left(i) (2 * i + 1) 
#define right(i) (2 * i + 2)


typedef struct FilaPrioridade FilaPrioridade;
typedef struct Grafo Grafo;
typedef struct Vertice Vertice;
typedef struct No No;

struct Grafo {
	int qtd_vertices;
	Vertice *vertices;
};

struct Vertice {
	float d;
	int pai;
	No *lista_adjacencia;
};

struct No {
	int vertice;
	float peso;
	No *proximo_no;
};

struct FilaPrioridade {
    int m;             // tamanho da heap
    int n;            // tamanho do arranjo
    Vertice **arr;   // arranjo de vértices onde a fila será estruturada
	int *pos;       // array para rastrear a posição de cada vértice na heap
};

FilaPrioridade *criar_fila_prioridade(int n) { // n é o tamanho do arranjo onde a fila será estruturada
    FilaPrioridade *Q = (FilaPrioridade *) malloc(sizeof(FilaPrioridade));
	// m0
	if (Q) {
		Q->arr = (Vertice **) malloc(n * sizeof(Vertice *));
		Q->pos = (int *) malloc(n * sizeof(int));

		if (Q->arr && Q->pos) {
			Q->m = Q->n = n;

			return Q;
		}

		if (Q->arr) 
			free(Q->arr);

        if (Q->pos) 
			free(Q->pos);

		free(Q);
	}
	return NULL;
}

void desalocar_fila_prioridade(FilaPrioridade *Q) {
    if (Q) {
		free(Q->arr); 
		free(Q->pos);
        free(Q);
    }
}

void trocar_vertices(FilaPrioridade *Q, Grafo *g, int i, int j) {
    // Faz a troca dos ponteiros dos vértices nas posições i e j da heap
    Vertice *temp = Q->arr[i];
    Q->arr[i] = Q->arr[j];
    Q->arr[j] = temp;
    
    // Computa os índices dos vértices no array original do grafo
    int vertex_i = Q->arr[i] - g->vertices;
    int vertex_j = Q->arr[j] - g->vertices;
    
    // Atualiza o array de posições
    Q->pos[vertex_i] = i;
    Q->pos[vertex_j] = j;
}

void min_heapify(FilaPrioridade *Q, Grafo *g, int i) {
    int l = left(i), r = right(i), menor = i;
    
    if (l < Q->m && Q->arr[l]->d < Q->arr[i]->d) 
        menor = l;

    if (r < Q->m && Q->arr[r]->d < Q->arr[menor]->d) 
        menor = r;

    if (menor != i) {
        trocar_vertices(Q, g, i, menor);
        min_heapify(Q, g, menor);
    }    
}

Vertice *extrair_minimo(FilaPrioridade *Q, Grafo *g) {
    if (Q->m <= 0) return NULL;
    
    Vertice *min = Q->arr[0];
    Q->arr[0] = Q->arr[Q->m - 1];
    
    // Atualiza a posição do vértice que foi movido para a raiz
    int vertex_idx = Q->arr[0] - g->vertices;
    Q->pos[vertex_idx] = 0;
    
    Q->m--;
    min_heapify(Q, g, 0);
    return min;
}

// i: (in) chave | value: (in) novo valor para a chave i
void diminuir_chave(FilaPrioridade *Q, Grafo *g, int alvo, float valor) {
    int i = Q->pos[alvo];  // Obtém a posição atual na heap usando o índice do vértice no grafo
    
    if (valor < Q->arr[i]->d) {
        Q->arr[i]->d = valor;
        
        while (i > 0 && Q->arr[parent(i)]->d > Q->arr[i]->d) {
            trocar_vertices(Q, g, i, parent(i));
            i = parent(i);
        }
    }
}

Grafo *criar_grafo(int qtd_vertices) {
	Grafo *g = (Grafo *) malloc(sizeof(Grafo));

	if (g) {
    	g->vertices = (Vertice *) malloc(qtd_vertices * sizeof(Vertice));

    	if (g->vertices) {
        	g->qtd_vertices = qtd_vertices;

        	for (int i = 0; i < g->qtd_vertices; i++)
            	g->vertices[i].lista_adjacencia = NULL;
       	 
        	return g;
    	}
    	free(g);
	}

	return NULL;
}

void desalocar_grafo(Grafo *g) {
	if (g) {
    	if (g->vertices) {
        	No *no_atual = NULL, *prox = NULL;
       	 
        	for (int i = 0; i < g->qtd_vertices; i++) {
            	no_atual = g->vertices[i].lista_adjacencia;

            	while (no_atual) {
                	prox = no_atual->proximo_no;
                	free(no_atual);
                	no_atual = prox;
            	}
        	}
        	free(g->vertices);
    	}
    	free(g);
	}
}

No *criar_no_adjacente(int vertice, float peso) {  // "vertice" é a posição do vértice vizinho no array
	No *novo_no = (No *) malloc(sizeof(No));

	if (novo_no) {
    	novo_no->vertice = vertice;
    	novo_no->peso = peso;
    	novo_no->proximo_no = NULL;
    	return novo_no;
	}
	return NULL;
}

int aresta_existe(Grafo *g, int u, int v) {
	No *no_atual = g->vertices[u].lista_adjacencia;
    
	// Percorre a lista adj. de u para verificar se v já está nela
	while (no_atual) {
    	if (no_atual->vertice == v)
        	return 1; // (u,v) já existe   
    	no_atual = no_atual->proximo_no;
	}
	return 0; // (u,v) não existe no grafo
}

void inserir_aresta(Grafo *g, int eh_grafo_dirigido, int u, int v, float peso) {
	if (g && !aresta_existe(g, u, v)) {
    	No *vizinho = criar_no_adjacente(v, peso);

    	if (vizinho) {
        	// Colocando v na lista adj. de u
        	vizinho->proximo_no = g->vertices[u].lista_adjacencia;
        	g->vertices[u].lista_adjacencia = vizinho;
       	 
        	// Coloca u na lista adj. de v quando o grafo for simples
        	vizinho = criar_no_adjacente(u, peso);
        	if (!eh_grafo_dirigido && vizinho) {
            	vizinho->proximo_no = g->vertices[v].lista_adjacencia;
            	g->vertices[v].lista_adjacencia = vizinho;
        	}
    	}
	}
}

void inicializar_fonte_unica(Grafo *g, int s, FilaPrioridade *Q, bool *S) {
	printf("O inicializar_fonte_unica está prestes a executar.\n");
	for (int i = 0; i < g->qtd_vertices; i++) {
    	g->vertices[i].d = FLT_MAX;
    	g->vertices[i].pai = -1;
		Q->arr[i] = &g->vertices[i];
		Q->pos[i] = i;
		S[i] = false;
		printf("O vértice %d tem pai %d e estimativa inicial de %f.\n", i, g->vertices[i].pai, g->vertices[i].d);
	}
	g->vertices[s].d = 0;
	diminuir_chave(Q, g, s, 0);

	printf("O vértice fonte s = %d teve sua estimativa atualizada para %f\n", s, g->vertices[s].d);
	printf("inicializar_fonte_unica totalmente finalizado.\n\n");
}

void relaxar(Grafo *g, FilaPrioridade *Q, int u, int v, float peso) {
	printf("Estimativa do vértice v = %d: %f.\n", v, g->vertices[v].d);
	printf("Estimativa do vértive u = %d: %f.\n", u, g->vertices[u].d);
	printf("w(%d,%d) = %f.\n", u, v, peso);

	if (g->vertices[v].d > g->vertices[u].d + peso) {
		printf("A aresta (%d,%d) com peso w(%d,%d) = %f irá relaxar, pois %f > %f + (%f).\n", 
		u, v, u, v, peso, g->vertices[v].d, g->vertices[u].d, peso);

    	g->vertices[v].d = g->vertices[u].d + peso;
		g->vertices[v].pai = u;
		diminuir_chave(Q, g, v, g->vertices[v].d);
	} else
		printf("A aresta (%d,%d) com peso w(%d,%d) = %f não sofreu relaxamento.\n",
		u, v, u, v, peso);
}

void exibir_lista_adjacencia(Grafo *g) {
	if (g) {
    	No *no_atual = NULL;

    	for (int i = 0; i < g->qtd_vertices; i++) {
        	no_atual = g->vertices[i].lista_adjacencia;
        	printf("\n[%d]", i);

        	while (no_atual) {
            	printf(" ~> [%d, %f]", no_atual->vertice, no_atual->peso);
            	no_atual = no_atual->proximo_no;
        	}
    	}
    	printf("\n\n");
	}
}

void exibir_caminho_minimo(Grafo *g, int u) {
	int pai = u;

	printf("%d ", u);
	while(pai != -1) {
    	pai = g->vertices[pai].pai;
		printf("<~ %d ", pai);
	}
}

void exibir_resumo_grafo(Grafo *g) {
    printf("\nEstado do grafo:\n");
    printf("vértice            | estimativa (d)                                   | pai (pi)           | caminho mínimo\n");
    printf("-------------------|--------------------------------------------------|--------------------|------------------------------------------------------\n");
    
    for (int i = 0; i < g->qtd_vertices; i++) {
        printf("%-18d | ", i);
        printf("%-48f | ", g->vertices[i].d);
        printf("%-18d | ", g->vertices[i].pai);
        exibir_caminho_minimo(g, i);
        printf("\n");
    }
}

bool *criar_conjunto(int qtd_elementos) {
	bool *S = (bool *) malloc(qtd_elementos * sizeof(bool));

	if (S) 
		return S;
	return NULL;
}

void desalocar_conjunto(bool *S) {
	if(S)
		free(S);
}

void dijkstra(Grafo *g, int s) {
	FilaPrioridade *Q = criar_fila_prioridade(g->qtd_vertices);
	assert(Q != NULL);

	bool *S = criar_conjunto(g->qtd_vertices);
	assert(S != NULL);

	inicializar_fonte_unica(g, s, Q, S);

	//printf("Estado inicial da heap:\n");
	//for (int i = 0; i < g->qtd_vertices; i++) {
	//	printf("Q->arr[%d]->d = %f\n", i, Q->arr[i]->d);
	//	printf("Q->arr[%d] = %p\n", i, Q->arr[i]);
	//	printf("Q->arr[%d] - g->vertices = %ld\n", i, Q->arr[i] - g->vertices);
	//}

    printf("\nRodando o Dijkstra a partir do vértice fonte s = %d\n\n", s);

    while (Q->m > 0) {
		//printf("Estado da heap antes da extração:\n");
		//for (int i = 0; i < g->qtd_vertices; i++) {
			//printf("Q->arr[%d] - g->vertices = %ld\n", i, Q->arr[i] - g->vertices);
			//printf("Q->arr[%d]->d = %f\n", i, Q->arr[i]->d);
			//printf("Q->arr[%d] = %p\n", i, Q->arr[i]);
		//}

		min_heapify(Q, g, 0);
        Vertice *u_vertice = extrair_minimo(Q, g);

        int u = u_vertice - g->vertices;
        S[u] = true;
        
        printf("Vértice %d extraído da fila. Estimativa d atual: %f\n", u, u_vertice->d);
		printf("Vertice %d marcado como processado, ou seja, agora S[%d] = %d.\n", u, u, S[u]);

        //printf("Estado da heap após a extração:\n");
		//for (int i = 0; i < g->qtd_vertices; i++) {
			//printf("Q->arr[%d] - g->vertices = %ld\n", i, Q->arr[i] - g->vertices);
			//printf("Q->arr[%d]->d = %f\n", i, Q->arr[i]->d);
			//printf("Q->arr[%d] = %p\n", i, Q->arr[i]);
		//}

        No *adj = g->vertices[u].lista_adjacencia;
        while (adj) {
            int v = adj->vertice;
            if (!S[v]) { // v ainda não fi processado
                printf("\nProcessando aresta (%d,%d):\n", u, v);
                relaxar(g, Q, u, v, adj->peso);
				diminuir_chave(Q, g, v, g->vertices[v].d);
            }
            adj = adj->proximo_no;
        }
        printf("\n");
    }

	desalocar_fila_prioridade(Q);
	desalocar_conjunto(S);
}

int main(void) {
	int eh_grafo_direcionado, qtd_vertices, u, v;
	float peso = 0;
    
	if (fscanf(stdin, "%d %d", &eh_grafo_direcionado, &qtd_vertices) == 2) {
    	Grafo *g = criar_grafo(qtd_vertices);
   	 
    	while (fscanf(stdin, "%d %d %f", &u, &v, &peso) != EOF) {
        	inserir_aresta(g, eh_grafo_direcionado, u, v, peso);
			printf("Aresta (%d,%d) com peso w(%d,%d) = %f adicionada ao grafo.\n", u, v, u, v, peso);
		}
		printf("\n");

		printf("Grafo de entrada:");
		exibir_lista_adjacencia(g);

        dijkstra(g, 0);

		exibir_resumo_grafo(g);
    	desalocar_grafo(g);
   	 
    	return EXIT_SUCCESS;
	}
	return EXIT_FAILURE;
}