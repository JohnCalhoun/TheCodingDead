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
#include <map>
#include <memory>
#include <random>
#include <sstream>
#include <string>
#include <thread>
#include <vector>
#include <boost/log/trivial.hpp>
#include <stdexcept>
#include <boost/json.hpp>

using namespace std;

class Graph {
    private:
    typedef map<Vertex::output_key, Vertex::output_value> output_container;    

    atomic_bool                     _terminated;
    atomic_uint                     _finished_tasks;
    condition_variable              _cv;
    int                             _num_of_threads;
    mutex                           _task_mutex;
    vector<thread>                  _threads;

    string                          _output_dir;
    vector<output_container>        _outputs;
    vector<unique_ptr<Vertex> >     _vertexs; 

    void _thread(int i, int threads);

    public: 
    Graph(string output_dir): 
        _terminated(false), 
        _num_of_threads(thread::hardware_concurrency()),
        _output_dir(output_dir){};

    void add_vertex(Vertex::ptr vertex_ptr);
    void start_computation();
    void run_iteration(int iteration_number);
    void stop_computation();
    void load(string load_dir);
};
