//
// Created by gurumurt on 29/04/20.
//

#include "test_task_execution.hpp"
#include "../../opencl_layer/environment.hpp"
#include "../initialize.hpp"

namespace tests{
    using namespace task_layer;
    void test_heroDB_tasks(short environment){

        uint _intent = 4;

        // random input
        uint m_size = pow(2, 5);
        uint m_rand_arr[] = {2, 2, 4, 7, 7, 7, 10, 10, 10, 10, 15, 15, 20, 25, 25, 25, 26, 26, 27, 27, 27, 27, 27, 28, 29,
                             29, 29, 30, 31, 32, 32, 32};

        initialize_tasks();

        task prefix_sum = tasks[4];
        prefix_sum.intent = 4;
        prefix_sum.data_size = m_size;
        prefix_sum.output_positions.push_back(1);
        prefix_sum.output_positions.push_back(2);

        stringstream s; //updating the command line args
        s<<" -DINTENT=" << prefix_sum.intent << " -DOFFSET_SIZE=" << prefix_sum.data_size/prefix_sum.intent;
        prefix_sum.task_kernel.mCmdArgs = s.str();
        prefix_sum.task_kernel.compile_kernel();

        dataPack d1,d2,d3;

        //Input
        d1.mPId = prefix_sum.task_kernel.mPId;
        d1.mDId = prefix_sum.task_kernel.mDId;
        d1.size = prefix_sum.data_size;
        d1.CPUlocation = reinterpret_cast<int *>(m_rand_arr);
        d1.alias = 1;
        d1.add_data();
        prefix_sum.task_data.push_back(d1);

        //output
        d2.mPId = prefix_sum.task_kernel.mPId;
        d2.mDId = prefix_sum.task_kernel.mDId;
        d2.size = prefix_sum.data_size;
        d2.CPUlocation = (int *) calloc((uint) d2.size, sizeof(uint));
        d2.alias = 2;
        d2.add_data();
        prefix_sum.task_data.push_back(d2);

        d3.mPId = prefix_sum.task_kernel.mPId;
        d3.mDId = prefix_sum.task_kernel.mDId;
        d3.size = prefix_sum.data_size/prefix_sum.intent;
        d3.CPUlocation = (int *) calloc((uint) d3.size, sizeof(uint));
        d3.alias = 3;
        d3.add_data();
        prefix_sum.task_data.push_back(d3);

        prefix_sum.set_task(environment);
        prefix_sum.execute_task();

        /*std::cout << "********************PARTIAL PREFIX SUM FOR KERNEL********************" << std::endl;
        std::cout << "input : ";
        openCL_layer::print_data(d1.alias,d1.mPId,d1.mDId,d1.size);
        std::cout << std::endl;
        std::cout << "partial PS : ";
        openCL_layer::print_data(d2.alias,d2.mPId,d2.mDId,d2.size);
        std::cout << std::endl;
        std::cout << "offsets : ";
        openCL_layer::print_data(d3.alias,d3.mPId,d3.mDId,d3.size);
        std::cout << std::endl;*/
    }
}