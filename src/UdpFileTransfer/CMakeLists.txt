add_executable(udp_file_transfer-receiver.out protocol.h receiver.c)
target_link_libraries(udp_file_transfer-receiver.out my_library)

add_executable(udp_file_transfer-sender.out protocol.h sender.c)
target_link_libraries(udp_file_transfer-sender.out my_library)
