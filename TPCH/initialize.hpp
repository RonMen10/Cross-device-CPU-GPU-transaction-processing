//
// Created by gurumurt on 28/04/20.
//

#ifndef HERO_TPCH_INITIALIZE_HPP
#define HERO_TPCH_INITIALIZE_HPP

#include "tpch_globals.hpp"
#include "TPCH.hpp"

static benchmark::TPCH dbase;

namespace TPCH{

    using namespace std;


    inline void initialize(){

        cout<<"Loaded TPCH!"<<endl;
    }
}

#endif //HERO_TPCH_INITIALIZE_HPP
