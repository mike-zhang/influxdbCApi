#include "influxdb.h"

int main() 
{
    int status;
    s_influxdb_client *client = influxdb_client_new("localhost:8086", "root", "root", "", 0);
    status = influxdb_create_database(client, "mydb");	
	printf("status=%d\n",status);
    influxdb_client_free(client);	
    return 0;
}
