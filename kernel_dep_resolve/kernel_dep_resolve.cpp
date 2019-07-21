#include <iostream>
#include <vector>
#include <stack>
#include <algorithm>

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
                               size_t num_of_kernels, std::stack<size_t>& free_kernels) {
    for (size_t kernel = 0; kernel < num_of_kernels; ++kernel) {
        if (is_free_kernel(dependencies, dependency_bool_vector, kernel)) {
            free_kernels.push(kernel);
        }
    }
    return free_kernels.size();
}

bool topological_sort(Dependencies& dependencies, const size_t num_of_kernels, std::vector<size_t>& resolved) {
    std::stack<size_t> free_kernels;
    bool dependency_bool_vector[dependencies.size()];
    size_t remaining_dependencies = dependencies.size();


    // in the begining all dependencies are marked
    for (size_t i = 0; i < dependencies.size(); ++i ) {
        dependency_bool_vector[i] = true;
    }
    // Get the kernels with no incoming dependencies
    auto num_of_free_kernels = get_free_kernels(dependencies, dependency_bool_vector, num_of_kernels, free_kernels);
    // Main loop
    while (num_of_free_kernels) {
        // get a free kernel
        auto free = free_kernels.top();
        // Add it to the resolved array
        resolved.emplace_back(free);
        // Remove from free_kernels stack
        free_kernels.pop();
        // Remove all dependencies with other kernels
        for (size_t i = 0; i < dependencies.size(); ++i) {
            if (dependency_bool_vector[i] && dependencies[i].first == free) {
                dependency_bool_vector[i] = false;
                remaining_dependencies--;

                // Check if other kernels are free now
                if (is_free_kernel(dependencies, dependency_bool_vector, dependencies[i].second)) {
                    // Add it to set of free kernels
                    free_kernels.push(dependencies[i].second);
                }
            }
        }
        num_of_free_kernels = free_kernels.size();
    }
    // if there is no more free kernels but there exist dependencies, a cycle is found
    return remaining_dependencies == 0;
}

int main(void) {
    const size_t num_of_kernels = 5; // number of kernels
    Dependencies dependencies; // vector for kernel dependencies
    std::vector<size_t> resolved;
    // Dependencies should be determined by an analyse phase on kernels
    // Making dependencies (making a kernel of kernel dependencies)
    dependencies.emplace_back(0, 2);
    dependencies.emplace_back(1, 3);
    dependencies.emplace_back(0, 4);

    for (auto elem : dependencies)
        std::cout << elem.first << " ----> " << elem.second << std::endl;

    std::vector<std::pair<size_t, size_t>> cycle;
    // Passing vector of dependencies
    if (topological_sort(dependencies, num_of_kernels, resolved)) {
        std::cout<< "No cycles between kernels"<< std::endl;
        for (auto elem : resolved) {
            std::cout<< elem <<" ";
        }
        std::cout<<std::endl;
    } else {
        std::cout<< "Kernels have circular dependency"<< std::endl;
        // finding all circles between kernels
        for (size_t i = 0; i < dependencies.size(); ++i) {
            for (size_t j = i; j < dependencies.size(); ++j) {
                if (dependencies[i].first == dependencies[j].second &&
                    // extra condition to take into account circles in disconnected kernel networks
                    std::find(cycle.begin(), cycle.end(), dependencies[std::make_pair(i,j).first]) == cycle.end()) {
                    cycle.emplace_back(i,j);
                }
            }
        }
    }
    for (auto elem : cycle) {
        std::cout<< dependencies[elem.first].first << " <---> "  <<dependencies[elem.second].first<< std::endl;
    }
    return 0;
}
