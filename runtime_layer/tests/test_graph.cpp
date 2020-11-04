//
// Created by gurumurt on 29/04/20.
//

#include "test_graph.hpp"
#include "../../TPCH/initialize.hpp"
#include "../runtime_globals.hpp"
namespace tests{

    using namespace runtime_layer;

    //performs conjunctive selection on TPCH data and returns output bitmap
    void test_heroDB_graphs(short dev_id, size_t input_size){

        TPCH::initialize();

        data_scan.clear();
        results.clear();

        input_size  = 1024;

        data_scan.insert(make_pair(1,make_pair(
                dbase.get_data("part","p_partkey"),
                dbase.get_size("part","p_partkey") - dbase.get_size("part","p_partkey")%32
                )));

        data_scan.insert(make_pair(2,make_pair(
                dbase.get_data("part","p_size"),
                dbase.get_size("part","p_size") - dbase.get_size("part","p_size")%32
                )));

        task_layer::initialize_tasks();

        //set operation
        query_graph q_;
        primitive_node sel1,sel2,log;

        string target_device = openCL_layer::mDeviceNames[dev_id];

        sel1.operation = task_layer::tasks[13];
        sel1.no_of_output_required =1;
        sel1.no_of_input_required = 1;
        sel1.no_of_parameters = 1;
        sel1.input_to_output_ratio=32;
        sel1.operation.intent = 32;
        sel1.add_parameter(1022);
        sel1.set_device(target_device);


        sel2.operation = tasks[13];
        sel2.no_of_output_required =1;
        sel2.no_of_input_required = 1;
        sel2.no_of_parameters = 1;
        sel2.input_to_output_ratio=32;
        sel2.operation.intent = 32;
        sel2.add_parameter(37);
        sel2.set_device(target_device);

        log.operation = tasks[4];
        log.no_of_output_required =1;
        log.no_of_input_required = 2;
        log.no_of_parameters = 0;
        log.input_to_output_ratio=1;
        log.operation.intent = 1;
        log.set_device(target_device);

        QEP_vertex s = add_vertex(sel1,q_.dag);
        QEP_vertex l = add_vertex(log,q_.dag);
        QEP_vertex s1 = add_vertex(sel2,q_.dag);

        data_edge d;
        d.data_pointer = 1;
        add_edge(q_.start_vertex,s,d,q_.dag);
        add_edge(s,l,q_.dag);
        d.data_pointer = 2;
        add_edge(q_.start_vertex,s1,d,q_.dag);
        add_edge(s1,l,q_.dag);
        add_edge(l,q_.end_vertex,q_.dag);

        cout<<"created test query!"<<endl;
        q_.execution_environment = 0; //Here I switch between CPU based primitive execution and GPU based OpenCL execution. 0 for OpenCl and 1 for CPU
        time_t start = clock();
        q_.execute_graph();
        time_t end = clock();
//        print_result();
        cout<<"Time: "<<(double) end - start<<endl;
    }
}