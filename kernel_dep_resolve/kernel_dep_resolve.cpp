#include <iostream>
#include <vector>

using Dependence = std::pair<size_t, size_t>; // <from, to>

// Find out if a kernel has no dependency (no input from other kernels to this one)
static bool is_free_kernel(const Dependence dependencies[], const bool dependency_bool_vector[], const size_t num_of_dependencies,
        size_t kernel) {
    bool free_kernel = true;
    for (size_t i = 0; i < num_of_dependencies && free_kernel; ++i) {
        free_kernel = (!dependency_bool_vector[i])|| (dependencies[i].second != kernel);
    }
    return free_kernel;
}

// Get the kernels with no dependency (no input from other kernels to those)
static size_t get_free_kernels(const Dependence dependencies[], const bool dependency_bool_vector[], const size_t num_of_dependencies,
        size_t num_of_kernels, bool free_kernels_bool_vector[]) {
    size_t  num_of_free_kernels = 0;
    for (size_t kernel = 0; kernel < num_of_kernels; ++kernel) {
        if (is_free_kernel(dependencies, dependency_bool_vector, num_of_dependencies, kernel)) {
            free_kernels_bool_vector[kernel] = true;
            num_of_free_kernels++;
        }
    }
    return num_of_free_kernels;
}

bool topological_sort(Dependence dependencies[0], const size_t num_of_dependencies, const size_t num_of_kernels, std::vector<size_t>& resolved) {
    bool free_kernels_bool_vector[num_of_kernels];
    bool dependency_bool_vector[num_of_dependencies];
    size_t num_of_free_kernels = 0;
    size_t remaining_dependencies = num_of_dependencies;

    // in the begining all dependencies are marked
    for (size_t i = 0; i < num_of_dependencies; ++i ) {
        dependency_bool_vector[i] = true;
    }
    // Get the kernels with no incoming dependencies
    num_of_free_kernels = get_free_kernels(dependencies, dependency_bool_vector, num_of_dependencies, num_of_kernels, free_kernels_bool_vector);
    // Main loop
    while (num_of_free_kernels > 0) {
        // Get first kernel
        size_t kernel;
        for (kernel = 0; free_kernels_bool_vector[kernel] != true; ++kernel);
        // Remove from free_kernels boolean vector
        free_kernels_bool_vector[kernel] = false;
        num_of_free_kernels--;
        // Add it to the resolved array
        resolved.emplace_back(kernel);
        // Remove all dependencies with other kernels
        for (size_t i = 0; i < num_of_dependencies; ++i) {
            if (dependency_bool_vector[i] && dependencies[i].first == kernel) {
                dependency_bool_vector[i] = false;
                remaining_dependencies--;
                // Check if other kernels are free now
                if (is_free_kernel(dependencies, dependency_bool_vector, num_of_dependencies, dependencies[i].second)) {
                    // Add it to set of free kernels
                    free_kernels_bool_vector[dependencies[i].second] = true;
                    num_of_free_kernels++;
                }
            }
        }
    }
    return remaining_dependencies == 0;
}

static void kernel_dependency(Dependence dependencies[],const size_t from, const size_t to, size_t& index) {
    dependencies[index] = std::make_pair(from, to);
    //std::cout << dependencies[index].first << " ----> " << dependencies[index].second << std::endl;
    index++;
}

int main(void) {
    const size_t num_of_dependencies = 3; //8; /* number of dependencies */
    const size_t num_of_kernels = 4;//8; /* number of kernels */
    Dependence dependencies[num_of_dependencies] = {std::make_pair(0,0)}; // vector for kernel dependencies
    std::vector<size_t> resolved;
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

    //Passing array of dependencies
    if ( topological_sort(dependencies, num_of_dependencies, num_of_kernels, resolved))
        std::cout<< "No cycles between kernels"<< std::endl;
    else
        std::cout<< "Kernels have circular dependency"<< std::endl;
    for (auto elem : resolved) {
        std::cout<< elem <<" ";
    }
    std::cout<<std::endl;

    return 0;
}
