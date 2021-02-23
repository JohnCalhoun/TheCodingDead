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
#include <boost/timer/timer.hpp>
#include "graph.hpp"
using namespace std;

void Graph::_thread(int i, int threads){
    while(true){    
        {
            unique_lock lock(_task_mutex);
            _cv.wait(lock);
        }
        if(_terminated){
            break;
        }
        for(auto it= _vertexs.begin() + i; it < _vertexs.end(); it+=threads){
            if(_phase == "update"){
                (*it)->update();
            }else if(_phase == "simulate"){
                vector<Vertex::output> out = (*it)->simulate();
                for(auto x: out){
                    _outputs[i][x.first] += x.second;
                }
            }
            _finished_tasks++;
        }
    }
}

void Graph::add_vertex(Vertex::ptr vertex_ptr){
    this->_vertexs.emplace_back(vertex_ptr);
}

void Graph::start_computation(){
    BOOST_LOG_TRIVIAL(debug) << "starting " << _num_of_threads << " threads"; 
    for (size_t i = 0; i < _num_of_threads; ++i){
        _threads.push_back(thread(bind(&Graph::_thread, this, i, _num_of_threads)));
        _outputs.emplace_back();
    }
    this_thread::sleep_for(chrono::milliseconds(500));
}
void Graph::_run_phase(string phase){
    BOOST_LOG_TRIVIAL(debug) << "starting phase: " << phase; 
    _phase=phase;
    _finished_tasks = 0;
    int _total_tasks = _vertexs.size();

    _cv.notify_all();
    while( _total_tasks != _finished_tasks ){
        this_thread::sleep_for(chrono::nanoseconds(500));
    }
}
void Graph::run_iteration(int iteration_number){  
    boost::timer::auto_cpu_timer t("Iteration duration: %ws wall, %us user + %ss system = %ts CPU (%p%)\n");
    _run_phase("update");
    _run_phase("simulate");
    
    output_container final_output;
    for(int i=0; i<_outputs.size(); ++i){
        for(pair<Vertex::output_key, Vertex::output_value> x: _outputs[i]){
            final_output[x.first] += x.second;
        }
        _outputs[i].clear();
    }
    ostringstream output_file_name;
    output_file_name << _output_dir << "/" << iteration_number << ".csv";
    ofstream output_file{output_file_name.str()};
    for(pair<Vertex::output_key, Vertex::output_value> x: final_output){
        _output_keys.insert(x.first);
        output_file << x.first << "," << x.second << endl;
    }
    output_file.close();
}

void Graph::stop_computation(){
    _terminated = true;
    _cv.notify_all();
    for (auto& th : _threads) th.join();

    ostringstream output_key_file_name;
    output_key_file_name << _output_dir << "/" << "keys.csv";

    ofstream output_file{output_key_file_name.str()};
    for(Vertex::output_key x: _output_keys){
        output_file << x << endl;
    }
    output_file.close();
}

void Graph::load(string load_dir){
    boost::timer::auto_cpu_timer t("Loading duration: %ws wall, %us user + %ss system = %ts CPU (%p%)\n");
    fstream stream;
    vector<string> vertex_files;
    vector<string> edge_files;


    for(auto& p: filesystem::directory_iterator(load_dir)){
        size_t found_edge = p.path().filename().string().find("edge");
        if (found_edge!=string::npos)
            edge_files.push_back(p.path());
        
        size_t found_vertex = p.path().filename().string().find("vertex");
        if (found_vertex!=string::npos)
            vertex_files.push_back(p.path());
    }
    BOOST_LOG_TRIVIAL(debug) << "Loading Graph Definition from: " << load_dir;

    unordered_map<boost::json::string, Vertex::ptr> vertexs_tmp;
    for(auto vertex_file: vertex_files){
        BOOST_LOG_TRIVIAL(debug) << "Loading Vertex File: " << vertex_file;
        stream.open(vertex_file);
        string line;
        int line_number=0;
        while(getline(stream,line)){
            try{
                boost::json::value args = boost::json::parse(line);
                Vertex* new_vertex;

                boost::json::string type = args.at("type").as_string();
                if(type == "room"){
                    new_vertex =new Room(
                        args.at("id").as_string().data(),
                        args.at("width").as_double(), 
                        args.at("height").as_double()
                    );
                }else if(type == "person"){
                    new_vertex =new Person(
                        args.at("id").as_string().data()
                    );
                }else{
                    BOOST_LOG_TRIVIAL(fatal) << "unknow vertex type: "<< type;
                    throw runtime_error("InvalidVertexDefinition"); 
                }
                vertexs_tmp[args.at("id").as_string()] = new_vertex;
                add_vertex(new_vertex);
            }catch(const std::exception& e){
                BOOST_LOG_TRIVIAL(fatal) << "Vertex Failed: "<< e.what() 
                    << ":" << vertex_file << ":" << line_number;
                throw runtime_error("InvalidVertexDefinition"); 
            }
            line_number++;
        }
        stream.close();
    }
    for(auto edge_file: edge_files){
        BOOST_LOG_TRIVIAL(debug) << "Loading Edge File: " << edge_file;
        stream.open(edge_file);
        string line;
        int line_number=0;
        while(getline(stream,line)){    
            try {
                boost::json::value args = boost::json::parse(line);
                vertexs_tmp.at(
                    args.at("from").as_string()
                )->create_edge_to(
                    vertexs_tmp.at(args.at("to").as_string())
                );
            }catch(const std::exception& e){
                BOOST_LOG_TRIVIAL(fatal) << "Edge Failed: "<< e.what() 
                    << ":" << edge_file << ":" << line_number;
                throw runtime_error("InvalidEdgeDefinition"); 
            }
            line_number++;
        }
        stream.close();
    }
    
    random_device rd;
    mt19937 g(rd());
    
    shuffle(_vertexs.begin(), _vertexs.end(), g);
}

Graph::Graph(string output_dir): 
    _terminated(false), 
    _num_of_threads(thread::hardware_concurrency()),
    _output_dir(output_dir){};

