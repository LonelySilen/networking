
#include "sctp_helper.h"

#include <string.h>

sctp_assoc_t sctp_address_to_associd(int sock_fd, struct sockaddr* sa,
                                     socklen_t salen) {
	struct sctp_paddrparams sp;
	int siz;

	siz = sizeof(struct sctp_paddrparams);
	memset(&sp, 0, siz);
	memcpy(&sp.spp_address, sa, salen);
	sctp_opt_info(sock_fd, 0, SCTP_PEER_ADDR_PARAMS, &sp, &siz);
	return sp.spp_assoc_id;
}

int sctp_get_no_strms(int sock_fd, struct sockaddr* to, socklen_t tolen) {
	int retsz;
	struct sctp_status status;
	retsz = sizeof(status);	
	memset(&status, 0, sizeof(status));

	status.sstat_assoc_id = sctp_address_to_associd(sock_fd,to,tolen);
	getsockopt(sock_fd, IPPROTO_SCTP, SCTP_STATUS, &status, &retsz);
	return status.sstat_outstrms;
}
