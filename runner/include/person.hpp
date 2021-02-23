#ifndef PERSON_H_INCLUDED 
#define PERSON_H_INCLUDED

#include <vector>
#include "vertex.hpp"
#include <cstdlib>
#include "position.hpp"
#include <map>

using namespace std;

class Person : public Vertex {
    private:
    Position<float> _position;

    public:
    vector<output> interact(map<string, vector<ptr> > others);

    Person(string id);
};

#endif
