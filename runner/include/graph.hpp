#include "vertex.hpp"
#include "room.hpp"
#include "person.hpp"
#include <algorithm>
#include <atomic>
#include <chrono>
#include <deque>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <memory>
#include <random>
#include <sstream>
#include <string>
#include <thread>
#include <vector>
#include <boost/log/trivial.hpp>
#include <stdexcept>
#include <boost/json.hpp>
#include <unordered_set>
#include <utility>

using namespace std;

class Graph {
    private:
    typedef unordered_map<Vertex::output_key, Vertex::output_value> output_container;    
    typedef unordered_set<Vertex::output_key> OutputKeys;
    typedef unordered_set<unique_ptr<Vertex> > VertexContainer;

    string                          _phase;
    int                             _num_of_threads;

    string                          _output_dir;
    vector<output_container>        _outputs;
    OutputKeys                      _output_keys;
    vector<Room>                    _rooms;
    vector<Person>                  _people; 

    void _thread(int i, int threads);
    void _run_phase(string phase);
    float json_value_to_float(boost::json::value value);
    
    public: 
    Graph(string output_dir);

    void run_iteration(int iteration_number);
    void write_output();
    void load(string load_dir);
};
