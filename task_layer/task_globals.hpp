//
// Created by gurumurt on 28/04/20.
//

#ifndef HERO_TASK_GLOBALS_HPP
#define HERO_TASK_GLOBALS_HPP

#pragma once

#include <string>
#include "../headers.hpp"
#include "classes/task.hpp"
#include <dirent.h>

namespace task_layer{

    extern std::string KERNEL_ROOT_DIR; /// kernels folder, from where default tasks can be loaded
    extern short no_of_kernels;
    extern std::vector<std::string> kernels;    ///loaded kernel names
    extern void setupKernelNames(std::string _path=KERNEL_ROOT_DIR);    /// reads files from kernel root and prepares kernel for tasks
    extern std::vector<task> tasks;  /// vector of tasks
}

#endif //HERO_TASK_GLOBALS_HPP
