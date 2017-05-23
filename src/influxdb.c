#include <stdlib.h>
#include <string.h>

#include "influxdb.h"

char *influxdb_strdup(const char *s)
{
    char *d;

    if (s == NULL)
        return NULL;

    d = malloc(sizeof (char) * (strlen(s) + 1));
    if (d == NULL)
        return NULL;

    strcpy(d, s);
    return d;
}

void init_string(s_influxdb_string *s) 
{
	s->len = 0;
	s->ptr = (char*)malloc(s->len+1);
	if (s->ptr == NULL) 
	{
		fprintf(stderr, "malloc() failed\n");
		exit(EXIT_FAILURE);
	}
	s->ptr[0] = '\0';
}

size_t append_string(s_influxdb_string *s,char* append)
{
	size_t new_len = s->len;	
	if(append)
	{
		new_len = s->len + strlen(append);
		s->ptr = (char*)realloc(s->ptr,new_len+1);
		if(s->ptr == NULL)		
		{
			fprintf(stderr, "realloc() failed\n");
			new_len = 0;
		}	
		else
		{
			memcpy(s->ptr+s->len,append,strlen(append));
			s->ptr[new_len] = '\0';
			s->len = new_len;	
		}
	}
	return new_len;	
}

s_influxdb_client *influxdb_client_new(char *host,char *uname,char *password,char *dbname,char ssl)
{
    s_influxdb_client *client = malloc(sizeof (s_influxdb_client));

	if (!uname) uname = "";
	if (!password) password = "";
	if (!dbname)
	{
		free(client);
		client=NULL;
	}
	else 
	{
    	client->schema = ssl ? "https" : "http";
    	client->host = influxdb_strdup(host);
    	client->uname = influxdb_strdup(uname);
    	client->password = influxdb_strdup(password);
    	client->dbname = curl_easy_escape(NULL, dbname, 0);
    	client->ssl = ssl;
	}
    return client;
}

int influxdb_client_get_url_with_credential(
	s_influxdb_client *client,
	char (*buffer)[],
	size_t size,
	char *path,
	char *uname,
	char *password)
{
    char *uname_enc = curl_easy_escape(NULL, uname, 0);
    char *password_enc = curl_easy_escape(NULL, password, 0);

    (*buffer)[0] = '\0';
    strncat(*buffer, client->schema, size);
    strncat(*buffer, "://", size);
    strncat(*buffer, client->host, size);
    strncat(*buffer, path, size);
/*
    if (strchr(path, '?'))
        strncat(*buffer, "&", size);
    else
        strncat(*buffer, "?", size);

    strncat(*buffer, "u=", size);
    strncat(*buffer, uname_enc, size);
    strncat(*buffer, "&p=", size);
    strncat(*buffer, password_enc, size);
*/
    free(uname_enc);
    free(password_enc);

    return strlen(*buffer);
}

int	influxdb_client_get_url(
	s_influxdb_client *client,
	char (*buffer)[],
	size_t size,
	char *path)
{
    return influxdb_client_get_url_with_credential(client, buffer, size, path,
                                                   client->uname,
                                                   client->password);
}

size_t influxdb_client_write_data(
	char *buf,
	size_t size,
	size_t nmemb,
	void *userdata)
{
    size_t realsize = size * nmemb;
    if (userdata != NULL)
    {
        char **buffer = userdata;
        *buffer = realloc(*buffer, strlen(*buffer) + realsize + 1);
        strncat(*buffer, buf, realsize);
    }
    return realsize;
}

int influxdb_client_curl(char *url,char *reqtype,char *body,char **response)
{
    CURLcode c;
    CURL *handle = curl_easy_init();

    if (reqtype != NULL)
        curl_easy_setopt(handle, CURLOPT_CUSTOMREQUEST, reqtype);
    curl_easy_setopt(handle, CURLOPT_URL, url);
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION,
                     influxdb_client_write_data);
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, response);
    if (body != NULL)
        curl_easy_setopt(handle, CURLOPT_POSTFIELDS,
                         body);

    c = curl_easy_perform(handle);

    if (c == CURLE_OK)
    {
        long status_code = 0;
        if (curl_easy_getinfo(handle, CURLINFO_RESPONSE_CODE,
                              &status_code) == CURLE_OK)
            c = status_code;
    }

    curl_easy_cleanup(handle);
    return c;
}

