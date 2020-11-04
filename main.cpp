//#include "cpp_layer/tests/test_cpp_execution.hpp"
//#include "opencl_layer/tests/test_openCL_execution.hpp"

//#include "task_layer/tests/test_task_execution.hpp"

//#include "runtime_layer/tests/test_graph.hpp"
//#include "runtime_layer/tests/test_exchange_graph.hpp"

#include "runtime_layer/tests/tpch_queries/query6.hpp"
//#include "runtime_layer/tests/tpch_queries/query1.hpp"

// RM includes
#include "src/include/hashing.hpp"
using namespace std;
#include <array>

#include <iostream>
//#include "task_layer/tests/test_task_execution.hpp"
#include "runtime_layer/tests/tpch_queries/queryHashJoin.hpp"
//#include "cpp_layer/tests/test_cpp_execution.cpp"
//#include "opencl_layer/tests/test_openCL_execution.cpp"

int main() {
// ***** - - - added code by RM - - - ******


   
    /*
    MainArray testArray;
    vector <int> data;
    for ( int i = 1; i < 11; i++)
    {
        data.push_back(i);
    }
    
    testArray.createArray(data.size(), data);
    */
   // MainArray testArray;
    vector <int> a;
    for ( int i = 1; i < 3073; i++)
    {
        a.push_back(i);
    }
    vector <int> b;
    for ( int i = 1; i < 2049; i++)
    {
        b.push_back(2*i);
    }
    /*time_t start = clock();
    testArray.createArray(data);
    testArray.createTableJoin(data);
    time_t end = clock();
    cout <<" time: "<<(double) end - start<<" ms"<<endl;*/
 // ******* End of RM tests *******

    /**
     * Simple example for running a single task over OpenCL and a task
     */
    //tests::test_heroDB_openCL();
    ///tests::test_heroDB_cpp();

    /**
     * Simple example for running a single task over CPP - parameter{1} or OpenCL - parameter{0}
     */

    //tests::test_heroDB_tasks(0);

    /**
     * Examples to run a sequence of operation. The ones below run 2-selection whose results are merged using logical-AND
     *
     *
     */
    //tests::test_heroDB_graphs(1,pow(2,28));
///    tests::test_heroDB_exchange_graphs(0,pow(2,28));




    /**
     * TPCH queries - runs complete query in GPU / CPU mentioned the device destination
     */
    //tests::query_6();
    tests::query_hashJoin(a, b);
///    tests::query_1();

    return 0;
}