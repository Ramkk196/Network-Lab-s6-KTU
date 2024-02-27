Client
#include<stdio.h>
#include<string.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<fcntl.h>
#include<stdlib.h>
main(int argc, char * argv[])
{
int i,j,n;
int sock_fd;
struct sockaddr_in servaddr;
char buff[100];
if(argc != 3)
{
fprintf(stderr, "Usage: ./client IPaddress_of_server port\n");
exit(1);
}
if((sock_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
{
printf("Cannot create socket\n");
exit(1);
}
bzero((char*)&servaddr, sizeof(servaddr));
servaddr.sin_family = AF_INET;
servaddr.sin_port = htons(atoi(argv[2]));
inet_pton(AF_INET, argv[1], &servaddr.sin_addr);
n = sendto(sock_fd, "", 1,0, (struct sockaddr*)&servaddr, sizeof(servaddr));
if( n < 0)
{
perror("error in sending");
exit(1);
}
if((n=recvfrom(sock_fd, buff, sizeof(buff),0, NULL, NULL)) == -1)
  perror("read error from server:");
exit(1);
}
printf(" the current time of the system is %s\n", buff);
}
Server
#include<stdio.h>
#include<string.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<fcntl.h>
#include<stdlib.h>
#include<time.h>
main(int argc, char * argv[])
{
int n;
int sock_fd;
int i,j,k;
int childpid;
char buffer[100];
time_t curtime;
struct sockaddr_in servaddr, cliaddr;
int len = sizeof(cliaddr);
if(argc != 2)
{
fprintf(stderr, "Usage: ./server port\n");
exit(1);
}
if((sock_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
{
printf("Cannot create socket\n");
exit(1);
}
bzero((char*)&servaddr, sizeof(servaddr));
servaddr.sin_family = AF_INET;
servaddr.sin_port = htons(atoi(argv[1]));
servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
if(bind(sock_fd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0)
{
perror("bind failed:");
exit(1);
}
while(1)
{
if((n = recvfrom(sock_fd,buffer , sizeof(buffer), 0, (struct sockaddr *)&cliaddr, &len)) == -1)
{
perror("size not received:");
exit(1);
}
childpid = fork();
if(childpid == 0)
{
time(&curtime);
sprintf( buffer, "= %s", ctime(&curtime));
n = sendto(sock_fd, buffer, sizeof(buffer),0, (struct sockaddr*)&cliaddr, sizeof(cliaddr));
if( n < 0)
{
perror("error in sending");
exit(1);
}
exit(1);
}
}
}