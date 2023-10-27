message("[debug] current looking path: ${CMAKE_CURRENT_SOURCE_DIR}")

add_executable(tcp_file_transfer-server.out server.c)
target_link_libraries(tcp_file_transfer-server.out my_library)

add_executable(tcp_file_transfer-client.out client.c)
target_link_libraries(tcp_file_transfer-client.out my_library)

add_executable(tcp_file_transfer-client_nr.out client_nr.c)
target_link_libraries(tcp_file_transfer-client_nr.out my_library)
