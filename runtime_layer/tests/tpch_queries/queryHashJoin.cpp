#include "queryHashJoin.hpp"
#include <vector>



namespace tests{

    using namespace std;
    using namespace runtime_layer;
    //int num = 1024;

    void query_hashJoin(vector<int> a, vector<int> b){

        TPCH::initialize();

        data_scan.clear();
        results.clear();

       
        //Initialization of input
        //These arrays are just to test your input from the router goes here

        int vectorSize = a.size();
        const int castedValue = const_cast<const int&>(vectorSize);
        int* ar = new int[castedValue];
        int count = 0;
        for (int i = 0; i < vectorSize; i++){
            ar[i] = a[i];
            count += 1;
        }
        int buildArrSize = count;
        data_scan.insert(make_pair(1, make_pair(ar, buildArrSize)));
       
        // Probe array

        int vectorSize2 = b.size();
        const int castedValue2 = const_cast<const int&>(vectorSize2);
        int* ar2 = new int[castedValue2];
        int count2 = 0;
        for (int i = 0; i < vectorSize2; i++){
            ar2[i] = b[i];
            count2 += 1;
        }
        int probeArrSize = count2;

        data_scan.insert(make_pair(2, make_pair(ar2, probeArrSize)));
        
        //Parameters (sizes of arrays)
        auto c1 = types::Integer(buildArrSize);
        auto c2 = types::Integer(probeArrSize);
        
        task_layer::initialize_tasks();

        //set operation
        query_graph q_;
        primitive_node hashB, hashP;

        string target_device = openCL_layer::mDeviceNames[0];
        q_.execution_environment = 0;

        hashB.operation = task_layer::tasks[12];
        hashB.no_of_output_required =1;
        hashB.no_of_input_required =1;
        hashB.no_of_parameters = 1;
        hashB.add_parameter(c1.value);
        hashB.input_to_output_ratio = 32;
        hashB.operation.intent = 32;
        hashB.set_device(target_device);

        hashP.operation = tasks[3];
        hashP.no_of_output_required =1;
        hashP.no_of_input_required =2;
        hashP.no_of_parameters = 2;
        hashP.add_parameter(c1.value);
        hashP.add_parameter(c2.value);
        hashP.input_to_output_ratio = 32;
        hashP.operation.intent = 32;
        hashP.set_device(target_device);

        QEP_vertex s2 = add_vertex(hashB,q_.dag);
        QEP_vertex s3 = add_vertex(hashP,q_.dag);


        data_edge d;
        d.data_pointer = 1;
        add_edge(q_.start_vertex,s2,d,q_.dag);
        d.data_pointer = 2;
        add_edge(q_.start_vertex,s3,d,q_.dag);
        add_edge(s2,s3,q_.dag);
        add_edge(s3,q_.end_vertex,q_.dag); 


        time_t start = clock();
        q_.execute_graph();
        print_result();
        time_t end = clock();
        if(!q_.execution_environment)
            cout<<"OpenCL - " << endl;
        else
            cout<<"C++"<<endl;
        cout<<target_device<<" : "<<(double) end - start<<" ms"<<endl;
        cout<< "The size of hash build array is: " <<buildArrSize <<endl;
        cout<< "The size of hash probe array is: " <<probeArrSize <<endl;
       
    }
}