#pragma clang diagnostic push
#pragma ide diagnostic ignored "hicpp-signed-bitwise"
//
// Created by bhuvanesh on 21.01.20.
//

#ifndef DISPATCHER_DATA_PARALLELISM_H
#define DISPATCHER_DATA_PARALLELISM_H

#include <utility>
#include <boost/thread/thread.hpp>
#include "../include/headers.h"
#include "../include/globals.h"
#include "../include/API/data_api.h"
#include "../include/API/kernel_api.h"
#include "../include/API/Environment.h"
#include "../include/API/Importkernel.h"
#include <thread>
#include <queue>
#include <boost/lockfree/queue.hpp>

namespace morsel_driven {
    //classes used in the namespace

    class ColorModifier {
        int code;
    public:
        explicit ColorModifier(int pCode) : code(pCode) {}

        friend std::ostream &
        operator<<(std::ostream &os, const ColorModifier &mod) {
            return os << "\033[" << mod.code << "m";
        }
    };

    ColorModifier ng_color_yellow(33), ng_color_green(32), ng_color_blue(34), ng_color_default(39), ng_color_magenta(
            35), ng_color_cyan(36);

    class Execution_Device {
    public:


        cl_command_queue transfer_queue{};
        cl_device_id device_id{};
        int device_index{-1};
        cl_context device_context{};
        size_t morsel_size{}; //size of the morsel chunks
        size_t morsels_processed{};
        string device_type;
        ColorModifier printColor{ng_color_yellow};
        map<string, double> processed_time_per_operation;

        Execution_Device() = default;
    };

    class Column {
    public:
        unsigned int *column_values{nullptr};
        string name;
        size_t noOfRows{};


    };

    class Table {
    public:
        Column *columns;
        string table_name;
        int no_of_columns{};
        int no_of_rows{};

        Table() {
            columns = nullptr;
        }

        Table(string table_name, int noOfColumns, int noOfRows, bool isGenerateData) {
            this->no_of_columns = noOfColumns;
            this->no_of_rows = noOfRows;
            this->table_name = std::move(table_name);
            this->columns = new Column[noOfColumns];
            //create column space to hold rowSize rows
            for (int i = 0; i < noOfColumns; i++) {
                columns[i].column_values = new unsigned int[noOfRows];
                columns[i].noOfRows = (size_t) noOfRows;
                if (isGenerateData) {
                    columns[i].name = this->table_name + "_" + to_string(i);
                    for (int j = 0; j < noOfRows; j++) {
                        columns[i].column_values[j] = j;
                    }
                }
            }
        }
    };


    /*
   * Operation setup happens here
   */
    class Operation_Type {
    public:
        string name;
        string source_location;
        string source;
        size_t no_of_args{};
        size_t no_of_params{};
        size_t kernel_local_chunk_size{32};
        bool is_normal_to_bitmap_result{false};
        bool is_bitmap_to_normal_result{false};

        Operation_Type() = default;
    };

    class Operation_ {
    public:
        Operation_Type *operationType;
        string name;
        vector<int *> args;
        vector<string> input_arg_names;
        string result_name;
        vector<int> params;
        size_t fetched_until{0};
        size_t processed_until{0};
        size_t rear_fetched_until{0};
        size_t rear_processed_until{0};
        size_t data_size{0};
        Column *selectionColumn{};

        explicit Operation_(Operation_Type *operationType) {
            this->operationType = operationType;
        }
    };

    class Operation_Node {
    public:
        bool is_initialized{false};
        Operation_ *_operation{nullptr};
        Execution_Device executionDevice;
    };

    class Exchange_Edge {
    public:
        string input_data_name;
        string fetched_data_name; //both will be same if processed on same device
        bool input_EOD{false};
        bool is_virtual_vertex{false}; // true one edge of the vertex is virtual vertex
        int input_device_index{-1};
        int consumer_device_index{-1};
        bool is_work_shared{false};
        size_t processed_until{0};
        size_t fetched_until{0};
        size_t consumed_until{0};
        size_t rear_fetched_until{0};
        size_t rear_processed_until{0};
        size_t data_size{0};
        size_t morsel_size{0};
    };

    //types
    typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS, Operation_Node, Exchange_Edge> ng_type_execution_graph;
    typedef boost::graph_traits<ng_type_execution_graph>::vertex_descriptor ng_type_QEP_vertex;
    typedef boost::graph_traits<ng_type_execution_graph>::edge_descriptor ng_type_QEP_edge;
    typedef boost::graph_traits<ng_type_execution_graph>::in_edge_iterator ng_type_incoming_edges;
    typedef boost::graph_traits<ng_type_execution_graph>::out_edge_iterator ng_type_outgoing_edges;


    class VertexInfo {
    public:
        Operation_Node *current_node{nullptr};
        vector<Operation_Node *> incoming_nodes;
        vector<Exchange_Edge *> incoming_edges;
        int number_of_incoming_edges{0};
        Exchange_Edge *outgoing_edge{nullptr};
        int number_of_outgoing_edges{0};
    };

    class QueryPlanWithData {
    public:
        Table *tables;
        int no_of_tables;
        ng_type_execution_graph *query_execution_graph;
        ng_type_QEP_vertex start_vertex{};

        explicit QueryPlanWithData(int noOfTables) {
            this->no_of_tables = noOfTables;
            this->tables = new Table[noOfTables];
            query_execution_graph = new ng_type_execution_graph();
        }
    };

    enum class DEVICE_USAGE_MODE {
        ONLY_CPU,
        ONLY_GPU,
        BOTH_CPU_GPU
    };


