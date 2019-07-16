#include <iostream>
#include <vector>
#include <utility>
#include <array>

using Dependence = std::pair<size_t, size_t>; // <from, to>

/* Find out if a kernel has no dependency (no input from other kernels to this one) */
static bool is_free_kernel(std::vector<Dependence>& dependencies, bool* dependency_bool_vector, size_t num_of_dependencies,
                           size_t kernel) {
    size_t free_kernel = 1;
    for (auto dependence : dependencies) {
        if (free_kernel) {
            free_kernel = std::get<1>(dependence) != kernel;
            for (int i = 0; i < num_of_dependencies; i++) {
                free_kernel = !dependency_bool_vector[i];
            }
        }
    }
    return free_kernel;
}

/* Get the kernels with no dependency (no input from other kernels to those) */
static size_t get_free_kernels(std::vector<Dependence>& dependencies, bool* dependency_bool_vector , size_t num_of_dependencies,
                               size_t num_of_kernels, bool* free_kernels_bool_vector) {
    size_t kernel, num_of_free_kernels = 0;
    for (kernel = 0; kernel< num_of_kernels; kernel++) {
        if (is_free_kernel(dependencies, dependency_bool_vector, num_of_dependencies, kernel)) {
            free_kernels_bool_vector[kernel] = true;
            num_of_free_kernels++;
        }
    }
    return num_of_free_kernels;
}

size_t topological_sort(std::vector<Dependence> dependencies, size_t num_of_dependencies, size_t num_of_kernels, std::vector<size_t>& resolved) {
    bool free_kernels_bool_vector[num_of_kernels];
    bool dependency_bool_vector[num_of_dependencies];
    //std::vector<size_t> resolved;
    size_t kernel, a, num_of_free_kernels, resolved_size = 0,
           remaining_dependencies = num_of_dependencies;

    /* All arcs start off in the graph */
    for (auto elem : dependency_bool_vector ) {
        elem = true;
    }
    /* Get the kernels with no incoming dependencies */
    num_of_free_kernels = get_free_kernels(dependencies, dependency_bool_vector, num_of_dependencies, num_of_kernels, free_kernels_bool_vector);
    /* Main loop */
    while (num_of_free_kernels > 0) {
        /* Get first kernel */
        for (kernel = 0; free_kernels_bool_vector[kernel] != 1; kernel++);
        /* Remove from free_kernels boolean vector */
        free_kernels_bool_vector[kernel] = false;
        num_of_free_kernels--;
        /* Add it to the resolved array */
        resolved.emplace_back(kernel);
        resolved_size++;
        /* Remove all arcs connecting it to its neighbours */
        for (a = 0; a < num_of_dependencies; a++) {
            //for (auto elem : dependency_bool_vector) {
            if (dependency_bool_vector[a] && std::get<0>(dependencies[a]) == kernel) {
                dependency_bool_vector[a] = false;
                remaining_dependencies--;
                /* Check if neighbour is now a root */
                if (is_free_kernel(dependencies, dependency_bool_vector, num_of_dependencies, std::get<1>(dependencies[a]))) {
                    /* Add it to set of vertices */
                    free_kernels_bool_vector[std::get<1>(dependencies[a])] = 1;
                    num_of_free_kernels++;
                }
            }
        }
        }
        return remaining_dependencies == 0;
    }

    void kernel_dependency(std::vector<Dependence>& dependence, size_t from, size_t to) {
        dependence.emplace_back(from,to);
    }

    int main(void) {
        const size_t num_of_dependencies = 8; /* number of dependencies */
        const size_t num_of_kernels = 8; /* number of kernels */
        std::vector<Dependence> dependencies; // vector for kernel dependencies
        std::vector<size_t> resolved;
        size_t acyclic;

        // Dependencies should be determined by an analyse phase on kernels
        // Making dependencies (making an array of kernel dependencies)
        kernel_dependency(dependencies, 0, 2);
        kernel_dependency(dependencies, 1, 3);
        kernel_dependency(dependencies, 2, 3);
        kernel_dependency(dependencies, 2, 4);
        kernel_dependency(dependencies, 2, 5);
        kernel_dependency(dependencies, 3, 6);
        kernel_dependency(dependencies, 5, 7);
        kernel_dependency(dependencies, 5, 7);

        //Passing vector of dependencies
        acyclic = topological_sort(dependencies, num_of_dependencies, num_of_kernels, resolved);
        std::cout<< "No cycle between kernels: "<< acyclic << std::endl;
        for (auto& elem : resolved) {
            std::cout<< elem <<" ";
        }
        std::cout<<std::endl;

        return 0;
    }
