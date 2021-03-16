
file(GLOB ALL_NET_FILES
    "include/*.hh"
    "source/*.cc"
    "include/poll/*.hh"
    "source/poll/*.cc"
)

include_directories(../base/include)
find_library(base_lib REQUIRED)


add_library(net_lib ${ALL_NET_FILES})

target_link_libraries(net_lib PUBLIC base_lib)