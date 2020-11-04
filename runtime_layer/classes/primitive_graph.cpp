//
// Created by gurumurt on 29/04/20.
//

#include "primitive_graph.hpp"
#include "../../cpp_layer/data_interface.hpp"


runtime_layer::query_graph::query_graph(){

    primitive_node s_node;
    primitive_node e_node;

    s_node.operation.task_kernel.mKernelName = "data_scan";
    s_node.no_of_input_required  = 0;
    e_node.operation.task_kernel.mKernelName = "data_display";
    e_node.no_of_output_required = 0;

    start_vertex = add_vertex(s_node,this->dag);
    end_vertex = add_vertex(e_node,this->dag);
}

using namespace boost;
template < typename TimeMap > class bfs_time_visitor:public default_bfs_visitor {
    typedef typename property_traits < TimeMap >::value_type T;
public:
    bfs_time_visitor(TimeMap tmap, T & t):m_timemap(tmap), m_time(t) { }
    template < typename Vertex, typename Graph >
    void discover_vertex(Vertex u, const Graph & g) const
    {
        put(m_timemap, u, m_time++);
    }
    TimeMap m_timemap;
    T & m_time;
};


void runtime_layer::query_graph::tps_flatten(){

    deque<QEP_vertex> topologicalSorted;
    boost::topological_sort(this->dag,front_inserter(topologicalSorted));

    int N = num_vertices(dag);
    cout<<"Printing queue"<<endl;
    cout<<"**************"<<endl;
    for (int i = 0; i < N; ++i){

        cout<<dag[topologicalSorted[i]].operation.task_kernel.mKernelName<<endl;
        process_queue.push_back(topologicalSorted.at(i));
    }
    cout<<"**************"<<endl;
}

void runtime_layer::query_graph::bfs_flatten(){

    typedef graph_traits <execution_graph>::vertex_descriptor Vertex;
    typedef boost::graph_traits <execution_graph>::vertices_size_type Size;
    typedef Size* Iiter;

    int N = num_vertices(dag);

    // a vector to hold the discover time property for each vertex
    std::vector < Size > dtime(num_vertices(dag));

    Size time = 0;
    bfs_time_visitor < Size * >vis(&dtime[0], time);
    breadth_first_search(dag, start_vertex, visitor(vis));

    // Use std::sort to order the vertices by their discover time
    std::vector<boost::graph_traits<execution_graph>::vertices_size_type > discover_order(N);
    boost::integer_range < int >range(0, N);
    std::copy(range.begin(), range.end(), discover_order.begin());
    std::sort(discover_order.begin(), discover_order.end(),
              boost::indirect_cmp < Iiter, std::less < Size > >(&dtime[0]));

    cout<<"Printing queue"<<endl;
    cout<<"**************"<<endl;
    for (int i = 0; i < N; ++i){

            cout<<dag[discover_order[i]].operation.task_kernel.mKernelName<<endl;
            process_queue.push_back(discover_order[i]);
    }
    cout<<"**************"<<endl;
}


/**
 * Load data into OpenCL - function called when data_scan is encountered
 */
void runtime_layer::query_graph::load_data(QEP_vertex v){

    //Adding all data into device / in OpenCL
    this->dag[v].no_of_input_required = data_scan.size();

    for(auto d_ : data_scan){

        int assignedID = this->dag[v].place_data(d_.second.first,d_.second.second);
        this->dag[v].input_size = d_.second.second;

        outgoing_edges eo, eo_end;
        for(boost::tie(eo, eo_end) = out_edges(v,this->dag); eo!=eo_end; ++eo){

            if(this->dag[*eo].data_pointer == d_.first){

                this->dag[*eo].data_pointer = assignedID;
                this->dag[*eo].data_size = d_.second.second;
            }
        }
    }
}

void runtime_layer::query_graph::display_result() {

    incoming_edges ei, ei_end;

    for(boost::tie(ei,ei_end) = in_edges(this->end_vertex,dag); ei!=ei_end;++ei){

        int alias  = dag[*ei].data_pointer;
        int data_size = dag[*ei].data_size;
        int p = dag[source(*ei,dag)].pID;
        int d = dag[source(*ei,dag)].dID;

        //Updated for storing results from execution environment
        if(this->execution_environment)
            results.push_back((int*) cpp_layer::get_data(alias,data_size));
        else
            results.push_back((int*) openCL_layer::get_data(alias,p,d,data_size));
        total_result_size = data_size;
    }
}

/**
 * This does a naive execution
 *
 */
void runtime_layer::query_graph::execute_graph() {

    form_processing_queue();

    for(QEP_vertex v : process_queue){

        if(this->dag[v].operation.task_kernel.get_kernel_name() == "data_scan"){

            this->load_data(v);
            continue;
        }
        std::cout<<"test"<<endl;
        if(this->dag[v].operation.task_kernel.get_kernel_name() == "data_display"){

            this->display_result();
            continue;
        }

        //setting input for the node
        this->set_input(v);
        this->prepare_output_buffer(v);

        //execute
        this->dag[v].compile_task();
        this->dag[v].execute_node(this->execution_environment);
    }
}

void runtime_layer::query_graph::set_input(QEP_vertex v) {

    incoming_edges ei, ei_end;
    short incoming_size=0;
    for(boost::tie(ei,ei_end) = in_edges(v,dag); ei!=ei_end;++ei){

        //Check if the input device is different than execution device
        if((this->dag[source(*ei,dag)].pID != this->dag[target(*ei,dag)].pID)||
           (this->dag[source(*ei,dag)].dID != this->dag[target(*ei,dag)].dID)){

            openCL_layer::transfer_data(this->dag[*ei].data_pointer,
                                        this->dag[source(*ei,dag)].pID,
                                        this->dag[source(*ei,dag)].dID,
                                        this->dag[target(*ei,dag)].pID,
                                        this->dag[target(*ei,dag)].dID,
                                        this->dag[*ei].data_size);
        }

        this->dag[v].input_data.push_back(this->dag[*ei].data_pointer);
        this->dag[v].input_size = (this->dag[v].input_size < this->dag[*ei].data_size)?this->dag[*ei].data_size : this->dag[v].input_size;
        incoming_size++;
    }
}

void runtime_layer::query_graph::prepare_output_buffer(QEP_vertex v) {

    //preparing output
    int output_pointer = this->dag[v].prepare_output();
    size_t output_size = this->dag[v].overall_output_size;
    outgoing_edges eo, eo_end;
    for(boost::tie(eo,eo_end) = out_edges(v,this->dag); eo!=eo_end;++eo) {

        this->dag[*eo].data_pointer = output_pointer;
        this->dag[*eo].data_size = output_size;
    }
}

void runtime_layer::query_graph::form_processing_queue() {

    tps_flatten();
//    bfs_flatten();
}
