CC = gcc
CFLAGS = -g -O2 -Wall
LIBS = -lcurl
INFLUXAPI=../src/

all:
	make influxdbLib
	make test_createdb
	make test_deletedb
	make test_insertdata
	make test_deletedata
	make test_querydata

influxdbLib:
	$(CC) -c $(INFLUXAPI)influxdb.c -o $(INFLUXAPI)influxdb.o
	ar rcs $(INFLUXAPI)libinfluxdb.a $(INFLUXAPI)influxdb.o
	
test_createdb:test_createdb.o
	$(CC) -o test_createdb test_createdb.o $(LIBS) -linfluxdb -L$(INFLUXAPI)

test_deletedb:test_deletedb.o
	$(CC) -o test_deletedb test_deletedb.o $(LIBS) -linfluxdb -L$(INFLUXAPI)

test_insertdata:test_insertdata.o
	$(CC) -o test_insertdata test_insertdata.o $(LIBS) -linfluxdb -L$(INFLUXAPI)

test_deletedata:test_deletedata.o
	$(CC) -o test_deletedata test_deletedata.o $(LIBS) -linfluxdb -L$(INFLUXAPI)

test_querydata:test_querydata.o
	$(CC) -o test_querydata test_querydata.o $(LIBS) -linfluxdb -L$(INFLUXAPI)

clean:
	rm -rf $(INFLUXAPI)libinfluxdb.a
	rm -rf $(INFLUXAPI)influxdb.o
	rm -rf test_createdb test_deletedb test_insertdata test_deletedata test_querydata
	rm -f *.o

.c.o:
	$(CC) $(CFLAGS) -c -o $*.o $< -I$(INFLUXAPI)


