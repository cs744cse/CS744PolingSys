// Tier 2 Server side C  program to Socket programming
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <mysql/mysql.h>
#define PORT 8080 //default port
#define USER "guptas"
#define PASSWD "cs744iitb"

char * sqlfunction(char *, char*,int );
char * makedata(char *,char*,char*);
void communicate(int);
int sqlfunction2(char *);

int main(int argc, char const *argv[])
{
    int server_fd , new_socketfd, pid, flag  , portno , server_length, client_length;
    struct sockaddr_in server_address, client_address ;
    int opt = 1;
    
    if (argc < 2)
    {	fprintf(stderr, "ERROR, no port provided\n");
	    exit(1);  
    }
            
    // Creating socket file descriptor
    server_fd = socket(AF_INET , SOCK_STREAM, 0);
    
    if (server_fd < 0)
    {   perror("Socket Creation Failed");
        exit(EXIT_FAILURE);
    }
    
    // The bzero( str, n) function copies n bytes, each with a value of zero, into string s.
    bzero( (char *) &server_address, sizeof(server_address));
    
    //getting port number from argv[1] and initializing server's socketaddr_in struct (predefined) in socket.h
    portno = atoi(argv[1]);
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(portno);
    server_length = sizeof(server_address);
    /*// Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                                                  &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }*/
	// Tell me the need of forcefully attaching port to server
      
    // Binding socket
    if (bind(server_fd, (struct sockaddr *)&server_address, server_length)<0)
    {   perror("Bind() failed");
        exit(EXIT_FAILURE);
    }
    
    listen(server_fd , 5);
    client_length = sizeof(client_address);
    
    while(1)
    {
        new_socketfd = accept(server_fd, (struct sockaddr *)&client_address, &client_length); 
        if(new_socketfd < 0)
	{
        	perror("Accept() Error");
        	exit(EXIT_FAILURE);
	}
    
        pid = fork();
	if(pid < 0)
	{	perror("ERROR on fork");
	 	exit(EXIT_FAILURE);
	}
        
	if(pid == 0)
	{	close(server_fd);
	 	communicate(new_socketfd);
	 	exit(0);
	}
        
	else close(new_socketfd);
    }
    
	return 0;
}

void communicate (int sock)
{
    int comflag=0,vote=-1;
    char inbuffer[20];
    memset(inbuffer,'\0',sizeof(inbuffer));
    char* outbuffer = (char*)malloc(4096 * sizeof(char));

    comflag = read(sock, inbuffer , 20);
    if(comflag<0)
    {
	    perror("ERROR reading from socket");
	    exit(EXIT_FAILURE);
    }
    printf("Message from auth server :%s\n" , inbuffer);
    outbuffer = sqlfunction(inbuffer , outbuffer ,comflag);
    if(NULL == outbuffer)
    {
        perror("ERROR accesing DB ");
    
    }
    comflag=write(sock ,outbuffer ,strlen(outbuffer) );
    if (comflag < 0)
    {
	    perror("ERROR writing to socket");
	    exit(EXIT_FAILURE);
    }
    comflag = read(sock, inbuffer , 20);
    if(comflag<0)
    {
	    perror("ERROR reading from socket");
	    exit(EXIT_FAILURE);
    }
    vote=sqlfunction2(inbuffer);
    if(vote == -1)
    {
        perror("ERROR Registering VOTE contact SYSAD");
        comflag=write(sock ,"ERROR Registering VOTE contact SYSAD",strlen("ERROR Registering VOTE contact SYSAD") );
        if (comflag < 0)
        {
            perror("ERROR writing to socket");
            exit(EXIT_FAILURE);
        }
    
    }
    comflag=write(sock ,"VOTE REGISTERED",strlen("VOTE REGISTERED") );
    if (comflag < 0)
    {
	    perror("ERROR writing to socket");
	    exit(EXIT_FAILURE);
    }
    free(outbuffer);
	
    return;
}

char* sqlfunction(char * buffer , char* data,int size)
{	
    char region[20];
    memset(region, '\0', sizeof(region));
	int count,flag;
    MYSQL *con = mysql_init(NULL);
    strncpy(region,buffer,size);
    printf("%d",size);
    region[size]='\0';
    printf("\n%s:%d",region,strlen(region));
    
	if (con == NULL)
	{  
		fprintf(stderr, "%s\n", mysql_error(con));
		return NULL;
	}
	if (mysql_real_connect(con, LOCAL_HOST, USER, PASSWD, NULL, 0, NULL, 0) == NULL) 
	{ 
		fprintf(stderr, "%s\n", mysql_error(con));
		mysql_close(con);
		return NULL;
	} 
	if (mysql_query(con, "USE testdb")) 
	{   
		fprintf(stderr, "%s\n", mysql_error(con));
		mysql_close(con);
		return NULL;
    }
    char query[100] = "SELECT id,name FROM candidates WHERE region='";
    strcat(query,region);
    strcat(query,"'");

	if(mysql_query(con, query)!=0)
	{	
        fprintf(stderr,"%s\n", mysql_error(con));
		mysql_close(con);
		return NULL;
	}
	
	else
	{   
		MYSQL_RES *query_results = mysql_store_result(con);
		if (query_results)
		{
			MYSQL_ROW row;
			count=0;
			while((row = mysql_fetch_row(query_results))!=0)
			{	
				    data = makedata(row[0],row[1],data);
					count++;
            }
                        
        }
        mysql_close(con);
        if(count==0)
            return NULL;

        return data;
	}

}

char * makedata(char* data1, char* data2 , char* data)
{   int len = strlen(data);
    int len1 = strlen(data1);
    int len2 = strlen(data2);
    data = (char*)realloc(data,(len + len1 + len2)*sizeof(char));
    strcat(data,data1);
    strcat(data,data2);
    strcat(data,"$");
    
    return data;
}

int sqlfunction2(char * buffer)
{	
    char query[100];
    char region[20];
    char id[2];
    MYSQL *con = mysql_init(NULL);
    sscanf(buffer,"%s %s",id, region);
    printf("\nUpdaring Votes for id : %s\tregion : %s\n",id,region);
    
	if (con == NULL)
	{   
		fprintf(stderr, "%s\n", mysql_error(con));
		return -1;
	}
	if (mysql_real_connect(con, LOCAL_HOST, USER, PASSWD, NULL, 0, NULL, 0) == NULL) 
	{   
		fprintf(stderr, "%s\n", mysql_error(con));
		mysql_close(con);
		return -1;
	} 
	if (mysql_query(con, "USE testdb")) 
	{   
		fprintf(stderr, "%s\n", mysql_error(con));
		mysql_close(con);
		return -1;
    }

    sprintf(query,"UPDATE candidates SET votes = votes + 1 WHERE id= '%s' AND region = '%s'",id,region);
	if(mysql_query(con, query)!=0)
	{
        fprintf(stderr,"%s\n", mysql_error(con));
		mysql_close(con);
		return -1;
    }
    
    mysql_close(con);
    return 1;
}
