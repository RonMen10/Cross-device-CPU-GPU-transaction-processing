//
// Created by gurumurt on 16/05/20.
//

#include "primitives.hpp"

namespace cpp_layer{

    /**
     * Selection operations
     * @param args
     * @param parameters
     * @param size
     */
    void selection_between_block( std::vector<int*> args, std::vector<int> parameters, size_t size) {

        for (int j = 0; j < size; ++j) {

            size_t pos = 32 * j;
            unsigned int bm = 0;
            for (unsigned int i=0; i<32; ++i){
                    bm |= ((parameters[0] <= args[0][pos])&&(parameters[1] < args[0][pos])  )? 0x1 << i : 0;
                    pos++;
            }
            args[1][j] = bm;
        }
    }

    void selection_less_than_block( std::vector<int*> args, std::vector<int> parameters, size_t size) {

        for (int j = 0; j < size; ++j) {

            size_t pos = 32 * j;
            unsigned int bm = 0;
            for (unsigned int i=0; i<32; ++i){
                    bm |= (args[0][pos] < parameters[0]) ? 0x1 << i : 0;
                    pos++;
            }
            args[1][j] = bm;
        }
    }

    void selection_greater_than_block( std::vector<int*> args, std::vector<int> parameters, size_t size) {

        for (int j = 0; j < size; ++j) {

            size_t pos = 32 * j;
            unsigned int bm = 0;
            for (unsigned int i=0; i<32; ++i){
                    bm |= (parameters[0] <= args[0][pos])? 0x1 << i : 0;
                    pos++;
            }
            args[1][j] = bm;
        }
    }

    /**
     * logical block
     */
    void logical_block(std::vector<int*> args, std::vector<int> parameters, size_t size) {

        for (int i = 0; i < size; ++i) {

            args[2][i] = args[0][i] & args[1][i] ;
        }

    }

    void logical_block_or(std::vector<int*> args, std::vector<int> parameters, size_t size) {

        for (int i = 0; i < size; ++i) {

            args[2][i] = args[0][i] | args[1][i] ;
        }

    }

    /**
     * materialize
     * @param input
     * @param bitmap
     * @param result
     */
    void materialize(std::vector<int*> args, std::vector<int> parameters, size_t size) {

        for (int j = 0; j < size; ++j) {

            size_t pos = 32 * j;
            for (unsigned int i = 0; i < 32; ++i) {
                args[2][pos + i] = ((args[1][j] & (1 << i)) >> i) * args[0][pos + i];
            }
        }
    }

    void partial_prefix_sum(std::vector<int*> args, std::vector<int> parameters, size_t size){

        args[1][0] = 0;
        std::cout<<"running prefix-sum"<<std::endl;
        for (int j = 1; j < size; ++j) {

            args[1][j] = args[1][j-1] + (args[0][j-1]==args[0][j]);
        }
    }

    void ordered_aggregate_count_block(  std::vector<int*> args, std::vector<int> parameters, size_t size) {

        for (int i = 0; i < size; ++i) {

            args[1][args[0][i]] +=1;
        }
    }

    void ordered_aggregate_sum_block(  std::vector<int*> args, std::vector<int> parameters, size_t size) {

        for (int i = 0; i < size; ++i) {

            args[2][args[0][i]] +=args[1][i];
        }
    }

    void arith_sum(std::vector<int*> args, std::vector<int> parameters, size_t size) {

        for (int i = 0; i < size; ++i) {

            args[2][i] = args[0][i] + args[1][i];
        }
    }

    void arith_mul(std::vector<int*> args, std::vector<int> parameters, size_t size) {

        for (int i = 0; i < size; ++i) {

            args[2][i] = args[0][i] * args[1][i];
        }
    }

    void aggregate_block(std::vector<int*> args, std::vector<int> parameters, size_t size) {

        for (int j = 0; j < size; ++j) {

            int agg = 0;
            size_t pos = 32 * j;
            for (unsigned int i=0;i < 32; i++){
                agg +=  args[0][pos + i];
            }

            args[1][0] += agg;
        }
    }
    
}
