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
    vertex_groups[vertex_ptr->label].insert(vertex_ptr); 
}

void Vertex::remove_edge(ptr vertex_ptr){
    lock_guard<mutex> guard(edge_mutex);
    vertex_groups[vertex_ptr->label].erase(vertex_ptr);
}

void Vertex::_update_edges(){
    
}

void Vertex::create_edge_to(ptr vertex_ptr){
    add_edge(vertex_ptr);
}

void Vertex::delete_edge_to(ptr vertex_ptr){
    remove_edge(vertex_ptr);
}

void  Vertex::update(){
    _update_edges(); 
}
vector<Vertex::output> Vertex::simulate(){
    return vector<Vertex::output>();
}

void Vertex::set_seed(std::uint32_t seed){
    _seed = seed;
}
Vertex::Vertex(string id, string label_name): id(id), label(label_name){}


