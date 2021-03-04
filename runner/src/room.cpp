#include <vector>
#include "vertex.hpp"
#include "room.hpp"
#include "map"
#include <cassert>

using namespace std;

vector<Vertex::output>  Room::simulate(){
    vector<Vertex::output> out;
    return out;
}

vector<Vertex::output> Room::interact(Vertex::MapToVertexs others){
    vector<Vertex::output> out;
    return out;
}

void Room::add_door(float in_x, float in_y, float out_x, float out_y,  Room* other){
    assert(0 <= in_x && in_x <= _width);
    assert(0 <= in_y && in_y <= _height);
    doors.emplace_back(in_x, in_y, out_x, out_y, other);
}

Room::Room(string id, float width, float height): Vertex(id, "Room"), _width(width), _height(height){};
Room::Room(const Room& source):
    _height(source._height),
    _width(source._width),
    doors(source.doors),
    Vertex(source.id, "Room")
{};

Room::Door::Door(float in_x, float in_y, float out_x, float out_y, Room* other): 
    in_position(in_x,in_y), 
    out_position(out_x,out_y), 
    destination(other){}
