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
    if(_phase == "update"){
        size_t room_chunk_size = _rooms.size()/threads;
        auto room_it=_rooms.begin();
        
        advance(room_it, room_chunk_size * i);
        auto room_end = room_it;
        advance(room_end, room_chunk_size);

        while(room_it != room_end  && room_it != _rooms.end()){
            (*room_it).update();
            room_it++;
        }
    }else if(_phase =="simulate"){
        size_t people_chunk_size = _people.size()/threads;
        auto people_it=_people.begin();
        
        advance(people_it, people_chunk_size * i);
        auto people_end = people_it;
        advance(people_end, people_chunk_size);

        while(people_it != people_end  && people_it != _people.end()){
            vector<Vertex::output> out = (*people_it)->interact();
            for(auto x: out){
                _outputs[i][x.first] += x.second;
            }
            people_it++;
        }
    }
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
    unordered_map<boost::json::string, size_t> rooms_tmp;
    for(auto vertex_file: vertex_files){
        BOOST_LOG_TRIVIAL(debug) << "Loading Vertex File: " << vertex_file;

        stream.open(vertex_file);
        string line;
        int line_number=0;
        while(getline(stream,line)){
            try{
                boost::json::value args = boost::json::parse(line);

                boost::json::string type = args.at("type").as_string();
                if(type == "room"){
                    _rooms.emplace_back(
                        args.at("id").as_string().data(),
                        json_value_to_float(args.at("width")),
                        json_value_to_float(args.at("height"))
                    );
                    rooms_tmp.emplace(
                        args.at("id").as_string(),
                        _rooms.size()-1
                    );
                }else if(type == "person"){
                    Person* new_vertex =new Person(
                        args.at("id").as_string().data()
                    );
                    vertexs_tmp[args.at("id").as_string()] = new_vertex;
                    _people.emplace(new_vertex);
                }else{
                    BOOST_LOG_TRIVIAL(fatal) << "unknow vertex type: "<< type;
                    throw runtime_error("InvalidVertexDefinition"); 
                }
            }catch(const std::exception& e){
                BOOST_LOG_TRIVIAL(fatal) << "Vertex Failed: "<< e.what() 
                    << ":" << vertex_file << ":" << line_number;
                throw runtime_error("InvalidVertexDefinition"); 
            }
            line_number++;
        }
        stream.close();
    }
    _rooms.shrink_to_fit(); 
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
                    _rooms[rooms_tmp.at(args.at("roomA").as_string())].add_door(
                        json_value_to_float(args.at("roomAx")),  
                        json_value_to_float(args.at("roomAy")),  
                        json_value_to_float(args.at("roomBx")),  
                        json_value_to_float(args.at("roomBy")),  
                        &_rooms[rooms_tmp.at(args.at("roomB").as_string())]
                    );
                    _rooms[rooms_tmp.at(args.at("roomB").as_string())].add_door(
                        json_value_to_float(args.at("roomBx")),  
                        json_value_to_float(args.at("roomBy")),  
                        json_value_to_float(args.at("roomAx")),  
                        json_value_to_float(args.at("roomAy")),  
                        &_rooms[rooms_tmp.at(args.at("roomA").as_string())]
                    );
                }else if(type == "in"){
                    Person* person = dynamic_cast<Person*>(vertexs_tmp.at(args.at("person").as_string()));
                    person->put_in_room(
                        &_rooms[rooms_tmp.at(args.at("room").as_string())]
                    );
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
    seeds.resize(_rooms.size());
    seq.generate(seeds.begin(), seeds.end());


    auto room_iter = _rooms.begin();
    auto seed_iter = seeds.begin();
    while(room_iter != _rooms.end()){
        (*room_iter).set_seed(*seed_iter);
        room_iter++;
        seed_iter++;
    }
   
    seeds.clear();
    seeds.resize(_people.size());
    seq.generate(seeds.begin(), seeds.end());


    auto people_iter = _people.begin();
    seed_iter = seeds.begin();
    while(people_iter != _people.end()){
        (*people_iter)->set_seed(*seed_iter);
        people_iter++;
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

