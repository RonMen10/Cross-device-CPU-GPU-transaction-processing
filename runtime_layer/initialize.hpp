//
// Created by gurumurt on 29/04/20.
//

#ifndef HERO_INITIALIZE_HPP
#define HERO_INITIALIZE_HPP

#include "../headers.hpp"
#include "../task_layer/initialize.hpp"

namespace runtime_layer{

    using namespace std;

    /**
     * initializes the system for execution. in turn initializes openCL and tasks
     */
    inline void initialize_graphs(){

        task_layer::initialize_tasks();
    }
}

#endif //HERO_INITIALIZE_HPP