    QueryPlanWithData *ng_plan;
    vector<Operation_ *> ng_selections;
    //namespace level variables prefixed with ng
    size_t ng_morsel_size = (std::size_t) 1024;
    short ng_total_no_of_devices = 0;
    int ng_operation_counter = 0;
    vector<Execution_Device> ng_execution_devices;
    vector<Operation_Node *> ng_selection_nodes;
    vector<Operation_Node *> ng_logical_nodes;
    vector<ng_type_QEP_vertex> ng_selection_vertexes;
    vector<ng_type_QEP_vertex> ng_logical_vertexes;
    vector<Operation_Type *> ng_kernel_operations;
    Operation_Type *ng_selection_greater_than, *ng_selection_less_than, *ng_logical_and, *ng_logical_or, *ng_aggregation;
    static boost::lockfree::queue<ng_type_QEP_vertex> *ng_processing_queue;
    Operation_Node *ng_last_node;
    std::stringstream ng_summarizer;
    bool ng_is_work_sharing_enabled = true;

    //function definitions
    string print_data(const string &name, int device_index, size_t data_size, bool flag_bits = false);

    string get_result_buffer_name(int device_index, const string &operation_name);

    string get_operation_alias_name();

    string get_input_buffer_name(int device_index, const string &operation_name, int input_param_index);

    string get_input_buffer_name_for_device_replication(int device_index, const string &buffer_name);

    void setup_devices(DEVICE_USAGE_MODE mode);

    void setup_operators();

    Operation_ *create_selection_operation(Operation_Type *operation_type, Column *column, int condition_value);

    void create_test_plan();

    void create_execution_graph();

    void create_bfs_execution_pipeline();

    Operation_Node *create_node(Operation_ *operation);

    Operation_Node *create_logical_node(Operation_Type *operationType);

    Exchange_Edge *create_edge(size_t data_size, bool is_virtual);

    void
    create_buffer(const string &alias, int device_index, size_t buffer_size,
                  bool is_throw_error_if_buffer_present = true);

    VertexInfo *findVertexDetails(ng_type_QEP_vertex vertex);

    VertexInfo *findIncomingVertexDetails(ng_type_QEP_vertex current_vertex, int incoming_edge_index);

    void checkError(cl_int err, const string &message);


    bool is_buffer_present(const string &alias, cl_device_id device_id);

    void add_kernel_to_device(cl_device_id device_id, Operation_Type *operationType);

    cl_mem get_buffer(const string &alias, cl_device_id device_id);

    string create_sub_buffer(const string &buffer_alias, int device_index, int offset, size_t size,
                             bool is_throw_error_if_buffer_present = true);

    string compute_alias_name_for_offset(const string &name, size_t offset);

    void copy_data_from_device_memory(const string &source_alias, int source_device_index, int source_offset, int size,
                                      const string &destination_alias,
                                      int destination_device_index, int destination_offset);

    void copy_data_from_memory(unsigned int *memory_data, int memory_offset, int size, const string &destination_alias,
                               int destination_device_index, int destination_offset);

    size_t compute_result_size(Operation_ *operation);

    size_t compute_result_morsel_size(Operation_ *operation);

    size_t get_result_buffer_offset(Operation_ *operation, int offset);

    void execute_processing_queue();

    void thread_processing(int device_index);

    void transfer_data(VertexInfo *vertexInfo, int device_index);

    void process_data(VertexInfo *vertexInfo, int device_index);

    void work_sharing_transfer_and_process_data(ng_type_QEP_vertex current_vertex, int incoming_edge_index,
                                                int device_index);

    void testPop() {
        ng_type_QEP_vertex currentVertex;
        bool isPopSuccess = ng_processing_queue->pop(currentVertex);
        int index = 0;
        while (isPopSuccess) {
            VertexInfo *vertexInfo = findVertexDetails(currentVertex);
            create_buffer("result_" + to_string(index++), 0, 1024, false);
            isPopSuccess = ng_processing_queue->pop(currentVertex);
        }
    }

    string temp_print_dict() {
        // Select the correct type for calling the equal_range function
        stringstream ss;

        for (auto &itr : data_dictionary) {
            ss << itr.first.first << endl;
        }
        return ss.str();
    }

    void run() {
        ng_is_work_sharing_enabled = false;
        setup_devices(DEVICE_USAGE_MODE::BOTH_CPU_GPU);
        setup_operators();
        create_test_plan();
        create_execution_graph();
        create_bfs_execution_pipeline();
        execute_processing_queue();
        cout << endl << endl << endl << ng_color_cyan << "final  result:"
             << print_data(ng_last_node->_operation->result_name, ng_last_node->executionDevice.device_index,
                           compute_result_size(ng_last_node->_operation),
                           true) << endl << ng_color_default;
    }

    string source_loc = "kernels/mapper/";
    void setup_operators() {
        ng_selection_greater_than = new Operation_Type();
        ng_selection_greater_than->name = "selection_block_greater_than";
        ng_selection_greater_than->source_location = source_loc;
        ng_selection_greater_than->source = readKernelFile(
                ng_selection_greater_than->source_location + ng_selection_greater_than->name + ".cl");
        ng_selection_greater_than->no_of_args = 2;
        ng_selection_greater_than->no_of_params = 1;
        ng_selection_greater_than->is_normal_to_bitmap_result = true;

        ng_selection_less_than = new Operation_Type();
        ng_selection_less_than->name = "selection_block_less_than";
        ng_selection_less_than->source_location = source_loc;
        ng_selection_less_than->source = readKernelFile(
                ng_selection_less_than->source_location + ng_selection_less_than->name + ".cl");
        ng_selection_less_than->no_of_args = 2;
        ng_selection_less_than->no_of_params = 1;
        ng_selection_less_than->is_normal_to_bitmap_result = true;

        ng_logical_and = new Operation_Type();
        ng_logical_and->name = "logical_block_and";
        ng_logical_and->source_location = source_loc;
        ng_logical_and->source = readKernelFile(ng_logical_and->source_location + ng_logical_and->name + ".cl");
        ng_logical_and->no_of_args = 3;
        ng_logical_and->no_of_params = 0;
        ng_logical_and->kernel_local_chunk_size = 1;

        ng_logical_or = new Operation_Type();
        ng_logical_or->name = "logical_block_or";
        ng_logical_or->source_location = source_loc;
        ng_logical_or->source = readKernelFile(ng_logical_or->source_location + ng_logical_or->name + ".cl");
        ng_logical_or->no_of_args = 3;
        ng_logical_or->no_of_params = 0;
        ng_logical_or->kernel_local_chunk_size = 1;
    }

