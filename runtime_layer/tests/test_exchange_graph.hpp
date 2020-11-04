//
// Created by bala on 03.05.20.
//

#ifndef HERO_TEST_EXCHANGE_GRAPH_HPP
#define HERO_TEST_EXCHANGE_GRAPH_HPP

#include "../../headers.hpp"
#include "../classes/primitive_graph.hpp"
#include "../morsel_driven/exchange.hpp"
#include <iomanip>


namespace tests{
    /**
     * tests simple conjuctive selection (ie. A<x && B<y) on given device with parallelism
     * @param dev_id target devide idx
     * @param input_size size of data to generate
     * */
    void test_heroDB_exchange_graphs(short dev_id=0, size_t input_size=pow(2,10));
}
#endif //HERO_TEST_EXCHANGE_GRAPH_HPP
