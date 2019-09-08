#include <cstdlib>
#include <iostream>
#include <string>
#include <fstream>
#include <streambuf>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include "json.hpp"

using namespace std;
using json = nlohmann::json;

string get_file_ext(const string& s) {

   size_t i = s.rfind('.', s.length());
   if (i != string::npos) {
      return(s.substr(i+1, s.length() - i));
   }

   return("");
}

inline bool is_file_exist(const string& name) {
  struct stat buffer;   
  return (stat (name.c_str(), &buffer) == 0); 
}

// Harry - Begin

int add_argv(const char* val, int* p_argc, char*** p_argv) {
    const int MAX_NUM_ARG = 1024;

    if ((*p_argc) >= MAX_NUM_ARG - 1) {
        return -1;
    }

    if ((*p_argc) == 0) {
        (*p_argv) = (char**) malloc(sizeof (char*) * MAX_NUM_ARG);
    }

    (*p_argv)[(*p_argc)] = (char*) malloc(sizeof (char) * MAX_NUM_ARG);
    strncpy((*p_argv)[(*p_argc)], val, MAX_NUM_ARG);
    (*p_argv)[(*p_argc)][strlen(val)] = '\0';
    (*p_argc)++;

    (*p_argv)[(*p_argc)] = NULL;

    return 0;
}

int main(int argc, char** argv) {
	int volo_start(int*, char***);
}
// Harry - End


string convert_json_to_command(const string& json_str, int* p_argc, char*** p_argv) {
    json j = json::parse(json_str);
    string command;
	
	(*p_argc) = 0;
	add_argv("transcoder", p_argc, p_argv);
    
    for (auto& el : j.items()) { //3 Are you sure all fields in JSON can be read in order ?
        
        command.append("-" + el.key() + " "); //4 How about options without value like "-y", "-re",
        command.append(el.value()); 
        command.append(" ");
		
		add_argv( ("-" + el.key()).c_str() , p_argc, p_argv); // Harry
		add_argv( el.value().c_str() , p_argc, p_argv); // Harry
    }
    
    return command;
}

int main(int argc, char** argv) {
    const char* file = "file.json";
    
    if (!is_file_exist(file)) {
        cout << "The file doesn't exist" << endl; // 2 all ffmpeg error message are printed through stderr, we will write own callback fun to output logs.
        return 1;
    }
    if (!(get_file_ext(file) == "json")) {
        cout << "The file isn't json format" << endl;
        return 1;
    }
    
    ifstream t(file);
    string j((std::istreambuf_iterator<char>(t)),
        istreambuf_iterator<char>());
        
    cout << convert_json_to_command(j, &argc, &argv) << endl;
    
    return 0;
}
