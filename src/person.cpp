#include <vector>
#include "vertex.hpp"
#include <cstdlib>
#include "person.hpp"
using namespace std;

vector<Vertex::output>  Person::_update(){
    vector<Vertex::output> out = {output(label, 1)};
    return out;
}