size_t writefunc(void *ptr,size_t size,size_t nmemb,s_influxdb_string *s)
{
	size_t new_len = s->len + size*nmemb;
	s->ptr = (char*)realloc(s->ptr, new_len+1);
	if (s->ptr == NULL) 
	{
		fprintf(stderr, "realloc() failed\n");
		exit(EXIT_FAILURE);
	}
	memcpy(s->ptr+s->len, ptr, size*nmemb);
	s->ptr[new_len] = '\0';
	s->len = new_len;
	return size*nmemb;
}

int get_response_body(char* url,s_influxdb_string *outstr)
{
	CURL *curl;
	CURLcode res;

	curl = curl_easy_init();
	if(curl) 
	{
		init_string(outstr);

		curl_easy_setopt(curl, CURLOPT_URL,url);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA,outstr);
		res = curl_easy_perform(curl);
		/* always cleanup */
		curl_easy_cleanup(curl);
	}
	return 0;
}

int influxdb_client_get(s_influxdb_client *client,char *path,char **res)
{
    int status;
    char url[INFLUXDB_URL_MAX_SIZE];
    char *buffer = calloc(1, sizeof (char));

    influxdb_client_get_url(client, &url, INFLUXDB_URL_MAX_SIZE, path);

    status = influxdb_client_curl(url, NULL, NULL, &buffer);

    if (status >= 200 && status < 300 && res != NULL)
        *res = buffer;
    //    *res = json_tokener_parse(buffer);

    free(buffer);
	//printf("influxdb_client_get , status : %d\n",status);
    return status;
}

int influxdb_client_post(s_influxdb_client *client,char *path,char *body,char **res)
{
    int status;
    char url[INFLUXDB_URL_MAX_SIZE];
    char *buffer = calloc(1, sizeof (char));

    influxdb_client_get_url(client, &url, INFLUXDB_URL_MAX_SIZE, path);

    status = influxdb_client_curl(url, NULL, body, &buffer);

    if (status >= 200 && status < 300 && res != NULL)
        *res = buffer; 

    free(buffer);
	//printf("influxdb_client_post , status : %d\n",status);
    return status;
}

void influxdb_client_free(s_influxdb_client *client)
{
    if (client)
    {
        curl_free(client->uname);
        curl_free(client->password);
        curl_free(client->dbname);
        free(client->host);
        free(client);
    }
}

int influxdb_create_database(s_influxdb_client *client, char *dbname)
{
    int c;
	char buf[1024]={0};
	sprintf(buf,"q=CREATE DATABASE %s",dbname);
    c = influxdb_client_post(client, "/query",buf, NULL);
    return c;
}

int influxdb_delete_database(s_influxdb_client *client, char *dbname)
{
    int c;
	char buf[1024]={0};
	sprintf(buf,"q=DROP DATABASE %s",dbname);
	c = influxdb_client_post(client, "/query",buf, NULL);
	return c;
}

int influxdb_insert(s_influxdb_client *client,char *query)
{
	int c=-1;
	char path[INFLUXDB_URL_MAX_SIZE]={0};		
	if(client && query)
	{
		sprintf(path,"/write?db=%s",client->dbname);	
		c=influxdb_client_post(client,path,query,NULL);
	}
	//printf("influxdb_insert ,c = %d\n",c);
	return c == 204;
}

int influxdb_delete(s_influxdb_client *client,char *query)
{	
	int c;	
	char path[INFLUXDB_URL_MAX_SIZE]={0};
	char body[INFLUXDB_URL_MAX_SIZE]={0};	
	sprintf(path,"/query?db=%s",client->dbname);
	sprintf(body,"q=%s",query);	
	c = influxdb_client_post(client,path,body,NULL);	
	return c;
}

int influxdb_query(s_influxdb_client *client,char *query,s_influxdb_string *outstr)
{
    int status;
    char url[INFLUXDB_URL_MAX_SIZE];
	CURL *curl = curl_easy_init();
	char path[INFLUXDB_URL_MAX_SIZE]={0};	
	sprintf(path,"/query?db=%s&q=",client->dbname);
	if(curl) 
	{
		char *output = curl_easy_escape(curl,query,strlen(query));
		if(output) 
		{
			//printf("Encoded: %s\n", output);
			strncat(path,output,INFLUXDB_URL_MAX_SIZE);
			curl_free(output);
		}
		curl_easy_cleanup(curl);
	}	
    influxdb_client_get_url(client,&url,INFLUXDB_URL_MAX_SIZE,path);    
	status=get_response_body(url,outstr);	
	//printf("influxdb_query status : %d\n",status);
    return status;
}
