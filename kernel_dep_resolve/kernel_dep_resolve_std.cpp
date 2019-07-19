#include <iostream>
#include <list>
#include <vector>

std::vector<int> resolve_dependencies(const std::vector<std::vector<int>>& kernels,
                                 std::vector<int> indegree)
{
    std::vector<int> ordered_kernels;
    ordered_kernels.reserve(indegree.size()+1);
    //list to store kernels with indegree 0
    std::vector<int> free_kernels;
    for (size_t i = 1;  i < indegree.size();  i++) {
        if (indegree[i] == 0)
            free_kernels.push_back(i);
    }
    while (!free_kernels.empty()) {
        const int x = free_kernels.front();
        //free_kernels.pop_front();
        free_kernels.erase(free_kernels.begin());
        ordered_kernels.push_back(x);
        for (const int kernel: kernels[x]) {
            if (--indegree[kernel] == 0)
                free_kernels.push_back(kernel);
        }
    }
    return ordered_kernels;
}

int main()
{
    int kernel_num, dependency_num;
    std::cin >> kernel_num >> dependency_num;
    std::vector<std::vector<int>> kernels(kernel_num+1);
    std::vector<int> indegree(kernel_num+1);
    for (int i = 1;  i <= dependency_num;  i++) {
        int kernel1, kernel2;
        std::cin >> kernel1 >> kernel2;
        kernels[kernel1].push_back(kernel2);
        ++indegree[kernel2];
    }
    for (int i = 1;  i <= kernel_num;  i++) {
        std::cout << "kernel [" << i << "] :";
        for (int dependencies: kernels[i]) {
            std::cout << dependencies << "  ";
        }
        std::cout << std::endl;
    }
    auto const& ordered_kernels = resolve_dependencies(kernels, std::move(indegree));
    std::cout<< "order: ";
    for (auto kernel: ordered_kernels)
        std::cout << kernel << "  ";
    return 0;
}

