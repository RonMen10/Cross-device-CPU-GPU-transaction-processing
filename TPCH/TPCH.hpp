//
// Created by Bala on 21/04/20.
//

#ifndef DISPATCHER_TPCH_HPP
#define DISPATCHER_TPCH_HPP

#pragma once

#include "common/runtime/Database.hpp"
#include "common/runtime/Import.hpp"
#include "common/runtime/Types.hpp"

#include <boost/algorithm/string.hpp>


namespace benchmark {

    using namespace std;

    class TPCH {

        runtime::Database tpch;
        static int columnID;

    public:

        TPCH() {

            if(TPCHlocation.empty())
                TPCHlocation = "/Users/ronald/Desktop/DBSE---Project/CODD-pro-master/data/tpch/sf1"; // workaround to provide location
            runtime::importTPCH(TPCHlocation, this->tpch);

        }


        int* get_data(string TPCHtable, string TPCHcol) {

            vector<string> str;
            boost::split(str, TPCHcol, boost::is_any_of("."));

            return this->tpch[TPCHtable][TPCHcol].data<int>();
        }


        //Currently only works with integer values and this has to be changed in future work
        //Dev has to be changed from short to cl_device_id values in later stages
        /**
         * Funtion to load TPCH data directly into target device
         * @param TPCHcol - <Table_name>.<Column_name> of the column to be loaded
         * @param dev - ID of the target device
         * @param partialSize - This is optional variable that loads only a partial size of the column
         * @return - ID for the column inserted to be referenced while processing
         */

        int prepareBuffer(string TPCHcol, short platformID=0, short dev = 0, size_t partialSize = 0) {

            vector<string> str;
            boost::split(str, TPCHcol, boost::is_any_of("."));

            int *col = this->tpch[str[0]][str[1]].data<int>();
            size_t size = this->tpch[str[0]][str[1]].data_.size() - partialSize;

            if (partialSize) {
                size = partialSize;
            }

            return columnID;
        }

        /**
         * Get the total size of TPCH data pointed
         * @return data_size
         */
        int get_size(string TPCHtable, string TPCHcol){

            return this->tpch[TPCHtable][TPCHcol].data_.size();
        }

    };
}
#endif //DISPATCHER_TPCH_HPP
