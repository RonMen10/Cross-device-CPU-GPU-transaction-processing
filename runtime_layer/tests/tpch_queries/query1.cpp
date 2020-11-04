//
// Created by gurumurt on 06/05/20.
//

#include "query1.hpp"

namespace tests{

    using namespace std;
    using namespace runtime_layer;

    /**
     * Performs TPCH query 1 over GPU
     *
     * SELECT
     *     l_returnflag,
     *     l_linestatus,
     *     sum(l_quantity) as sum_qty,
     *     sum(l_extendedprice) as sum_base_price,
     *     sum(l_extendedprice * (1 - l_discount)) as sum_disc_price,
     *     sum(l_extendedprice * (1 - l_discount) * (1 + l_tax)) as sum_charge,
     *     avg(l_quantity) as avg_qty,
     *     avg(l_extendedprice) as avg_price,
     *     avg(l_discount) as avg_disc,
     *     count(*) as count_order
     * FROM
     *     lineitem
     * WHERE
     *     l_shipdate <= date '1998-12-01' - interval '90' day
     * GROUP BY
     *     l_returnflag,
     *     l_linestatus
     * ORDER BY
     *     l_returnflag,
     *     l_linestatus;
     *
     */

    void
    query_1(){

        TPCH::initialize();

        data_scan.clear();
        results.clear();
        short input_id = 1;
        //Initialization of input

        //Filter input
        data_scan.insert(make_pair(input_id++,make_pair(
                dbase.get_data("lineitem","l_shipdate"),
                dbase.get_size("lineitem","l_shipdate") - dbase.get_size("lineitem","l_shipdate")%32 //%32 is for making sure the data is not having any unalignement
        )));

        //ordering input
        data_scan.insert(make_pair(input_id++,make_pair(
                dbase.get_data("lineitem","l_returnflag"),
                dbase.get_size("lineitem","l_returnflag") - dbase.get_size("lineitem","l_returnflag")%32
        )));

        data_scan.insert(make_pair(input_id++,make_pair(
                dbase.get_data("lineitem","l_linestatus"),
                dbase.get_size("lineitem","l_linestatus") - dbase.get_size("lineitem","l_linestatus")%32
        )));

        //Aggregation input
        data_scan.insert(make_pair(input_id++,make_pair(
                dbase.get_data("lineitem","l_extendedprice"),
                dbase.get_size("lineitem","l_extendedprice") - dbase.get_size("lineitem","l_extendedprice")%32
        )));

        data_scan.insert(make_pair(input_id++,make_pair(
                dbase.get_data("lineitem","l_quantity"),
                dbase.get_size("lineitem","l_quantity") - dbase.get_size("lineitem","l_quantity")%32
        )));

        data_scan.insert(make_pair(input_id++,make_pair(
                dbase.get_data("lineitem","l_discount"),
                dbase.get_size("lineitem","l_discount") - dbase.get_size("lineitem","l_discount")%32
        )));


        data_scan.insert(make_pair(input_id++,make_pair(
                dbase.get_data("lineitem","l_tax"),
                dbase.get_size("lineitem","l_tax") - dbase.get_size("lineitem","l_tax")%32
        )));

        task_layer::initialize_tasks();

        //set operation
        query_graph q_;
        primitive_node sel1,sel2,sel3,mate1,mate2,mate3,mate4,mate5,mate6;

        string target_device = openCL_layer::mDeviceNames[1];
        q_.execution_environment = 0;

        // --- constants --- parameters Q6
        auto c1 = types::Date::castString("1998-12-01");
        auto c2 = types::Date::castString("1998-09-01");


        sel1.operation = task_layer::tasks[11];
        sel1.no_of_output_required =1;
        sel1.no_of_input_required = 1;
        sel1.no_of_parameters = 2;
        sel1.input_to_output_ratio=32;
        sel1.operation.intent = 32;
        sel1.add_parameter(c1.value);
        sel1.set_device(target_device);

        mate1.operation = tasks[5];
        mate1.no_of_output_required =1;
        mate1.no_of_input_required = 2;
        mate1.no_of_parameters = 0;
        mate1.input_to_output_ratio=32;
        mate1.overall_output_size= dbase.get_size("lineitem","l_linestatus")- dbase.get_size("lineitem","l_linestatus")%32;
        mate1.operation.intent = 32;
        mate1.set_device(target_device);

        mate2.operation = tasks[5];
        mate2.no_of_output_required =1;
        mate2.no_of_input_required = 2;
        mate2.no_of_parameters = 0;
        mate2.input_to_output_ratio=32;
        mate2.overall_output_size= dbase.get_size("lineitem","l_linestatus")- dbase.get_size("lineitem","l_linestatus")%32;
        mate2.operation.intent = 32;
        mate2.set_device(target_device);

        mate3.operation = tasks[5];
        mate3.no_of_output_required =1;
        mate3.no_of_input_required = 2;
        mate3.no_of_parameters = 0;
        mate3.input_to_output_ratio=32;
        mate3.overall_output_size= dbase.get_size("lineitem","l_linestatus")- dbase.get_size("lineitem","l_linestatus")%32;
        mate3.operation.intent = 32;
        mate3.set_device(target_device);

        mate4.operation = tasks[5];
        mate4.no_of_output_required =1;
        mate4.no_of_input_required = 2;
        mate4.no_of_parameters = 0;
        mate4.input_to_output_ratio=32;
        mate4.overall_output_size= dbase.get_size("lineitem","l_linestatus")- dbase.get_size("lineitem","l_linestatus")%32;
        mate4.operation.intent = 32;
        mate4.set_device(target_device);

        mate5.operation = tasks[5];
        mate5.no_of_output_required =1;
        mate5.no_of_input_required = 2;
        mate5.no_of_parameters = 0;
        mate5.input_to_output_ratio=32;
        mate5.overall_output_size= dbase.get_size("lineitem","l_linestatus")- dbase.get_size("lineitem","l_linestatus")%32;
        mate5.operation.intent = 32;
        mate5.set_device(target_device);

        mate6.operation = tasks[5];
        mate6.no_of_output_required =1;
        mate6.no_of_input_required = 2;
        mate6.no_of_parameters = 0;
        mate6.input_to_output_ratio=32;
        mate6.overall_output_size= dbase.get_size("lineitem","l_linestatus")- dbase.get_size("lineitem","l_linestatus")%32;
        mate6.operation.intent = 32;
        mate6.set_device(target_device);

        QEP_vertex s1 = add_vertex(sel1,q_.dag);
        QEP_vertex m1 = add_vertex(mate1,q_.dag);
        QEP_vertex m2 = add_vertex(mate2,q_.dag);
        QEP_vertex m3 = add_vertex(mate3,q_.dag);
        QEP_vertex m4 = add_vertex(mate4,q_.dag);
        QEP_vertex m5 = add_vertex(mate5,q_.dag);
        QEP_vertex m6 = add_vertex(mate6,q_.dag);

        data_edge d;
        //Feed input to device
        d.data_pointer = 1;
        add_edge(q_.start_vertex,s1,d,q_.dag);
        d.data_pointer = 2;
        add_edge(q_.start_vertex,m1,d,q_.dag);
        d.data_pointer = 3;
        add_edge(q_.start_vertex,m2,d,q_.dag);
        d.data_pointer = 4;
        add_edge(q_.start_vertex,m3,d,q_.dag);
        d.data_pointer = 5;
        add_edge(q_.start_vertex,m4,d,q_.dag);
        d.data_pointer = 6;
        add_edge(q_.start_vertex,m5,d,q_.dag);
        d.data_pointer = 7;
        add_edge(q_.start_vertex,m6,d,q_.dag);

        //Perform selection and materialization
        add_edge(s1,m1,q_.dag);
        add_edge(s1,m2,q_.dag);
        add_edge(s1,m3,q_.dag);
        add_edge(s1,m4,q_.dag);
        add_edge(s1,m5,q_.dag);
        add_edge(s1,m6,q_.dag);

        //Fetch materialized results back
        add_edge(m1,q_.end_vertex,q_.dag);
        add_edge(m2,q_.end_vertex,q_.dag);
        add_edge(m3,q_.end_vertex,q_.dag);
        add_edge(m4,q_.end_vertex,q_.dag);
        add_edge(m5,q_.end_vertex,q_.dag);
        add_edge(m6,q_.end_vertex,q_.dag);


        //Execution
        time_t start = clock();
        q_.execute_graph();
        time_t end = clock();
        cout<<"select & materialize time : "<<(double)end - start<<" ms"<<endl;
        //performing multi-dimensional sorting here
        //Any result from execution are stored inside results vector pointed by int pointer
        //Making a pair of sorting arrays and dependant arrays


        vector<pair<vector<int>,vector<int>>> sort_by_value;
        for(int i=0;i<total_result_size;i++){

            pair<vector<int>,vector<int>> p;
            p.first.push_back(results[0][i]);
            p.first.push_back(results[1][i]);

            p.second.push_back(results[2][i]);
            p.second.push_back(results[3][i]);
            p.second.push_back(results[4][i]);
            p.second.push_back(results[5][i]);
            sort_by_value.push_back(p);
        }

        struct ordering{

            bool operator()(pair<vector<int>,vector<int>> const& a,
                            pair<vector<int>,vector<int>> const& b){

                for(int i =0; i<a.first.size();i++)
                    return a < b;
            }
        };

        start = clock();
        sort(sort_by_value.begin(),sort_by_value.end(),ordering());
        vector<int> ag_key,q;
        end = clock();
        cout<<"sort time : "<<(double)end - start<<" ms"<<endl;

        //Preparing prefix sum values
        start = clock();
        vector<int> first_val = sort_by_value[0].first;
        ag_key.push_back(0);
        q.push_back(sort_by_value[0].second[1]);
        for(size_t i =1;i<total_result_size; i++){

            q.push_back(sort_by_value[i].second[1]);
            vector<int> second_val =  sort_by_value[i].first;
            for(int j =0;j<second_val.size();j++){
                if(first_val[j] != second_val[j]){
                    ag_key.push_back(ag_key[ag_key.size()-1] + 1);
                    break;
                }
                ag_key.push_back(ag_key[ag_key.size()-1]);
            }
            first_val = second_val;
        }
        end = clock();
        cout<<"scatter time : "<<(double)end - start<<" ms"<<endl;

        //After sorting performing aggregation back in GPU
        data_scan.clear();
        results.clear();
        //create prefix sum using the keys

        //Re-starting execution in device
        input_id = 1;

        //result positions processed from CPU are fed into GPU
        data_scan.insert(make_pair(input_id++,make_pair(
                &ag_key[0],
                ag_key.size() - ag_key.size()%32
        )));

        data_scan.insert(make_pair(input_id++,make_pair(
                &q[0],
                q.size() - q.size()%32
        )));

        data_scan.insert(make_pair(input_id++,make_pair(
                &ag_key[0],
                ag_key.size() - ag_key.size()%32
        )));

        data_scan.insert(make_pair(input_id++,make_pair(
                &ag_key[0],
                ag_key.size() - ag_key.size()%32
        )));

        data_scan.insert(make_pair(input_id++,make_pair(
                &q[0],
                q.size() - q.size()%32
        )));

        data_scan.insert(make_pair(input_id++,make_pair(
                &ag_key[0],
                ag_key.size() - ag_key.size()%32
        )));

        data_scan.insert(make_pair(input_id++,make_pair(
                &q[0],
                q.size() - q.size()%32
        )));

        data_scan.insert(make_pair(input_id++,make_pair(
                &ag_key[0],
                ag_key.size() - ag_key.size()%32
        )));

        data_scan.insert(make_pair(input_id++,make_pair(
                &q[0],
                q.size() - q.size()%32
        )));

        data_scan.insert(make_pair(input_id++,make_pair(
                &ag_key[0],
                ag_key.size() - ag_key.size()%32
        )));

        data_scan.insert(make_pair(input_id++,make_pair(
                &q[0],
                q.size() - q.size()%32
        )));

        data_scan.insert(make_pair(input_id++,make_pair(
                &ag_key[0],
                ag_key.size() - ag_key.size()%32
        )));

        data_scan.insert(make_pair(input_id++,make_pair(
                &q[0],
                q.size() - q.size()%32
        )));

        data_scan.insert(make_pair(input_id++,make_pair(
                &ag_key[0],
                ag_key.size() - ag_key.size()%32
        )));

        data_scan.insert(make_pair(input_id++,make_pair(
                &q[0],
                q.size() - q.size()%32
        )));

        query_graph q_1;
        primitive_node agg1,agg2,agg3,agg4,agg5,agg6,agg7,agg8;

        agg1.operation = tasks[8];
        agg1.no_of_output_required =1;
        agg1.no_of_input_required = 2;
        agg1.no_of_parameters = 0;
        agg1.input_to_output_ratio=1;
        agg1.overall_output_size= ag_key[ag_key.size() - 1];
        agg1.operation.intent = 1;
        agg1.set_device(target_device);

        agg2.operation = tasks[6];
        agg2.no_of_output_required =1;
        agg2.no_of_input_required = 1;
        agg2.no_of_parameters = 0;
        agg2.input_to_output_ratio=1;
        agg2.overall_output_size= ag_key[ag_key.size() - 1];
        agg2.operation.intent = 1;
        agg2.set_device(target_device);

        agg3.operation = tasks[8];
        agg3.no_of_output_required =1;
        agg3.no_of_input_required = 2;
        agg3.no_of_parameters = 0;
        agg3.input_to_output_ratio=1;
        agg3.overall_output_size= ag_key[ag_key.size() - 1];
        agg3.operation.intent = 1;
        agg3.set_device(target_device);

        agg4.operation = tasks[8];
        agg4.no_of_output_required =1;
        agg4.no_of_input_required = 2;
        agg4.no_of_parameters = 0;
        agg4.input_to_output_ratio=1;
        agg4.overall_output_size= ag_key[ag_key.size() - 1];
        agg4.operation.intent = 1;
        agg4.set_device(target_device);


        agg5.operation = tasks[8];
        agg5.no_of_output_required =1;
        agg5.no_of_input_required = 2;
        agg5.no_of_parameters = 0;
        agg5.input_to_output_ratio=1;
        agg5.overall_output_size= ag_key[ag_key.size() - 1];
        agg5.operation.intent = 1;
        agg5.set_device(target_device);

        agg6.operation = tasks[8];
        agg6.no_of_output_required =1;
        agg6.no_of_input_required = 2;
        agg6.no_of_parameters = 0;
        agg6.input_to_output_ratio=1;
        agg6.overall_output_size= ag_key[ag_key.size() - 1];
        agg6.operation.intent = 1;
        agg6.set_device(target_device);

        agg7.operation = tasks[8];
        agg7.no_of_output_required =1;
        agg7.no_of_input_required = 2;
        agg7.no_of_parameters = 0;
        agg7.input_to_output_ratio=1;
        agg7.overall_output_size= ag_key[ag_key.size() - 1];
        agg7.operation.intent = 1;
        agg7.set_device(target_device);


        agg8.operation = tasks[8];
        agg8.no_of_output_required =1;
        agg8.no_of_input_required = 2;
        agg8.no_of_parameters = 0;
        agg8.input_to_output_ratio=1;
        agg8.overall_output_size= ag_key[ag_key.size() - 1];
        agg8.operation.intent = 1;
        agg8.set_device(target_device);


        QEP_vertex ag1 = add_vertex(agg1,q_1.dag);
        QEP_vertex ag2 = add_vertex(agg2,q_1.dag);
        QEP_vertex ag3 = add_vertex(agg3,q_1.dag);
        QEP_vertex ag4 = add_vertex(agg4,q_1.dag);
        QEP_vertex ag5 = add_vertex(agg5,q_1.dag);
        QEP_vertex ag6 = add_vertex(agg6,q_1.dag);
        QEP_vertex ag7 = add_vertex(agg7,q_1.dag);
        QEP_vertex ag8 = add_vertex(agg8,q_1.dag);

        //Feed input to device
        d.data_pointer=1;
        add_edge(q_1.start_vertex,ag1,d,q_1.dag);
        d.data_pointer=2;
        add_edge(q_1.start_vertex,ag1,d,q_1.dag);
        d.data_pointer=3;
        add_edge(q_1.start_vertex,ag2,d,q_1.dag);
        d.data_pointer=4;
        add_edge(q_1.start_vertex,ag3,d,q_1.dag);
        d.data_pointer=5;
        add_edge(q_1.start_vertex,ag3,d,q_1.dag);
        d.data_pointer=6;
        add_edge(q_1.start_vertex,ag4,d,q_1.dag);
        d.data_pointer=7;
        add_edge(q_1.start_vertex,ag4,d,q_1.dag);
        d.data_pointer=8;
        add_edge(q_1.start_vertex,ag5,d,q_1.dag);
        d.data_pointer=9;
        add_edge(q_1.start_vertex,ag5,d,q_1.dag);
        d.data_pointer=10;
        add_edge(q_1.start_vertex,ag6,d,q_1.dag);
        d.data_pointer=11;
        add_edge(q_1.start_vertex,ag6,d,q_1.dag);
        d.data_pointer=12;
        add_edge(q_1.start_vertex,ag7,d,q_1.dag);
        d.data_pointer=13;
        add_edge(q_1.start_vertex,ag7,d,q_1.dag);
        d.data_pointer=14;
        add_edge(q_1.start_vertex,ag8,d,q_1.dag);
        d.data_pointer=15;
        add_edge(q_1.start_vertex,ag8,d,q_1.dag);

        add_edge(ag1,q_1.end_vertex,q_1.dag);
        add_edge(ag2,q_1.end_vertex,q_1.dag);
        add_edge(ag3,q_1.end_vertex,q_1.dag);
        add_edge(ag4,q_1.end_vertex,q_1.dag);
        add_edge(ag5,q_1.end_vertex,q_1.dag);
        add_edge(ag6,q_1.end_vertex,q_1.dag);
        add_edge(ag7,q_1.end_vertex,q_1.dag);
        add_edge(ag8,q_1.end_vertex,q_1.dag);

        start = clock();
        q_1.execute_graph();
        end = clock();
        if(!q_.execution_environment)
            cout<<"OpenCL - ";
        else
            cout<<"C++"<<endl;
        cout<<"aggregation time : "<<(double)end - start<<" ms"<<endl;

    }
}