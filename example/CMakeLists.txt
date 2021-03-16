# add_executable(SIMPLE_TIME time/time_server.cc time/time_server_main.cc)



file( GLOB time_server_source time_server/*.cc )
add_executable( time_server ${time_server_source} )
target_link_libraries(time_server base_lib)
target_link_libraries(time_server net_lib)


file( GLOB pingpong_server_source pingpong/pingpong_server.cc )
add_executable( pingpong_server ${pingpong_server_source} )
target_link_libraries(pingpong_server base_lib)
target_link_libraries(pingpong_server net_lib)


file( GLOB pingpong_client_source pingpong/pingpong_client.cc )
add_executable( pingpong_client ${pingpong_client_source} )
target_link_libraries(pingpong_client base_lib)
target_link_libraries(pingpong_client net_lib)
