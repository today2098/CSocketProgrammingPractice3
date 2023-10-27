message("[debug] current looking path: ${CMAKE_CURRENT_SOURCE_DIR}")

add_executable(file_server-server.out file_server-server.c)
target_link_libraries(file_server-server.out my_library)

add_executable(file_server-client.out file_server-client.c)
target_link_libraries(file_server-client.out my_library)
