
file(GLOB ALL_NET_FILES
    "include/*.hh",
    "source/*.cc",
)

message("NET_LIB: ${ALL_NET_FILES}")

add_library(net_lib ${ALL_NET_FILES})

# target_link_libraries(net_lib PUBLIC spdlog::spdlog)