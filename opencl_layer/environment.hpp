//
// Created by gurumurt on 28/04/20.
//

#ifndef HERO_ENVIRONMENT_HPP
#define HERO_ENVIRONMENT_HPP

#pragma once

#include "../headers.hpp"
#include "openCL_globals.hpp"

namespace openCL_layer {

    /*!
    * This function helps to extract all the necessary information about the devices that are available.
    */
    inline void setup_environment() {

        cl_uint platform_count;

        // Get the platforms
        cl_int err = clGetPlatformIDs(0, NULL, &platform_count);
        if (err) {
            std::cout << "Error on fetching Platforms information- (" << err << ")" << std::endl;
        } else {
            cl_platform_id *platform = (cl_platform_id *) malloc(sizeof(cl_platform_id) * platform_count);

            // Creating context
            cl_context *context = (cl_context *) malloc(sizeof(cl_context) * platform_count);

            // Allocate space for devices
            err = clGetPlatformIDs(platform_count, platform, NULL);
            if (err) {
                std::cout << "Error on allocating space for devices- (" << err << ")" << std::endl;

            } else {
                cl_device_id **device = new cl_device_id *[platform_count];
                cl_uint *device_local_count = new cl_uint[platform_count];

                // Get device data
                for (uint i = 0; i < platform_count; i++) {

                    // Getting the number of devices in the current Platforms
                    clGetDeviceIDs(platform[i], CL_DEVICE_TYPE_ALL, NULL, NULL, &device_local_count[i]);

                    // Getting the devices
                    device[i] = new cl_device_id[device_local_count[i]];
                    clGetDeviceIDs(platform[i], CL_DEVICE_TYPE_ALL, device_local_count[i], device[i], NULL);

                    // Creating context for single Platforms
                    context[i] = clCreateContext(NULL, device_local_count[i], device[i], NULL, NULL, &err);

                    // platform name
                    char _p_name[1024];
                    clGetPlatformInfo(platform[i], CL_PLATFORM_NAME, 1024, &_p_name, NULL);

                    // Set Platform values
                    platforms _platforms;
                    _platforms.set_context(context[i]);
                    _platforms.set_platform_id(platform[i]);
                    _platforms.set_platform_name(_p_name);

                    // get key for mapping devices
                    std::string _key = (_platforms.mPlatformName.find("CPU") != std::string::npos) ? "CPU_"
                                                                                                         : "GPU_";
                    // Create device list
                    std::vector<devices> _deviceList;

                    // Command queue for each devices
                    for (uint j = 0; j < device_local_count[i]; j++) {

                        std::vector<cl_command_queue> _cmd_queue;

                        // device name
                        char _d_name[1024];
                        clGetDeviceInfo(device[i][j], CL_DEVICE_NAME, 1024, &_d_name, NULL);

                        _cmd_queue.push_back(clCreateCommandQueue(context[i], device[i][j],
                                                                  CL_QUEUE_PROFILING_ENABLE, &err));
                        cl_uint _max_wgs;
                        clGetDeviceInfo(device[i][j], CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(cl_uint), &_max_wgs, NULL);

                        devices _devices;
                        _devices.set_cmd_queue(_cmd_queue);
                        _devices.set_device_id(device[i][j]);
                        _devices.set_device_name(_d_name);
                        _devices.set_max_work_group_size(_max_wgs);

                        // add to the device list
                        _deviceList.push_back(_devices);

                        // make the map of devices
                        _key += std::to_string(j + 1);
                        mDevices.insert(std::make_pair(_key, std::make_pair(i, j)));
                        mDeviceNames.push_back(_key);
                    }

                    // add to the platform
                    _platforms.set_devices(_deviceList);

                    // add to the final platform list
                    mPlatFormList.push_back(_platforms);
                }
            }
        }
    }

    /*!
     * Just prints the available devices for end users.
     */
    inline void print_environment() {

        if (mPlatFormList.size()) {

            std::cout << "---------------------------" << std::endl;
            for (int _iterP = 0; _iterP < mPlatFormList.size(); _iterP++) {
                platforms _pf = mPlatFormList[_iterP];
                std::cout << "PLATFORM_NAME :: " << _pf.mPlatformName << " -- " << _iterP << std::endl;

                for (int _iterD = 0; _iterD < _pf.mDeviceList.size(); _iterD++) {
                    devices _dev = _pf.mDeviceList[_iterD];
                    std::cout << "\tDEVICE_NAME :: " << _dev.get_device_name() << " -- " << _iterD << std::endl;
                }
            }
            std::cout << "---------------------------" << std::endl;

        } else {
            std::cout << "No platforms fetched!" << std::endl;
        }
    }

    /*!
     * This function returns a list of all available devices with all their information.
     * @return vector of platforms
     */
    inline  std::vector<platforms> get_environment() {
        return mPlatFormList;
    }

    /*!
     * Provides a list of available device names
     * @return vector of device name strings
     */
    inline std::vector<std::string> get_device_names() {
        return mDeviceNames;
    }

    /*!
     * This helps in providing the platform and device id's for the given device name
     * @param _dev_name A device name from the available list
     * @return a pair of platform id and device id
     */
    inline std::pair<uint, uint> get_id(std::string _dev_name) {
        std::map<std::string, std::pair<uint, uint>>::iterator _iter = mDevices.find(_dev_name);

        if (_iter != mDevices.end()) {
            return (*_iter).second;
        } else {
            std::cerr << "Invalid device name!!" << std::endl;
            exit(0);
        }
    }
}

#endif //HERO_ENVIRONMENT_HPP
