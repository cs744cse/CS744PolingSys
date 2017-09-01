#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <string.h>
#include <stdlib.h>

void tokenize(char *);
void error(char *msg)
{
    perror(msg);
    exit(1);
}
int count ;
int main(int argc, char *argv[])
{
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[256];
    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);

    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");
    
    printf("Enter Your VOTER ID (12 Digits):");
    
    bzero(buffer,256);
    fgets(buffer,255,stdin);
    
    n = write(sockfd,buffer,strlen(buffer));
    if (n < 0) 
         error("ERROR writing to socket");
    
    bzero(buffer,256);
    
    n = read(sockfd,buffer,255);
    char *verif = (char *)malloc((n+1)*sizeof(*verif));
    strncpy(verif, buffer,n);
    verif[n] = '\0';
    if(!(strcmp(verif,"FAILED")))
    {
        printf("Authentication Failed\n");
        exit(1);
    }
    else if(!(strcmp(verif,"ALREADY VOTED")))
    {
        printf("Already Voted.\n");
        exit(1);
    }
    printf("Region : ");
    
    if (n < 0) 
         error("ERROR reading from socket");
    
    printf("%s\n",buffer);

    n = read(sockfd,buffer,256);
    
    printf("Candidate List :\n");
    
    if (n < 0) 
         error("ERROR reading from socket");

    tokenize(buffer);
    int choice;
    char ch[2];
    printf("\nChoose Your Candidate : ");
     scanf("%d",&choice);
     if(choice > count)
     {
         error("Invalid Selection");
         exit(1);
     }
     sprintf(ch,"%d",choice);
    //printf("Candidate ",);
    //printf("%s %d ",verif,strlen(verif));
    char *selected = NULL;
    // char c=(char)choice;
     
    selected = (char*)malloc(strlen(ch)+strlen(verif)*sizeof(char));
    strncpy(selected,ch,2);
    strcat(selected," ");
    strcat(selected,verif);
    printf(" %s ",selected);
    n = write(sockfd,selected,strlen(selected));
    if(n<0)
    {
        error("Writing Error:");
        exit(1);
    }
    bzero(buffer,256);
    n = read(sockfd,buffer,256);
    printf("\n%s\n",buffer);
    return 0;
}



 void tokenize(char *list)
 {
    char delim[] = "$";
    char* token;
    
        for (token = strtok(list, delim); token; token = strtok(NULL, delim))
        {
            printf("%c : %s\n", token[0], (token+1));
            count++;
        }
 }