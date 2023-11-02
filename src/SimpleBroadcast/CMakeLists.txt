message("[debug] current looking path: ${CMAKE_CURRENT_SOURCE_DIR}")

add_executable(simple_broadcast-sender.out sender.c)
target_link_libraries(simple_broadcast-sender.out my_library)

add_executable(simple_broadcast-receiver.out receiver.c)
target_link_libraries(simple_broadcast-receiver.out my_library)
