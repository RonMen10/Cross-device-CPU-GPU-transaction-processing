//
// Created by gurumurt on 28/04/20.
//

#ifndef HERO_TASK_HPP
#define HERO_TASK_HPP

#pragma once

#include "../../headers.hpp"
#include "kernel_pack.hpp"
#include "data_pack.hpp"

#include "../../opencl_layer/execution_interface.hpp"
#include "../../cpp_layer/execution_interface.hpp"

namespace task_layer{

    enum TASK_ENVIRONMENT {CPP, OPENCL};

    /**
     * Class combines kernels with data and adds runtime information
     */
    class task{

        TASK_ENVIRONMENT environment{TASK_ENVIRONMENT::OPENCL}; /// This points the execution environment {CPP, OpenCL...}
        uint pID,dID;   ///device indexes. Important: these has to be same as the ones in dataPack and kernelPack

    public:

        task();

        void set_task(short e=0);
        vector<size_t> output_positions;    ///position ids of output from this task
        kernelPack task_kernel;             /// executable kernel

        vector<dataPack> task_data;         ///set of io data for task
        size_t data_size;                   /// data size to translate for parallelism

        vector<int> additional_parameters;  ///runtime parameters


        cl_device_id dev_id;

        void set_platform_device(uint p, uint d);   ///this function sets indexes on task,kernel,data together
        uint intent{1};                             ///number of data processed by a single thread
        size_t local_size{1};

        void execute_task();                        ///executes task in mentioned target
    };
}
#endif //HERO_TASK_HPP
