message("[debug] current looking path: ${CMAKE_CURRENT_SOURCE_DIR}")

add_library(my_library SHARED common_port.h directory.h handle_socket_address.c handle_error.c presentation_layer.c)
target_compile_options(my_library PUBLIC -Wall)
target_include_directories(my_library PUBLIC ${PROJECT_SOURCE_DIR}/lib)
