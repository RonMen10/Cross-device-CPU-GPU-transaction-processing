//
// Created by gurumurt on 28/04/20.
//

#ifndef HERO_KERNEL_PACK_HPP
#define HERO_KERNEL_PACK_HPP

#include "../../headers.hpp"
#include "../../opencl_layer/kernel_interface.hpp"

namespace task_layer{
    using namespace std;

    /**
     * Class to combine all kernel information
     */
    class kernelPack{

    public:
        uint mPId, mDId; /// device related index
        std::string mKernelName; ///kernel name
        std::string mKernelSrc; /// kernel function in string
        std::string mCmdArgs;   ///additional arguments to be included during compilation, these args can also be included as external paramters while execution

        // number of work item

        kernelPack();
        kernelPack(string name, string src,string cmdArgs,int mPId,int mDId);

        //compile inside openCL
        /**
         * Compiles kernel in the target. No checker available to see if the data are all initialized
         */
        void compile_kernel();

        // setters
        void set_platform_id(uint _p_id);

        void set_device_id(uint _d_id);

        void set_kernel_name(std::string _kernel_name);

        void set_kernel_source(std::string _kernel_src);

        void set_command_args(std::string _cmd_args);

        // getters
        uint get_platform_id();

        uint get_device_id();

        std::string get_kernel_name();

        std::string get_kernel_source();

        std::string get_command_args();

        ~kernelPack();
    };
}
#endif //HERO_KERNEL_PACK_HPP
