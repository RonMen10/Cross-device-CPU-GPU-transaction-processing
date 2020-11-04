//
// Created by gurumurt on 29/04/20.
//

#ifndef HERO_PRIMITIVE_NODE_HPP
#define HERO_PRIMITIVE_NODE_HPP

#include "../../task_layer/classes/task.hpp"
#include "../runtime_globals.hpp"
#include "../../task_layer/initialize.hpp"

namespace runtime_layer{

    using namespace task_layer;

    /**
     * Class that provides task information with input and output information
     */
    class primitive_node{

    public:
        task operation; ///task
        PRIMITIVE type; ///type of task
        vector<int> input_data; ///input data alias
        vector<int> output_data;/// ouput data alias
        size_t input_to_output_ratio{1}; ///no. of output generated for given set of input
        size_t overall_output_size{0};  ///final output from task
        size_t no_of_input_required{1}; ///amount of input required
        size_t no_of_output_required{1};///amount of output provided
        size_t no_of_parameters{1};     ///amount of parameters required
        size_t input_size{1};           ///size of the input. This translates to number of parallel processing spawned
        uint pID{0};
        uint dID{0};

        primitive_node();
        primitive_node(string device_name);
        primitive_node(PRIMITIVE type);
        primitive_node(PRIMITIVE type, size_t no_of_input_required,size_t no_of_output_required);

        /**
         * Adds data into target
         * @param data pointer to data
         * @param size size of data. This is same as input_size
         * @param q optional cmd-Q ID for sending data
         * @return 1 for success
         */
        int place_data(int *data, size_t size, uint q = 0);
        void retrieve_data(int alias,size_t size,uint q=0);
        /**
         * prepares output for the task
         * @param partial_size can also create partial result (optional, and not being used now)
         * @param q cmd-Q ID: optional
         * @return 1 for success
         */
        int prepare_output(size_t partial_size = 0, uint q = 0);

        /**
         * Appends parameter for the task
         * @param parameter
         */
        void add_parameter(int parameter);

        /**
         * Executes the node
         */
        void execute_node(short envi_type=0);

        /**
         * Sets device for the node
         * @param device_name string of device name
         */
        void set_device(string device_name);

        /**
         * compiles kernel within this node
         */
        void compile_task();
    };
}

#endif //HERO_PRIMITIVE_NODE_HPP
