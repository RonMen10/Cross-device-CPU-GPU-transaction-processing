//
// Created by gurumurt on 28/04/20.
//

#include "kernel_pack.hpp"

namespace task_layer{

    using namespace std;

    kernelPack::kernelPack() {}

    kernelPack::kernelPack(string name, string src,string cmdArgs, int mPId,int mDId) {

        this->mKernelName = name;
        this->mKernelSrc = src;
        this->mPId = mPId;
        this->mDId = mDId;
        this->mCmdArgs = cmdArgs;

        compile_kernel();
    }

    void kernelPack::compile_kernel(){

        openCL_layer::add_kernel(mPId,mDId,mKernelName,mKernelSrc,mCmdArgs);

    }

    // setters
    void kernelPack::set_platform_id(uint _p_id) {
        mPId = _p_id;
    }

    void kernelPack::set_device_id(uint _d_id) {
        mDId = _d_id;
    }

    void kernelPack::set_kernel_name(std::string _kernel_name) {
        mKernelName = _kernel_name;
    }

    void kernelPack::set_kernel_source(std::string _kernel_src) {
        mKernelSrc = _kernel_src;
    }

    void kernelPack::set_command_args(std::string _cmd_args) {
        mCmdArgs = _cmd_args;
    }

    // setters
    uint kernelPack::get_platform_id() {
        return mPId;
    }

    uint kernelPack::get_device_id() {
        return mDId;
    }

    std::string kernelPack::get_kernel_name() {
        return mKernelName;
    }

    std::string kernelPack::get_kernel_source() {
        return mKernelSrc;
    }

    std::string kernelPack::get_command_args() {
        return mCmdArgs;
    }


    kernelPack::~kernelPack() {}
}