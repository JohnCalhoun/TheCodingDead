#include "vertex.hpp"
#include "edge.hpp"
#include <vector>
#include <memory>
#include <chrono>
#include <thread>
#include <atomic>
#include <deque>
#include <filesystem>
#include <iostream>
#include <sstream>
#include <fstream>
#include <map>
#include <string>

class Graph {
    private:
    typedef std::map<Vertex::output_key, Vertex::output_value> output_container;    
    std::vector<std::unique_ptr<Vertex> > _vertexs; 
    std::vector<std::thread>            _threads;
    std::atomic_bool                    _terminated;
    std::vector<Vertex*>                 _tasks;
    std::mutex                          _task_mutex;
    std::condition_variable             _cv;
    std::condition_variable             _cv_finished;
    std::mutex                          _finished_tasks_mutex;
    std::atomic_uint                    _finished_tasks;
    std::vector<Vertex*>::iterator      _vertex_ptr;
    std::vector<output_container*>      _outputs;
    int                                 _num_of_threads;
    std::string                         _output_dir;

    std::vector<std::string> _parse_line(std::string line){
        std::vector<std::string> args;
        std::string arg;
        std::istringstream linestream(line);
        while(std::getline(linestream, arg, ',')){
            args.push_back(arg);
        }
        return args;
    }

    void _thread(int i, int threads){
        while(true){    
            {
                std::unique_lock lock(_task_mutex);
                _cv.wait(lock);
            }
            if(_terminated){
                break;
            }
            for(auto it= _vertexs.begin() + i; it < _vertexs.end(); it+=threads){
                std::vector<Vertex::output> out = (*it)->update();
                for(auto x: out){
                    (*_outputs[i])[x.first] += x.second;
                }
                _finished_tasks++;
            }
        }
    }

    public: 
    Graph(std::string output_dir): _terminated(false), _output_dir(output_dir){
        _num_of_threads = std::thread::hardware_concurrency();
    };

    void add_vertex(Vertex &vertex){
        auto temp = std::make_unique<Vertex>(vertex);
        this->_vertexs.push_back(std::move(temp));
    }

    void start_computation(){
        std::cout << "starting " << _num_of_threads << " threads" << std::endl;
        for (size_t i = 0; i < _num_of_threads; ++i){
            _threads.push_back(std::thread(std::bind(&Graph::_thread, this, i, _num_of_threads)));
            _outputs.push_back(new output_container);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    void run_iteration(int iteration_number){  
        auto start_t = std::chrono::high_resolution_clock::now();
        _finished_tasks = 0;
        int _total_tasks = _vertexs.size();
        
        _cv.notify_all();
        while( _total_tasks != _finished_tasks ){
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        
        output_container final_output;
        for(int i=0; i<_outputs.size(); ++i){
            for(std::pair<Vertex::output_key, Vertex::output_value> x: *_outputs[i]){
                final_output[x.first] += x.second;
            }
            delete _outputs[i];
            _outputs[i] = new output_container;
        }
        std::ostringstream output_file_name;
        output_file_name << _output_dir << "/" << iteration_number << ".csv";
        std::ofstream output_file{output_file_name.str()};
        for(std::pair<Vertex::output_key, Vertex::output_value> x: final_output){
            output_file << x.first << "," << x.second << std::endl;
        }
        output_file.close();

        auto stop_t = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop_t - start_t);
        std::cout << "iteration duration: " << duration.count()/1000000.0 << std::endl; 
    }
    void stop_computation(){
        _terminated = true;
        _cv.notify_all();
        for (auto& th : _threads) th.join();
        for (size_t i = 0; i < _num_of_threads; ++i){
            delete _outputs[i];
        }
    }

    void load(std::string load_dir){
        auto start_t = std::chrono::high_resolution_clock::now();
        std::fstream stream;
        std::vector<std::string> vertex_files;
        std::vector<std::string> edge_files;


        for(auto& p: std::filesystem::directory_iterator(load_dir)){
            std::size_t found_edge = p.path().filename().string().find("edge");
            if (found_edge!=std::string::npos)
                edge_files.push_back(p.path());
            
            std::size_t found_vertex = p.path().filename().string().find("vertex");
            if (found_vertex!=std::string::npos)
                vertex_files.push_back(p.path());
        }

        std::map<std::string, Vertex*> vertexs_tmp;
        for(auto vertex_file: vertex_files){
            std::cout<<"loading " << vertex_file << std::endl;
            stream.open(vertex_file);
            std::string line;
            while(std::getline(stream,line)){    
                auto args = _parse_line(line);
                vertexs_tmp[args[0]] = new Vertex(args[1]);
            }
            stream.close();
        }
        std::cout << "loaded " << vertexs_tmp.size() << " vertexs " << std::endl;
        for(auto edge_file: edge_files){
            std::cout<<"loading " << edge_file << std::endl;
            stream.open(edge_file);
            std::string line;
            while(std::getline(stream,line)){    
                auto args = _parse_line(line);
                vertexs_tmp[args[0]]->create_edge(*vertexs_tmp[args[1]]);
            }
            stream.close();
        }
        for(auto v: vertexs_tmp){
            add_vertex(*v.second);
        }
        auto stop_t = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop_t - start_t);
        std::cout << "loading duration: " << duration.count()/1000000.0 << std::endl; 
    }
};
