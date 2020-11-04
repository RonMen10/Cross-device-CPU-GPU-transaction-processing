//
// Created by gurumurt on 28/04/20.
//

#ifndef HERO_DATA_INTERFACE_HPP
#define HERO_DATA_INTERFACE_HPP
#pragma once
#include "../headers.hpp"
#include "openCL_globals.hpp"

namespace openCL_layer {



//        std::map<std::pair<int, cl_device_id>, cl_mem> mDataDictionary;


    /*!
     * Function to delete data buffer in a device
     * @param name name of the buffer
     * @param source device location of the buffer
     */
    inline void delete_data(int _name, cl_device_id _source) {

        if (!mDataDictionary.count(std::make_pair(_name, _source)))
            return;

        clReleaseMemObject(mDataDictionary.find(std::make_pair(_name, _source))->second);

        //Remove from data dictionary
        std::map<std::pair<int, cl_device_id>, cl_mem>::iterator _iter;
        for (_iter = mDataDictionary.begin(); _iter != mDataDictionary.end(); _iter++) {
            if ((_iter->first) == std::make_pair(_name, _source)) {
                mDataDictionary.erase(_iter);
                break;
            }
        }
    }

    /**
     * Function to add a data array into target device
     * @tparam T Template for input data type (currently we use only integers however)
     * @param _alias a reference ID for the data to access it in runtime
     * @param _data pointer to data
     * @param _platformId target platform
     * @param _deviceId target device inside the platform target
     * @param _size size of the input feed into device
     * @param _m for adding local memory space (currently in development and is kept as 1 for default)
     * @param _cmd_queue_loc in-case more than one command queue are available for a device, the cmd-Q idx for adding data can be passed. Default : 0
     * @return 1 if sucess, else 0
     */
    template<typename T>
    inline  int add_data(int _alias, T *_data, int _platformId, int _deviceId, size_t _size, size_t _m = 1,
                 uint _cmd_queue_loc = 0) {

        if (mPlatFormList.empty()) {

            std::cerr << "No list of platforms is available. Please set it first using set_platform_list()."
                      << std::endl;
            return 0;

        } else {

            cl_mem _m_data_buffer;
            cl_int _m_err;

            // Fetch the details regarding the device
            cl_context _m_context = mPlatFormList[_platformId].mContext;
            cl_command_queue _m_device_queue = mPlatFormList[_platformId].mDeviceList[_deviceId].get_cmd_queue()[_cmd_queue_loc];
            cl_device_id _m_dev = mPlatFormList[_platformId].mDeviceList[_deviceId].get_device_id();
            // Create data buffer on device


            _m_data_buffer = clCreateBuffer(_m_context, NULL, (_size + 1) * _m * sizeof(T), NULL,
                                            &_m_err);

            clFinish(_m_device_queue);
            clFlush(_m_device_queue);

            if (_m_err != CL_SUCCESS) {
                std::cerr << "Error on creating buffer- " << _alias << " ( " << _m_err << " )" << std::endl;
                return 0;
            }

            cl_event _m_write_event;
            //Add data into the device
            _m_err = clEnqueueWriteBuffer(_m_device_queue, _m_data_buffer, CL_TRUE, 0, _size * sizeof(T),
                                          _data, 0, NULL, &_m_write_event);
            if (_m_err != CL_SUCCESS) {
                std::cerr << "Error on Write buffers- " << _alias << " ( " << _m_err << " )" << std::endl;
                return 0;
            }

            clFinish(_m_device_queue);
            clFlush(_m_device_queue);

            cl_ulong _m_time_start = 0;
            cl_ulong _m_time_end = 0;

            clGetEventProfilingInfo(_m_write_event, CL_PROFILING_COMMAND_START, sizeof(_m_time_start), &_m_time_start,
                                    NULL);
            clGetEventProfilingInfo(_m_write_event, CL_PROFILING_COMMAND_END, sizeof(_m_time_end), &_m_time_end, NULL);
            mCopyTime = _m_time_end - _m_time_start;

            mDataDictionary.insert(std::make_pair(std::make_pair(_alias, _m_dev), _m_data_buffer));
            return 1;
        }
    }
/**
 * Function to add an empty array into target device
 * @tparam T Template for input data type (currently we use only integers however)
 * @param _alias a reference ID for the data to access it in runtime
 * @param _platformId target platform
 * @param _deviceId target device inside the platform target
 * @param _size size of the input feed into device
 * @param _m for adding local memory space (currently in development and is kept as 1 for default)
 * @param _cmd_queue_loc in-case more than one command queue are available for a device, the cmd-Q idx for adding data can be passed. Default : 0
 */
    template<typename T>
    inline void add_new_data(int _alias, int _platformId, int _deviceId, size_t _size, size_t _m, uint _cmd_queue_loc) {

         

        T *_m_new_data_pointer = (T *) calloc(_size, sizeof(unsigned int));
        add_data(_alias, _m_new_data_pointer, _platformId, _deviceId, _size, _cmd_queue_loc);
    }