    void setup_devices(DEVICE_USAGE_MODE mode) {
        setup_environment();
        print_environment();
        cl_device_type dev_type;
        for (short i = 0, j = 0; i < total_no_of_devices; i++) {
            clGetDeviceInfo(device[i][0], CL_DEVICE_TYPE, sizeof(dev_type), &dev_type, nullptr);
            if ((dev_type == CL_DEVICE_TYPE_CPU &&
                 (mode == DEVICE_USAGE_MODE::ONLY_CPU || mode == DEVICE_USAGE_MODE::BOTH_CPU_GPU)) ||
                (dev_type == CL_DEVICE_TYPE_GPU &&
                 (mode == DEVICE_USAGE_MODE::ONLY_GPU || mode == DEVICE_USAGE_MODE::BOTH_CPU_GPU))) {
                ng_execution_devices.emplace_back(Execution_Device());
                ng_execution_devices[j].device_id = device[i][0];
                cl_context device_context = data_queue[ng_execution_devices[j].device_id].context;
                ng_execution_devices[j].device_context = device_context;
                ng_execution_devices[j].device_index = j;
                ng_execution_devices[j].transfer_queue = clCreateCommandQueue(device_context,
                                                                              ng_execution_devices[j].device_id,
                                                                              cqp,
                                                                              &err);
                if (dev_type == CL_DEVICE_TYPE_GPU) {
                    ng_execution_devices[j].device_type = "GPU";
                    ng_execution_devices[j].printColor = ng_color_green;
                }
                if (dev_type == CL_DEVICE_TYPE_CPU) {
                    ng_execution_devices[j].device_type = "CPU";
                    ng_execution_devices[j].printColor = ng_color_blue;
                }
                j += 1;
                ng_total_no_of_devices += 1;
            }
        }
    }

    void execute_processing_queue() {
        vector<boost::thread *> threads;
        time_t parallel_execution_start = clock();
        //add kernels
        for (int device_index = 0; device_index < ng_total_no_of_devices; device_index++) {
            for (auto &ng_kernel_operation : ng_kernel_operations) {
                add_kernel_to_device(ng_execution_devices[device_index].device_id, ng_kernel_operation);
            }
        }
        for (int device_index = 0; device_index < ng_total_no_of_devices; device_index++) {
            threads.emplace_back(new boost::thread(std::bind(&thread_processing, device_index)));
            threads[device_index]->yield();
            ng_summarizer << ng_execution_devices[device_index].printColor << "execution thread created for device: "
                          << ng_execution_devices[device_index].device_type << endl << ng_color_default;
        }
        for (int device_index = 0; device_index < ng_total_no_of_devices; device_index++) {
            threads[device_index]->join();
        }
        time_t parallel_execution_end = clock();
        cout << ng_summarizer.str() << flush;
        cout<<endl<<ng_color_magenta<<"time taken :"<<(parallel_execution_end-parallel_execution_start);
        //  cout<<endl<<temp_print_dict()<<flush;
    }


    void thread_processing(int device_index) {
        std::thread transfer_data_thread;
        //do until processing queue is empty
        while (!ng_processing_queue->empty()) {
            ng_type_QEP_vertex currentVertex;
            bool isPopSuccess = ng_processing_queue->pop(currentVertex);
            if (isPopSuccess) {
                auto *poppedVertexInfo = findVertexDetails(currentVertex);
                poppedVertexInfo->current_node->executionDevice = ng_execution_devices[device_index];
                //check whether all data available to process
                for (int in_edges = 0; in_edges < poppedVertexInfo->number_of_incoming_edges; in_edges++) {
                    //check whether incoming edges is complete else do work sharing
                    if ((!poppedVertexInfo->incoming_edges[in_edges]->is_virtual_vertex) &&
                        !poppedVertexInfo->incoming_edges[in_edges]->input_EOD && ng_is_work_sharing_enabled) {
                        //work sharing comes here
                        work_sharing_transfer_and_process_data(currentVertex,
                                                               in_edges, device_index);

                    }
                    if (poppedVertexInfo->current_node->_operation->data_size == 0) {
                        poppedVertexInfo->current_node->_operation->data_size = poppedVertexInfo->
                                incoming_edges[in_edges]->data_size;
                        poppedVertexInfo->current_node->_operation->rear_processed_until = poppedVertexInfo->current_node->_operation->data_size;
                    }

                }
                //create transfer data thread
                transfer_data_thread = thread(transfer_data, poppedVertexInfo, device_index);
                process_data(poppedVertexInfo, device_index);
                transfer_data_thread.join();
            } else {
                break;
            }
        }
        ng_summarizer << ng_execution_devices[device_index].printColor << "all operations complete in device: "
                      << ng_execution_devices[device_index].device_type
                      << endl;

    }

