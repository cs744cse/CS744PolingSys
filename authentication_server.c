// Tier 1 Server side C/C++ program to Socket programming
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#define PORT 8080 //default port

int main(int argc, char const *argv[])
{
    int server_fd , new_socketfd, pid, flag  , portno , server_length, client_length;
    struct sockaddr_in server_address, client_address ;
    //char buffer[1024] = {0};  used that in local buffer of communicate function
    int opt = 1;
    
    if (argc < 2)
    {	    fprintf(stderr, "ERROR, no port provided\n");
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
    int comflag=0;
    char buffer[256];
    bzero(buffer, 256);
    
    comflag = read(sock, buffer , 255);
    if(comflag<0)
    {
	    perror("ERROR reading from socket");
	    exit(EXIT_FAILURE);
    }
    printf("Message from client (ith) :%s\n" , buffer);
    
    comflag=write(sock , "I got your message" ,19 );
    if (comflag < 0)
    {
	    perror("ERROR writing to socket");
	    exit(EXIT_FAILURE);
    }
	
    return;
}
