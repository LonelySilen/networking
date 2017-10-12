
all: epoll epoll_client select

epoll: epoll.c
	gcc -o epoll epoll.c

epoll_client: epoll_client.c
	gcc -o epoll_client epoll_client.c

select: select.c
	gcc -o select select.c

clean:
	rm -f epoll epoll_client select
