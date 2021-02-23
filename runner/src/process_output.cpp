#include <iostream>
#include <string>
#include <boost/program_options.hpp>
#include <filesystem>
#include <fstream>
#include <unordered_map>
#include <stdexcept>

namespace po = boost::program_options;
using namespace std;

int main(int argc, char *argv[]){
    string data_dir;

    po::options_description desc("Allowed options");
    desc.add_options()
        ("help", "show this message")
        ("data-dir", po::value<string>(&data_dir), "directory to read data files");

    po::variables_map vm;
    po::store(
        po::command_line_parser(argc, argv).options(desc).run()
        , vm);
    po::notify(vm);
    if(vm.count("help")){
        cout << desc;
        return 0;
    }

    if(!(vm.count("data-dir"))){
        cout << "Missing Required options" << endl;
        cout << desc;
        return 1;
    }
    

    ostringstream output_file_name;
    output_file_name << data_dir << "/" << "combined.csv";
    ofstream output_file{output_file_name.str()};
   
    // read keys
    ostringstream key_file_name;
    key_file_name << data_dir << "/" << "keys.csv";

    fstream key_file;
    key_file.open(key_file_name.str());
    string line;
    cout << "Reading key file: " << key_file_name.str()<<endl;
    int line_number = 0;

    // read in keys
    vector<string> keys;
    while(getline(key_file, line)){
        if(line_number!=0){
            output_file << ",";
        }
        keys.push_back(line);
        output_file << line;
        line_number++;
    }
    output_file << endl;

    unordered_map<string, int> data;
    int iteration =0;

    while(true){
        ostringstream file_name;
        file_name << data_dir << "/" << iteration << ".csv";
        if(filesystem::exists(file_name.str())){ 
            ifstream data_file{file_name.str()};
            string line;
            while(getline(data_file,line)){
                auto comma_pos = line.find_first_of(",");
                if(comma_pos == string::npos){
                    throw runtime_error("InvalidLine");
                }
                string key = line.substr(0, comma_pos);
                int value = stoi(line.substr(comma_pos +1, line.size()));
                data[key] = value;
            }
            int pos =0;
            for(auto& x: keys){
                if(pos !=0 ){
                    output_file << ",";
                }
                pos++;
                output_file << data[x];
            }
            output_file << endl;
            for(auto& x: data){
                x.second = 0;
            }
            iteration++;
        }else{
            break;
        }
    }
    output_file.close();
}

