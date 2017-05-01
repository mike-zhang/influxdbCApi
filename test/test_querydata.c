#include <stdio.h>
#include "influxdb.h"

int main(void)
{		
	s_influxdb_string outstr;	
	s_influxdb_client *client = influxdb_client_new("localhost:8086","root","root","ctest1",0);
	influxdb_query(client,"select * from cpu_load limit 10",&outstr);
	printf("%s\n",outstr.ptr);

	return 0;
}

