//
// Created by gurumurt on 28/04/20.
//

#pragma once
#include "data_pack.hpp"
#include "../../cpp_layer/data_interface.hpp"


namespace task_layer{

    dataPack::dataPack() {}

    dataPack::dataPack(int alias,int* CPUlocation,uint pID, uint dID,size_t size, uint q=0){

        this->mPId = pID;
        this->mDId = dID;
        this->size = size;
        this->alias = alias;
        this->CPUlocation = CPUlocation;
        this->qIndex = q;
        add_data();
    }

    void dataPack::add_data(){

        openCL_layer::add_data(this->alias,this->CPUlocation,this->mPId,this->mDId,this->size,1,this->qIndex);

        /**
         * Adding a copy of data into CPP layer as well -- this has to be definitely improved
         *
         */
         cpp_layer::add_data(this->alias,this->CPUlocation);

    }

    void dataPack::get_data(){

     this->CPUlocation = reinterpret_cast<int *>(openCL_layer::get_data(this->alias, this->mPId, this->mDId,
                                                                        this->size));

    }

    dataPack::~dataPack() {}
}