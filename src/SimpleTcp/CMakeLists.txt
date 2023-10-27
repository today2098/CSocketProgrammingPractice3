message("[debug] current looking path: ${CMAKE_CURRENT_SOURCE_DIR}")

add_executable(simple_tcp-server.out server.c)
add_executable(simple_tcp-client.out client.c)
add_executable(simple_tcp-client_nr0.out client_nr0.c)
add_executable(simple_tcp-client_nr.out client_nr.c)
