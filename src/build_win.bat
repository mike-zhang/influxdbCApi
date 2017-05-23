gcc -static -c -DCURL_STATICLIB influxdb.c -I/thrdLibs/curl-7.51.0/include  
ar rcs libinfluxdb.a influxdb.o
rm -rf *.o

