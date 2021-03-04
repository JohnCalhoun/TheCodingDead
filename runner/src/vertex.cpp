#include <algorithm>
#include <iostream>
#include <mutex>
#include <string>
#include <utility>
#include <vector>
#include "vertex.hpp"
using namespace std;
    
void Vertex::add_edge(ptr vertex_ptr){
    lock_guard<mutex> guard(edge_mutex);
    this->edges_to_add.push_back(vertex_ptr);
}

void Vertex::remove_edge(ptr vertex_ptr){
    lock_guard<mutex> guard(edge_mutex);
    this->edges_to_remove.push_back(vertex_ptr);
}

void Vertex::_update_edges(){
    lock_guard<mutex> guard(this->edge_mutex);
    for(auto edge: edges_to_remove){
        vertex_groups[edge->label].erase(edge);
    }

    for(auto edge: edges_to_add){
        vertex_groups[edge->label].insert(edge); 
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

void  Vertex::update(){
    _update_edges(); 
    _update();
}
vector<Vertex::output> Vertex::simulate(){
    return vector<Vertex::output>();
}

void Vertex::set_seed(std::uint32_t seed){
    _seed = seed;
}
Vertex::Vertex(string id, string label_name): id(id), label(label_name){}


