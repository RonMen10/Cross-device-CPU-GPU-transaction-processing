//
// Created by gurumurt on 28/04/20.
//

#include "task.hpp"

namespace task_layer{

    task::task(){

        pID = 0;
        dID = 0;
}

    void task::set_platform_device(uint p, uint d){

        pID = p;
        dID = d;
        task_kernel.mPId = p;
        task_kernel.mDId = d;
    }


    void task::set_task(short e){

        switch(e){

            case 0 : this->environment = TASK_ENVIRONMENT::OPENCL;
                        break;

            case 1 : this->environment = TASK_ENVIRONMENT::CPP;
                        break;

            default: this->environment = TASK_ENVIRONMENT::OPENCL;
                        break;
        }
    }

    void task::execute_task(){

        cout<<"Executing: "<<this->task_kernel.mKernelName<<endl;

        //prepare execution data

        switch(this->environment){

            case TASK_ENVIRONMENT::OPENCL : {

                    //This prepares data for openCL
                    vector<int> dataPointer;
                    for(dataPack d : this->task_data)
                        dataPointer.push_back(d.alias);

                    openCL_layer::execute(this->pID,this->dID,this->task_kernel.mKernelName,dataPointer,this->additional_parameters,this->data_size/this->intent);
                }
                break;

            case TASK_ENVIRONMENT::CPP : {

                //This prepares data for CPP using CPUlocation
                vector<int> dataPointer;
                for(dataPack d : this->task_data){

                    cpp_layer::mDataDictionary[d.alias] =d.CPUlocation;
                    dataPointer.push_back(d.alias);
                }
                cpp_layer::execute(this->task_kernel.mKernelName,dataPointer,this->additional_parameters,this->data_size/this->intent);
            }
        }
    }
}