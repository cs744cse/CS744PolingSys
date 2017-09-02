// Tier 1 Server side C/C++ program to Socket programming
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <mysql/mysql.h>
#include <netdb.h>
#define PORT 8080 //default port
#define SQLUSERNAME "guptas"
#define SQLP "cs744iitb"
#define SQLDB "USE testdb"
#define BACKEND argv[2]

int sqlfunction(char *);
int connection_to_back(char *);
void read_write(int , char *);
int main(int argc, char const *argv[])
{
    int server_fd , new_socketfd, pid, portno, flag , server_length, client_length;
    struct sockaddr_in server_address, client_address ;
    int opt = 1;
	char  backend[20];
	strcpy(backend,argv[2]);
    
    if (argc < 3)
    {	    fprintf(stderr, "ERROR, no port provided or backend IP not provided n\n");
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
    server_address.sin_port = htons( portno );
    server_length = sizeof(server_address);

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
	 	communicate(new_socketfd,backend);
	 	exit(0);
	}
        
	else close(new_socketfd);
    }
    
	return 0;
}

void communicate (int sock, char *backend)
{
    int comflag=0;
    char buffer[256];
    char buffer1[256];
    char msg[256];
    int sock_id;
    bzero(buffer, 256);
    bzero(buffer1, 256);
    comflag = read(sock, buffer , 255);
    if(comflag<0)
    {
	    perror("ERROR reading from socket");
	    exit(EXIT_FAILURE);
    }
    printf("Message from client :%s\n" , buffer);

    comflag = sqlfunction(buffer);
    if(comflag<0)
    {
        perror("ERROR accesing DB ");
    
    }
    else if(comflag == 0)
    {
        sprintf(msg,"%s","FAILED");
        comflag=write(sock , msg ,strlen(msg) );
											for(int i=0;i<10;i++)
												printf("==");
        printf("\nSome Intruder Tried Login.\n");
    }
    else if(comflag == 2)
    {
        sprintf(msg,"%s","ALREADY VOTED");
        comflag=write(sock , msg ,strlen(msg) );
											for(int i=0;i<10;i++)
												printf("==");
        printf("\nSomeone Tried to vote Again.\n");
    }
    else
    {
        comflag=write(sock , buffer ,strlen(buffer) );								//writing Region to client
        sock_id = connection_to_back(backend);									//creating connection with backend
	    read_write(sock_id,buffer);										//reading and writing with backend (list fetching)
        comflag=write(sock , buffer ,strlen(buffer) );								//writing the list to client
        // bzero(buffer,256);	
        comflag = read(sock,buffer1,256);									//reading Clients response 
        read_write(sock_id, buffer1);										//reading and writing back RESPONSE to Backend. 
        comflag = write(sock,buffer1,strlen(buffer1));								//Giving Back the acknowledgment to client 
    }	
    
    if (comflag < 0)
    {
	    perror("ERROR writing to socket");
	    exit(EXIT_FAILURE);
    }
	
    return;
}

int sqlfunction(char * buffer)
{	char id[13];
    memset(id, '\0', sizeof(id));
	int count;
    MYSQL *con = mysql_init(NULL);
	if (con == NULL)
	{
		fprintf(stderr, "%s\n", mysql_error(con));
		return -1;
	}
	if (mysql_real_connect(con, LOCAL_HOST , SQLUSERNAME, SQLP, NULL, 0, NULL, 0) == NULL) 
	{
		fprintf(stderr, "%s\n", mysql_error(con));
		mysql_close(con);
		return -1;
	} 
	if (mysql_query(con, SQLDB)) 
	{
		fprintf(stderr, "%s\n", mysql_error(con));
		mysql_close(con);
		return -1;
    }

 char query[100] = "SELECT region FROM login WHERE id ='";
 char query1[100];
 unsigned int num_rows;
 strncpy(id,buffer,12);
 id[12] = '\0';
 strcat(query, id);
 strcat(query, "'");

 	if(mysql_query(con, query)!=0)
	{	fprintf(stderr,"%s\n", mysql_error(con));
		mysql_close(con);
		return -1;
	}

	else
	{
		MYSQL_RES *query_results = mysql_store_result(con);
		if (query_results)
		{
            MYSQL_ROW row;
			if((row = mysql_fetch_row(query_results))!=0)
			{	
											for(int i=0;i<10;i++)
												printf("==");
                printf("\nClient Authenticated.\n");
                sprintf(query1,"UPDATE login SET voted = '1' WHERE id = '%s' AND voted = '0'",id);
                if(mysql_query(con, query1)!=0)
                {	fprintf(stderr,"%s\n", mysql_error(con));
                    mysql_close(con);
                    return -1;
                }
                num_rows = mysql_affected_rows(con);
                if(num_rows != 0)
                { strcpy(buffer, row[0]);
                return 1;
                }
                else{
                    return 2;
                }
            }
			return 0;    			//No result found; 
		}
	}
}

int connection_to_back(char *backend)
{
    int sockfd, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;   
sockfd = socket(AF_INET, SOCK_STREAM, 0);
    
    if (sockfd < 0) 
        error("ERROR opening socket");
    server = gethostbyname(backend);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(PORT);
    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");
    return sockfd;
}
void read_write(int sockfd, char *buffer)
{
    int n;
    n = write(sockfd,buffer,strlen(buffer));
    if (n < 0) 
    {
         error("ERROR writing to socket");
         exit(1);
    }
bzero(buffer,256);
    n = read(sockfd,buffer,256);
    if (n < 0) 
    {
        error("ERROR reading from socket");
        exit(1);
    }
    // printf("\n%s\n",buffer);
}
