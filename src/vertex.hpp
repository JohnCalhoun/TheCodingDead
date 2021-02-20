#include<vector>
#include <algorithm>
#include <mutex>
#include <iostream>
#include <string>
#include <utility>

class Vertex {
    public:
    typedef int output_value;
    typedef std::string output_key;
    typedef std::pair<output_key, output_value> output;
    std::string label;

    private:
    std::vector<Vertex*> edges;
    std::vector<Vertex*> edges_to_add;
    std::vector<Vertex*> edges_to_remove;
    std::mutex edge_mutex;

    void add_edge(Vertex &vertex){
        std::lock_guard<std::mutex> guard(this->edge_mutex);
        this->edges_to_add.push_back(&vertex);
    }

    void remove_edge(Vertex &vertex){
        std::lock_guard<std::mutex> guard(this->edge_mutex);
        this->edges_to_remove.push_back(&vertex);
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
    std::vector<output>  update(){
        std::lock_guard<std::mutex> guard(this->edge_mutex);
        std::move(edges_to_add.begin(), edges_to_add.end(), std::back_inserter(edges));
      
        for(auto edge: edges_to_remove){
            auto i = std::find(std::begin(edges), std::end(edges), edge);
            if(i != std::end(this->edges)){
                this->edges.erase(i);
            }
        }
        edges_to_add.clear();
        edges_to_remove.clear();
        std::vector<output> out = {output(label, 1)};
        return out;

    }
    Vertex(const Vertex &other){
        edges = std::vector(other.edges);
        label = other.label;
    }

    Vertex(std::string label_name): label(label_name){}
};
