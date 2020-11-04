//
// Created by gurumurt on 28/04/20.
//

#ifndef HERO_DEVICES_HPP
#define HERO_DEVICES_HPP
#pragma once
#include "../../headers.hpp"
namespace openCL_layer{

    /**
     * This class holds the information related to a device in OpenCL
     *
     */
    class devices{

    public:
        std::vector<cl_command_queue> mCmdQueue; /// command queue through which we can send an operation to the device - an operation can be data push/pull or kernel execution
        cl_device_id mDeviceId;                  /// ID of the target device
        std::string mDeviceName;                 /// Reference name for the device to access them easily
        cl_uint mMaxWorkGroupSize;               /// number of threads that can run concurrently in a device


        devices();

        // setter
        void set_cmd_queue(std::vector<cl_command_queue> _cmd_queue);

        void set_device_id(cl_device_id _device_id);

        void set_device_name(std::string _name);

        void set_max_work_group_size(cl_uint _max_wgs);

        // getter
        std::vector<cl_command_queue> get_cmd_queue();

        cl_device_id get_device_id();

        std::string get_device_name();

        cl_uint get_max_work_group_size();

        ~devices();
    };

}
#endif //HERO_DEVICES_HPP
