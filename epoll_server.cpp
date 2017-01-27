#include<iostream>
#include<cstdlib>
#include<sys/epoll.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<unistd.h>
#include<cstring>
#include<fcntl.h>
#include<netdb.h>
#include<netinet/in.h>
#include<stdlib.h>
#include<string.h>
#include<stdio.h>
#include<netinet/in.h>
#define MAXLINE 1024
#define PORTNUM 3600
#define EPOLL_SIZE 20
typedef struct user
{
    int fd;
    char name[20];
    int level;
}USER;

int clientlist[1024];

void send_msg(struct epoll_event ev,char* msg);
int main(int argc,char **argv)
{
    int listenfd,clientfd,epollfd,suc;
    epoll_event ev,*events;
    struct sockaddr_in server_addr;
    USER *user,data;
    socklen_t socketlen;
    int eventn;
    char buf[MAXLINE];
    int optval=1;
    int i,readn;
    socketlen=sizeof(server_addr);
    listenfd=socket(AF_INET,SOCK_STREAM,0);
    setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&optval,sizeof(optval));

    events=(struct epoll_event*)calloc(EPOLL_SIZE,sizeof(epoll_event));
     
    server_addr.sin_family=AF_INET;
    server_addr.sin_port=htons(PORTNUM);
    server_addr.sin_addr.s_addr=htonl(INADDR_ANY);
    bind(listenfd,(struct sockaddr*)&server_addr,socketlen);
    epollfd=epoll_create1(0);

    listen(listenfd,5);
    ev.events=EPOLLIN;
    ev.data.fd=listenfd;
    epoll_ctl(epollfd,EPOLL_CTL_ADD,listenfd,&ev);
    memset(buf,0x00,MAXLINE);
    memset(clientlist,-1,sizeof(int)*1024);
    while(1)
    {
        eventn=epoll_wait(epollfd,events,EPOLL_SIZE,-1);
        for(i=0;i<eventn;i++)
        {
            if(listenfd==events[i].data.fd)
            {   
                memset(buf,0x00,MAXLINE);
                sprintf(buf,"당신의 이름은 무엇입니까?");
                socketlen=sizeof(server_addr);
                clientfd=accept(listenfd,(struct sockaddr*)&server_addr,&socketlen);
                clientlist[clientfd]=1;
                epoll_ctl(epollfd,EPOLL_CTL_ADD,clientfd,&ev);
                
                write(clientfd,buf,MAXLINE);
                memset(buf,0x00,MAXLINE);
                readn=read(clientfd,buf,20);
                user=(USER*)malloc(sizeof(user));
                user->fd=clientfd;
                strncpy(user->name,buf,readn-1);

                ev.events=EPOLLIN;
                ev.data.ptr=user;
                epoll_ctl(epollfd,EPOLL_CTL_MOD,clientfd,&ev);
                
                
            }
            else
            {
                user=(USER*)events[i].data.ptr;
                memset(buf,0x00,MAXLINE);
                readn=read(user->fd,buf,MAXLINE);
                if(readn<=0)
                {
                
                epoll_ctl(epollfd,EPOLL_CTL_DEL,user->fd,events);
                close(user->fd);
                clientlist[user->fd]=-1;
                free(user);
                
                }
                else
                {
                 send_msg(events[i],buf);
                }
            }
          }
        }
}
void send_msg(struct epoll_event ev,char* msg)
{
    int i;
    char buf[MAXLINE];
    USER* user_data;
    user_data=(USER*)ev.data.ptr;
    for(i=0;i<1024;i++)
    {
        memset(buf,0x00,MAXLINE);
        sprintf(buf,"%s:%s",user_data->name,msg);
        if(clientlist[i]==1)
        {
            write(i,buf,MAXLINE);
        }
    }
}
    











