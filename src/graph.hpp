#include "vertex.hpp"
#include "edge.hpp"
#include <vector>
#include <memory>
#include <chrono>
#include <thread>
#include <atomic>
#include <deque>

class Graph {
    private:
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
                (*it)->update();
                _finished_tasks++;
            }
        }
    }

    public: 
    Graph(): _terminated(false){
    };

    void add_vertex(Vertex &vertex){
        auto temp = std::make_unique<Vertex>(vertex);
        this->_vertexs.push_back(std::move(temp));
    }

    void start_computation(){
        int threads = std::thread::hardware_concurrency();
        std::cout << "starting " << threads << " threads" << std::endl;
        for (size_t i = 0; i < threads; ++i){
            _threads.push_back(std::thread(std::bind(&Graph::_thread, this, i, threads)));
        }
    }
    void run_iteration(){  
        auto start_t = std::chrono::high_resolution_clock::now();
        _finished_tasks = 0;
        int _total_tasks = _vertexs.size();
        
        _cv.notify_all();
        while( _total_tasks != _finished_tasks ){
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        auto stop_t = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop_t - start_t);
        std::cout << "iteration duration: " << duration.count()/1000000.0 << std::endl; 
    }
    void stop_computation(){
        _terminated = true;
        _cv.notify_all();
        for (auto& th : _threads) th.join();
    }
};
