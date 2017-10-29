
all: hostent getaddr getserv getaddrinfo

getaddrinfo: getaddrinfo.c
	gcc -o getaddrinfo -g getaddrinfo.c

hostent: hostent.c
	gcc -o hostent hostent.c

getaddr: getaddr.c
	gcc -o getaddr getaddr.c

getserv: getserv.c
	gcc -o getserv getserv.c

clean:
	rm -rvf hostent getaddr getserv getaddrinfo
