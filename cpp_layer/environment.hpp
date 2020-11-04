//
// Created by gurumurt on 28/04/20.
//

#ifndef HERO_CPP_ENVIRONMENT_HPP
#define HERO_CPP_ENVIRONMENT_HPP

#pragma once

#include "../headers.hpp"
#include "cpp_globals.hpp"
#include "operations/primitives.hpp"

namespace cpp_layer {

    using namespace std;

    /*!
    * This function helps to extract all the necessary information about the C++ execution necessary
    * 1. we add all the available primitives into mkernelDict
    */
    inline void setup_environment() {

        //Added the primitive function calls
        mKernelDictionary["selection_between_block"] =selection_between_block;
        mKernelDictionary["selection_less_than_block"] =selection_less_than_block;
        mKernelDictionary["selection_greater_than_block"] =selection_greater_than_block;

        mKernelDictionary["logical_block"] =logical_block;
        mKernelDictionary["logical_block_or"] =logical_block_or;

        mKernelDictionary["materialize"] =materialize;

        mKernelDictionary["partial_prefix_sum"] =partial_prefix_sum;

        mKernelDictionary["ordered_aggregate_count_block"] =ordered_aggregate_count_block;
        mKernelDictionary["ordered_aggregate_sum_block"] =ordered_aggregate_sum_block;

        mKernelDictionary["arith_sum"] =arith_sum;
        mKernelDictionary["arith_mul"] =arith_mul;
        mKernelDictionary["aggregate_block"] =aggregate_block;
        //MR added code:

        // End RM code
        cout<<mKernelDictionary.size()<< " primitives locked and loaded"<<endl;
    }

    /*!
     * Just prints the available devices for end users.
     */
    inline void print_environment() {

        cout<<"Running over C++"<<endl;
    }

    /*!
     * This is not required for C++
     * @return vector of platforms
     */
    inline  void get_environment() {
    }

    /*!
     * This is also not required for C++
     * @return vector of device name strings
     */
    inline void get_device_names() {
    }

    /*!
     * Also not required
     */
    inline void get_id(std::string _dev_name) {

    }
}

#endif //HERO_CPP_ENVIRONMENT_HPP
