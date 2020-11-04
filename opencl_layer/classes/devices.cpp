//
// Created by gurumurt on 28/04/20.
//

#include "devices.hpp"
namespace openCL_layer{

    devices::devices() {}

    // setter
    void devices::set_cmd_queue(std::vector<cl_command_queue> _cmd_queue) {
        mCmdQueue = _cmd_queue;
    }

    void devices::set_device_id(cl_device_id _device_id) {
        mDeviceId = _device_id;
    }

    void devices::set_device_name(std::string _name) {
        mDeviceName = _name;
    }

    void devices::set_max_work_group_size(cl_uint _max_wgs) {
        mMaxWorkGroupSize = _max_wgs;
    }


    // getter
    std::vector<cl_command_queue> devices::get_cmd_queue() {
        return mCmdQueue;
    }

    cl_device_id devices::get_device_id() {
        return mDeviceId;
    }

    std::string devices::get_device_name() {
        return mDeviceName;
    }

    cl_uint devices::get_max_work_group_size() {
        return mMaxWorkGroupSize;
    }

    devices::~devices() {}
}