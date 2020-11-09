main:
	gcc -c ip_checksum.c
	ar -rc libip_checksum.a ip_checksum.o
	gcc prodcon.c -L. -lip_checksum -o prodcon -lrt -lpthread
