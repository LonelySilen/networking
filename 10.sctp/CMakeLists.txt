
add_executable(10.sctp-client sctp_client.c)
target_link_libraries(10.sctp-client -lsctp)

add_executable(10.sctp-server sctp_server.c sctp_helper.c)
target_link_libraries(10.sctp-server -lsctp)