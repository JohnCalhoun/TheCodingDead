#ifndef VERTEX_H_INCLUDED 
#define VERTEX_H_INCLUDED

#include <algorithm>
#include <iostream>
#include <mutex>
#include <string>
#include <utility>
#include <vector>
#include <map>
#include <cstdint>
#include <unordered_set>
using namespace std;

class Vertex {
    public:
    typedef Vertex*         ptr;
    typedef ptr             Edge;
    typedef int             output_value;
    typedef string          output_key;
    typedef pair<output_key, output_value> output;
    typedef map<string, unordered_set<ptr> > MapToVertexs;

    string id;
    string label;

    protected:
    vector<Edge>        edges_to_add;
    vector<Edge>        edges_to_remove;
    mutex               edge_mutex;
    MapToVertexs        vertex_groups;
    std::uint32_t       _seed;
    
    void _update_edges();
    virtual vector<output> _update();

    public:
    void add_edge(ptr vertex_ptr);
    void remove_edge(ptr vertex_ptr);
    virtual vector<output> interact(MapToVertexs others);
    void create_edge_to(ptr vertex_ptr);
    void delete_edge_to(ptr vertex_ptr);
    void update();
    virtual vector<output> simulate();
    Vertex(string id, string label_name);
    virtual ~Vertex() = default;
    virtual void set_seed(std::uint32_t seed);
};

#endif
