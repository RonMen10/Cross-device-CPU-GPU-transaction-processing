//
// Created by gurumurt on 29/04/20.
//

#include "runtime_globals.hpp"
#include <iomanip>

namespace runtime_layer{

    using namespace std;

    size_t total_data_set_in_device=0;
//    map<string,pair<uint,uint>> devices;

    vector<int*> results;
    size_t total_result_size=0;
    int get_alias(){

        return ++total_data_set_in_device;
    }

    void print_result(){

        for(int i=0;i<total_result_size;i++) {
            cout<<" | ";
            for(int* result_column : results){
                    cout<< setw(12)
                        << result_column[i]
                        <<" | ";
            }
            cout<<endl;
        }
        cout<<total_result_size<<" : total values"<<endl;
    }
}