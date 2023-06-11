cmake_minimum_required(VERSION 3.25)
project(cpp)

if (OPENSSL_FOUND)

    init_target (print_client_tls)

    build_executable (${TARGET_NAME} ${SOURCE_FILES} ${HEADER_FILES})

    link_boost ()
    link_openssl()
    final_target ()

    set_target_properties(${TARGET_NAME} PROPERTIES FOLDER "examples")

endif()
include_directories(libs/websocketpp)
include_directories(libs/json/include)
add_executable(cpp main.cpp)
