#include<vector>
#include <algorithm>
#include <mutex>
#include <iostream>
#include <string>

class Vertex {
    public:
    const std::string label;

    private:
    std::vector<Vertex*> edges;
    std::mutex edge_mutex;

    void add_edge(Vertex &vertex){
        std::lock_guard<std::mutex> guard(this->edge_mutex);
        this->edges.push_back(&vertex);
    }

    void remove_edge(Vertex &vertex){
        std::lock_guard<std::mutex> guard(this->edge_mutex);
        auto i = std::find(std::begin(this->edges), std::end(this->edges), &vertex);
        if(i != std::end(this->edges)){
            this->edges.erase(i);
        }
    }

    public:
    void create_edge(Vertex &vertex){
        this->add_edge(vertex);
        vertex.add_edge(*this);
    }

    void delete_edge(Vertex &vertex){
        this->remove_edge(vertex);
        vertex.remove_edge(*this);
    }
    void update(){
        std::cout << "Vertex Update Called" << std::endl;
    }
    Vertex(const Vertex &other){
        this->edges = std::vector(other.edges);
    }

    Vertex(std::string label): label(label){}
};
