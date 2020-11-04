//
// Created by gurumurt on 28/04/20.
//

#ifndef HERO_CPP_INITIALIZE_HPP
#define HERO_CPP_INITIALIZE_HPP

#include "../headers.hpp"
#include "environment.hpp"

namespace cpp_layer{

    using namespace std;

    /**
     * Initializes OpenCL environment
     */
    inline void initialize_cpp(){

        setup_environment();
        cout<<"C++ primitives ready!"<<endl;
    }
}

#endif //HERO_CPP_INITIALIZE_HPP
