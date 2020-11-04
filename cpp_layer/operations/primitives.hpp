//
// Created by gurumurt on 16/05/20.
//

#ifndef HERO_PRIMITIVES_HPP
#define HERO_PRIMITIVES_HPP


#include "../../headers.hpp"

namespace cpp_layer{

    /**
     * Selection operations
     */
    void selection_between_block( std::vector<int*> args, std::vector<int> parameters, size_t size);
    void selection_less_than_block( std::vector<int*> args, std::vector<int> parameters, size_t size);
    void selection_greater_than_block( std::vector<int*> args, std::vector<int> parameters, size_t size);

    /**
     * logical block
     */
    void logical_block(std::vector<int*> args, std::vector<int> parameters, size_t size);
    void logical_block_or(std::vector<int*> args, std::vector<int> parameters, size_t size);

    /**
     * materialize
     */
    void materialize(std::vector<int*> args, std::vector<int> parameters, size_t size);

    /**
     * Partial prefix sum block
     */
    void partial_prefix_sum(std::vector<int*> args, std::vector<int> parameters, size_t size);

    /**
     * ordered aggregation
     */
    void ordered_aggregate_count_block(  std::vector<int*> args, std::vector<int> parameters, size_t size);
    void ordered_aggregate_sum_block(  std::vector<int*> args, std::vector<int> parameters, size_t size);

    /**
     * arithmetic block
     */
    void arith_sum(std::vector<int*> args, std::vector<int> parameters, size_t size);
    void arith_mul(std::vector<int*> args, std::vector<int> parameters, size_t size);

    /**
     * Aggregation
     */
    void aggregate_block(std::vector<int*> args, std::vector<int> parameters, size_t size);

    /**
     * RM code start
     */
}

#endif //HERO_PRIMITIVES_HPP
