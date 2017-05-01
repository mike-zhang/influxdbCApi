#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdint.h>
#include "influxdb.h"

#define N 10000

int main(void)
{
	int t,i,j;
	int64_t tstamp = 1422568543702900257;
	
	s_influxdb_client *client = influxdb_client_new("localhost:8086","root","root","mydb",0);

	t=0;
	for(i=0;i<20;i++)
	{
		char buf1[N * 512]={0},response[10][1024]={0};
		int status=0;
		for(j=0;j<N;j++,t++)
		{
			char buf2[512]={0};			
			if(j>0) 
				strcat(buf1,"\n");
			sprintf(buf2,"cpu_load,host=server_%d,region=us-west value=0.%d %lld",j,t%100,tstamp);
			if(j%100 == 1) tstamp += 1;
			//printf("buf2 : %s\n",buf2);			
			strcat(buf1,buf2);
		}
		status = influxdb_insert(client,buf1);		
		printf("status : %d\n",status);
	}	
	
	return 0;
}

