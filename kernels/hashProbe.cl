__kernel void hashProbe(__global unsigned int *inpProbe, __global unsigned int *inpBuild, __global unsigned int *result, unsigned int buildArrSize, unsigned int probeArrSize) {
    
    size_t d = (probeArrSize/64) * get_global_id(0);
        for(unsigned int i=0; i < (probeArrSize/64); i++){
            for(unsigned int j=0; j < buildArrSize;){
                if( inpBuild[j] == inpProbe[i + d]){
                    result[i+d] = j;
                    break;
                }else if(j < (buildArrSize - 1)){
                    j += 1;
                }else{
                    result[i+d] = -1;
                    break;
                }
            }
        }
  
}