    void work_sharing_transfer_and_process_data(ng_type_QEP_vertex current_vertex, int incoming_edge_index,
                                                int device_index) {
        VertexInfo *vertexInfo = findIncomingVertexDetails(current_vertex, incoming_edge_index);
        auto currentNode = vertexInfo->current_node;
        //wait till initialization complete
        while (!currentNode->is_initialized);
        ng_summarizer << endl << ng_execution_devices[device_index].printColor << " work sharing of "
                      << currentNode->_operation->name << endl;
        ng_summarizer << "   Input buffers:" << endl;
        //create buffers
        for (int i = 0; i < vertexInfo->number_of_incoming_edges; i++) {
            create_buffer(vertexInfo->incoming_edges[i]->fetched_data_name, device_index,
                          vertexInfo->incoming_edges[i]->data_size,
                          true);
            ng_summarizer << "        " << i << "     " << vertexInfo->incoming_edges[i]->fetched_data_name << endl;
        }
        //create result buffer which will be used further by the current processing device
        // as (one of the) input for the next operation
        string result_buffer_name = get_input_buffer_name_for_device_replication(device_index,
                                                                                 vertexInfo->outgoing_edge->input_data_name);
        size_t result_data_size = compute_result_size(currentNode->_operation);
        create_buffer(result_buffer_name, device_index, result_data_size);
        ng_summarizer << "    Output buffer name:" << result_buffer_name << endl;
        vertexInfo->outgoing_edge->is_work_shared = true;
        //do transfer and process data
        ng_summarizer << endl << "   Work share processed from " << currentNode->_operation->rear_fetched_until
                      << " till";
        while (currentNode->_operation->fetched_until <
               (currentNode->_operation->rear_fetched_until - ng_morsel_size)) {
            currentNode->_operation->rear_fetched_until = currentNode->_operation->rear_fetched_until - ng_morsel_size;
            //transfer data
            vector<string> input_param_aliases;
            //create sub buffers and copy
            for (int i = 0; i < vertexInfo->number_of_incoming_edges; i++) {
                string input_sub_buffer_alias =
                        compute_alias_name_for_offset(currentNode->_operation->input_arg_names[i],
                                                      currentNode->_operation->rear_fetched_until);
                create_sub_buffer(currentNode->_operation->input_arg_names[i],
                                  device_index, currentNode->_operation->rear_fetched_until, ng_morsel_size);
                input_param_aliases.emplace_back(currentNode->_operation->input_arg_names[i]);
                //if its a virtual vertex then we need to copy from main memory
                if (vertexInfo->incoming_edges[i]->is_virtual_vertex) {
                    copy_data_from_memory(currentNode->_operation->selectionColumn->column_values,
                                          currentNode->_operation->rear_fetched_until,
                                          ng_morsel_size,
                                          input_sub_buffer_alias, device_index, 0);
                } else {
                    copy_data_from_device_memory(vertexInfo->incoming_edges[i]->input_data_name,
                                                 vertexInfo->incoming_edges[i]->input_device_index,
                                                 currentNode->_operation->rear_fetched_until,
                                                 ng_morsel_size,
                                                 input_sub_buffer_alias, device_index, 0);
                }

            }
            //create result sub buffer
            size_t result_buffer_offset = get_result_buffer_offset(currentNode->_operation,
                                                                   currentNode->_operation->rear_fetched_until);
            size_t result_sub_buffer_size = compute_result_morsel_size(currentNode->_operation);
            //create result sub buffer
            create_sub_buffer(result_buffer_name, device_index, result_buffer_offset,
                              result_sub_buffer_size);
            //add result buffer
            input_param_aliases.emplace_back(compute_alias_name_for_offset(result_buffer_name,
                                                                           get_result_buffer_offset(
                                                                                   currentNode->_operation,
                                                                                   currentNode->_operation->rear_fetched_until)));
            //process data
            execute(ng_execution_devices[device_index].device_id, currentNode->_operation->operationType->name,
                    input_param_aliases, currentNode->_operation->params,
                    ng_morsel_size / currentNode->_operation->operationType->kernel_local_chunk_size);
            vertexInfo->outgoing_edge->rear_processed_until = get_result_buffer_offset(currentNode->_operation,
                                                                                       currentNode->_operation->rear_fetched_until);
            currentNode->_operation->rear_processed_until = currentNode->_operation->rear_fetched_until;
        }
        ng_summarizer << " till " << currentNode->_operation->rear_processed_until << endl << ng_color_default;
    }


