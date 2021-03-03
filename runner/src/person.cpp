#include <vector>
#include "vertex.hpp"
#include <cstdlib>
#include "person.hpp"
#include <boost/log/trivial.hpp>
#include <random>
#include <boost/math/constants/constants.hpp>
#include <algorithm>
#include <stdexcept>

using namespace std;

const vector<Person::State> Person::all_states = {Person::State::wander, Person::State::seek_door};
map<Person::State, vector<float> > Person::transition_probabilties = {
    {Person::State::wander, { .9, .1}},
    {Person::State::seek_door, { .1, .9}}
};

void Person::set_state_transition(Person::State from, Person::State to, float value){
    int i;
    for(i=0; i<Person::all_states.size(); ++i){
        if(Person::all_states[i] == to){
            break;
        }
    }
    transition_probabilties[from][i]=value;
}

Person::State Person::string_to_state(string name){
    if(name == "wander"){
        return Person::State::wander;
    }else if(name == "seek_door"){
        return Person::State::seek_door;
    }else{
        BOOST_LOG_TRIVIAL(fatal) << "State Name: "<< name;
        throw runtime_error("InvalidStateName");
    }
}

void Person::Initialize(){
    for(auto &x: Person::all_states){
        transition_probabilties.try_emplace(x,Person::all_states.size(), 0.0f);
    }
}
vector<Vertex::output> Person::interact(Vertex::MapToVertexs others){
    vector<Vertex::output> out;
    if(current_room_ptr){
        //out.emplace_back(current_room_ptr->id + ":" + label, 1);
    }
    if(state == wander){
        uniform_real_distribution<> distrib(0, boost::math::float_constants::pi);
        position.move(distrib(random_generator),speed);
        position.clip(current_room_ptr->_width,current_room_ptr->_height);
    }else if(state == seek_door){
        if(!current_door_ptr){
            uniform_int_distribution<> distrib(0, current_room_ptr->doors.size()-1);
            current_door_ptr = &current_room_ptr->doors[distrib(random_generator)];
        }
        float distance_to_door = position.distance_to(current_door_ptr->in_position);
        if( distance_to_door > 1){
            position.move(
                position.angle_to(current_door_ptr->in_position),
                min(speed, distance_to_door)
            );
            position.clip(current_room_ptr->_width,current_room_ptr->_height);
        }else{
            put_in_room(current_door_ptr->destination);
            position = current_door_ptr->out_position;
            current_door_ptr = nullptr;
            state = wander;
        }
    }
    _state_transition();
    return out;
}

void Person::_state_transition(){
    State old_state = state;
    discrete_distribution<> dist(
        transition_probabilties.at(state).begin(), 
        transition_probabilties.at(state).end()
    );
    state = all_states[dist(random_generator)];
    if(old_state == State::wander && state != State::wander){
        current_door_ptr = nullptr;
    }
}

void Person::put_in_room(Room* room){
    if(current_room_ptr){
        current_room_ptr->remove_edge(this);
    }
    current_room_ptr = room;
    room->add_edge(this);
}

void Person::set_seed(std::uint32_t seed){
    Vertex::set_seed(seed);
    random_generator.seed(_seed);
}

Person::Person(string id): Vertex(id, "Person"), state(Person::State::wander), position(0,0), speed(.5), current_room_ptr(nullptr), current_door_ptr(nullptr) {}
