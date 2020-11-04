//
// Created by gurumurt on 28/04/20.
//

#ifndef HERO_INITIALIZE_HPP
#define HERO_INITIALIZE_HPP

#include "../headers.hpp"
#include "environment.hpp"

namespace openCL_layer{

    using namespace std;

    /**
     * Initializes OpenCL environment
     */
    inline void initialize_openCL(){

        setup_environment();
        print_environment();
        cout<<"OpenCL up and running!"<<endl;
    }
}

#endif //HERO_INITIALIZE_HPP
