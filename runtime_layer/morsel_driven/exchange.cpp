//
// Created by gurumurt on 01/05/20.
//

#include "exchange.hpp"

namespace morsel_driven {

    std::mutex mute;

    using namespace runtime_layer;
    exchange_query_graph::exchange_query_graph() {

        //Initialize the device
        no_of_devices = openCL_layer::mDeviceNames.size();
        device_ = new int[no_of_devices];

        m = new std::mutex[no_of_devices];
        start_processing = new std::condition_variable[no_of_devices];
        ready = new bool[no_of_devices];
        processed=new bool[no_of_devices];
        process_vertex = new runtime_layer::QEP_vertex[no_of_devices];

        for(short i = 0;i<no_of_devices;i++){

            ready[i] = false;
            processed[i] = false;
        }

    }

    void exchange_query_graph::form_lockfree_processing_queue() {

        query_graph::form_processing_queue();

        for (QEP_vertex v_ : process_queue) {//feed into lockfree queue

            //in case of data scan => load the data here itself
            if(this->dag[v_].operation.task_kernel.get_kernel_name() == "data_scan"){

                this->load_data(v_);

                outgoing_edges eo, eo_end;
                for(boost::tie(eo, eo_end) = out_edges(v_,this->dag); eo!=eo_end; ++eo){

                    this->dag[*eo].input_ID_source = this->dag[*eo].data_pointer; //re-initializing the data alias to source pointer. This can be read by the target device to make copy of the data
                    this->dag[*eo].ready_for_next_step = true; //re-initializing the data alias to source pointer. This can be read by the target device to make copy of the data
                }

                //add the appropriate edge information
                continue;
            }
            else if(this->dag[v_].operation.task_kernel.get_kernel_name() == "data_display")
                continue;

            this->ex_processing_queue.push(v_);
        }
    }

    void exchange_query_graph::execute_exchange_graph() {

        form_lockfree_processing_queue();

        /**
         * Using normal std threads
         */

        std::thread d_thread[no_of_devices];

        for (short i = 0; i < no_of_devices; i++)
            d_thread[i] = thread(&exchange_query_graph::process_in_device,this,i);

        for (short i = 0; i < no_of_devices; i++)
                d_thread[i].join();

        cout<<"Display result"<<endl;
//        this->display_result();
    }

    std::mutex m[2];
    std::condition_variable start_processing[2];
    bool ready[2]= {false,false};
    bool processed[2]={false,false};
    runtime_layer::QEP_vertex process_vertex[2];


    void exchange_query_graph::process_in_device(int i) {

        std::thread p_thread = thread(&exchange_query_graph::process_op,this,i);

        //Each device compete in parallel to select one operation to run
        while(!this->ex_processing_queue.empty()){

            this->ex_processing_queue.pop(process_vertex[i]);
            cout<< std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count()<<"\t"
                <<"started: "<<this->dag[process_vertex[i]].operation.task_kernel.get_kernel_name()<<" \t "<< openCL_layer::mDeviceNames[i]<<endl;
            transfer_op(i);
            cout<< std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count()<<"\t"
                <<"transfer done for: "<<this->dag[process_vertex[i]].operation.task_kernel.get_kernel_name()<<" \t "<< openCL_layer::mDeviceNames[i]<<endl;
            //send data to worker thread
            {
                std::lock_guard<std::mutex> lk(m[i]);
                ready[i]=true;
            }

            start_processing[i].notify_one();

            //Wait for worker
            {
                std::unique_lock<std::mutex> lk(m[i]);
                start_processing[i].wait(lk, [this, i]{return processed[i];});
            }

            processed[i] = false;

            cout<< std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count()<<"\t"
                <<"End of operation "<<this->dag[process_vertex[i]].operation.task_kernel.get_kernel_name()<<"\t"
                << openCL_layer::mDeviceNames[i]<<endl;
        }

        {
            //breaking the processing loop
            process_vertex[i] = INT_MAX;
            std::lock_guard<std::mutex> lk(m[i]);
            ready[i]=true;
        }

        start_processing[i].notify_one();

        //Wait for worker
        {
            std::unique_lock<std::mutex> lk(m[i]);
            start_processing[i].wait(lk, [this, i]{return processed[i];});
        }

        p_thread.join();
    }

    void exchange_query_graph::transfer_op(short i){


        //Allocate buffer for placing data to be transferred
        //before we get the input data size, we have to determine the number of data to be actually transferred or the
        // number of incoming edges available

        //Setting target device
        string device_name = openCL_layer::mDeviceNames[i];
        std::pair<uint,uint> p_d = openCL_layer::mDevices[device_name];

        this->dag[process_vertex[i]].pID = p_d.first;
        this->dag[process_vertex[i]].dID = p_d.second;

        incoming_edges ei, ei_end;
        size_t data_size_of_input = 0;


        cout<< std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count()<<"\t"
            <<"before wait \t"<<this->dag[process_vertex[i]].operation.task_kernel.get_kernel_name()<<"\t"
            << openCL_layer::mDeviceNames[i]<<endl;
        /**
         * This code loops the thread until all the values of input are set
         */
        do{
            bool input_set = true;
            for(boost::tie(ei, ei_end) = in_edges(process_vertex[i],this->dag); ei!=ei_end; ++ei){
                input_set &= (this->dag[*ei].ready_for_next_step);
            }

            if(input_set)
                break;

        }while(true);

        for(boost::tie(ei, ei_end) = in_edges(process_vertex[i],this->dag); ei!=ei_end; ++ei){
            data_size_of_input = (data_size_of_input < this->dag[*ei].data_size) ? this->dag[*ei].data_size : data_size_of_input;
        }

        cout<< std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count()<<"\t"
            <<"after wait for data "<<this->dag[process_vertex[i]].operation.task_kernel.get_kernel_name()<<"\t"
            << openCL_layer::mDeviceNames[i]<<endl;

        this->dag[process_vertex[i]].input_size = data_size_of_input;
        this->set_input(process_vertex[i]);
        this->prepare_output_buffer(process_vertex[i]);

        cout<< std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count()<<"\t"
            <<"data set for "<<this->dag[process_vertex[i]].operation.task_kernel.get_kernel_name()<<"\t"
            << openCL_layer::mDeviceNames[i]<<endl;
        //loop on the edges to transfer data from source to target in morsels
        //exit once data are transferred
    }

    void exchange_query_graph::process_op(short i) {

        while(true){

            //wait until data is sent
            std::unique_lock<std::mutex> lk(m[i]);
            start_processing[i].wait(lk,[this, i]{return ready[i];});
            ready[i] = false;

            if(process_vertex[i] == INT_MAX){

                processed[i] = true;
                lk.unlock();
                start_processing[i].notify_one();
                return;
            }


            //processing

            this->dag[process_vertex[i]].compile_task();
            this->dag[process_vertex[i]].execute_node();
            cout<< std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count()<<"\t"
                <<"processed : "<<this->dag[process_vertex[i]].operation.task_kernel.get_kernel_name()<<" \t "<< openCL_layer::mDeviceNames[i]<<endl;
            //We have to stop the dependent operation until the current one is executed. This can be done, by manipulating
            // the spin lock we have for looking up the data size. Here we have another exchange value ready_for_next_step
            // which is set to be true after execution completes
            outgoing_edges eo, eo_end;
            for(boost::tie(eo, eo_end) = out_edges(process_vertex[i],this->dag); eo!=eo_end; ++eo){

                this->dag[*eo].ready_for_next_step = true;
            }

            //After processing
            processed[i] = true;
            lk.unlock();
            start_processing[i].notify_one();
        }
    }
}