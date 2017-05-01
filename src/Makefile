CC = gcc
CFLAGS = -g -O2 -Wall
LIBS = -lcurl

all:
	make influxdbLib	

influxdbLib:influxdb.o 
	ar rcs libinfluxdb.a influxdb.o 
	
clean:
	rm -rf libinfluxdb.a
	rm -rf influxdb.o	
	rm -f *.o

.c.o:
	$(CC) $(CFLAGS) -c -o $*.o $<


