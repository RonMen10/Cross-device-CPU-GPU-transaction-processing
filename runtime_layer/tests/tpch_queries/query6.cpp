//
// Created by gurumurt on 05/05/20.
//

#include "query6.hpp"

namespace tests{

    using namespace std;
    using namespace runtime_layer;

    /**
     * Performs TPCH query 6 over GPU
     *
     * SELECT
     *    sum(l_extendedprice * l_discount) as revenue
     * FROM
     *     lineitem
     * WHERE
     *     l_shipdate >= date '1994-01-01'
     *     AND l_shipdate < date '1994-01-01' + interval '1' year
     *     AND l_discount between 0.06 - 0.01 AND 0.06 + 0.01
     *     AND l_quantity < 24;
     *
     */
    void query_6(){

        TPCH::initialize();

        data_scan.clear();
        results.clear();

        //Initialization of input
        data_scan.insert(make_pair(1,make_pair(
                dbase.get_data("lineitem","l_shipdate"),
                dbase.get_size("lineitem","l_shipdate") - dbase.get_size("lineitem","l_shipdate")%32 //%32 is for making sure the data is not having any unalignement
        )));

        data_scan.insert(make_pair(2,make_pair(
                dbase.get_data("lineitem","l_discount"),
                dbase.get_size("lineitem","l_discount") - dbase.get_size("lineitem","l_discount")%32
        )));

        data_scan.insert(make_pair(3,make_pair(
                dbase.get_data("lineitem","l_quantity"),
                dbase.get_size("lineitem","l_quantity") - dbase.get_size("lineitem","l_quantity")%32
        )));

        /**
         * TODO: re-using data of data is not currently supported
         *
         * This has to be included into the edge information for easy processing
         */

        data_scan.insert(make_pair(4,make_pair(
                dbase.get_data("lineitem","l_extendedprice"),
                dbase.get_size("lineitem","l_extendedprice") - dbase.get_size("lineitem","l_extendedprice")%32
        )));


        // --- constants --- parameters Q6
        auto c1 = types::Date::castString("1994-01-01");
        auto c2 = types::Date::castString("1995-01-01");
        auto c3 = types::Numeric<12, 2>::castString("0.05");
        auto c4 = types::Numeric<12, 2>::castString("0.07");
        auto c5 = types::Integer(24);

        task_layer::initialize_tasks();

        //set operation
        query_graph q_;
        primitive_node sel1,sel2,sel3,log1,log2,mate1,mate2,arith,agg;

        string target_device = openCL_layer::mDeviceNames[0];
        q_.execution_environment = 0;

        sel1.operation = task_layer::tasks[1];
        sel1.no_of_output_required =1;
        sel1.no_of_input_required = 1;
        sel1.no_of_parameters = 2;
        sel1.input_to_output_ratio=32;
        sel1.operation.intent = 32;
        sel1.add_parameter(c1.value);
        sel1.add_parameter(c2.value);
        sel1.set_device(target_device);


        sel2.operation = tasks[1];
        sel2.no_of_output_required =1;
        sel2.no_of_input_required = 1;
        sel2.no_of_parameters = 2;
        sel2.input_to_output_ratio=32;
        sel2.operation.intent = 32;
        sel2.add_parameter(c3.value);
        sel2.add_parameter(c4.value);
        sel2.set_device(target_device);

        sel3.operation = tasks[8];
        sel3.no_of_output_required =1;
        sel3.no_of_input_required = 1;
        sel3.no_of_parameters = 1;
        sel3.input_to_output_ratio=32;
        sel3.operation.intent = 32;
        sel3.add_parameter(c5.value);
        sel3.set_device(target_device);

        log1.operation = tasks[4];
        log1.no_of_output_required =1;
        log1.no_of_input_required = 2;
        log1.no_of_parameters = 0;
        log1.input_to_output_ratio=1;
        log1.operation.intent = 1;
        log1.set_device(target_device);

        log2.operation = tasks[4];
        log2.no_of_output_required =1;
        log2.no_of_input_required = 2;
        log2.no_of_parameters = 0;
        log2.input_to_output_ratio=1;
        log2.operation.intent = 1;
        log2.set_device(target_device);

        mate1.operation = tasks[3];
        mate1.no_of_output_required =1;
        mate1.no_of_input_required = 2;
        mate1.no_of_parameters = 0;
        mate1.input_to_output_ratio=32;
        mate1.overall_output_size= dbase.get_size("lineitem","l_linestatus")- dbase.get_size("lineitem","l_linestatus")%32;
        mate1.operation.intent = 32;
        mate1.set_device(target_device);

        mate2.operation = tasks[3];
        mate2.no_of_output_required =1;
        mate2.no_of_input_required = 2;
        mate2.no_of_parameters = 0;
        mate2.input_to_output_ratio=32;
        mate2.overall_output_size= dbase.get_size("lineitem","l_linestatus")- dbase.get_size("lineitem","l_linestatus")%32;
        mate2.operation.intent = 32;
        mate2.set_device(target_device);

        arith.operation = tasks[11];
        arith.no_of_output_required =1;
        arith.no_of_input_required = 2;
        arith.no_of_parameters = 0;
        arith.input_to_output_ratio=1;
        arith.operation.intent = 1;
        arith.set_device(target_device);

        agg.operation = tasks[6];
        agg.no_of_output_required =1;
        agg.no_of_input_required = 1;
        agg.no_of_parameters = 0;
        agg.overall_output_size=1;
        agg.input_to_output_ratio=32;
        agg.operation.intent = 32;
        agg.set_device(target_device);

        QEP_vertex s1 = add_vertex(sel1,q_.dag);
        QEP_vertex s2 = add_vertex(sel2,q_.dag);
        QEP_vertex s3 = add_vertex(sel3,q_.dag);
        QEP_vertex l1 = add_vertex(log1,q_.dag);
        QEP_vertex l2 = add_vertex(log2,q_.dag);
        QEP_vertex m1  = add_vertex(mate1,q_.dag);
        QEP_vertex m2  = add_vertex(mate2,q_.dag);
        QEP_vertex a  = add_vertex(arith,q_.dag);
        QEP_vertex ag = add_vertex(agg,q_.dag);

        data_edge d;
        d.data_pointer = 1;
        add_edge(q_.start_vertex,s1,d,q_.dag);
        d.data_pointer = 2;
        add_edge(q_.start_vertex,s2,d,q_.dag);
        d.data_pointer = 3;
        add_edge(q_.start_vertex,s3,d,q_.dag);
        d.data_pointer = 2;
        add_edge(q_.start_vertex,m1,d,q_.dag);
        d.data_pointer = 4;
        add_edge(q_.start_vertex,m2,d,q_.dag);
        add_edge(s1,l1,q_.dag);
        add_edge(s3,l1,q_.dag);
        add_edge(l1,l2,q_.dag);
        add_edge(s2,l2,q_.dag);
        add_edge(l2,m1,q_.dag);
        add_edge(l2,m2,q_.dag);
        add_edge(m1,a,q_.dag);
        add_edge(m2,a,q_.dag);
        add_edge(a,ag,q_.dag);
        add_edge(a,q_.end_vertex,q_.dag);

        time_t start = clock();
        q_.execute_graph();
        print_result();
        time_t end = clock();
        if(!q_.execution_environment)
            cout<<"OpenCL - ";
        else
            cout<<"C++"<<endl;
        cout<<target_device<<" : "<<(double) end - start<<" ms"<<endl;
    }
}