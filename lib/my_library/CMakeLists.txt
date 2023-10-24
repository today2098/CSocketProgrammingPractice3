add_library(my_library SHARED die_with_system_message.h die_with_system_message.c directory.h print_socket_address.h print_socket_address.c)
target_include_directories(my_library PUBLIC ${PROJECT_SOURCE_DIR}/lib)
