#include <algorithm>
#include <iostream>
#include <mutex>
#include <string>
#include <utility>
#include <vector>
#include "vertex.hpp"
using namespace std;
    
void Vertex::add_edge(ptr vertex_ptr){
    lock_guard<mutex> guard(this->edge_mutex);
    this->edges_to_add.push_back(vertex_ptr);
}

void Vertex::remove_edge(ptr vertex_ptr){
    lock_guard<mutex> guard(this->edge_mutex);
    this->edges_to_remove.push_back(vertex_ptr);
}

void Vertex::_update_edges(){
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
vector<Vertex::output> Vertex::_update(){
    return vector<Vertex::output>();
};

void Vertex::create_edge_to(ptr vertex_ptr){
    add_edge(vertex_ptr);
    vertex_ptr->add_edge(this);
}

void Vertex::delete_edge_to(ptr vertex_ptr){
    remove_edge(vertex_ptr);
    vertex_ptr->remove_edge(this);
}

vector<Vertex::output>  Vertex::update(){
    _update_edges(); 
    return _update();
}
