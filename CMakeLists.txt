cmake_minimum_required(VERSION 3.8)
project(Hero)

set(CMAKE_CXX_STANDARD 14)

add_executable(Hero

        # global files are added here

        main.cpp
        init.hpp
        globals.cpp
        globals.hpp
        headers.hpp
        utilities.hpp

        ## cpp layer files are added here
        cpp_layer/cpp_globals.hpp
        cpp_layer/cpp_globals.cpp

        cpp_layer/initialize.hpp
        cpp_layer/data_interface.hpp
        cpp_layer/execution_interface.hpp

        cpp_layer/operations/primitives.cpp
        cpp_layer/operations/primitives.hpp

        cpp_layer/tests/test_cpp_execution.cpp
        cpp_layer/tests/test_cpp_execution.hpp

        ## Opencl layer files are added here
        opencl_layer/initialize.hpp
        opencl_layer/environment.hpp
        opencl_layer/openCL_globals.cpp
        opencl_layer/openCL_globals.hpp
        opencl_layer/data_interface.hpp
        opencl_layer/kernel_interface.hpp
        opencl_layer/execution_interface.hpp

        opencl_layer/classes/devices.hpp
        opencl_layer/classes/devices.cpp
        opencl_layer/classes/platforms.hpp
        opencl_layer/classes/platforms.cpp

        opencl_layer/tests/test_openCL_execution.cpp
        opencl_layer/tests/test_openCL_execution.hpp

        # Task layer files are added here
        task_layer/task_globals.cpp
        task_layer/task_globals.hpp
        task_layer/initialize.hpp
        task_layer/classes/task.hpp
        task_layer/classes/task.cpp
        task_layer/classes/data_pack.cpp
        task_layer/classes/data_pack.hpp
        task_layer/classes/kernel_pack.cpp
        task_layer/classes/kernel_pack.hpp

        task_layer/tests/test_task_execution.cpp
        task_layer/tests/test_task_execution.hpp

        # TPCH data store and load files are added here
        TPCH/initialize.hpp
        TPCH/tpch_globals.cpp
        TPCH/tpch_globals.hpp

        TPCH/common/algebra/Operators.hpp
        TPCH/common/algebra/Operators.cpp
        TPCH/common/algebra/Pipeline.hpp
        TPCH/common/algebra/Pipeline.cpp
        TPCH/common/algebra/Types.hpp
        TPCH/common/algebra/Types.cpp

        TPCH/common/runtime/Import.hpp
        TPCH/common/runtime/Import.cpp
        TPCH/common/runtime/Database.hpp
        TPCH/common/runtime/Database.cpp
        TPCH/common/runtime/Barrier.hpp
        TPCH/common/runtime/Concurrency.hpp
        TPCH/common/runtime/Concurrency.cpp
        TPCH/common/runtime/Hash.hpp
        TPCH/common/runtime/Hashmap.hpp
        TPCH/common/runtime/Hashmap.cpp
        TPCH/common/runtime/Memory.hpp
        TPCH/common/runtime/MemoryPool.hpp
        TPCH/common/runtime/MemoryPool.cpp
        TPCH/common/runtime/Mmap.hpp
        TPCH/common/runtime/PartitionedDeque.hpp
        TPCH/common/runtime/Profile.cpp
        TPCH/common/runtime/Query.hpp
        TPCH/common/runtime/SIMD.hpp
        TPCH/common/runtime/String.hpp
        TPCH/common/runtime/String.cpp
        TPCH/common/runtime/Stack.hpp
        TPCH/common/runtime/Types.hpp
        TPCH/common/runtime/Types.cpp
        TPCH/common/runtime/Util.hpp

        TPCH/common/Compat.hpp
        TPCH/common/defs.hpp
        TPCH/common/Util.hpp

        # primitive graphs layer files are files
        runtime_layer/classes/primitive_node.cpp
        runtime_layer/classes/primitive_node.hpp
        runtime_layer/classes/primitive_graph.cpp
        runtime_layer/classes/primitive_graph.hpp

        runtime_layer/morsel_driven/exchange.cpp
        runtime_layer/morsel_driven/exchange.hpp

        runtime_layer/runtime_globals.cpp
        runtime_layer/runtime_globals.hpp
        runtime_layer/initialize.hpp

        runtime_layer/tests/test_graph.cpp
        runtime_layer/tests/test_graph.hpp
        runtime_layer/tests/test_exchange_graph.cpp
        runtime_layer/tests/test_exchange_graph.hpp

        runtime_layer/tests/tpch_queries/query6.cpp
        runtime_layer/tests/tpch_queries/query6.hpp
        runtime_layer/tests/tpch_queries/query1.cpp
        runtime_layer/tests/tpch_queries/query1.hpp

        # RM added files
        src/include/hashing.hpp
        src/hashing.cpp
        src/hashArray.cpp
        runtime_layer/tests/tpch_queries/queryHashJoin.cpp
        runtime_layer/tests/tpch_queries/queryHashJoin.hpp

        )

FIND_PACKAGE(Boost 1.58 COMPONENTS system chrono filesystem)
FIND_PACKAGE(OpenCL)
FIND_PACKAGE(Threads)


INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${OpenCL_INCLUDE_DIR} )

LINK_DIRECTORIES(/usr/local/cuda-8.0/include)

TARGET_LINK_LIBRARIES( Hero LINK_PUBLIC ${Boost_LIBRARIES} ${OpenCL_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
