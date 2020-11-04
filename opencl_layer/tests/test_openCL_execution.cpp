//
// Created by gurumurt on 29/04/20.
//

#include "test_openCL_execution.hpp"

namespace tests{

    /**
     * Testing function that generates 32 random integer values and performs partial prefix sum for them in OpenCL environment
     */
    void test_heroDB_openCL(){

        uint _intent = 4;

        // random input
        uint m_size = pow(2, 5);
        uint m_rand_arr[] = {2, 2, 4, 7, 7, 7, 10, 10, 10, 10, 15, 15, 20, 25, 25, 25, 26, 26, 27, 27, 27, 27, 27, 28, 29,
                             29, 29, 30, 31, 32, 32, 32};


        openCL_layer::setup_environment();
        uint _p_id = 0;
        uint _d_id = 0;

        uint _offset_arr_size = m_size / _intent;

        // Add data to the respective devices
        openCL_layer::add_data(1, m_rand_arr, _p_id, _d_id, m_size);
        openCL_layer::add_data(2, (uint *) calloc((uint) m_size, sizeof(uint)), _p_id, _d_id, m_size);
        openCL_layer::add_data(3, (uint *) calloc((uint) _offset_arr_size, sizeof(uint)), _p_id, _d_id,
                          _offset_arr_size);

        std::cout<<"Data name-->Device name"<<std::endl;
        std::cout<<"---------------------"<<std::endl;
        openCL_layer::lookup_data_buffer();
        std::cout<<"---------------------"<<std::endl;

        // Add kernels to the respective devices
        std::string kernel_name = "partial_prefix_sum";
        std::string kernel_src = utilities::readFile("/home/gurumurt/Hero/kernels/partial_prefix_sum.cl"); // bala's system path

        // Add the values that you need for defining at Kernel
        std::stringstream _sStream;
        _sStream << " -DINTENT=" << _intent << " -DOFFSET_SIZE=" << _offset_arr_size;

        if (!kernel_src.empty()) {
            openCL_layer::add_kernel(_p_id, _d_id, kernel_name, kernel_src, _sStream.str());

            // Pass array of elements
            std::vector<int> arguments;
            arguments.push_back(1);
            arguments.push_back(2);
            arguments.push_back(3);

            // Pass constant Params
            std::vector<int> param;

            openCL_layer::execute(_p_id, _d_id, kernel_name, arguments, param, _offset_arr_size, 1);

            std::cout << "********************PARTIAL PREFIX SUM FOR KERNEL********************" << std::endl;
            std::cout << "input : ";
            openCL_layer::print_data(1, _p_id, _d_id, m_size);
            std::cout << std::endl;
            std::cout << "partial PS : ";
            openCL_layer::print_data(2, _p_id, _d_id, m_size);
            std::cout << std::endl;
            std::cout << "offsets : ";
            openCL_layer::print_data(3, _p_id, _d_id, _offset_arr_size);
        } else {
            std::cout << "Error in reading Bit Mask kernel source file.\n";
        }
    }
}