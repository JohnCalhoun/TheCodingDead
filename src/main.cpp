#include <iostream>
#include "graph.hpp"

int main()
{
    std::cout << "Creating Graph" << std::endl;
    Vertex v1("room");
    Vertex v2("room");
    v1.create_edge(v2);
    v1.delete_edge(v2);

    Graph graph;
    graph.add_vertex(v1);
    graph.add_vertex(v2);

    graph.start_computation();
    std::cout << "running first itteration" << std::endl;
    graph.run_iteration();
    std::cout << "running second itteration" << std::endl;
    graph.run_iteration();
    graph.stop_computation();
    return 0;
}

