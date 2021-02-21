#ifndef PERSON_H_INCLUDED 
#define PERSON_H_INCLUDED

#include <vector>
#include "vertex.hpp"
#include <cstdlib>

using namespace std;

class Person : public Vertex {
    private:
    float _x;
    float _y;

    public:
    vector<output>  _update();

    Person(): Vertex("Person"){
        _x = (float) rand()/RAND_MAX;
        _y = (float) rand()/RAND_MAX;
    }
};

#endif