    void transfer_data(VertexInfo *vertexInfo, int device_index) {

        auto currentNode = vertexInfo->current_node;
        size_t data_size = 0;
        size_t offset = 0;
        //create buffers
        //result buffer

        string result_buffer_name = get_result_buffer_name(device_index, currentNode->_operation->name);
        data_size = currentNode->_operation->data_size;
        size_t result_data_size = compute_result_size(currentNode->_operation);
        create_buffer(result_buffer_name, device_index, result_data_size);
        ng_summarizer << ng_execution_devices[device_index].printColor
                      << "********************************************************************" << endl;
        ng_summarizer << " initialization complete  -->" << currentNode->_operation->name << "----"
                      << currentNode->_operation->operationType->name << " ----> "
                      << ng_execution_devices[device_index].device_type << endl;
        if (vertexInfo->number_of_outgoing_edges > 0) {
            vertexInfo->outgoing_edge->input_device_index = device_index;
            vertexInfo->outgoing_edge->input_data_name = result_buffer_name;

            vertexInfo->outgoing_edge->data_size = result_data_size;
        }
        currentNode->_operation->result_name = result_buffer_name;
        ng_summarizer << "     output param name :" << result_buffer_name << endl;
        ng_summarizer << "     input  params mapping " << endl;
        //input buffers
        size_t max_data_size = 0;
        for (int i = 0; i < vertexInfo->number_of_incoming_edges; i++) {
            //if current device is same as execution device
            if (currentNode->executionDevice.device_index == vertexInfo->incoming_edges[i]->input_device_index) {
                vertexInfo->incoming_edges[i]->fetched_data_name = vertexInfo->incoming_edges[i]->input_data_name;
                vertexInfo->incoming_edges[i]->fetched_until = vertexInfo->incoming_edges[i]->data_size;
                vertexInfo->incoming_edges[i]->rear_processed_until = vertexInfo->incoming_edges[i]->data_size;
                currentNode->_operation->input_arg_names.push_back(vertexInfo->incoming_edges[i]->fetched_data_name);
            } else {
                string input_buffer_name;
                if (vertexInfo->incoming_edges[i]->is_virtual_vertex) {
                    input_buffer_name = get_input_buffer_name(device_index, currentNode->_operation->name, i);
                } else {
                    input_buffer_name = get_input_buffer_name_for_device_replication(device_index,
                                                                                     vertexInfo->incoming_edges[i]->input_data_name);
                }
                vertexInfo->incoming_edges[i]->fetched_data_name = input_buffer_name;
                currentNode->_operation->input_arg_names.push_back(input_buffer_name);
                create_buffer(input_buffer_name, device_index,
                              data_size, (!vertexInfo->incoming_edges[i]->is_work_shared));
            }
            //set the max
            if (vertexInfo->incoming_edges[i]->data_size > max_data_size) {
                max_data_size = vertexInfo->incoming_edges[i]->data_size;
            }
            ng_summarizer << "         " << to_string(i) << " incoming edge field: "
                          << (vertexInfo->incoming_edges[i]->is_virtual_vertex
                              ? "array "
                              : vertexInfo->incoming_edges[i]->input_data_name)
                          << " -->fetch field :" << vertexInfo->incoming_edges[i]->fetched_data_name << endl;
            vertexInfo->incoming_edges[i]->consumer_device_index = device_index;
        }
        vertexInfo->current_node->_operation->rear_processed_until = max_data_size;
        vertexInfo->current_node->_operation->rear_fetched_until = max_data_size;
        vertexInfo->current_node->is_initialized = true;

        ng_summarizer << endl << "********************************************************************"
                      << ng_color_default << endl;
        offset = 0;
        //transfer chunk by chunk -chunk size = morsels_size
        while (offset < vertexInfo->current_node->_operation->rear_fetched_until) {
            // incoming edges
            for (int i = 0; i < vertexInfo->number_of_incoming_edges; i++) {
                //create sub buffers and copy data
                if (vertexInfo->incoming_edges[i]->fetched_until < vertexInfo->incoming_edges[i]->data_size) {
                    //if current device is  same as execution device already sub buffers and data is present
                    if (currentNode->executionDevice.device_index !=
                        vertexInfo->incoming_edges[i]->input_device_index) {
                        string input_sub_buffer_alias =
                                compute_alias_name_for_offset(currentNode->_operation->input_arg_names[i], offset);
                        if (vertexInfo->incoming_edges[i]->is_work_shared &&
                            is_buffer_present(input_sub_buffer_alias, ng_execution_devices[device_index].device_id)) {
                            vertexInfo->incoming_edges[i]->fetched_until = offset + ng_morsel_size;
                        } else {
                            create_sub_buffer(currentNode->_operation->input_arg_names[i],
                                              device_index, offset, ng_morsel_size);
                            //if its a virtual vertex then we need to copy from main memory
                            if (vertexInfo->incoming_edges[i]->is_virtual_vertex) {
                                copy_data_from_memory(currentNode->_operation->selectionColumn->column_values, offset,
                                                      ng_morsel_size,
                                                      input_sub_buffer_alias, device_index, 0);
                            } else {
                                copy_data_from_device_memory(vertexInfo->incoming_edges[i]->input_data_name,
                                                             vertexInfo->incoming_edges[i]->input_device_index, offset,
                                                             ng_morsel_size,
                                                             input_sub_buffer_alias, device_index, 0);
                            }
                            vertexInfo->incoming_edges[i]->fetched_until = offset + ng_morsel_size;
                        }
                    }
                }
            }
            //  cout<<" transfer chunking process for "<<currentNode._operation->name<<" "<<offset<<" "<<data_size<<endl;
            offset += ng_morsel_size;
            currentNode->_operation->fetched_until = offset;
        }
        ng_summarizer << endl << ng_execution_devices[device_index].printColor << "transferred till" << offset
                      << " transfer complete for operation "
                      << currentNode->_operation->name << endl << ng_color_default;

    }


    void process_data(VertexInfo *vertexInfo, int device_index) {
        Operation_Node *currentNode = vertexInfo->current_node;
        while (!currentNode->is_initialized);
        size_t data_size = currentNode->_operation->data_size;
        size_t result_data_size =
                compute_result_size(currentNode->_operation);
        size_t offset = 0;
        bool is_sub_buffer_created = false;
        while (offset < currentNode->_operation->rear_processed_until) {
            size_t result_buffer_offset = get_result_buffer_offset(currentNode->_operation, offset);
            size_t result_sub_buffer_size = compute_result_morsel_size(currentNode->_operation);
            if (!is_sub_buffer_created) {
                //create result sub buffer
                create_sub_buffer(currentNode->_operation->result_name, device_index, result_buffer_offset,
                                  result_sub_buffer_size);
                is_sub_buffer_created = true;
            }
            vector<string> input_param_aliases;
            for (int i = 0; i < currentNode->_operation->operationType->no_of_args - 1; i++) {
                input_param_aliases.emplace_back(
                        compute_alias_name_for_offset(currentNode->_operation->input_arg_names[i], offset));
            }
            bool isDataAvailableForProcessing = true;
            for (const auto &input_param_aliase : input_param_aliases) {
                if (!is_buffer_present(input_param_aliase, ng_execution_devices[device_index].device_id) ||
                    offset >= currentNode->_operation->fetched_until) {
                    isDataAvailableForProcessing = false;
                    break;
                }
            }
            if (!isDataAvailableForProcessing) {
                continue;
            }
            //add result buffer
            input_param_aliases.emplace_back(compute_alias_name_for_offset(currentNode->_operation->result_name,
                                                                           get_result_buffer_offset(
                                                                                   currentNode->_operation, offset)));
            execute(currentNode->executionDevice.device_id, currentNode->_operation->operationType->name,
                    input_param_aliases, currentNode->_operation->params,
                    ng_morsel_size / currentNode->_operation->operationType->kernel_local_chunk_size);
            //need to add locks
            currentNode->_operation->processed_until = offset + ng_morsel_size;
            if (vertexInfo->number_of_outgoing_edges > 0) {
                vertexInfo->outgoing_edge->processed_until = offset + ng_morsel_size;
            }
            offset += ng_morsel_size;
            is_sub_buffer_created = false;
        }
        if (vertexInfo->number_of_outgoing_edges > 0) {
            vertexInfo->outgoing_edge->input_EOD = true;
        }
        ng_summarizer << ng_execution_devices[device_index].printColor << "process complete for operation :"
                      << vertexInfo->current_node->_operation->name << " in device: "
                      << ng_execution_devices[device_index].device_type << endl;
        ng_summarizer << ng_color_default;

    }

