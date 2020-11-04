//
// Created by gurumurt on 28/04/20.
//

#ifndef HERO_PLATFORMS_HPP
#define HERO_PLATFORMS_HPP
#pragma once
#include "devices.hpp"

namespace openCL_layer{

    /**
     * This class holds the platforms that are accessible via OpenCL. A platfrom can have along with other information, more than one device to execute.
     */
    class platforms {

    public:
        cl_context mContext;                /// Calling context for the device
        cl_platform_id mPlatFormId;         /// ID of the platform
        std::string mPlatformName;          /// Name of the platform for easy access
        std::vector<devices> mDeviceList;   /// All the devices available within the platform


        platforms();

        // setter
        void set_context(cl_context _context);

        void set_platform_id(cl_platform_id _platform_id);

        void set_platform_name(std::string _name);

        void set_devices(std::vector<devices> _device_list);

        ~platforms();

    };
}
#endif //HERO_PLATFORMS_HPP
