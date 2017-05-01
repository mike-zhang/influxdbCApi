#include <stdio.h>
#include "influxdb.h"

int main(void)
{	
	s_influxdb_client *client = influxdb_client_new("localhost:8086","root","root","ctest1",0);	
	influxdb_delete(client,"delete from cpu_load");	
	return 0;
}

