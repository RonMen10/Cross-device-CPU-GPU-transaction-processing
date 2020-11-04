//
// Created by gurumurt on 28/04/20.
//


#include "task_globals.hpp"

namespace task_layer{

    std::string KERNEL_ROOT_DIR = "/Users/ronald/Desktop/DBSE---Project/CODD-pro-master/kernels/";
    short no_of_kernels = 2;
    std::vector<std::string> kernels;

    vector<task> tasks;

    void setupKernelNames(std::string _path){

        kernels.clear();

        DIR *directory;
        struct dirent *ent;
        if ((directory = opendir(_path.c_str())) != NULL) {

            while ((ent = readdir (directory)) != NULL){

                std::string s(ent->d_name);
                size_t lastIndex = s.find_last_of(".");
                s = s.substr(0,lastIndex);
                if(s.size() == 1 || s.size() == 0)
                    continue;
                kernels.emplace_back(s);
            }

            closedir (directory);
        }
        else
            std::cerr<<"could not find kernel root directory"<<std::endl;
    }
}