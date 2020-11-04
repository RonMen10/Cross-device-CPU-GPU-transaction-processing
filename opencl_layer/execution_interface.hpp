//
// Created by gurumurt on 28/04/20.
//

#ifndef HERO_EXECUTION_INTERFACE_HPP
#define HERO_EXECUTION_INTERFACE_HPP
#pragma once
#include "../headers.hpp"
#include "openCL_globals.hpp"

namespace openCL_layer{

    /*!
     * It helps to execute the kernel that are built previously along with all the arguments
     * @param _platformId The platform id containing the device
     * @param _deviceId The id of the device
     * @param _kernelName The unique name for the kernel
     * @param _args List of arguments needed for the for the kernel
     * @param _param List of constant values needed
     * @param _globalSize
     * @param _localSize
     * @param _addLocalBuffer
     * @param _addLocalBufferFactor
     * @param ev
     * @param _cmd_queue_loc The position of the command queue which is needed. Default value is 0
     */
//    template<typename T>
    inline void
    execute(int _platformId, int _deviceId, std::string _kernelName, std::vector<int> _args, std::vector<int> _param,
            size_t _globalSize, size_t _localSize = 1, short _addLocalBuffer = 0, short _addLocalBufferFactor = 1,
            cl_event *ev = &mE, uint _cmd_queue_loc = 0);

    /*!
     * Fetches the execution time of kernel
     * @return time for execution of last kernel
     */
    inline double get_execution_time();




    inline void extract_execution_time(cl_event *_ev = &mE);

    inline void
    execute(int _platformId, int _deviceId, std::string _kernelName, std::vector<int> _args, std::vector<int> _param,
            size_t _globalSize, size_t _localSize, short _addLocalBuffer, short _addLocalBufferFactor,
            cl_event *ev, uint _cmd_queue_loc) {

//        if(_localSize == 1 && _globalSize > 1024){
//            _localSize = 1024;
//        }

        if (mPlatFormList.size() >= _platformId && mPlatFormList[_platformId].mDeviceList.size() >= _deviceId) {

            cl_int _m_err = 0;

            // Fetch the details regarding the device
            cl_command_queue _m_device_queue = mPlatFormList[_platformId].mDeviceList[_deviceId].get_cmd_queue()[_cmd_queue_loc];
            cl_device_id _m_device = mPlatFormList[_platformId].mDeviceList[_deviceId].get_device_id();

            // Set the kernel arguments
            std::pair<std::string, cl_device_id> _m_pair = make_pair(_kernelName, _m_device);
            auto _m_iter = mKernelDictionary.find(
                    _m_pair);
            short _m_args_size = (short) _args.size();

            for (int i = 0; i < _m_args_size; i++) {

                std::pair<int, cl_device_id> _m_buf_pair = std::make_pair(_args[i], _m_device);
                cl_mem _m_argument_buffer = mDataDictionary.find(_m_buf_pair)->second;
                _m_err |= clSetKernelArg((*_m_iter).second, i, sizeof(cl_mem), &_m_argument_buffer);
                if (_m_err != CL_SUCCESS) {
                    std::cerr << ": Error setting kernel arguments (" << _m_err
                              << ")-"
                              << _kernelName
                              << " - arg "
                              << _args[i]
                              << std::endl;
                    mExecTime = 0;
                    return;
                }

            }

            //Add parameters
            short _m_param_size = (short) _param.size();
            for (int i = 0; i < _m_param_size; i++) {
                _m_err |= clSetKernelArg((*_m_iter).second, i + _m_args_size, sizeof(int), &_param[i]);
                if (_m_err != CL_SUCCESS) {
                    std::cerr << ": Error setting kernel parameter arguments (" << _m_err
                              << ")-"
                              << _kernelName
                              << " - "
                              << i + _m_args_size
                              << std::endl;
                    return;
                }
            }


            //Execute kernel
            _m_err = clEnqueueNDRangeKernel(_m_device_queue, (*_m_iter).second, 1, NULL, &_globalSize,
                                            (_localSize==1?NULL:&_localSize), 0, NULL, NULL);

            if (_m_err != CL_SUCCESS) {
                std::cerr << ": Error in execution of a kernel (" << _m_err
                          << ")-"
                          << _kernelName
                          //                 <<" - No. of Parameters : "
                          //                 << _m_args_size
                          //                 <<" - No. of values : "
                          //                 << _m_param_size
                          << std::endl;
                return;
            }

            clFinish(_m_device_queue);
            clFlush(_m_device_queue);

//            extract_execution_time();
        } else {
            std::cerr << "Invalid platform and device request" << std::endl;
        }
    }

    inline  double get_execution_time() {
        return mExecTime;
    }


    inline void extract_execution_time(cl_event *_ev) {
        cl_ulong time_start = 0;
        cl_ulong time_end = 0;

        clGetEventProfilingInfo(*_ev, CL_PROFILING_COMMAND_START, sizeof(time_start), &time_start, NULL);
        clGetEventProfilingInfo(*_ev, CL_PROFILING_COMMAND_END, sizeof(time_end), &time_end, NULL);
        mExecTime = time_end - time_start;
    }

}

#endif //HERO_EXECUTION_INTERFACE_HPP
