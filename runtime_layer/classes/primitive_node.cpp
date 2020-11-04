//
// Created by gurumurt on 29/04/20.
//

#include "primitive_node.hpp"


namespace runtime_layer{

    using namespace task_layer;

    primitive_node::primitive_node() {

    }

    primitive_node::primitive_node(PRIMITIVE type) {

        /**
         * TODO: switch based on types
         * Here comes switch case that sets the default values for primitives given their type
         */
        task t = task_layer::tasks[type];

        switch (type) {

        }
    }

    void primitive_node::compile_task(){

        this->operation.set_platform_device(this->pID,this->dID);
        this->operation.task_kernel.mPId = this->pID;
        this->operation.task_kernel.mDId = this->dID;
        this->operation.task_kernel.compile_kernel();

    }

    int primitive_node::place_data(int *data, size_t size, uint q) {

            dataPack d;
            d.mDId = this->dID;
            d.mPId = this->pID;
            d.size = size;
            d.alias = get_alias();
            d.qIndex = q;
            d.CPUlocation = data;

            d.add_data();
            this->operation.task_data.push_back(d);
            this->input_data.push_back(d.alias);
            return d.alias;
    }

    void primitive_node::retrieve_data(int alias, size_t size, uint q) {

        for(short i=0;i<this->operation.task_data.size();i++){

            if(this->operation.task_data[i].alias == alias){

                operation.task_data[i].get_data();
                return;
            }
        }
    }

    int primitive_node::prepare_output(size_t partial_size, uint q) {

        dataPack d;

        if(!partial_size)
            partial_size = this->input_size;

        if(this->overall_output_size==0)
            this->overall_output_size= partial_size; //this->input_to_output_ratio;



        d.mDId = this->dID;
        d.mPId = this->pID;
        d.size = partial_size;
        d.alias = get_alias();
        d.qIndex = q;
        d.CPUlocation = (int *) calloc((uint) d.size, sizeof(uint));
        d.add_data();

        this->operation.task_data.push_back(d);
        this->output_data.push_back(d.alias);
        return d.alias;
    }

    void primitive_node::add_parameter(int parameter) {

        if (this->no_of_parameters > this->operation.additional_parameters.size())
            this->operation.additional_parameters.push_back(parameter);
    }

    void primitive_node::execute_node(short envi_type){

        vector<int> dataPointer;

        for (int in : input_data) {

            dataPointer.push_back(in);
        }
        for (int out : output_data) {

            dataPointer.push_back(out);
        }
        std::cout<<this->operation.task_kernel.mKernelName<<endl;
        if(envi_type == 0) {

            openCL_layer::execute(this->pID, this->dID,
                                  this->operation.task_kernel.mKernelName, dataPointer,
                                  this->operation.additional_parameters,
                                  this->input_size / this->input_to_output_ratio);
        } else{

            //Run the node in CPP
            cpp_layer::execute(this->operation.task_kernel.mKernelName,dataPointer,this->operation.additional_parameters,this->input_size/this->input_to_output_ratio);
        }
    }

    void primitive_node::set_device(string device_name) {

        std::pair<uint,uint> p_d = openCL_layer::mDevices[device_name];
        this->pID = p_d.first;
        this->dID = p_d.second;

        this->operation.task_kernel.mPId = p_d.first;
        this->operation.task_kernel.mDId = p_d.second;

        for(auto & i : this->operation.task_data){

            i.mPId = p_d.first;
            i.mDId = p_d.second;
        }
    }
}