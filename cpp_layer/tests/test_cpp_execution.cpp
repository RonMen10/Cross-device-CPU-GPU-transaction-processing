//
// Created by gurumurt on 16/05/20.
//

#include "test_cpp_execution.hpp"


namespace tests{

    /**
     * Testing function that generates 32 random integer values and performs partial prefix sum for them in OpenCL environment
     */
    void test_heroDB_cpp(){

        uint _intent = 4;

        // random input
        uint m_size = pow(2, 5);
        uint m_rand_arr[] = {2, 2, 4, 7, 7, 7, 10, 10, 10, 10, 15, 15, 20, 25, 25, 25, 26, 26, 27, 27, 27, 27, 27, 28, 29,
                             29, 29, 30, 31, 32, 32, 32};


        cpp_layer::initialize_cpp();

        // Add data to the respective devices
        cpp_layer::add_data(1, m_rand_arr);
        cpp_layer::add_data(2, (uint *) calloc((uint) m_size, sizeof(uint)));

        std::cout<<"data added"<<std::endl;
        std::string kernel_name = "partial_prefix_sum";

        // Pass array of elements
        std::vector<int> arguments;
        arguments.push_back(1);
        arguments.push_back(2);

        // Pass constant Params
        std::vector<int> param;

        std::cout<<"execution ready"<<std::endl;
        cpp_layer::execute(kernel_name, arguments, param, m_size);
        std::cout<<"execution done"<<std::endl;
        cpp_layer::print_data(2, m_size);
    }
}