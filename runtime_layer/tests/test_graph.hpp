//
// Created by gurumurt on 29/04/20.
//

#ifndef HERO_TEST_GRAPH_HPP
#define HERO_TEST_GRAPH_HPP

#include "../../headers.hpp"
#include "../classes/primitive_graph.hpp"

namespace tests{

    /**
     * tests simple conjuctive selection (ie. A<x && B<y) on given device without parallelism
     * @param dev_id target devide idx
     * @param input_size size of data to generate
     */
    void test_heroDB_graphs(short dev_id=0, size_t input_size=pow(2,10));
}
#endif //HERO_TEST_GRAPH_HPP
