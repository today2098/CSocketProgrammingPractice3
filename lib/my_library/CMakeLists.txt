add_library(my_library SHARED handle_error.h handle_error.c directory.h get_socket_address.h get_socket_address.c)
target_include_directories(my_library PUBLIC ${PROJECT_SOURCE_DIR}/lib)
