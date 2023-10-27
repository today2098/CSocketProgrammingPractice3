message("[debug] current looking path: ${CMAKE_CURRENT_SOURCE_DIR}")

add_executable(multi_tcp_client-server.out server.c)
target_link_libraries(multi_tcp_client-server.out pthread my_library)  # pthreadを含む．

add_executable(multi_tcp_client-client_nr.out client_nr.c)
target_link_libraries(multi_tcp_client-client_nr.out my_library)
