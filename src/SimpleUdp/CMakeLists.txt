message("[debug] current looking path: ${CMAKE_CURRENT_SOURCE_DIR}")

add_executable(simple_udp-receiver.out receiver.c)
add_executable(simple_udp-sender.out sender.c)
add_executable(simple_udp-sender_nr0.out sender_nr0.c)
add_executable(simple_udp-sender_nr.out sender_nr.c)
