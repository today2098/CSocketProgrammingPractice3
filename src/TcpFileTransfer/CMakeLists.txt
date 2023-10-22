add_executable(tcp_file_transfer-server.out protocol.h server.c)
target_link_libraries(tcp_file_transfer-server.out my_library)

add_executable(tcp_file_transfer-client.out protocol.h client.c)
target_link_libraries(tcp_file_transfer-client.out my_library)
