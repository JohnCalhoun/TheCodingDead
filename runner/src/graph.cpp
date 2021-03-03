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
#include <cstdint>
#include <iterator>
using namespace std;

void Graph::_thread(int i, int threads){ 
    size_t chunk_size = _vertexs.size()/threads;
    auto it=_vertexs.begin();
    
    advance(it, chunk_size * i);
    auto end = it;
    advance(end, chunk_size);

    while(it != end  && it != _vertexs.end()){
        if(_phase == "update"){
            (*it)->update();
        }else if(_phase == "simulate"){
            vector<Vertex::output> out = (*it)->simulate();
            for(auto x: out){
                _outputs[i][x.first] += x.second;
            }
        }
        it++;
    }
}

void Graph::add_vertex(Vertex::ptr vertex_ptr){
    this->_vertexs.emplace(vertex_ptr);
}

void Graph::_run_phase(string phase){
    BOOST_LOG_TRIVIAL(debug) << "starting phase: " << phase; 
    vector<thread> threads;
    _phase = phase;
    threads.reserve(_num_of_threads);
    for (size_t i = 0; i < _num_of_threads; ++i){
        threads.push_back(thread(bind(&Graph::_thread, this, i, _num_of_threads)));
        _outputs.emplace_back();
    }
    for (size_t i = 0; i < _num_of_threads; ++i){
        threads[i].join();
    }
}
void Graph::run_iteration(int iteration_number){  
    output_container final_output;
    {
        boost::timer::auto_cpu_timer t("Iteration duration: %ws wall, %us user + %ss system = %ts CPU (%p%)\n");
        _run_phase("update");
        _run_phase("simulate");
        
        for(int i=0; i<_outputs.size(); ++i){
            for(pair<Vertex::output_key, Vertex::output_value> x: _outputs[i]){
                final_output[x.first] += x.second;
            }
            _outputs[i].clear();
        }
    }
    {
        boost::timer::auto_cpu_timer t("Write output duration: %ws wall, %us user + %ss system = %ts CPU (%p%)\n");
        ostringstream output_file_name;
        output_file_name << _output_dir << "/" << iteration_number << ".csv";
        ofstream output_file{output_file_name.str()};
        for(pair<Vertex::output_key, Vertex::output_value> x: final_output){
            _output_keys.insert(x.first);
            output_file << x.first << "," << x.second << endl;
        }
        output_file.close();
    }
}

void Graph::write_output(){
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
    Person::Initialize();
    string global_config_file = load_dir + "config.json";
    BOOST_LOG_TRIVIAL(debug) << "Loading global config from: " << global_config_file;
    ifstream global_config_stream(global_config_file); 
    string global_config_content(
        (istreambuf_iterator<char>(global_config_stream)),
        (istreambuf_iterator<char>())
    );
    boost::json::value global_config = boost::json::parse(global_config_content);
    boost::json::object transition_probabilities = global_config.at("PersonTransitionProbabilities").as_object();
    for(auto &from: transition_probabilities){
        for(auto &to: from.value().as_object()){
            Person::set_state_transition(
                Person::string_to_state(from.key_c_str()),
                Person::string_to_state(to.key_c_str()),
                to.value().as_double()
            );
        }
    }
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
                        json_value_to_float(args.at("width")),
                        json_value_to_float(args.at("height"))
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
                boost::json::string type = args.at("type").as_string();

                if(type == "door"){
                    Room* room_a = dynamic_cast<Room*>(vertexs_tmp.at(args.at("roomA").as_string()));
                    Room* room_b = dynamic_cast<Room*>(vertexs_tmp.at(args.at("roomB").as_string()));
                    room_a->add_door(
                        json_value_to_float(args.at("roomAx")),  
                        json_value_to_float(args.at("roomAy")),  
                        json_value_to_float(args.at("roomBx")),  
                        json_value_to_float(args.at("roomBy")),  
                        room_b 
                    );
                    room_b->add_door(
                        json_value_to_float(args.at("roomBx")),  
                        json_value_to_float(args.at("roomBy")),  
                        json_value_to_float(args.at("roomAx")),  
                        json_value_to_float(args.at("roomAy")),  
                        room_a 
                    );
                }else if(type == "in"){
                    Person* person = dynamic_cast<Person*>(vertexs_tmp.at(args.at("person").as_string()));
                    Room* room = dynamic_cast<Room*>(vertexs_tmp.at(args.at("room").as_string()));
                    person->put_in_room(room);
                }
            }catch(const std::exception& e){
                BOOST_LOG_TRIVIAL(fatal) << "Edge Failed: "<< e.what() 
                    << ":" << edge_file << ":" << line_number << " " << line;
                throw runtime_error("InvalidEdgeDefinition"); 
            }
            line_number++;
        }
        stream.close();
    }
    
    random_device rd;
    mt19937 g(rd());
    
    seed_seq seq{rd(), rd()};
    vector<std::uint32_t> seeds;
    seeds.resize(_vertexs.size());
    seq.generate(seeds.begin(), seeds.end());


    auto vertex_iter = _vertexs.begin();
    auto seed_iter = seeds.begin();
    while(vertex_iter != _vertexs.end()){
        (*vertex_iter)->set_seed(*seed_iter);
        vertex_iter++;
        seed_iter++;
    }
}
float Graph::json_value_to_float(boost::json::value value){
    float out;
    if(value.is_int64()){
        out = value.as_int64();
    }else if(value.is_uint64()){
        out = value.as_uint64();
    }else if(value.is_double()){
        out = value.as_double();
    }else{
        throw runtime_error("InvalidType");
    }
    return out;
}
Graph::Graph(string output_dir): 
    _num_of_threads(thread::hardware_concurrency()),
    _output_dir(output_dir){};

