#include <vector>
#include "vertex.hpp"
#include "room.hpp"
#include "map"

using namespace std;

vector<Vertex::output>  Room::simulate(){
    vector<Vertex::output> out = {output(label, 1)};
    map<string, vector<Vertex::ptr> > vertex_groups;

    for(auto v: edges){
        vertex_groups[v->label].push_back(v); 
    }
    for(auto v: edges){
        vector<Vertex::output> interact_output = v->interact(vertex_groups);
        for(int i = 0; i< interact_output.size(); i++)
            out.push_back(interact_output[i]);
    }
    return out;
}

vector<Vertex::output> Room::interact(map<string, vector<Vertex::ptr> > others){
    vector<Vertex::output> out;
    return out;
}


Room::Room(string id, float width, float height): Vertex(id, "Room"), _width(width), _height(height){};
