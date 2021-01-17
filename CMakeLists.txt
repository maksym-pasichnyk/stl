cmake_minimum_required(VERSION 3.15)
project(rosy)

set(CMAKE_CXX_STANDARD 20)

add_library(stl INTERFACE)
target_include_directories(stl INTERFACE include)
target_sources(stl INTERFACE
    include/net/addr.hpp
    include/net/ip.hpp
    include/net/udp.hpp
    include/net/tcp.hpp
    include/alloc.hpp
    include/layout.hpp
    include/non_null.hpp
    include/option.hpp
#    include/overload.hpp
    include/raw_vec.hpp
    include/stdint.hpp
    include/unique.hpp
    include/drop.hpp
    include/vec.hpp
)
