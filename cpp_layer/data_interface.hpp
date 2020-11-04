//
// Created by gurumurt on 28/04/20.
//

#ifndef HERO_DATA_CPP_INTERFACE_HPP
#define HERO_DATA_CPP_INTERFACE_HPP
#pragma once
#include "../headers.hpp"
#include "cpp_globals.hpp"

namespace cpp_layer {



//        std::map<std::pair<int, cl_device_id>, cl_mem> mDataDictionary;

    /*!
     * Function to delete data buffer
     * @param name name of the buffer
     */
    inline void delete_data(int _name) {

        if(!mDataDictionary.count(_name))
            return;

        delete mDataDictionary[_name];
        mDataDictionary.erase(_name);
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
    inline  int add_data(int _alias, T *_data) {

        mDataDictionary[_alias] = _data;
        return 1;
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
    inline void add_new_data(int _alias, size_t _size) {

        T *_m_new_data_pointer = (T *) calloc(_size, sizeof(unsigned int));
        mDataDictionary[_alias] =_m_new_data_pointer;
    }

    /**
     * Function to lookup the currently available data aliases
     */
    inline void lookup_data_buffer() {

        for (auto it = mDataDictionary.cbegin(); it != mDataDictionary.cend(); ++it) {
            std::cout << it->first<< std::endl;
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
    inline void print_data(int _name, size_t _dataSize, bool _flagBits=false) {

        int *_m_result = (int*)mDataDictionary[_name];

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
    get_data(int _name, size_t _dataSize) {

        unsigned int* _m_result_data = (unsigned int *) calloc((unsigned int) _dataSize, sizeof(unsigned int));
        std::copy((int*)mDataDictionary[_name],(int*)mDataDictionary[_name]+_dataSize,_m_result_data);

        return _m_result_data;
    }

    inline unsigned int get_first_data(int _name, int _platformId, int _deviceId, uint _cmd_queue_loc) {

        return 1;
    }

    inline unsigned int get_last_data(int _name, int _platformId, int _deviceId, size_t _dataSize, uint _cmd_queue_loc) {

        return 1;
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
    inline void transfer_data(int _name) {

    }


    inline std::map<int, void*> get_data_dictionary() {
        return mDataDictionary;
    }

    /*!
     * Clears all data buffers
     */
    inline void clear_data() {

        std::map<int, void*>::iterator _iter;
        for (_iter = mDataDictionary.begin(); _iter != mDataDictionary.end(); _iter++) {

            delete _iter->second;
        }

        mDataDictionary.clear();
    }
}

#endif //HERO_DATA_CPP_INTERFACE_HPP
