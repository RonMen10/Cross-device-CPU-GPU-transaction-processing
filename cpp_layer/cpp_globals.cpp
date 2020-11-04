//
// Created by gurumurt on 28/04/20.
//

#include "cpp_globals.hpp"

namespace cpp_layer{

    std::map<std::string,void(*)(std::vector<int*> args, std::vector<int> parameters, size_t size)> mKernelDictionary;
    std::map<int, void*> mDataDictionary;

    double mExecTime = 0.0;
    time_t start_timer=clock();
    time_t end_timer=clock();
}