    string get_result_buffer_name(int device_index, const string &operation_name) {
        return "result_" + operation_name + "_" + ng_execution_devices[device_index].device_type;
    }

    string get_input_buffer_name(int device_index, const string &operation_name, int input_param_index) {
        return "input_" + operation_name + "_" + ng_execution_devices[device_index].device_type + "_p_" + to_string(
                input_param_index);
    }

    string get_input_buffer_name_for_device_replication(int device_index, const string &buffer_name) {
        string new_name = buffer_name;
        return new_name.replace(12, 3, ng_execution_devices[device_index].device_type);
    }

    string get_operation_alias_name() {
        return "op_" + to_string(ng_operation_counter++);
    }

    void create_test_plan() {
        ng_plan = new QueryPlanWithData(1);
        ng_plan->tables[0] = Table("table1", 3, pow(2, 16), true);
        ng_selections.emplace_back(
                create_selection_operation(ng_selection_greater_than, &ng_plan->tables[0].columns[0], 30));
        ng_selections.emplace_back(
                create_selection_operation(ng_selection_greater_than, &ng_plan->tables[0].columns[1], 25));
        ng_selections.emplace_back(
                create_selection_operation(ng_selection_less_than, &ng_plan->tables[0].columns[1], 200));
        //add kernel operations
        ng_kernel_operations.emplace_back(ng_selection_greater_than);
        ng_kernel_operations.emplace_back(ng_selection_less_than);
    }

    void create_execution_graph() {
        int number_of_selections = (int) ng_selections.size();
        int number_of_logical_nodes = (int) (ng_selections.size() - 1);
        ng_type_QEP_vertex virtual_vertex = add_vertex(Operation_Node(), *ng_plan->query_execution_graph);
        if (number_of_selections < 2) {
            cerr << "minimum 2 selection required";
            exit(1);
        }
        for (int i = 0; i < number_of_selections; i++) {
            ng_selection_nodes.emplace_back(create_node(ng_selections[i]));
            ng_selection_vertexes.emplace_back(add_vertex(*ng_selection_nodes[i], *ng_plan->query_execution_graph));
            //add edge for virtual vertex and selection vertexes
            Exchange_Edge *edge = create_edge(ng_selection_nodes[i]->_operation->data_size, true);
            add_edge(virtual_vertex, ng_selection_vertexes[i], *edge, *ng_plan->query_execution_graph);
        }
        ng_plan->start_vertex = ng_selection_vertexes[0];
        for (int i = 0; i < number_of_logical_nodes; i++) {
            ng_logical_nodes.emplace_back(create_logical_node(ng_logical_and));
            ng_logical_vertexes.emplace_back(add_vertex(*ng_logical_nodes[i], *ng_plan->query_execution_graph));
            //add edges from selection to logical
            if (i == 0) {
                ng_kernel_operations.emplace_back(ng_logical_and);
                //add left edge for logical vertex 0 from selection vertex 0
                Exchange_Edge *edgeLeft = create_edge(ng_selection_nodes[0]->_operation->data_size, false);
                add_edge(ng_selection_vertexes[0], ng_logical_vertexes[i], *edgeLeft, *ng_plan->query_execution_graph);
            } else {
                //add left edge for logical vertex i from logical vertex i-1
                Exchange_Edge *edgeLeft = create_edge(ng_logical_nodes[i - 1]->_operation->data_size, false);
                add_edge(ng_logical_vertexes[i - 1], ng_logical_vertexes[i], *edgeLeft,
                         *ng_plan->query_execution_graph);
            }
            //add right edge for logical vertex i from selection vertex i+1
            Exchange_Edge *edgeRight = create_edge(ng_selection_nodes[i + 1]->_operation->data_size, false);
            add_edge(ng_selection_vertexes[i + 1], ng_logical_vertexes[i], *edgeRight, *ng_plan->query_execution_graph);
        }
    }

    Operation_Node *create_node(Operation_ *operation) {
        auto *node = new Operation_Node();
        node->_operation = operation;
        node->_operation->name = get_operation_alias_name();
        return node;
    }

    Operation_Node *create_logical_node(Operation_Type *operationType) {
        auto *logicalNode = new Operation_Node();
        logicalNode->_operation = new Operation_(operationType);
        logicalNode->_operation->name = get_operation_alias_name();
        return logicalNode;
    }

    Exchange_Edge *create_edge(size_t data_size, bool is_virtual) {
        auto *edge = new Exchange_Edge();
        edge->processed_until = 0;
        edge->consumed_until = 0;
        edge->data_size = data_size;
        edge->rear_fetched_until = data_size;
        edge->is_virtual_vertex = is_virtual;
        edge->input_EOD = false;
        return edge;
    }

    Operation_ *create_selection_operation(Operation_Type *operation_type, Column *column, int condition_value) {
        auto operation_ = new Operation_(operation_type);
        operation_->selectionColumn = column;
        operation_->data_size = column->noOfRows;
        operation_->params.emplace_back(condition_value);
        return operation_;
    }

    template<typename TimeMap>
    class bfs_time_visitor : public boost::default_bfs_visitor {
        typedef typename boost::property_traits<TimeMap>::value_type T;
    public:
        bfs_time_visitor(TimeMap tmap, T &t) : m_timemap(tmap), m_time(t) {}

        template<typename Vertex, typename Graph>
        void discover_vertex(Vertex u, const Graph &g) const {
            put(m_timemap, u, m_time++);
        }

        TimeMap m_timemap;
        T &m_time;
    };

