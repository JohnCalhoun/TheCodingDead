#include "edge.hpp"
#include "vertex.hpp"
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

    vector<string> _parse_line(string line){
        vector<string> args;
        string arg;
        istringstream linestream(line);
        while(getline(linestream, arg, ',')){
            args.push_back(arg);
        }
        return args;
    }

    void _thread(int i, int threads){
        while(true){    
            {
                unique_lock lock(_task_mutex);
                _cv.wait(lock);
            }
            if(_terminated){
                break;
            }
            for(auto it= _vertexs.begin() + i; it < _vertexs.end(); it+=threads){
                vector<Vertex::output> out = (*it)->update();
                for(auto x: out){
                    _outputs[i][x.first] += x.second;
                }
                _finished_tasks++;
            }
        }
    }

    public: 
    Graph(string output_dir): 
        _terminated(false), 
        _output_dir(output_dir),
        _num_of_threads(thread::hardware_concurrency()){};

    void add_vertex(Vertex::ptr vertex_ptr){
        this->_vertexs.emplace_back(vertex_ptr);
    }

    void start_computation(){
        BOOST_LOG_TRIVIAL(debug) << "starting " << _num_of_threads << " threads"; 
        for (size_t i = 0; i < _num_of_threads; ++i){
            _threads.push_back(thread(bind(&Graph::_thread, this, i, _num_of_threads)));
            _outputs.emplace_back();
        }
        this_thread::sleep_for(chrono::milliseconds(500));
    }
    void run_iteration(int iteration_number){  
        auto start_t = chrono::high_resolution_clock::now();
        _finished_tasks = 0;
        int _total_tasks = _vertexs.size();
        
        _cv.notify_all();
        while( _total_tasks != _finished_tasks ){
            this_thread::sleep_for(chrono::nanoseconds(500));
        }
        
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
            output_file << x.first << "," << x.second << endl;
        }
        output_file.close();

        auto stop_t = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::microseconds>(stop_t - start_t);
        BOOST_LOG_TRIVIAL(debug) << "Iteration " << iteration_number << " Duration: " << duration.count()/1000000.0; 
    }
    void stop_computation(){
        _terminated = true;
        _cv.notify_all();
        for (auto& th : _threads) th.join();
    }

    void load(string load_dir){
        auto start_t = chrono::high_resolution_clock::now();
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

        map<string, Vertex::ptr> vertexs_tmp;
        for(auto vertex_file: vertex_files){
            BOOST_LOG_TRIVIAL(debug) << "Loading Vertex File: " << vertex_file;
            stream.open(vertex_file);
            string line;
            while(getline(stream,line)){    
                auto args = _parse_line(line);
                Vertex::ptr new_vertex =new Vertex(args[1]);
                vertexs_tmp[args[0]] = new_vertex;
                add_vertex(new_vertex);
            }
            stream.close();
        }
        for(auto edge_file: edge_files){
            BOOST_LOG_TRIVIAL(debug) << "Loading Edge File: " << edge_file;
            stream.open(edge_file);
            string line;
            while(getline(stream,line)){    
                auto args = _parse_line(line);
                vertexs_tmp.at(args[0])->create_edge_to(vertexs_tmp.at(args[1]));
            }
            stream.close();
        }
        
        random_device rd;
        mt19937 g(rd());
        
        shuffle(_vertexs.begin(), _vertexs.end(), g);
        auto stop_t = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::microseconds>(stop_t - start_t);
        BOOST_LOG_TRIVIAL(debug) << "Loading Duration: " << duration.count()/1000000.0; 
    }
};
