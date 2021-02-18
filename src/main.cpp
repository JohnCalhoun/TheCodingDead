#include <iostream>
#include "graph.hpp"

int main()
{
    std::cout << "Creating Graph" << std::endl;

    Graph graph;

    Vertex* old;
    for(int i=0; i<1000000; i++){
        Vertex v("room");
        graph.add_vertex(v);
        if(old){
            v.create_edge(*old);
        }
        old = &v;
    }
    
    graph.start_computation();
    std::cout << "running iteration"<<std::endl;
    graph.run_iteration();
    std::cout << "running iteration"<<std::endl;
    graph.run_iteration();
    std::cout << "shutting down" << std::endl;
    graph.stop_computation();
    return 0;
}

