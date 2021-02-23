#include <vector>
#include "vertex.hpp"
#include <cstdlib>
#include "person.hpp"
#include <boost/log/trivial.hpp>
using namespace std;

vector<Vertex::output> Person::interact(map<string, vector<Vertex::ptr> > others){
    vector<Vertex::output> out;
    if(edges.size() > 0){
        out.push_back(output(edges[0]->id + ":" + label, 1));
    }
    out.push_back(output(label,1));
    return out;
}
Person::Person(string id): Vertex(id, "Person"){}
