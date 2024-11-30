#include <stdio.h>
#include <float.h>
#include <limits.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>

typedef struct Grafo {
    double **W;
    int **Pi;
    int qtd_vertices;
} Grafo;

Grafo *criar_grafo(int qtd_vertices) {
    Grafo *g = (Grafo *) malloc(sizeof(Grafo));

    if (!g) 
        return NULL;

    g->qtd_vertices = qtd_vertices;
    g->W = (double **) malloc(g->qtd_vertices * sizeof(double *));
    g->Pi = (int **) malloc(g->qtd_vertices * sizeof(int *));

    if (!g->W || !g->Pi) {
        if (g->W)
            free(g->W);

        if (g->Pi)
            free(g->Pi);

        free(g);
        return NULL;
    }

    for (int i = 0; i < g->qtd_vertices; i++) {
        g->W[i] = (double *) malloc(g->qtd_vertices * sizeof(double));
        g->Pi[i] = (int *) malloc(g->qtd_vertices * sizeof(int));

        if (!g->W[i] || !g->Pi[i]) {
            for (int j = 0; j < i; j++) {
                free(g->W[j]);
                free(g->Pi[j]);
            }
            free(g->W);
            free(g->Pi);
            free(g);
            return NULL;
        }

        for (int j = 0; j < g->qtd_vertices; j++) {
            g->W[i][j] = (i == j) ? 0 : DBL_MAX;
            g->Pi[i][j] = -1;
        }
    }
    return g;
}

void destruir_grafo(Grafo *g) {
    if (g) {
        for (int i = 0; i < g->qtd_vertices; i++) {
            if (g->W[i])
                free(g->W[i]);

            if (g->Pi[i])
                free(g->Pi[i]);
        }

        if (g->W)
            free(g->W);
        
        if (g->Pi)
            free(g->Pi);

        free(g);
    }
}

void inserir_aresta(Grafo *g, int eh_grafo_direcionado, int u, int v, double peso) {
    if (g && g->W && g->W[u] && g->Pi && g->Pi[u]) {
        g->W[u][v] = peso;
        g->Pi[u][v] = u;    // <====================================================

        if (!eh_grafo_direcionado) {
            g->W[v][u] = peso;
            g->Pi[v][u] = v;        // <====================================================
        }
    }
}

void exibir_grafo(Grafo *g) {
    if (g) {
        printf("\n");
        printf("Matriz de pesos (W):\n");
        
        printf("    ");
        for (int j = 0; j < g->qtd_vertices; j++)
            printf(" %8d ", j);
        printf("\n");


        for (int i = 0; i < g->qtd_vertices; i++) {
            printf("%2d |", i);
            for (int j = 0; j < g->qtd_vertices; j++) {
                if (g->W[i][j] == DBL_MAX) 
                    printf(" %8s ", "INF");
                else
                    printf(" %8lf ", g->W[i][j]);
            }
            printf("\n");
        }

        printf("\nMatriz de predecessores (Pi):\n");

        printf("    ");
        for (int j = 0; j < g->qtd_vertices; j++)
            printf(" %3d  ", j);
        printf("\n");

        for (int i = 0; i < g->qtd_vertices; i++) {
            printf("%1d |", i);
            for (int j = 0; j < g->qtd_vertices; j++) {
                if (g->Pi[i][j] == -1) 
                    printf(" %4s ", "NULL");
                else
                    printf(" %4d ", g->Pi[i][j]);
            }
            printf("\n");
        }
        printf("\n");
    }
}

void floyd_warshall(Grafo *g) {
    for (int k = 0; k < g->qtd_vertices; k++) {
        for (int i = 0; i < g->qtd_vertices; i++) {
            for (int j = 0; j < g->qtd_vertices; j++) {
                if (g->W[i][k] + g->W[k][j] < g->W[i][j]) {
                    g->Pi[i][j] = g->Pi[k][j];
                    g->W[i][j] = g->W[i][k] + g->W[k][j];
                }
            }
        }
    }
}

int main(void) {
    int u, v, qtd_vertices, eh_grafo_direcionado;
    double peso;

    fscanf(stdin, "%d %d", &eh_grafo_direcionado, &qtd_vertices);
    Grafo *g = criar_grafo(qtd_vertices);

    while (fscanf(stdin, "%d %d %lf", &u, &v, &peso) != EOF) {
        if (u < g->qtd_vertices && v < g->qtd_vertices) {
            inserir_aresta(g, eh_grafo_direcionado, u, v, peso);
		    printf("Aresta (%d,%d) com peso w(%d,%d) = %lf adicionada ao grafo.\n", u, v, u, v, peso);
        }
	}

    exibir_grafo(g);
    floyd_warshall(g);
    exibir_grafo(g);
    destruir_grafo(g);

    return EXIT_SUCCESS;
}