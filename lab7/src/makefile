CC=gcc
CFLAGS=-I.

all : tcpclient tcpserver udpclient udpserver

tcpclient :
	$(CC) tcpclient.c -o tcpclient $(CFLAGS)

tcpserver :
	$(CC) tcpserver.c -o tcpserver $(CFLAGS)

udpclient :
	$(CC) udpclient.c -o udpclient $(CFLAGS)

udpserver :
	$(CC) udpserver.c -o udpserver $(CFLAGS)

clean :
	rm -f udpclient udpserver tcpserver tcpclient