    /**
     * Function to lookup the currently available data aliases
     */
    inline void lookup_data_buffer() {

        for (auto it = mDataDictionary.cbegin(); it != mDataDictionary.cend(); ++it) {
            char _m_name[100];
            clGetDeviceInfo(it->first.second, CL_DEVICE_NAME, 100, &_m_name, NULL);
            std::cout << it->first.first << "-->" << _m_name << std::endl;
        }
    }

    /**
     * Prints data pointed by alias in the target device
     * @param _name Alias of the data
     * @param _platformId target platform
     * @param _deviceId target device in target platform
     * @param _dataSize Size of data to fetch and print
     * @param _flagBits optional : to print data in its bit format
     * @param _cmd_queue_loc optional : to get data from a specified cmd-Q
     */
    inline void print_data(int _name, int _platformId, int _deviceId, size_t _dataSize, bool _flagBits=false, uint _cmd_queue_loc=0) {

         

        if (mPlatFormList.empty()) {

            std::cerr << "No list of platforms is available. Please set it first using set_platform_list()."
                      << std::endl;

        } else {
            cl_int _m_err = 0;

            // Fetch the details regarding the device
            cl_command_queue _m_device_queue = mPlatFormList[_platformId].mDeviceList[_deviceId].get_cmd_queue()[_cmd_queue_loc];
            cl_device_id _m_dev = mPlatFormList[_platformId].mDeviceList[_deviceId].get_device_id();


            cl_mem _m_argument_buffer = mDataDictionary.find(std::make_pair(_name, _m_dev))->second;

            cl_event _m_read_event;

            unsigned int *_m_result = new unsigned int[_dataSize];
            _m_err = clEnqueueReadBuffer(_m_device_queue, _m_argument_buffer, CL_TRUE, 0,
                                         _dataSize * sizeof(unsigned int),
                                         _m_result, 0, NULL, &_m_read_event);

            clFinish(_m_device_queue);
            clFlush(_m_device_queue);

            if (_flagBits) {
                for (int i = 0; i < _dataSize; i++) {
                    std::bitset<32> x(_m_result[i]);
                    std::cout << x << "\t";
                }
            } else {
                for (int i = 0; i < _dataSize; i++) {
                    std::cout << (int) _m_result[i] << "\t";
                }
            }
        }
    }

    /**
     * retrieve data from OpenCL to C++ pointer
     * @param _name alias of data
     * @param _platformId target platform
     * @param _deviceId target device in target platform
     * @param _dataSize size of data to fetch from device
     * @param _cmd_queue_loc
     * @return pointer to starting position of fetched data
     */
    inline unsigned int *
    get_data(int _name, int _platformId, int _deviceId, size_t _dataSize, uint _cmd_queue_loc=0) {

         

        unsigned int *_m_result_data;

        if (mPlatFormList.empty()) {

            std::cerr << "No list of platforms is available. Please set it first using set_platform_list()."
                      << std::endl;
        } else {
            cl_int _m_err = 0;

            // Fetch the details regarding the device
            // cl_context _m_context = mPlatFormList[_platformId].get_context();
            cl_command_queue _m_device_queue = mPlatFormList[_platformId].mDeviceList[_deviceId].get_cmd_queue()[_cmd_queue_loc];
            cl_device_id _m_dev = mPlatFormList[_platformId].mDeviceList[_deviceId].get_device_id();

            cl_mem _m_argument_buffer = mDataDictionary.find(std::make_pair(_name, _m_dev))->second;

            _m_result_data = (unsigned int *) calloc((unsigned int) _dataSize, sizeof(unsigned int));
            _m_err = clEnqueueReadBuffer(_m_device_queue, _m_argument_buffer, CL_TRUE, 0,
                                         _dataSize * sizeof(unsigned int),
                                         _m_result_data, 0, NULL, NULL);
            clFinish(_m_device_queue);
            clFlush(_m_device_queue);
        }
        return _m_result_data;
    }

