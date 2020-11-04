#ifndef HERO_HASHJOIN_HPP
#define HERO_HASHJOIN_HPP

#include "../../../headers.hpp"
#include "../../classes/primitive_graph.hpp"
#include "../../morsel_driven/exchange.hpp"
#include "../../../TPCH/initialize.hpp"
#include "../../runtime_globals.hpp"

namespace tests{

    using namespace std;
    using namespace runtime_layer;

    void query_hashJoin(vector<int> a, vector<int> b);
}

#endif //HERO_HASHJOIN_HPP