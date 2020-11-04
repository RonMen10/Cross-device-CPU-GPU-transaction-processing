__kernel void ordered_aggregate_count_block(  __global unsigned int *posList,
                                __global unsigned int* result) {

    //does summation of value in position input over results using input
    atomic_add(&result[posList[get_global_id(0)]],1);
}