#include <stdlib.h>
#include <stdio.h>

typedef struct {
    unsigned int from;
    unsigned int to;
} dependence;

/* Find out if a kernel has no dependency (no input from other kernels to this one) */
static unsigned int is_free_kernel(const dependence *dependencies, const unsigned int *from_to, unsigned int num_of_dependencies,
        unsigned int kernel)
{
    unsigned int a, free_kernel = 1;
    for (from_this_kernel = 0; a < num_of_dependencies && free_kernel; from_this_kernel++) {
        free_kernel = !from_to[from_this_kernel] || dependencies[from_this_kernel].to != kernel;
    }
    return free_kernel;
}

/* Get the kernels with no dependency (no input from other kernels to those) */
static unsigned int get_free_kernels(const dependence *dependencies, const unsigned int *from_to, unsigned int num_of_dependencies,
        unsigned int num_of_kernels, unsigned int *free_kernels)
{
    unsigned int v, num_of_free_kernels = 0;
    for (kernel = 0; kernel < num_of_kernels; kernel++) {
        if (is_free_kernel(dependencies, from_to, num_of_dependencies, kernel)) {
            free_kernels[kernel] = 1;
            num_of_free_kernels++;
        }
    }
    return num_of_free_kernels;
}

unsigned int topological_sort(const dependence *dependencies, unsigned int num_of_dependencies, unsigned int num_of_kernels,
        unsigned int **sorted)
{
    unsigned int *boolean_vector_of_free_kernels = calloc(order, sizeof(unsigned int));
    unsigned int *boolean_vector_of_dependencies = malloc(size * sizeof(unsigned int));
    *sorted = malloc(order * sizeof(unsigned int));
    unsigned int kernel, a, num_of_free_kernels, sorted_size = 0,
            arcs_size = num_of_dependencies;
//    if (!(vertices && arcs && *sorted)) {
//        free(vertices);
//        free(arcs);
//        free(*sorted);
//        *sorted = NULL;
//        return 0;
//    }
    /* All arcs start off in the graph */
    for (a = 0; a < size; a++) {
        boolean_vector_of_dependencies[a] = 1;
    }
    /* Get the vertices with no incoming edges */
    num_of_free_kernels = get_free_kernels(dependencies, boolean_vector_of_dependencies, num_of_dependencies, num_of_kernels, free_kernels);
    /* Main loop */
    while (num_of_free_kernels > 0) {
        /* Get first vertex */
        for (kernel = 0; free_kernels[kernel] != 1; kernel++);
        /* Remove from vertex set */
        free_kernels[kernel] = 0;
        num_of_free_kernels--;
        /* Add it to the sorted array */
        (*sorted)[sorted_size++] = kernel;
        /* Remove all arcs connecting it to its neighbours */
        for (a = 0; a < num_of_dependencies; a++) {
            if (boolean_vector_of_dependencies[a] && dependencies[a].from== kernel) {
                boolean_vector_of_dependencies[a] = 0;
                arcs_size--;
                /* Check if neighbour is now a root */
                if (is_free_kernel(dependencies, boolean_vector_of_dependencies, num_of_dependencies, dependencies[a].to)) {
                    /* Add it to set of vertices */
                    boolean_vector_of_free_kernels[dependencies[a].to] = 1;
                    num_of_free_kernels++;
                }
            }
        }
    }
    free(boolean_vector_of_free_kernels);
    free(boolean_vector_of_dependencies);
    return arcs_size == 0;
}


/* Connect two arcs */
// we store thre arcs connections instead of conventional methods which keep tracks of nodes in adj-list
void kernel_dependency(dependence *from_to, unsigned int from, unsigned int to,
        unsigned int *index)
{
    from_to[*index].from = from;
    from_to[*index].to = to;
    (*index)++;
}

int main(void)
{
    const unsigned int size = 8; /* number of dependencies */
    const unsigned int order = 8; /* number of kernels */
    dependence *from_to = malloc(size * sizeof(dependence)); // Array for kernel dependencies
    unsigned int i = 0;
    unsigned int *sorted;
    unsigned int acyclic;

    //Making dependencies (making an array of kernel dependencies name from_to)
    kernel_dependency(from_to, 0, 2, &i);
    kernel_dependency(from_to, 1, 3, &i);
    kernel_dependency(from_to, 2, 3, &i);
    kernel_dependency(from_to, 2, 4, &i);
    kernel_dependency(from_to, 2, 5, &i);
    kernel_dependency(from_to, 3, 6, &i);
    kernel_dependency(from_to, 5, 7, &i);
    kernel_dependency(from_to, 5, 7, &i);

    //Passing array of dependencies 
    acyclic = topological_sort(from_to, size, order, &sorted);
    printf("Graph is acyclic: %u\n", acyclic);
    for (i = 0; i < order; i++) {
        printf("%u ", sorted[i]);
    }
    putchar('\n');

    free(sorted);
    free(from_to);

    return 0;
}