    void create_bfs_execution_pipeline() {
        ng_type_execution_graph *graph = ng_plan->query_execution_graph;
        ng_type_QEP_vertex start_vertex = ng_plan->start_vertex;
        typedef boost::graph_traits<ng_type_execution_graph>::vertex_descriptor Vertex;
        typedef boost::graph_traits<ng_type_execution_graph>::vertices_size_type Size;
        typedef Size *Iiter;
        int N = num_vertices(*graph);
        ng_processing_queue = new boost::lockfree::queue<ng_type_QEP_vertex>(N);
        // a vector to hold the discover time property for each vertex
        std::vector<Size> dtime(N);

        Size time = 0;
        bfs_time_visitor<Size *> vis(&dtime[0], time);
        breadth_first_search(*graph, start_vertex, visitor(vis));

        // Use std::sort to order the vertices by their discover time
        std::vector<boost::graph_traits<ng_type_execution_graph>::vertices_size_type> discover_order(N);
        boost::integer_range<int> range(0, N);
        std::copy(range.begin(), range.end(), discover_order.begin());
        std::sort(discover_order.begin(), discover_order.end(),
                  boost::indirect_cmp<Iiter, std::less<> >(&dtime[0]));
        stringstream executionOrder;
        executionOrder << "****************" << endl;
        executionOrder << "EXECUTION ORDER" << endl;

        for (int i = 0; i < N; ++i) {
            if ((*ng_plan->query_execution_graph)[discover_order[i]]._operation != nullptr) {
                executionOrder << (*ng_plan->query_execution_graph)[discover_order[i]]._operation->name << endl;
                ng_processing_queue->push(discover_order[i]);
                ng_last_node = &(*ng_plan->query_execution_graph)[discover_order[i]];
            }
        }
        executionOrder << "****************" << endl;
        ng_summarizer << ng_color_yellow << executionOrder.str() << endl << ng_color_default << flush;
    }

    VertexInfo *findVertexDetails(ng_type_QEP_vertex vertex) {
        auto vertexInfo = new VertexInfo();
        ng_type_execution_graph *graph = ng_plan->query_execution_graph;
        ng_type_incoming_edges initial, end;
        int index = 0;
        for (boost::tie(initial, end) = in_edges(vertex, *graph);
             initial != end;
             ++initial) {
            vertexInfo->incoming_edges.emplace_back(&(*graph)[*initial]);
            ng_type_QEP_vertex sourceVertex = source(*initial, *graph);
            vertexInfo->incoming_nodes.emplace_back(&(*graph)[sourceVertex]);
            index++;
        }
        vertexInfo->number_of_incoming_edges = index;
        index = 0;
        ng_type_outgoing_edges start_out, end_out;
        for (boost::tie(start_out, end_out) = out_edges(vertex, *graph);
             start_out != end_out;
             ++start_out) {
            vertexInfo->outgoing_edge = &(*graph)[*start_out];
            index++;
        }
        vertexInfo->number_of_outgoing_edges = index;
        vertexInfo->current_node = &(*graph)[vertex];
        return vertexInfo;
    }

    VertexInfo *findIncomingVertexDetails(ng_type_QEP_vertex current_vertex, int incoming_edge_index) {
        ng_type_execution_graph *graph = ng_plan->query_execution_graph;
        ng_type_incoming_edges initial, end;
        ng_type_QEP_vertex sourceVertex = -1;
        int index = 0;
        for (boost::tie(initial, end) = in_edges(current_vertex, *graph);
             initial != end;
             ++initial) {
            if (incoming_edge_index == index) {
                sourceVertex = source(*initial, *graph);
                break;
            }
            index++;
        }
        if (sourceVertex == -1) {
            cout << ng_summarizer.str() << endl << flush;
            cerr << "invalid code flow please debug :( " << endl;
            exit(1);
        }
        return findVertexDetails(sourceVertex);
    }

    size_t compute_result_size(Operation_ *operation) {
        size_t result_size = operation->data_size;
        if (operation->operationType->is_normal_to_bitmap_result) {
            result_size = operation->data_size / 32;
        }
        if (operation->operationType->is_bitmap_to_normal_result) {
            result_size = operation->data_size * 32;
        }
        return result_size;
    }

    size_t compute_result_morsel_size(Operation_ *operation) {
        size_t result_morsel_size = ng_morsel_size;
        if (operation->operationType->is_normal_to_bitmap_result) {
            result_morsel_size = ng_morsel_size / 32;
        }
        if (operation->operationType->is_bitmap_to_normal_result) {
            result_morsel_size = ng_morsel_size * 32;
        }
        return result_morsel_size;
    }

    size_t get_result_buffer_offset(Operation_ *operation, int offset) {
        auto result_buffer_offset = (size_t) offset;
        if (operation->operationType->is_normal_to_bitmap_result) {
            result_buffer_offset = offset / 32;
        }
        if (operation->operationType->is_bitmap_to_normal_result) {
            result_buffer_offset = offset * 32;
        }
        return result_buffer_offset;
    }

    string compute_alias_name_for_offset(const string &name, size_t offset) {
        return name + '_' + to_string((int) offset);
    }

//open cl related functions
    void checkError(cl_int err, const string &message) {
        if (err != CL_SUCCESS) {
            cout << ng_summarizer.str() << flush;
            cerr << "ERROR occured, code :" << err << " message:" << message << endl;
            exit(EXIT_FAILURE);
        }
    }


    bool is_buffer_present(const string &alias, cl_device_id device_id) {
        return data_dictionary.count(make_pair(alias, device_id)) > 0;
    }

    cl_mem get_buffer(const string &alias, cl_device_id device_id) {
        if (is_buffer_present(alias, device_id)) {
            return data_dictionary.find(make_pair(alias, device_id))->second;
        } else {
            cout << ng_summarizer.str() << flush;
            cerr << "buffer not present for alias " << alias;
            exit(1);
        }
    }

    void create_buffer(const string &alias, int device_index, size_t buffer_size,
                       bool is_throw_error_if_buffer_present) {
        cl_int error_code = 0;
        cl_context device_context = ng_execution_devices[device_index].device_context;
        cl_device_id device_id = ng_execution_devices[device_index].device_id;
        //check whether buffer is already present
        if (!is_buffer_present(alias, device_id)) {
            cl_mem buffer_local = clCreateBuffer(device_context, CL_MEM_READ_WRITE, buffer_size * sizeof(int),
                                                 nullptr,
                                                 &error_code);
            checkError(error_code, "error while creating buffer for alias " + alias);
            data_dictionary.insert(
                    make_pair(make_pair(alias, device_id), buffer_local));
        } else if (is_throw_error_if_buffer_present) {
            cout << ng_summarizer.str() << flush;
            cerr << "invalid code flow buffer already present for alias:" << alias;
            exit(1);
        }

    }

