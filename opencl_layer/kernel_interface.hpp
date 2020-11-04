//
// Created by gurumurt on 28/04/20.
//

#ifndef HERO_KERNEL_INTERFACE_HPP
#define HERO_KERNEL_INTERFACE_HPP

#pragma once
#include "../headers.hpp"
#include "openCL_globals.hpp"

namespace openCL_layer {

//        std::map<std::pair<std::string, cl_device_id>, cl_kernel> mKernelDictionary;

    inline void delete_kernel(std::string _kernel_name, cl_device_id _device);

    inline  const char *get_char(std::string _val);

    inline cl_kernel
        build_kernel(cl_context _context, cl_device_id _device, const char *_kernelName, const char *_kernelSrc,
                     std::string _cmdAgrs);
    /*!
     * It helps in building a kernel in the required device
     * @param _platformId The platform id containing the device
     * @param _deviceId The device id
     * @param _kernelName Unique name for the kernel and should be same as in the kernel source code
     * @param _kernelSrc The source code of the kernel to be created
     * @param _cmdAgrs THe extra param values that needed
     */
    inline void
    add_kernel(int _platformId, int _deviceId, std::string _kernelName, std::string _kernelSrc, std::string _cmdAgrs) {

        if (mPlatFormList.size() >= _platformId && mPlatFormList[_platformId].mDeviceList.size() >= _deviceId) {

            // Fetch the details regarding the device
            cl_context _m_context = mPlatFormList[_platformId].mContext;
            cl_device_id _m_device = mPlatFormList[_platformId].mDeviceList[_deviceId].get_device_id();

            if(mKernelDictionary.count(make_pair(_kernelName, _m_device))) {
                delete_kernel(_kernelName, _m_device);
            }

            // build kernel withe the available information
            cl_kernel _m_cl_kernel = build_kernel(_m_context, _m_device, get_char(_kernelName),
                                                  get_char(_kernelSrc), _cmdAgrs);

            if(_m_cl_kernel != NULL) {
                // add to the kernel dictionary
                mKernelDictionary.insert(make_pair(make_pair(_kernelName, _m_device), _m_cl_kernel));

            } else {
                std::cerr << "Kernel building failed!! - " << _kernelName << std::endl;
            }

        } else {
            std::cerr << "Invalid platform and device request" << std::endl;
        }
    }

    /*
     * Private functions
     */

    inline  void delete_kernel(std::string _kernel_name, cl_device_id _device) {

        std::pair<std::string, cl_device_id> _m_pair = make_pair(_kernel_name, _device);
        cl_kernel kernel = mKernelDictionary.find(_m_pair)->second;
        clReleaseKernel(kernel);

        std::map<std::pair<std::string, cl_device_id>, cl_kernel>::iterator _iter;
        _iter = mKernelDictionary.find(_m_pair);

        if(_iter != mKernelDictionary.end()) {
            mKernelDictionary.erase(_iter);
        }
    }

    inline const char *get_char(std::string _val) {
        char *_m_val = new char[_val.length() + 1];
        strcpy(_m_val, _val.c_str());
        return _m_val;
    }

    inline cl_kernel build_kernel(cl_context _context, cl_device_id _device, const char *_kernelName,
                                       const char *_kernelSrc, std::string _cmdAgrs) {

        cl_program _m_program = clCreateProgramWithSource(_context, 1, &_kernelSrc, NULL, NULL);
        cl_int _m_err = clBuildProgram(_m_program, 1, &_device, _cmdAgrs.c_str(), NULL, NULL);

        if (_m_err != CL_SUCCESS) {
            size_t _m_log_size;
            clGetProgramBuildInfo(_m_program, _device, CL_PROGRAM_BUILD_LOG, 0, NULL, &_m_log_size);
            char _m_log[_m_log_size];
            clGetProgramBuildInfo(_m_program, _device, CL_PROGRAM_BUILD_LOG, _m_log_size, _m_log, NULL);
            std::cerr << ": Error building kernel " << _kernelName << "." << "( " << _m_err << " )" << std::endl;
            std::cerr << _m_log << std::endl;
            return NULL;
        }

        cl_kernel _m_kernel = clCreateKernel(_m_program, _kernelName, &_m_err);

        return _m_kernel;
    }
}

#endif //HERO_KERNEL_INTERFACE_HPP
