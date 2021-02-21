#include <vector>
#include "vertex.hpp"
#include "room.hpp"

using namespace std;

vector<Vertex::output>  Room::_update(){
    vector<Vertex::output> out = {output(label, 1)};
    return out;
}

