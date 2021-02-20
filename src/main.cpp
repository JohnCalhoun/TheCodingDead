#include <iostream>
#include "graph.hpp"
#include <string>

int main(int argc, char *argv[])
{
    std::cout << "loading Graph" << std::endl;
    Graph graph;
    graph.load(argv[1]);

    graph.start_computation();
    std::cout << "running iteration"<<std::endl;
    graph.run_iteration();
    std::cout << "running iteration"<<std::endl;
    graph.run_iteration();
    std::cout << "shutting down" << std::endl;
    graph.stop_computation();
    return 0;
}

