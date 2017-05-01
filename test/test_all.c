#include "influxdb.h"

int main() 
{
    int status;
    s_influxdb_string outstr;	
    s_influxdb_client *client = influxdb_client_new("localhost:8086", "root", "root", "mydb", 0);
    
    /*create db*/
    status = influxdb_create_database(client, "mydb");	
	printf("status=%d\n",status);
    /*do insert*/
    status = influxdb_insert(client,"cpu_load,host=server_1,region=us-west value=0.2");		
	printf("status : %d\n",status);
    
    /*do query*/
    influxdb_query(client,"select * from cpu_load limit 10",&outstr);
	printf("%s\n",outstr.ptr);
    
    /*delete db*/
    status = influxdb_delete_database(client,"mydb");	
	printf("status=%d\n",status);
    
    influxdb_client_free(client);	
    return 0;
}

