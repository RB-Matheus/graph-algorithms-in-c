#include <stdio.h>
#include <float.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>

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
	No *novo_no = (No*) malloc(sizeof(No));

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

void inicializar_fonte_unica(Grafo *g, int s) {
	printf("O inicializar_fonte_unica está prestes a executar.\n");
	for (int i = 0; i < g->qtd_vertices; i++) {
    	g->vertices[i].d = FLT_MAX;
    	g->vertices[i].pai = -1;
		printf("O vértice %d tem pai %d e estimativa inicial de %f.\n", i, g->vertices[i].pai, g->vertices[i].d);
	}
	g->vertices[s].d = 0;
	printf("O vértice fonte s = %d teve sua estimativa atualizada para %f\n", s, g->vertices[s].d);
	printf("inicializar_fonte_unica totalmente finalizado.\n\n");
}

void relaxar(Grafo *g, int u, int v, float peso) {
	printf("Estimativa do vértice v = %d: %f.\n", v, g->vertices[v].d);
	printf("Estimativa do vértive u = %d: %f.\n", u, g->vertices[u].d);
	printf("w(%d,%d) = %f.\n", u, v, peso);

	if (g->vertices[v].d > g->vertices[u].d + peso) {
		printf("A aresta (%d,%d) com peso w(%d,%d) = %f irá relaxar, pois %f > %f + (%f).\n", 
		u, v, u, v, peso, g->vertices[v].d, g->vertices[u].d, peso);

    	g->vertices[v].d = g->vertices[u].d + peso;
    	g->vertices[v].pai = u;
	} else
		printf("A aresta (%d,%d) com peso w(%d,%d) = %f não sofreu relaxamento.\n",
		u, v, u, v, peso);
}

bool bellman_ford(Grafo *g, int s) {
	inicializar_fonte_unica(g, s);

	for (int i = 0; i < g->qtd_vertices - 1; i++) {
    	for (int u = 0; u < g->qtd_vertices; u++) {
        	No *aux = g->vertices[u].lista_adjacencia; // i
       	
        	while (aux) {
            	relaxar(g, u, aux->vertice, aux->peso);
            	aux = aux->proximo_no;
        	}
    	}
	}
    
	printf("Checagem de restrição de ciclo prestes a começar.\n");
	for (int u = 0; u < g->qtd_vertices; u++) {
    	No *aux = g->vertices[u].lista_adjacencia;
       	 
    	while (aux) {
        	if (g->vertices[aux->vertice].d > g->vertices[u].d + aux->peso) {
				printf("Ops! Ciclo encontrado!\n");
				printf("%f > %f + %f\n", g->vertices[aux->vertice].d, g->vertices[u].d, aux->peso);
				printf("(%d,%d) com w(%d,%d) = %f\n", u, aux->vertice, u, aux->vertice, aux->peso);
            	return false;
			}
        	aux = aux->proximo_no;
    	}
	}
	printf("Nenhuma restrição de ciclo encontrada.\n");
	return true;
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
    printf("vertice            | estimativa (d)            | pai (pi)           | caminho mínimo\n");
    printf("-------------------|---------------------------|--------------------|------------------------------------------------------\n");
    
    for (int i = 0; i < g->qtd_vertices; i++) {
        printf("%-18d | ", i);
        printf("%-25f | ", g->vertices[i].d);
        printf("%-18d | ", g->vertices[i].pai);
        exibir_caminho_minimo(g, i);
        printf("\n");
    }
}

int main(void) {
	int eh_grafo_direcionado, qtd_vertices, u, v;
	float peso = 0;
    
	if (fscanf(stdin, "%d %d", &eh_grafo_direcionado, &qtd_vertices) == 2) {
    	Grafo *g = criar_grafo(qtd_vertices);
   	 
    	while (fscanf(stdin, "%d %d %f", &u, &v, &peso) != EOF) {
        	inserir_aresta(g, eh_grafo_direcionado, u, v, peso);
			printf("Aresta (%d,%d) com peso w = %f adicionada ao grafo.\n", u, v, peso);
		}
		printf("\n");

		printf("Grafo de entrada:");
		exibir_lista_adjacencia(g);
	 	printf("O algoritmo de Bellman-ford obteve êxito? %d.\n", bellman_ford(g, 0));
		exibir_resumo_grafo(g);
    	desalocar_grafo(g);
   	 
    	return EXIT_SUCCESS;
	}
	return EXIT_FAILURE;
}