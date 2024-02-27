#include <stdio.h>
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 
#include <time.h>
#define PORT 8081
#define MAXLINE 1024
// Driver 
 
int main() {
int sockfd;
char buffer[MAXLINE];
time_t t;
time(&t);
char *hello = ctime(&t); //"Hello from client : Current Time is ";
// char *currtime = 
ctime(&t); 
struct sockaddr_in servaddr;
// printf("\n current time is : %s",ctime(&t));
// Creating socket file descriptor
if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
perror("socket creation failed"); 
exit(EXIT_FAILURE);
}
memset(&servaddr, 0, sizeof(servaddr));
// Filling server information 
servaddr.sin_family = AF_INET; 
servaddr.sin_port = htons(PORT); 
servaddr.sin_addr.s_addr = INADDR_ANY;
int n, len;
sendto(sockfd, (const char *)hello, strlen(hello), MSG_CONFIRM, (const struct sockaddr *) &servaddr,sizeof(servaddr)); 
printf("Hello TIME  sent to the Server.\n");
n = recvfrom(sockfd, (char *)buffer, MAXLINE,MSG_WAITALL, (struct sockaddr *) &servaddr, &len);
buffer[n] = '\0';
printf("Server TIME : %s\n", buffer);
close(sockfd); 
return 0;
}







#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#define PORT 4950
#define BUFSIZE 1024
void send_to_all(int j, int i, int sockfd, int nbytes_recvd, char *recv_buf, fd_set *master)
{
if (FD_ISSET(j, master)){
if (j != sockfd && j != i) {
if (send(j, recv_buf, nbytes_recvd, 0) == -1) {
perror("send");
}
}
}
}
void send_recv(int i, fd_set *master, int sockfd, int fdmax)
{
int nbytes_recvd, j;
char recv_buf[BUFSIZE], buf[BUFSIZE];
for(int k=0;k<BUFSIZE;k++) recv_buf[k]='\0';
if ((nbytes_recvd = recv(i, recv_buf, BUFSIZE, 0)) <= 0) {
if (nbytes_recvd == 0) {
printf("Client %d quit.\n", (i-3));
}else {
perror("recv");
}
close(i);FD_CLR(i, master);
}else {
for(j = 0; j <= fdmax; j++){
char climsg[1035];
for(int k=0;k<1035;k++) climsg[k]='\0';
char cli[11]="(Client  ):";
cli[8]=(i-3)+'0';
strcat(climsg, cli);
strcat(climsg, recv_buf);
send_to_all(j, i, sockfd, strlen(climsg), climsg, master );
}
}
}
void connection_accept(fd_set *master, int *fdmax, int sockfd, struct sockaddr_in *client_addr)
{
socklen_t addrlen;
int newsockfd;
addrlen = sizeof(struct sockaddr_in);
if((newsockfd = accept(sockfd, (struct sockaddr *)client_addr, &addrlen)) == -1) {
perror("accept");
exit(1);
}else {
FD_SET(newsockfd, master);
if(newsockfd > *fdmax){
*fdmax = newsockfd;
}
printf("new connection from %s on port %d \n",inet_ntoa(client_addr->sin_addr),
ntohs(client_addr->sin_port));
}
}
void connect_request(int *sockfd, struct sockaddr_in *my_addr)
{
int yes = 1;
if ((*sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
perror("Socket");
exit(1);}
my_addr->sin_family = AF_INET;
my_addr->sin_port = htons(4950);
my_addr->sin_addr.s_addr = INADDR_ANY;
memset(my_addr->sin_zero, '\0', sizeof my_addr->sin_zero);
if (setsockopt(*sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
perror("setsockopt");
exit(1);
}
if (bind(*sockfd, (struct sockaddr *)my_addr, sizeof(struct sockaddr)) == -1) {
perror("Unable to bind");
exit(1);
}
if (listen(*sockfd, 10) == -1) {
perror("listen");
exit(1);
}
printf("\nTCP server waiting for client on port 4950:-\n");
fflush(stdout);
}
int main()
{
fd_set master;
fd_set read_fds;
int fdmax, i;
int sockfd= 0;
struct sockaddr_in my_addr, client_addr;
FD_ZERO(&master);
FD_ZERO(&read_fds);
connect_request(&sockfd, &my_addr);
FD_SET(sockfd, &master);
fdmax = sockfd;
while(1){
read_fds = master;if(select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1){
perror("select");
exit(4);
}
for (i = 0; i <= fdmax; i++){
if (FD_ISSET(i, &read_fds)){
if (i == sockfd)
connection_accept(&master, &fdmax, sockfd,
&client_addr);
else
send_recv(i, &master, sockfd, fdmax);
}
}
}
return 0;
}
