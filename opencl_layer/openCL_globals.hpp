//
// Created by gurumurt on 17/05/20.
//

#ifndef HERO_OPENCL_GLOBALS_HPP
#define HERO_OPENCL_GLOBALS_HPP

#include "../headers.hpp"
#include "classes/platforms.hpp"

namespace openCL_layer{

    extern std::vector<platforms> mPlatFormList;
    extern std::map<std::string, std::pair<uint, uint>> mDevices;
    extern std::vector<std::string> mDeviceNames;

    extern std::map<std::pair<std::string, cl_device_id>, cl_kernel> mKernelDictionary;//given a kernel name and device ID, returns the compiled kernel
    extern std::map<std::pair<int, cl_device_id>, cl_mem> mDataDictionary;// given a id and device ID, returns the data buffer

    extern  cl_device_id get_device_id(uint PID, uint DID);

    extern std::string get_device_name(uint PID, uint DID);

    extern double mCopyTime;
    extern cl_event mE;
    extern double mExecTime;
}

#endif //HERO_OPENCL_GLOBALS_HPP
