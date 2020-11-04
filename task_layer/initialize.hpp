//
// Created by gurumurt on 28/04/20.
//

#ifndef HERO_INITIALIZE_TASK_HPP
#define HERO_INITIALIZE_TASK_HPP

#pragma once

#include "../headers.hpp"
#include "task_globals.hpp"
#include "classes/task.hpp"
#include "../utilities.hpp"
#include "../globals.hpp"
#include "../opencl_layer/initialize.hpp"
#include "../cpp_layer/initialize.hpp"

namespace task_layer{

    using namespace std;

    /**
     * Initializes tasks. predefined tasks are identified from kernels folder
     */
    inline void initialize_tasks(){

        cpp_layer::initialize_cpp();
        openCL_layer::initialize_openCL();

        //Get all kernel files present in kernels
        setupKernelNames();

        tasks.clear();

        cout<<"No. of kernels found: "<<kernels.size()<<endl;

        size_t task_idx = 0;
        //preparing kernelpack
        for(string k : kernels){

            kernelPack kp;
            kp.mKernelName = k;
            kp.mKernelSrc = utilities::readFile(KERNEL_ROOT_DIR + k + ".cl");

            stringstream ss;
            ss<< "";
            kp.mCmdArgs = ss.str();

            task t;
            t.task_kernel = kp;
            t.set_platform_device(default_platform,default_device);
            tasks.emplace_back(t);
            cout<<task_idx<<" : "<<tasks[task_idx].task_kernel.mKernelName<<endl;
            task_idx++;
        }

        cout<<"Tasks set for kernels"<<endl;
    }
}

#endif //HERO_INITIALIZE_TASK_HPP
