//
// Created by bala on 03.05.20.
//

#include "test_exchange_graph.hpp"
#include "../runtime_globals.hpp"


namespace tests{

    using namespace runtime_layer;

void test_heroDB_exchange_graphs(short dev_id, size_t input_size){

    //set data to scan - TPCH not working
    int* a = new int[input_size];
    int* b = new int[input_size];

    for(int i =0;i<input_size;i++){

        a[i] = i;
        b[i] = i;
    }

    data_scan.clear();
    results.clear();

    data_scan.insert(make_pair(1,make_pair(a,input_size)));
    data_scan.insert(make_pair(2,make_pair(b,input_size)));

    task_layer::initialize_tasks();

    //set operation
    morsel_driven::exchange_query_graph q_;
    primitive_node sel1,sel2,log;

    string target_device = openCL_layer::mDeviceNames[dev_id];

    sel1.operation = task_layer::tasks[3];
    sel1.no_of_output_required =1;
    sel1.no_of_input_required = 1;
    sel1.no_of_parameters = 1;
    sel1.input_to_output_ratio=32;
    sel1.operation.intent = 32;
    sel1.add_parameter(1022);
    sel1.set_device(target_device);


    sel2.operation = tasks[3];
    sel2.no_of_output_required =1;
    sel2.no_of_input_required = 1;
    sel2.no_of_parameters = 1;
    sel2.input_to_output_ratio=32;
    sel2.operation.intent = 32;
    sel2.add_parameter(1022);
    sel2.set_device(target_device);

    target_device = openCL_layer::mDeviceNames[1];
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

    add_edge(q_.start_vertex,s,q_.dag);
    add_edge(s,l,q_.dag);
    add_edge(q_.start_vertex,s1,q_.dag);
    add_edge(s1,l,q_.dag);
    add_edge(l,q_.end_vertex,q_.dag);

    cout<<"executing test query!"<<endl;
    time_t start = clock();
    q_.execute_exchange_graph();
    time_t end = clock();

    cout<<"Computed "<< total_result_size <<" results in "<<(double) end - start<<" ms"<<endl;
}
}