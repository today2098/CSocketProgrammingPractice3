message("[debug] current looking path: ${CMAKE_CURRENT_SOURCE_DIR}")

add_executable(simple_multicast-sender.out sender.c)
target_link_libraries(simple_multicast-sender.out my_library)

add_executable(simple_multicast-receiver_oldapi.out receiver_oldapi.c)
target_link_libraries(simple_multicast-receiver_oldapi.out my_library)

# add_executable(simple_multicast-receiver.out receiver.c)
# target_link_libraries(simple_multicast-receiver.out my_library)
