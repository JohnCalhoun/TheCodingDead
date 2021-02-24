#ifndef VERTEX_H_INCLUDED 
#define VERTEX_H_INCLUDED

#include <algorithm>
#include <iostream>
#include <mutex>
#include <string>
#include <utility>
#include <vector>
#include <map>

using namespace std;

class Vertex {
    public:
    typedef Vertex*         ptr;
    typedef ptr             Edge;
    typedef int             output_value;
    typedef string          output_key;
    typedef pair<output_key, output_value> output;
    string label;
    string id;

    protected:
    vector<Edge> edges;
    vector<Edge> edges_to_add;
    vector<Edge> edges_to_remove;
    mutex edge_mutex;
    map<string, vector<ptr> > vertex_groups;

    void add_edge(ptr vertex_ptr);
    void remove_edge(ptr vertex_ptr);
    void _update_edges();
    virtual vector<output> _update();

    public:
    virtual vector<output> interact(map<string, vector<ptr> > others);
    void create_edge_to(ptr vertex_ptr);
    void delete_edge_to(ptr vertex_ptr);
    void update();
    virtual vector<output> simulate();
    Vertex(string id, string label_name);
    virtual ~Vertex() = default;
};

#endif
