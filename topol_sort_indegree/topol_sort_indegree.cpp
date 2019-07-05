#include <stdlib.h>
#include <stdio.h>

typedef struct {
    unsigned int first;
    unsigned int second;
} arc;
 
/* Find out if a vertex has no incoming arcs */
static unsigned int is_root(const arc *graph, const unsigned int *arcs, unsigned int size,
        unsigned int v)
{
    unsigned int a, root = 1;
    for (a = 0; a < size && root; a++) {
        root = !arcs[a] || graph[a].second != v;
    }
    return root;
}
 
/* Get the vertices with no incoming arcs */
static unsigned int get_roots(const arc *graph, const unsigned int *arcs, unsigned int size,
        unsigned int order, unsigned int *vertices)
{
    unsigned int v, vertices_size = 0;
    for (v = 0; v < order; v++) {
        if (is_root(graph, arcs, size, v)) {
            vertices[v] = 1;
            vertices_size++;
        }
    }
    return vertices_size;
}
 
unsigned int topological_sort(const arc *graph, unsigned int size, unsigned int order, 
        unsigned int **sorted)
{
    unsigned int *vertices = calloc(order, sizeof(unsigned int));
    unsigned int *arcs = malloc(size * sizeof(unsigned int));
    *sorted = malloc(order * sizeof(unsigned int));
    unsigned int v, a, vertices_size, sorted_size = 0,
            arcs_size = size;
    if (!(vertices && arcs && *sorted)) {
        free(vertices);
        free(arcs);
        free(*sorted);
        *sorted = NULL;
        return 0;
    }
    /* All arcs start off in the graph */
    for (a = 0; a < size; a++) {
        arcs[a] = 1;
    }
    /* Get the vertices with no incoming edges */
    vertices_size = get_roots(graph, arcs, size, order, vertices);
    /* Main loop */
    while (vertices_size > 0) {
        /* Get first vertex */
        for (v = 0; vertices[v] != 1; v++);
        /* Remove from vertex set */
        vertices[v] = 0;
        vertices_size--;
        /* Add it to the sorted array */
        (*sorted)[sorted_size++] = v;
        /* Remove all arcs connecting it to its neighbours */
        for (a = 0; a < size; a++) {
            if (arcs[a] && graph[a].first == v) {
                arcs[a] = 0;
                arcs_size--;
                /* Check if neighbour is now a root */
                if (is_root(graph, arcs, size, graph[a].second)) {
                    /* Add it to set of vertices */
                    vertices[graph[a].second] = 1;
                    vertices_size++;
                } 
            }
        }
    }
    free(vertices);
    free(arcs);
    return arcs_size == 0;
}

 
/* Connect two arcs */
void arc_connect(arc *arcs, unsigned int first, unsigned int second, 
        unsigned int *pos)
{
    arcs[*pos].first = first;
    arcs[*pos].second = second;
    (*pos)++;
}
 
int main(void)
{
    const unsigned int size = 8; /* Arcs */
    const unsigned int order = 8; /* Vertices */
    arc *arcs = malloc(size * sizeof(arc));
    unsigned int i = 0;
    unsigned int *sorted;
    unsigned int acyclic;
 
    arc_connect(arcs, 0, 2, &i);
    arc_connect(arcs, 1, 3, &i);
    arc_connect(arcs, 2, 3, &i);
    arc_connect(arcs, 2, 4, &i);
    arc_connect(arcs, 2, 5, &i);
    arc_connect(arcs, 3, 6, &i);
    arc_connect(arcs, 5, 7, &i);
    arc_connect(arcs, 5, 7, &i);
 
    acyclic = topological_sort(arcs, size, order, &sorted);
    printf("Graph is acyclic: %u\n", acyclic);
    for (i = 0; i < order; i++) {
        printf("%u ", sorted[i]);
    }
    putchar('\n');
 
    free(sorted);
    free(arcs);
 
    return 0;
}

