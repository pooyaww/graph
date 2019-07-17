#include <iostream>
#include <vector>
#include <utility>

using Dependence = std::pair<size_t, size_t>; // <from, to>

/* Find out if a kernel has no dependency (no input from other kernels to this one) */
static bool is_free_kernel(const Dependence dependencies[], const bool dependency_bool_vector[], size_t num_of_dependencies,
                           size_t kernel) {
    bool free_kernel = 1;
    for (int i = 0; i < num_of_dependencies && free_kernel; i++) {
        free_kernel = !dependency_bool_vector[i] || dependencies[i].second != kernel;
    }
    return free_kernel;
}

/* Get the kernels with no dependency (no input from other kernels to those) */
static size_t get_free_kernels(Dependence dependencies[], bool dependency_bool_vector[] , size_t num_of_dependencies,
                               size_t num_of_kernels, bool free_kernels_bool_vector[]) {
    size_t kernel, num_of_free_kernels = 0;
    for (kernel = 0; kernel < num_of_kernels; kernel++) {
        if (is_free_kernel(dependencies, dependency_bool_vector, num_of_dependencies, kernel)) {
            free_kernels_bool_vector[kernel] = true;
            num_of_free_kernels++;
        }
    }
    return num_of_free_kernels;
}

size_t topological_sort(Dependence dependencies[], size_t num_of_dependencies, size_t num_of_kernels, std::vector<size_t>& resolved) {
    bool free_kernels_bool_vector[num_of_kernels];
    bool dependency_bool_vector[num_of_dependencies];
    //std::vector<size_t> resolved;
    size_t kernel, a, num_of_free_kernels, resolved_size = 0,
           remaining_dependencies = num_of_dependencies;

    /* All arcs start off in the graph */
        for (a = 0; a < num_of_dependencies; a++ ) {
            dependency_bool_vector[a]=true;
        }
//    for (auto elem : dependency_bool_vector ) {
//        elem = true;
//    }
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
            if (dependency_bool_vector[a] && dependencies[a].first == kernel) {
                dependency_bool_vector[a] = false;
                remaining_dependencies--;
                /* Check if neighbour is now a root */
                if (is_free_kernel(dependencies, dependency_bool_vector, num_of_dependencies, dependencies[a].second)) {
                    /* Add it to set of vertices */
                    free_kernels_bool_vector[dependencies[a].second] = true;
                    num_of_free_kernels++;
                }
            }
        }
        }
        return remaining_dependencies == 0;
    }

    void kernel_dependency(Dependence dependence[], size_t from, size_t to, size_t& index) {
        dependence[index] = {std::make_pair(from, to)};
        std::cout << dependence[index].first << " ----> " << dependence[index].second << std::endl;
        index++;
    }

    int main(void) {
        const size_t num_of_dependencies = 3; //8; /* number of dependencies */
        const size_t num_of_kernels = 4;//8; /* number of kernels */
        Dependence dependencies[num_of_dependencies]; // vector for kernel dependencies
        std::vector<size_t> resolved;
        size_t acyclic;
        size_t index = 0;

        // Dependencies should be determined by an analyse phase on kernels
        // Making dependencies (making an array of kernel dependencies)
        kernel_dependency(dependencies, 0, 2, index);
        kernel_dependency(dependencies, 2, 1, index);
        kernel_dependency(dependencies, 2, 3, index);
        //kernel_dependency(dependencies, 2, 4, index);
        //kernel_dependency(dependencies, 2, 5, index);
        //kernel_dependency(dependencies, 3, 6, index);
        //kernel_dependency(dependencies, 5, 7, index);
        //kernel_dependency(dependencies, 5, 7, index);

        //Passing vector of dependencies
        acyclic = topological_sort(dependencies, num_of_dependencies, num_of_kernels, resolved);
        std::cout<< "No cycle between kernels: "<< acyclic << std::endl;
        for (auto& elem : resolved) {
            std::cout<< elem <<" ";
        }
        std::cout<<std::endl;

        return 0;
    }
