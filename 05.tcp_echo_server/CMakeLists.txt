
cmake_minimum_required(VERSION 3.0)
project(tcp_echo_demo)

add_executable(echo-server
    echo_server.c
    common.c
    common.h)

add_executable(echo-client
    echo_client.c
    common.c
    common.h)