    void add_kernel_to_device(cl_device_id device_id, Operation_Type *operationType) {
        add_kernel(operationType->name, device_id, operationType->source);
    }

    void copy_data_from_device_memory(const string &source_alias, int source_device_index, int source_offset, int size,
                                      const string &destination_alias,
                                      int destination_device_index, int destination_offset) {
        auto *intermediate_result = new unsigned int[size];
        cl_command_queue source_device_queue = ng_execution_devices[source_device_index].transfer_queue;
        cl_command_queue destination_device_queue = ng_execution_devices[destination_device_index].transfer_queue;
        cl_device_id source_device_id = ng_execution_devices[source_device_index].device_id;
        cl_device_id destination_device_id = ng_execution_devices[destination_device_index].device_id;
        //check whether both source data and destination buffer present in respective devices
        cl_mem source_buffer = get_buffer(source_alias, source_device_id);
        cl_mem destination_buffer = get_buffer(destination_alias, destination_device_id);
        cl_int source_read_status = 0;
        source_read_status = clEnqueueReadBuffer(source_device_queue, source_buffer, CL_FALSE,
                                                 source_offset * sizeof(int), size * sizeof(int),
                                                 intermediate_result, 0, nullptr, nullptr);
        clFinish(source_device_queue);
        checkError(source_read_status,
                   "error while reading data from buffer for " + source_alias + " offset:" + to_string(source_offset));
        cl_int destination_write_status = 0;
        destination_write_status = clEnqueueWriteBuffer(destination_device_queue, destination_buffer, CL_FALSE,
                                                        destination_offset * sizeof(int),
                                                        size * sizeof(int),
                                                        intermediate_result, 0, nullptr, nullptr);
        clFinish(destination_device_queue);
        checkError(destination_write_status, "error while writing data to sub buffer for " + destination_alias);

    }

    void copy_data_from_memory(unsigned int *memory_data, int memory_offset, int size, const string &destination_alias,
                               int destination_device_index, int destination_offset) {
        unsigned int *intermediate_result = &memory_data[0] + memory_offset;
        cl_command_queue destination_device_queue = ng_execution_devices[destination_device_index].transfer_queue;
        cl_device_id destination_device_id = ng_execution_devices[destination_device_index].device_id;
        //check whether both source data and destination buffer present in respective devices
        cl_mem destination_buffer = get_buffer(destination_alias, destination_device_id);
        cl_int destination_write_status = 0;
        destination_write_status = clEnqueueWriteBuffer(destination_device_queue, destination_buffer, CL_FALSE,
                                                        destination_offset * sizeof(int),
                                                        size * sizeof(int),
                                                        intermediate_result, 0, nullptr, nullptr);
        clFinish(destination_device_queue);
        checkError(destination_write_status,
                   " copy_data_from_memory error while writing data to sub buffer for " + destination_alias);

    }

    string create_sub_buffer(const string &buffer_alias, int device_index, int offset, size_t size,
                             bool is_throw_error_if_buffer_present) {
        cl_int error_code = 0;
        cl_device_id device_id = ng_execution_devices[device_index].device_id;
        //check whether buffer is present
        if (is_buffer_present(buffer_alias, device_id)) {
            string sub_buffer_alias = compute_alias_name_for_offset(buffer_alias, offset);
            //if already present erase key as old sub buffers may have different region sizes
            if (is_buffer_present(sub_buffer_alias, device_id)) {
                data_dictionary.erase(make_pair(sub_buffer_alias, device_id));
            }
            cl_buffer_region region;
            region.origin = offset * sizeof(int);
            region.size = size * sizeof(int);
            cl_mem buffer_memory = get_buffer(buffer_alias, device_id);
            cl_mem sub_buffer = clCreateSubBuffer(buffer_memory, CL_MEM_READ_WRITE, CL_BUFFER_CREATE_TYPE_REGION,
                                                  &region, &error_code);
            checkError(error_code, "Error while creating sub buffer :" + buffer_alias);
            data_dictionary.insert(make_pair(make_pair(sub_buffer_alias, device_id), sub_buffer));
            return sub_buffer_alias;

        } else {
            cout << ng_summarizer.str() << flush;
            cerr << "invalid code flow buffer with alias " << buffer_alias << " not present" << " device "
                 << device_index << endl;
            exit(1);
        }
    }


    string print_data(const string &name, int device_index, size_t data_size, bool flag_bits) {
        stringstream stream;
        cl_device_id device = ng_execution_devices[device_index].device_id;
        cl_command_queue command_queue = data_queue[device].queue;
        err = 0;
        cl_mem argument_buffer = data_dictionary.find(make_pair(name, device))->second;

        cl_event read_event;

        auto *result = new unsigned int[data_size];
        err = clEnqueueReadBuffer(command_queue, argument_buffer, CL_TRUE, 0, data_size * sizeof(unsigned int), result,
                                  0, nullptr, &read_event);

        clFinish(command_queue);
        clFlush(command_queue);
        int count = 1;
        if (flag_bits) {
            for (int i = 0; i < data_size; i++) {
                std::bitset<32> x(result[i]);
                for (int j = 0; j < 32; j++) {
                    if (x[j] == 1) {
                        stream << ((i * 32) + j) << " ";
                        if (count % 20 == 0) {
                            stream << endl;
                        }
                        count++;
                    }
                }
            }
        } else {
            for (int i = 0; i < data_size; i++) {
                stream << (int) result[i] << "  ";
                if (count % 20 == 0) {
                    stream << endl;
                }
                count++;
            }
        }
        return stream.str();
    }
}

#endif //DISPATCHER_DATA_PARALLELISM_H
