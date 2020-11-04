//
// Created by gurumurt on 01/05/20.
//

#ifndef HERO_EXCHANGE_HPP
#define HERO_EXCHANGE_HPP

#include "../../headers.hpp"
#include <thread>

#include <boost/graph/breadth_first_search.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/properties.hpp>
#include <boost/pending/indirect_cmp.hpp>
#include <boost/range/irange.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/topological_sort.hpp>
#include <boost/lockfree/queue.hpp>


#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/move/move.hpp>

#include <mutex>
#include <condition_variable>
#include "../classes/primitive_node.hpp"
#include "../classes/primitive_graph.hpp"


namespace morsel_driven{

    using namespace std;

    /**
     * Class to capture edge information. Not being used
     */
    class exchange_data_edge {

    public:
        int input_ID_target{-1}; //input ID in the execution device
        int input_ID_source{-1}; //input ID in the data source device
        int source_device_index{-1}; //ID of input device
        int target_device_index{-1}; //ID of target device

        bool input_EOD{false}; //Is the end of data reached
        size_t data_size{0};

        bool is_virtual_vertex{false}; //True one edge of the vertex is virtual vertex
        bool is_work_shared{false};//Flag to switch to data parallelism

        vector<size_t> processed_until{0};//idx until which data is processed in the execution device
        vector<size_t> fetched_until{0};//idx until which data is copied from source to target
    };


    /**
     * class to enable parallelism in query execution.
     * Provides 3 parallelisms across devices
     * 1) functional
     * 2) data
     * 3) pipelining
     */
    class exchange_query_graph : public runtime_layer::query_graph{

    public:


        std::mutex *m; /// mutex for isolation of thread access
        std::condition_variable *start_processing; /// variable indicating a node is ready to process. This is set only after the data is pushed into the target device
        bool *ready;    /// signals node is ready to be processed
        bool *processed;    //// signels node has been processed
        runtime_layer::QEP_vertex *process_vertex;  /// pointer to current processing vertex in target device

        boost::lockfree::queue<runtime_layer::QEP_vertex,boost::lockfree::capacity<20>> ex_processing_queue; /// A queue shared by multiple devices for parallel processing
        int *device_;   /// index to target devices
        size_t no_of_devices{0};    /// no. of devices running concurrently

        exchange_query_graph();

        /**
         * flattens query plan into sequence of primitive nodes. These can be shared across multiple devices in a lock-free fashion
         */
        void form_lockfree_processing_queue();
        /**
         * Executes given graph in with parallelism
         */
        void execute_exchange_graph();

        /**
         * processing function that pops a primitive node from shared queue and executes in respective device
         * @param i device idx
         */
        void process_in_device(int i);

        /**
         * starts execution of a primitive
         * @param i device idx
         */
        void process_op(short i);

        /**
         * starts data transfer for a primitive. During data transfer, threads spin until the data is ready to be processed (using @ready_for_next_step)
         * @param i device idx
         */
        void transfer_op(short i);
    };

}

#endif //HERO_EXCHANGE_HPP
