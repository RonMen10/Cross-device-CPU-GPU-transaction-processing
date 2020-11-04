//
// Created by gurumurt on 28/04/20.
//

#include "openCL_globals.hpp"

namespace openCL_layer{

    std::vector<platforms> mPlatFormList;
    std::map<std::string, std::pair<uint, uint>> mDevices;
    std::vector<std::string> mDeviceNames;

    std::map<std::pair<std::string, cl_device_id>, cl_kernel> mKernelDictionary;//given a kernel name and device ID, returns the compiled kernel
    std::map<std::pair<int, cl_device_id>, cl_mem> mDataDictionary;// given a id and device ID, returns the data buffer

    cl_device_id get_device_id(uint PID, uint DID){

        return mPlatFormList[PID].mDeviceList[DID].get_device_id();
    }

    std::string get_device_name(uint PID, uint DID){

        for(std::pair<std::string, std::pair<uint, uint>> d : mDevices){
            if(d.second.first == PID && d.second.first == PID)
                return d.first;
        }
    }
    cl_event mE;

    double mCopyTime = 0.0;
    double mExecTime = 0.0;
}
