//
// Created by gurumurt on 28/04/20.
//

#ifndef HERO_CPP_GLOBALS_HPP
#define HERO_CPP_GLOBALS_HPP

#include "../headers.hpp"

namespace cpp_layer{

    extern std::map<std::string, void(*)(std::vector<int*> args, std::vector<int> parameters, size_t size)> mKernelDictionary; /// global map of kernel,device_ID to the compiled kernel
    extern std::map<int, void*> mDataDictionary;/// global map of alias,device_ID to pushed data

    extern double mExecTime;
    extern time_t start_timer, end_timer;
}

#endif HERO_CPP_GLOBALS_HPP
