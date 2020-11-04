//
// Created by gurumurt on 29/04/20.
//

#ifndef HERO_GLOBALS_HPP
#define HERO_GLOBALS_HPP

#include "headers.hpp"

extern uint default_device; /// default device idx
extern uint default_platform; /// default platform idx
extern std::map<int,std::pair<int*,size_t>> data_scan; ///map that holds order to input pointer with its size to be included into target device
#endif //HERO_GLOBALS_HPP
