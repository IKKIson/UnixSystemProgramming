#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<string.h>
#include<sys/wait.h>
#include<string.h>
#include<sys/msg.h>

#define BUF_SIZE 30

void error_handling(char *message);
void read_childproc(int sig);

int main(int argc ,char* argv[])
{

	int serv_sock, clnt_sock;
	struct sockaddr_in serv_adr, clnt_adr;
	int fds1[2];
	int fds2[2];

	pid_t pid;
	struct sigaction act;
	socklen_t adr_sz;
	int str_len, state;
	char buf[BUF_SIZE];
	
	FILE *fp ;
        char msgbuf[BUF_SIZE];
        char filename[BUF_SIZE];
        int i=0, msg_len;
	
	//Message Queue value set
	key_t key;
	int msgid;

	//Message Queue Set Init
	key = ftok("keyfile", 1);
	msgid = msgget(key, IPC_CREAT|666);
	if(msgid == -1){
		perror("msgget");
		exit(1);
	}	
	
	//Check port num for socket
	if(argc !=2)
	{
		printf("Usage: %s<port>\n",argv[0]);
		exit(1);
	}

	act.sa_handler=read_childproc;
	sigemptyset(&act.sa_mask);
	act.sa_flags =0;
	state = sigaction(SIGCHLD, &act,0);

	serv_sock=socket(PF_INET,SOCK_STREAM,0);
	memset(&serv_adr,0,sizeof(serv_adr));
	serv_adr.sin_family=AF_INET;
	serv_adr.sin_addr.s_addr=htonl(INADDR_ANY);
	serv_adr.sin_port=htons(atoi(argv[1]));

	if(bind(serv_sock,(struct sockaddr*)&serv_adr, sizeof(serv_adr))==-1)
		error_handling("bind() error!");
	if(listen(serv_sock,5)==-1)
		error_handling("listen() error!");
	//pipe(fds1);
	//pipe(fds2);
	

	while(1)
	{

		adr_sz = sizeof(clnt_adr);
		clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_adr, &adr_sz);
		if(clnt_sock ==-1)
			continue;
		else
			puts("new client connected...");
		
		/* 
			process parent
			pipe index
				1 : write to other process
				0 : read from other process
			fds1[0] : write to process child
			fds2[1] : read from process child
		*/
		pid= fork();
		if(pid != 0) 
		{
			printf("Insert File Name : ");
			scanf("%s",filename);

			fp = fopen(filename,"wt");
			for(i=0; i<10; i++)
			{
			/*
				limt 10 message from clnt
				1. scanf message log file name
				2. read message from process B
				3. fwrite meassge to text file
				4.1. if msg is 'q' or 'Q' 
					break for loop
				4.2. (for loop)if get msg 10 count
					end of loop 
			*/
				//msg_len = read(fds1[0],msgbuf,BUF_SIZE);
				msg_len = msgrcv(msgid, msgbuf, BUF_SIZE, 0, 0);
				msgbuf[msg_len]=0;
				printf("Get Message from pc : %s\n",msgbuf);
				fwrite((void*)msgbuf,1,msg_len,fp);
				
				if(!strcmp(msgbuf,"q\n") || !strcmp(msgbuf,"Q\n"))
				{
					//write(fds2[1],msgbuf,msg_len);
					if(msgsnd(msgid, (void *)msgbuf, BUF_SIZE, IPC_NOWAIT) == -1){
                                       		perror("msegsnd");
                                        	exit(1);
                                	}
					break;
				}
				else {
					//write(fds2[1],msgbuf,msg_len);
					if(msgsnd(msgid, (void *)msgbuf, BUF_SIZE, IPC_NOWAIT) == -1){
                                        	perror("msegsnd");
                                        	exit(1);
                                	}
				}
				
			}
			fclose(fp);
			return 0;
		}
		
		/* 
			process child
			pipe index
				1 : write to other process
				0 : read from other process
			fds1[1] : write to process parent
			fds2[0] : read from process parent
		*/
		else if(pid == 0)
		{
			close(serv_sock);
			while((str_len=read(clnt_sock, buf, BUF_SIZE))!=0)
			{
			/*
				1. read from clnt
				2. write to process A using pipe fds1[0]
				3. read from pocess A using pipe fds2[1]
				4. check 'quit' message for clnt exit
				5. echo to clnt using wirte
			*/

				memset(msgbuf,0,sizeof(msgbuf));
				strcpy(msgbuf,buf);
				msgbuf[str_len]=0;
				//write(fds1[1],msgbuf,str_len);
				if(msgsnd(msgid, (void *)msgbuf, BUF_SIZE, IPC_NOWAIT) == -1){
					perror("msegsnd");
					exit(1);
				}				

				memset(msgbuf,0,sizeof(msgbuf));
				read(fds2[0],msgbuf,BUF_SIZE);
				
				i++;

				//printf("check in Process B i = %d\n", i);
				//printf("check in Process B msgbuf = %s\n", msgbuf);
				//each child processes handled i, msgbuf
				//A process i, msgbuf != B process i,msgbuf 

				if(!strcmp(msgbuf,"q\n")||!strcmp(msgbuf,"Q\n") || i == 10)
				{
					memset(msgbuf,0,sizeof(msgbuf));
					strcpy(msgbuf,"End Of Message\n");
					write(clnt_sock,msgbuf,16);
					break;
				}
				write(clnt_sock,msgbuf,str_len);
				memset(msgbuf, 0, sizeof(msgbuf));
			}
			
		close(clnt_sock);
		puts("Client Disconnected...");
		return 0;
		}
		else
			close(clnt_sock);
	}
	close(serv_sock);
	return 0;

}		

void error_handling(char* message)
{
	fputs(message,stderr);
	fputc('\n',stderr);
	exit(1);
}
void read_childproc(int sig)
{
	pid_t pid;
	int status;
	pid=waitpid(-1,&status,WNOHANG);
	printf("removed proc id:%d\n",pid);
}

