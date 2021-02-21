#include <algorithm>
#include <iostream>
#include <mutex>
#include <string>
#include <utility>
#include <vector>

using namespace std;

class Vertex {
    public:
    typedef Vertex*         ptr;
    typedef ptr             Edge;
    typedef int             output_value;
    typedef string          output_key;
    typedef pair<output_key, output_value> output;
    string label;

    private:
    vector<Edge> edges;
    vector<Edge> edges_to_add;
    vector<Edge> edges_to_remove;
    mutex edge_mutex;

    void add_edge(ptr vertex_ptr){
        lock_guard<mutex> guard(this->edge_mutex);
        this->edges_to_add.push_back(vertex_ptr);
    }

    void remove_edge(ptr vertex_ptr){
        lock_guard<mutex> guard(this->edge_mutex);
        this->edges_to_remove.push_back(vertex_ptr);
    }

    void _update_edges(){
        lock_guard<mutex> guard(this->edge_mutex);
        move(edges_to_add.begin(), edges_to_add.end(), back_inserter(edges));
      
        for(auto edge: edges_to_remove){
            auto i = find(begin(edges), end(edges), edge);
            if(i != end(this->edges)){
                this->edges.erase(i);
            }
        }
        edges_to_add.clear();
        edges_to_remove.clear();
    }

    public:
    void create_edge_to(ptr vertex_ptr){
        add_edge(vertex_ptr);
        vertex_ptr->add_edge(this);
    }

    void delete_edge_to(ptr vertex_ptr){
        remove_edge(vertex_ptr);
        vertex_ptr->remove_edge(this);
    }

    vector<output>  update(){
        _update_edges(); 
        vector<output> out = {output(label, 1)};
        return out;
    }

    Vertex(string label_name): label(label_name){}
};
