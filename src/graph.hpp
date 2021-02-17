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
    std::deque<Vertex*>                 _tasks;
    std::mutex                          _task_mutex;
    std::condition_variable             _cv;
    std::condition_variable             _cv_finished;
    std::mutex                          _finished_tasks_mutex;
    std::atomic_uint                    _finished_tasks;
    void _thread(){
        while(true){
            auto vertex = _get_task();
            if(!vertex)
                return;
            vertex->update();
            std::lock_guard lock(_finished_tasks_mutex);
            _finished_tasks++;
            _cv_finished.notify_all();
        }
    }
    Vertex* _get_task(){    
        std::unique_lock lock(_task_mutex);
        while (_tasks.empty()){
            if (_terminated)
                return nullptr;
            _cv.wait_for(lock, std::chrono::milliseconds(10));
        }
        auto rtn = std::move(_tasks.front());
        _tasks.pop_front();
        return rtn;
    }

    public: 
    Graph(): _terminated(false){};

    void add_vertex(Vertex &vertex){
        auto temp = std::make_unique<Vertex>(vertex);
        this->_vertexs.push_back(std::move(temp));
    }

    void start_computation(){
        std::cout << "starting " << std::thread::hardware_concurrency() << " threads" << std::endl;
        for (size_t i = 0; i < std::thread::hardware_concurrency(); ++i)
            _threads.push_back(std::thread(std::bind(&Graph::_thread, this)));
    }
    void run_iteration(){   
        _finished_tasks = 0;
        int _total_tasks = 0;
        {
            std::unique_lock lock(_task_mutex);
            for (auto i = std::begin(_vertexs); i != std::end(_vertexs); ++i){
                _total_tasks++;
                _tasks.push_back((*i).get());
                _cv.notify_one();
            }
        }
        std::unique_lock lock(_finished_tasks_mutex);
        _cv_finished.wait(lock,[_total_tasks,this]{ return _total_tasks == this->_finished_tasks; });
    }
    void stop_computation(){
        _terminated = true;
        _cv.notify_all();
        for (auto& th : _threads) th.join();
    }
};
