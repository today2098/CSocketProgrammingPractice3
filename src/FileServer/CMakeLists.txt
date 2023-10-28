message("[debug] current looking path: ${CMAKE_CURRENT_SOURCE_DIR}")

add_executable(file_server-server.out server.c)
target_link_libraries(file_server-server.out my_library)

add_executable(file_server-client_nr.out client_nr.c)
target_link_libraries(file_server-client_nr.out my_library)
