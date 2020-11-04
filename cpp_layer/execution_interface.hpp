//
// Created by gurumurt on 28/04/20.
//

#ifndef HERO_CPP_EXECUTION_INTERFACE_HPP
#define HERO_CPP_EXECUTION_INTERFACE_HPP
#pragma once
#include "../headers.hpp"
#include "cpp_globals.hpp"

namespace cpp_layer{

    /*!
     * It helps to execute the kernel that are built previously along with all the arguments
     * @param _platformId The platform id containing the device
     * @param _deviceId The id of the device
     * @param _kernelName The unique name for the kernel
     * @param _args List of arguments needed for the for the kernel
     * @param _param List of constant values needed
     * @param _globalSize
     * @param _localSize
     * @param _addLocalBuffer
     * @param _addLocalBufferFactor
     * @param ev
     * @param _cmd_queue_loc The position of the command queue which is needed. Default value is 0
     */
//    template<typename T>
    inline void
    execute(std::string _kernelName, std::vector<int> _args, std::vector<int> _param,
            size_t _globalSize);

    /*!
     * Fetches the execution time of kernel
     * @return time for execution of last kernel
     */
    inline double get_execution_time();


    inline void extract_execution_time(cl_event *_ev);

    inline void
    execute(std::string _kernelName, std::vector<int> _args, std::vector<int> _param,
            size_t _globalSize) {

        //Based on the kernel name call the primitive function with execution parameters added to it
        std::vector<int*> a;
        for (int arg_index : _args){
            a.push_back((int*)mDataDictionary[arg_index]);
        }

        start_timer = clock();
            mKernelDictionary[_kernelName](a,_param,_globalSize);
        end_timer = clock();
    }

    inline  double get_execution_time() {
        return mExecTime;
    }

    inline void extract_execution_time(cl_event *_ev) {

        mExecTime = (double) end_timer - start_timer;
    }

}

#endif HERO_CPP_EXECUTION_INTERFACE_HPP
