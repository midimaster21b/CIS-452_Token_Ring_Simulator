
all:
	gcc -Wall token_ring.c endpoint.c message.c -o token_ring -lpthread
