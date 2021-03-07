#ifndef PERSON_H_INCLUDED 
#define PERSON_H_INCLUDED

#include <vector>
#include "vertex.hpp"
#include <cstdlib>
#include <map>
#include "room.hpp"
#include <random> 
#include "position.hpp"
#include <boost/random/taus88.hpp>
#include <boost/math/constants/constants.hpp>

using namespace std;

class Person : public Vertex {
    private:
    enum State {
        wander,
        seek_door
    };
    static const vector<State>              all_states;
    static map<State, vector<float> >       transition_probabilties;
    static map<State, map<State, float>>    transition_probabilities_map;
    
    State state;
    Position position;
    float speed;
    Room* current_room_ptr;
    Room::Door* current_door_ptr;
    boost::taus88 random_generator;
    void _state_transition();     
    uniform_real_distribution<> random_angle;

    public:
    void interact();
    Person(string id);
    Person(const Person& source);
    void put_in_room(Room *);
    virtual void set_seed(std::uint32_t seed);
    static void Initialize();
    static State string_to_state(string name);
    static void set_state_transition(State from, State to, float value);
};

#endif
