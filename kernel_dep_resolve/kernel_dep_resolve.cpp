#include <iostream>
#include <vector>

using Dependencies = std::vector<std::pair<size_t, size_t>>; // <from, to>

// Find out if a kernel has no dependency (no input from other kernels to this one)
static bool is_free_kernel(const Dependencies& dependencies, const bool dependency_bool_vector[], const size_t kernel) {
    bool free_kernel = true;
    for (size_t i = 0; i < dependencies.size() && free_kernel; ++i) {
        free_kernel = (!dependency_bool_vector[i])|| (dependencies[i].second != kernel);
    }
    return free_kernel;
}

// Get the kernels with no dependency (no input from other kernels to those)
static size_t get_free_kernels(const Dependencies& dependencies, const bool dependency_bool_vector[],
                               size_t num_of_kernels, bool free_kernels_bool_vector[]) {
    size_t  num_of_free_kernels = 0;
    for (size_t kernel = 0; kernel < num_of_kernels; ++kernel) {
        if (is_free_kernel(dependencies, dependency_bool_vector, kernel)) {
            free_kernels_bool_vector[kernel] = true;
            num_of_free_kernels++;
        } else {
            free_kernels_bool_vector[kernel] = false;
        }
    }
    return num_of_free_kernels;
}

bool topological_sort(Dependencies& dependencies, const size_t num_of_kernels, std::vector<size_t>& resolved) {
    bool free_kernels_bool_vector[num_of_kernels];
    bool dependency_bool_vector[dependencies.size()];
    size_t num_of_free_kernels = 0;
    size_t remaining_dependencies = dependencies.size();

    // in the begining all dependencies are marked
    for (size_t i = 0; i < dependencies.size(); ++i ) {
        dependency_bool_vector[i] = true;
    }
    // Get the kernels with no incoming dependencies
    num_of_free_kernels = get_free_kernels(dependencies, dependency_bool_vector, num_of_kernels, free_kernels_bool_vector);
    // Main loop
    while (num_of_free_kernels) {
        // Get first kernel
        size_t kernel;
        for (kernel = 0; free_kernels_bool_vector[kernel] != true; ++kernel);
        // Remove from free_kernels boolean vector
        free_kernels_bool_vector[kernel] = false;
        num_of_free_kernels--;
        // Add it to the resolved array
        resolved.emplace_back(kernel);
        // Remove all dependencies with other kernels
        for (size_t i = 0; i < dependencies.size(); ++i) {
            if (dependency_bool_vector[i] && dependencies[i].first == kernel) {
                dependency_bool_vector[i] = false;
                remaining_dependencies--;
                // Check if other kernels are free now
                if (is_free_kernel(dependencies, dependency_bool_vector, dependencies[i].second)) {
                    // Add it to set of free kernels
                    free_kernels_bool_vector[dependencies[i].second] = true;
                    num_of_free_kernels++;
                }
            }
        }
    }
    return remaining_dependencies == 0;
}

int main(void) {
    const size_t num_of_kernels = 4; // number of kernels
    Dependencies dependencies; // vector for kernel dependencies
    std::vector<size_t> resolved;
    // Dependencies should be determined by an analyse phase on kernels
    // Making dependencies (making a kernel of kernel dependencies)
    dependencies.emplace_back(0, 2);
    dependencies.emplace_back(0, 1);
    dependencies.emplace_back(2, 1);
    dependencies.emplace_back(2, 3);

    for (auto elem : dependencies)
        std::cout << elem.first << " ----> " << elem.second << std::endl;

    //Passing vector of dependencies
    if ( topological_sort(dependencies, num_of_kernels, resolved))
        std::cout<< "No cycles between kernels"<< std::endl;
    else
        std::cout<< "Kernels have circular dependency"<< std::endl;
    for (auto elem : resolved) {
        std::cout<< elem <<" ";
    }
    std::cout<<std::endl;

    return 0;
}