    inline unsigned int get_first_data(int _name, int _platformId, int _deviceId, uint _cmd_queue_loc) {

         

        unsigned int _m_result_data;
        cl_event _m_read_event;

        if (mPlatFormList.empty()) {

            std::cerr << "No list of platforms is available. Please set it first using set_platform_list()."
                      << std::endl;
        } else {
            cl_int _m_err = 0;

            // Fetch the details regarding the device
            cl_command_queue _m_device_queue = mPlatFormList[_platformId].mDeviceList[_deviceId].get_cmd_queue()[_cmd_queue_loc];
            cl_device_id _m_dev = mPlatFormList[_platformId].mDeviceList[_deviceId].get_device_id();

            cl_mem _m_argument_buffer = mDataDictionary.find(std::make_pair(_name, _m_dev))->second;
            _m_err = clEnqueueReadBuffer(_m_device_queue, _m_argument_buffer, CL_TRUE, 0, sizeof(unsigned int),
                                         &_m_result_data, 0, NULL, &_m_read_event);

            clFinish(_m_device_queue);
            clFlush(_m_device_queue);
            clWaitForEvents(1, &_m_read_event);
        }

        return _m_result_data;
    }

    inline unsigned int get_last_data(int _name, int _platformId, int _deviceId, size_t _dataSize, uint _cmd_queue_loc) {

         

        unsigned int _m_result_data;
        cl_event _m_read_event;

        if (mPlatFormList.empty()) {

            std::cerr << "No list of platforms is available. Please set it first using set_platform_list()."
                      << std::endl;
        } else {
            cl_int _m_err = 0;

            // Fetch the details regarding the device
            cl_command_queue _m_device_queue = mPlatFormList[_platformId].mDeviceList[_deviceId].get_cmd_queue()[_cmd_queue_loc];
            cl_device_id _m_dev = mPlatFormList[_platformId].mDeviceList[_deviceId].get_device_id();

            cl_mem _m_argument_buffer = mDataDictionary.find(std::make_pair(_name, _m_dev))->second;
            _m_err = clEnqueueReadBuffer(_m_device_queue, _m_argument_buffer, CL_TRUE,
                                         ((int) _dataSize - 1) * sizeof(unsigned int), sizeof(unsigned int),
                                         &_m_result_data, 0, NULL,
                                         &_m_read_event);

            clFinish(_m_device_queue);
            clFlush(_m_device_queue);
            clWaitForEvents(1, &_m_read_event);
        }

        return _m_result_data;
    }

    /**
     * transfer data from one device to another
     * @param _name alias of data
     * @param _srcPlatformId source platform
     * @param _srcDeviceId device in source platform
     * @param _tarPlatformId target platform
     * @param _tarDeviceId target device in target platform
     * @param _size size of data to transfer
     * @param _cmd_queue_loc optional
     */
    inline void transfer_data(int _name, int _srcPlatformId, int _srcDeviceId, int _tarPlatformId, int _tarDeviceId,
                       size_t _size, uint _cmd_queue_loc=0) {

        if (mPlatFormList.empty()) {

            std::cerr << "No list of platforms is available. Please set it first using set_platform_list()."
                      << std::endl;
        } else {

            cl_int _m_err;

            // Fetch the device id's
            cl_command_queue _m_src_dev_queue = mPlatFormList[_srcPlatformId].mDeviceList[_srcDeviceId].get_cmd_queue()[_cmd_queue_loc];
            cl_device_id _m_source = mPlatFormList[_srcPlatformId].mDeviceList[_srcDeviceId].get_device_id();
            cl_device_id _m_target = mPlatFormList[_tarPlatformId].mDeviceList[_tarDeviceId].get_device_id();

            auto _m_result = get_data(_name,_srcPlatformId,_srcDeviceId,_size);

            //Copy data to target
            add_data(_name, _m_result, _tarPlatformId, _tarDeviceId, _size);
            delete_data(_name, _m_source);
            delete _m_result;
        }
    }


    inline std::map<std::pair<int, cl_device_id>, cl_mem> get_data_dictionary() {
        return mDataDictionary;
    }

    /*!
     * Clears all data buffers
     */
    inline void clear_data() {

        std::map<std::pair<int, cl_device_id>, cl_mem>::iterator _iter;
        for (_iter = mDataDictionary.begin(); _iter != mDataDictionary.end(); _iter++) {

            delete_data(_iter->first.first, _iter->first.second);
        }

        mDataDictionary.clear();
    }
}

#endif //HERO_DATA_INTERFACE_HPP
