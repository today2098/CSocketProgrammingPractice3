message("[debug] current looking path: ${CMAKE_CURRENT_SOURCE_DIR}")

add_executable(udp_file_transfer-receiver.out receiver.c)
target_link_libraries(udp_file_transfer-receiver.out my_library)

add_executable(udp_file_transfer-sender.out sender.c)
target_link_libraries(udp_file_transfer-sender.out my_library)

add_executable(udp_file_transfer-sender_nr.out sender_nr.c)
target_link_libraries(udp_file_transfer-sender_nr.out my_library)
