#include<sys/epoll.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<unistd.h>
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

void send_msg(char* msg);
int main(int argc,char **argv)
{
    int listenfd,clientfd,epollfd,suc;
    epoll_event ev,*events;
    struct sockaddr_in server_addr;
    USER *user,data;
    socklen_t socketlen;
    int eventn;
    int readn;
    int optval=1;
    socketlen=sizeof(server_addr);
    listenfd=socket(AF_INET,SOCK_STREAM,0);
    if(listenfd>=0)
    {
        printf("Success Make Server!!\n");
    }
    setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&optval,sizeof(optval));

    events=(struct epoll_event*)calloc(EPOLL_SIZE,sizeof(epoll_event));//make event_pool 
     
    server_addr.sin_family=AF_INET;//TCP/IP use
    server_addr.sin_port=htons(PORTNUM);
    server_addr.sin_addr.s_addr=htonl(INADDR_ANY);//127.0.0.1 
    bind(listenfd,(struct sockaddr*)&server_addr,socketlen);
    epollfd=epoll_create1(0);

    listen(listenfd,5);
    ev.events=EPOLLIN;
    ev.data.fd=listenfd;
    epoll_ctl(epollfd,EPOLL_CTL_ADD,listenfd,&ev);//Add listenfd
    memset(clientlist,-1,sizeof(int)*1024);//Not visit setting
    while(1)
    {
        eventn=epoll_wait(epollfd,events,EPOLL_SIZE,-1);//For infinity wating= -1
       
        for(int i=0;i<eventn;i++)//compare fd
        {
            if(listenfd==events[i].data.fd)//New client
            { 
                char name[1024];
                printf("New User!!\n");
                sprintf(name,"당신의 이름은 무엇입니까?");
                socketlen=sizeof(server_addr);
                clientfd=accept(listenfd,(struct sockaddr*)&server_addr,&socketlen);
                clientlist[clientfd]=1;  //visit setting 
                epoll_ctl(epollfd,EPOLL_CTL_ADD,clientfd,&ev);//Add new clientfd 
                write(clientfd,name,MAXLINE);//ask name to client
                memset(name,0x00,MAXLINE);
                readn=read(clientfd,name,MAXLINE);//get name for client
                user=(USER*)malloc(sizeof(user));//make USER
                user->fd=clientfd;
                strncpy(user->name,name,readn-1);//readn-1하는 이유는 read로 받은 이름값이 \n이 추가되어 있기  때문에 \n을 뺀 값만 저장
                ev.events=EPOLLIN;
                ev.data.ptr=user;
                epoll_ctl(epollfd,EPOLL_CTL_MOD,clientfd,&ev);//추가된 user구조체를 해당 기존에 있던 clientfd값에서 수정시킴
                sprintf(name,"%s님이 입장하셨습니다.\n",user->name);
                printf("%s",name);
                send_msg(name);//Send Message
                printf("Connect Success #%d!!!\n",user->fd);
                memset(name,0x00,MAXLINE);
                break;               
            }
            else
            {
                char info[1024];
                char buf[1024]; 
                user=(USER*)events[i].data.ptr;
                memset(buf,0x00,MAXLINE);
                readn=read(user->fd,buf,MAXLINE);
                if(readn==0)//If client request to exit
                {
                 printf("#%d Disconnect(ID:%s)\n",user->fd,user->name);
                 sprintf(info,"%s님이 퇴장하셨습니다.\n",user->name);
                 send_msg(info);//Send Message 
                 epoll_ctl(epollfd,EPOLL_CTL_DEL,user->fd,events);//Delete User
                 clientlist[user->fd]=-1;//Not visit setting -1
                 break;
                }
                else
                {                    
                 sprintf(info,"%s:%s",user->name,buf);
                 printf("%s",info);
                 send_msg(info);
                 memset(info,0x00,MAXLINE);
                 break;
                }
            }
          }
        }
    free(user);
    free(events);
    return 0;
}
void send_msg(char* msg)
{
    int i;
    for(i=0;i<MAXLINE;i++)
    {        
        if(clientlist[i]==1)
        {            
            write(i,msg,strlen(msg));          
        }
    }
}
