#include <iostream>
#include "graph.hpp"
#include <string>
#include <boost/program_options.hpp>
#include <boost/log/trivial.hpp>

namespace po = boost::program_options;
using namespace std;

int main(int argc, char *argv[]){
    int iterations;
    string graph_def;
    string output_dir;
    string log_level;

    po::options_description desc("Allowed options");
    desc.add_options()
        ("help", "show this message")
        ("graph-def-dir", po::value<string>(&graph_def), "directory holding the starting graph definition")
        ("output-dir", po::value<string>(&output_dir), "directory to write output files")
        ("iterations", po::value<int>(&iterations)->default_value(1),"number of iterations to run");

    po::variables_map vm;
    po::store(
        po::command_line_parser(argc, argv).options(desc).run()
        , vm);
    po::notify(vm);
    if(vm.count("help")){
        cout << desc;
        return 0;
    }

    if(!(vm.count("graph-def-dir") && vm.count("output-dir"))){
        cout << "Missing Required options" << endl;
        cout << desc;
        return 1;
    }

    Graph graph(output_dir);
    graph.load(graph_def);

    for(int i=0; i<iterations; ++i){
        cout << "Running Iteration: " << i << endl;
        graph.run_iteration(i);
    }
    graph.write_output();
    return 0;
}

