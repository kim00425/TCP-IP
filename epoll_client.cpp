#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include<iostream>
#include<pthread.h>
#define MAXLINE 1024
void *send_message(void *sock);
void *read_message(void *sock);
int main(int argc, char **argv)
{
  int socketfd;
  struct sockaddr_in sockaddr;
  int socklen;
  char buf[MAXLINE];
  int suc;
  pthread_t read_thread;
  pthread_t send_thread;
  void *return_thread;
  int optval=1;
  if(argc != 3)
  {
 
  printf("Usage : %s [ip address] [port]\n", argv[0]);
 	 return 1;
  }
  if( (socketfd = socket(AF_INET, SOCK_STREAM, 0)) < 0 )
  {
   perror("error!!");
 	 return 1;
  }
  setsockopt(socketfd,SOL_SOCKET,SO_REUSEADDR,&optval,sizeof(optval));
  sockaddr.sin_family = AF_INET;
  sockaddr.sin_addr.s_addr = inet_addr(argv[1]);
  sockaddr.sin_port = htons(atoi(argv[2]));

  socklen = sizeof(sockaddr);
  suc=connect(socketfd, (struct sockaddr *)&sockaddr, socklen);
  if(suc!=0)
  {
      printf("Fail Connect!!\n");
      return 1;
  }
 
   
  pthread_create(&read_thread,NULL,read_message,(void*)&socketfd);
  pthread_create(&send_thread,NULL,send_message,(void*)&socketfd);
  pthread_join(read_thread,NULL);
  pthread_join(send_thread,NULL);
  close(socketfd);

}
void *send_message(void *sock)
{
  int suc=*((int *)sock);
 
  char buf[1024];
  while(1)
  {
      
      fgets(buf,MAXLINE,stdin);
      if(!strcmp(buf,"Q\n")||!strcmp(buf,"q\n"))
      {
          printf("퇴장!!!\n");
          write(0,0,0);
          close(suc);
          exit(0);
      }
      write(suc,buf,strlen(buf));
      memset(buf,0x00,MAXLINE);     
  }
}
void *read_message(void *sock)
{
    int suc=*((int *)sock);
    
    char buf[1024];
    while(1)
    {   
    read(suc,buf,MAXLINE);
    fputs(buf,stdout);
    memset(buf,0x00,MAXLINE);
    }
}
