message("[debug] current looking path: ${CMAKE_CURRENT_SOURCE_DIR}")

add_executable(simple_multicast-receiver.out receiver.c)
add_executable(simple_multicast-receiver_oldapi.out receiver_oldapi.c)
add_executable(simple_multicast-sender.out sender.c)
