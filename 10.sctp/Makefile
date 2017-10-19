
all: sctp_client sctp_server

sctp_client: sctp_client.c
	gcc -o sctp_client sctp_client.c -lsctp

sctp_server: sctp_server.c sctp_helper.o
	gcc -o sctp_server sctp_server.c sctp_helper.o -lsctp

sctp_helper.o: sctp_helper.c
	gcc -c sctp_helper.c

clean:
	rm -rvf sctp_client sctp_server *.o
