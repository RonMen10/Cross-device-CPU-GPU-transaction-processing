//
// Created by gurumurt on 28/04/20.
//

#ifndef HERO_INIT_HPP
#define HERO_INIT_HPP

#include "opencl_layer/initialize.hpp"
#include "task_layer/initialize.hpp"
#include "TPCH/initialize.hpp"


namespace hero{

    void initialize(){

        openCL_layer::initialize_openCL();
        task_layer::initialize_tasks();
        TPCH::initialize();
    }
}

#endif //HERO_INIT_HPP
