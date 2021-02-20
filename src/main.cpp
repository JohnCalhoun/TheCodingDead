#include <iostream>
#include "graph.hpp"
#include <string>

int main(int argc, char *argv[])
{
    std::cout << "loading Graph" << std::endl;
    Graph graph("output");
    if(argc == 1){
        std::cout << "missing graph definition" << std::endl;
        return 1;
    }
    graph.load(argv[1]);

    graph.start_computation();
    std::cout << "running iteration"<<std::endl;
    graph.run_iteration(1);
    std::cout << "running iteration"<<std::endl;
    graph.run_iteration(2);
    std::cout << "shutting down" << std::endl;
    graph.stop_computation();
    return 0;
}

