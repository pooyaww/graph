#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

typedef struct {
    unsigned int from;
    unsigned int to;
} dependence;

/* Find out if a kernel has no dependency (no input from other kernels to this one) */
static bool is_free_kernel(const dependence *dependencies, bool *dependency_bool_vector, unsigned int num_of_dependencies,
        unsigned int kernel)
{
    unsigned int from_this_kernel, free_kernel = 1;
    for (from_this_kernel = 0; from_this_kernel < num_of_dependencies && free_kernel; from_this_kernel++) {
        free_kernel = !dependency_bool_vector[from_this_kernel] || dependencies[from_this_kernel].to != kernel;
    }
    return free_kernel;
}

/* Get the kernels with no dependency (no input from other kernels to those) */
static unsigned int get_free_kernels(const dependence *dependencies, bool *dependency_bool_vector, unsigned int num_of_dependencies,
        unsigned int num_of_kernels, bool *free_kernels_bool_vector)
{
    unsigned int kernel, num_of_free_kernels = 0;
    for (kernel = 0; kernel < num_of_kernels; kernel++) {
        if (is_free_kernel(dependencies, dependency_bool_vector, num_of_dependencies, kernel)) {
            free_kernels_bool_vector[kernel] = 1;
            num_of_free_kernels++;
        }
    }
    return num_of_free_kernels;
}

unsigned int topological_sort(const dependence *dependencies, unsigned int num_of_dependencies, unsigned int num_of_kernels,
        unsigned int **resolved)
{
    bool *free_kernels_bool_vector = calloc(num_of_kernels, sizeof(bool));
    bool *dependency_bool_vector = malloc(num_of_dependencies * sizeof(bool));
    *resolved = malloc( num_of_kernels* sizeof(unsigned int));
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
    for (a = 0; a < num_of_dependencies; a++) {
        dependency_bool_vector[a] = 1;
    }
    /* Get the vertices with no incoming edges */
    num_of_free_kernels = get_free_kernels(dependencies, dependency_bool_vector, num_of_dependencies, num_of_kernels, free_kernels_bool_vector);
    /* Main loop */
    while (num_of_free_kernels > 0) {
        /* Get first vertex */
        for (kernel = 0; free_kernels_bool_vector[kernel] != 1; kernel++);
        /* Remove from vertex set */
        free_kernels_bool_vector[kernel] = 0;
        num_of_free_kernels--;
        /* Add it to the sorted array */
        (*resolved)[sorted_size++] = kernel;
        /* Remove all arcs connecting it to its neighbours */
        for (a = 0; a < num_of_dependencies; a++) {
            if (dependency_bool_vector[a] && dependencies[a].from== kernel) {
                dependency_bool_vector[a] = 0;
                arcs_size--;
                /* Check if neighbour is now a root */
                if (is_free_kernel(dependencies, dependency_bool_vector, num_of_dependencies, dependencies[a].to)) {
                    /* Add it to set of vertices */
                    free_kernels_bool_vector[dependencies[a].to] = 1;
                    num_of_free_kernels++;
                }
            }
        }
    }
    free(free_kernels_bool_vector);
    free(dependency_bool_vector);
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
    const unsigned int num_of_dependencies = 8; /* number of dependencies */
    const unsigned int num_of_kernels = 8; /* number of kernels */
    dependence *dependencies = malloc(num_of_dependencies * sizeof(dependence)); // Array for kernel dependencies
    unsigned int i = 0;
    unsigned int *resolved;
    unsigned int acyclic;

    // Dependencies should be determined by an analyse phase on kernels
    // Making dependencies (making an array of kernel dependencies)
    kernel_dependency(dependencies, 0, 2, &i);
    kernel_dependency(dependencies, 1, 3, &i);
    kernel_dependency(dependencies, 2, 3, &i);
    kernel_dependency(dependencies, 2, 4, &i);
    kernel_dependency(dependencies, 2, 5, &i);
    kernel_dependency(dependencies, 3, 6, &i);
    kernel_dependency(dependencies, 5, 7, &i);
    kernel_dependency(dependencies, 5, 7, &i);

    //Passing array of dependencies
    acyclic = topological_sort(dependencies, num_of_dependencies, num_of_kernels, &resolved);
    printf("No cycle between kernels: %u\n", acyclic);
    for (i = 0; i < num_of_kernels; i++) {
        printf("%u ", resolved[i]);
    }
    putchar('\n');

    free(resolved);
    free(dependencies);

    return 0;
}

