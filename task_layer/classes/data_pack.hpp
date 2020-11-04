//
// Created by gurumurt on 28/04/20.
//

#ifndef HERO_DATA_PACK_HPP
#define HERO_DATA_PACK_HPP

#pragma once

#include "../../headers.hpp"
#include "../../opencl_layer/data_interface.hpp"

namespace task_layer{

    using namespace std;

    /**
     * Class to encapsulate all data information pushed into OpenCL environment
     */
    class dataPack {

    public:

        uint mPId,mDId; /// device related index
        uint qIndex{0}; /// queue used for pushing and fetching data
        int*  CPUlocation; /// location of the data in C++
        int alias; /// alias given to the data in OpenCL
        size_t size; /// data size

        dataPack();

        /**
         * overloaded constructor
         * @param alias
         * @param CPUlocation
         * @param pID
         * @param dID
         * @param size
         * @param q
         */
        dataPack(int alias,int* CPUlocation,uint pID, uint dID,size_t size, uint q);

        /**
         * Adds data into target. This requires all the member varibles initialized. currently no check are being done
         * so, developer has to manually check before calling this function
         */
        void add_data();

        /**
         * gets data from target device and places it into CPUlocation
         */
        void get_data();

        ~dataPack();

    };
}

#endif //HERO_DATA_PACK_HPP
