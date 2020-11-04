__kernel void arith_sum(__global unsigned int *input,__global unsigned int* data2,   __global unsigned int* res) {

        res[get_global_id(0)] = input[get_global_id(0)] + data2[get_global_id(0)];
}