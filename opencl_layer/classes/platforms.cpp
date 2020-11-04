//
// Created by gurumurt on 28/04/20.
//

#include "platforms.hpp"
namespace openCL_layer {

    platforms::platforms() {}

        // setter
        void platforms::set_context(cl_context _context) {
            platforms::mContext = _context;
        }

        void platforms::set_platform_id(cl_platform_id _platform_id) {
            platforms::mPlatFormId = _platform_id;
        }

        void platforms::set_platform_name(std::string _name) {
            platforms::mPlatformName = _name;
        }

        void platforms::set_devices(std::vector<devices> _device_list) {
            platforms::mDeviceList = _device_list;
        }

        platforms::~platforms(){}
}