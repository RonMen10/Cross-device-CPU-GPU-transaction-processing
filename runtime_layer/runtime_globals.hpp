//
// Created by gurumurt on 29/04/20.
//

#ifndef HERO_RUNTIME_GLOBALS_HPP
#define HERO_RUNTIME_GLOBALS_HPP

#include "../headers.hpp"

namespace runtime_layer {

    using namespace std;

    extern size_t total_data_set_in_device; /// internal counter for automatically provide aliases

    /**
     * provide alias ID that is not already available in any device
     * @return alias ID
     */
    extern int get_alias();

    extern vector<int *> results;   /// global vector ro hold all results
    extern size_t total_result_size; /// final result size
    enum PRIMITIVE {
        SELECTION,
        LOGICAL_AND,
        PARTIAL_PREFIX_SUM
    };

    /**
     * prints results
     */
    extern void print_result();
}

#endif //HERO_RUNTIME_GLOBALS_HPP