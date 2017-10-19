
#include <arpa/inet.h>
#include <netinet/sctp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

const uint16_t kServerPort = 4321;

static void Echo(FILE* input, int sock_fd, struct sockaddr* server_addr,
                 socklen_t server_addr_len) {
  struct sockaddr_in peer_addr;
  struct sctp_sndrcvinfo sri;
  const ssize_t kBufSize = 512;
  char send_buf[kBufSize], recv_buf[kBufSize];
  socklen_t peer_addr_len;
  size_t n_recv, n_read, n_send;
  int msg_flags;

  memset(&sri, 0, sizeof(sri));
  while (fgets(send_buf, kBufSize, input) != NULL) {
    if (send_buf[0] != '[') {
      printf("Error, line must be of the form '[streamnum] text '\n");
      continue;
    }

    sri.sinfo_stream = strtol(&send_buf[1], NULL, 0);
    n_read = strlen(send_buf);
    n_send = sctp_sendmsg(sock_fd, send_buf, n_read, server_addr,
                          server_addr_len, 0, 0, sri.sinfo_stream, 0, 0);
    if (n_send == -1) {
      perror("sctp_sendmsg()");
      continue;
    }

    if (n_read != n_send) {
      fprintf(stderr, "read buffer not fully sent: %ld != %ld\n",
              n_read, n_send);
    }

    peer_addr_len = sizeof(peer_addr);
    n_recv = sctp_recvmsg(sock_fd, recv_buf, kBufSize,
        (struct sockaddr*) &peer_addr, &peer_addr_len, &sri, &msg_flags);
    printf("From str:%d seq: %d (assoc:0x%x):",
           sri.sinfo_stream, sri.sinfo_ssn, (u_int) sri.sinfo_assoc_id);
    printf("%d.%s", n_read, recv_buf);
  }
}

static void EchoAll(FILE* input, int sock_fd, struct sockaddr* server_addr,
                    socklen_t addr_len) {
}

int main(int argc, char** argv) {
  int sock_fd;
  struct sockaddr_in server_addr;
  struct sctp_event_subscribe events;
  int echo_to_all = 0;
  int status;

  if (argc < 2) {
    fprintf(stderr, "Missing host argument - use '%s host [echo] '\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  if (argc > 2) {
    printf("Echoing message to all streams\n");
    echo_to_all = 1;
  }

  sock_fd = socket(AF_INET, SOCK_SEQPACKET, IPPROTO_SCTP);
  if (sock_fd == -1) {
    perror("socket()");
    exit(EXIT_FAILURE);
  }

  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(kServerPort);
  status = inet_pton(AF_INET, argv[1], &server_addr.sin_addr);
  if (status == -1) {
    perror("inet_pton()");
    exit(EXIT_FAILURE);
  }

  memset(&events, 0, sizeof(events));
  events.sctp_data_io_event = 1;
  status = setsockopt(sock_fd, IPPROTO_SCTP, SCTP_EVENTS,
                      &events, sizeof(events));
  if (status == -1) {
    perror("setsockopt()");
    exit(EXIT_FAILURE);
  }

  if (echo_to_all == 0) {
    Echo(stdin, sock_fd, (struct sockaddr*) &server_addr, sizeof(server_addr));
  } else {
    EchoAll(stdin, sock_fd, (struct sockaddr*) &server_addr,
            sizeof(server_addr));
  }

  close(sock_fd);

  exit(EXIT_SUCCESS);
}
