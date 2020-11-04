//
// Created by gurumurt on 29/04/20.
//

#ifndef HERO_PRIMITIVE_GRAPH_HPP
#define HERO_PRIMITIVE_GRAPH_HPP

#include "../../headers.hpp"
//#include "../../json.hpp"
#include <boost/graph/breadth_first_search.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/properties.hpp>
#include <boost/pending/indirect_cmp.hpp>
#include <boost/range/irange.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/topological_sort.hpp>
#include <boost/lockfree/queue.hpp>

#include "primitive_node.hpp"

namespace runtime_layer{

    /**
     * Additional information about the edges joining primitive nodes
     */
    class data_edge{

    public:
        int data_pointer=0;    ///alias of data
        size_t data_size=1;    /// size of data
        bool ready_for_next_step=false;   ///set to true when the data is available for processing

        /**
         * These are information for exchange edge to run
         */
        int input_ID_target{-1}; //input ID in the execution device
        int input_ID_source{-1}; //input ID in the data source device
        int source_device_index{-1}; //ID of input device
        int target_device_index{-1}; //ID of target device

        bool input_EOD{false}; //Is the end of data reached

        bool is_virtual_vertex{false}; //True one edge of the vertex is virtual vertex
        bool is_work_shared{false};//Flag to switch to data parallelism

        //these vectors will have more than one value if data parallelism is working
        vector<size_t> processed_until{0};//idx until which data is processed in the execution device
        vector<size_t> fetched_until{0};//idx until which data is copied from source to target
    };

    typedef boost::adjacency_list<boost::vecS,boost::vecS,boost::bidirectionalS,primitive_node, data_edge> execution_graph;
    typedef boost::graph_traits<execution_graph>::vertex_descriptor QEP_vertex;
    typedef boost::graph_traits<execution_graph>::edge_descriptor QEP_edge;
    typedef std::list<QEP_vertex> MakeOrder;
    typedef boost::graph_traits<execution_graph>::in_edge_iterator incoming_edges;
    typedef boost::graph_traits<execution_graph>::out_edge_iterator outgoing_edges;
    typedef boost::graph_traits<execution_graph>::edge_iterator edge_it;

    /**
     * Class to capture the nodes and bundle them into a single query graph
     */
    class query_graph{

    public:

        short execution_environment{0};

        query_graph();

        execution_graph dag;    /// query graph variable
        vector<QEP_vertex> process_queue; /// set of primitive nodes to be processed in sequence
        QEP_vertex start_vertex, end_vertex; /// an arbitrary start and end node signifying data scan and display

        /**
         * prepares the execution graph from json document. NOT IMPLEMENTED
         * @param json_file
         */
        void prepare_graph(string json_file);

        /**
         * uses Breadth-First Search for flattening
         */
        void bfs_flatten();


        /**
         * uses topological sorting for flattening
         */
        void tps_flatten();

        /**
         * flattens the given graph into sequence of primitives to execute
         */
        void form_processing_queue();

        /**
         * sets input data for the primitive node
         * @param v primitive node to execute
         */
        void set_input(QEP_vertex v);

        /**
         * Adds data into device tagged to the primitive node
         * @param v primitive node to execute
         */
        void load_data(QEP_vertex v);
        /**
         * fetches data from device and adds it into @results global variable
         */
        void display_result();
        /**
         * creates empty data for adding results from the primitive after processing
         * @param v primitive node to execute
         */
        void prepare_output_buffer(QEP_vertex v);
        /**
         * executes the complete graph on the target devices of the respective primitives in the sequence from flattening the graph
         */
        void execute_graph();
    };


}
#endif //HERO_PRIMITIVE_GRAPH_HPP
