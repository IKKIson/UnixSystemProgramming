#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/msg.h>
#include <pthread.h>
#include <unistd.h>

//Define Proccessor
#define REQUEST_SEILPOLICY_FILE 1
#define REQUEST_NEW_RESERVATION 2
#define REQUEST_CHECK_RESERVATION 3

#define MQ_KEY_NAME "PhoneReservation"
#define MAX_BUFFER_SIZE 100

//Define Function
int *MQsend(int *msgid);
int *MQrecive(int *msgid);

//Define Global Value


//Define Message Struct



//Main - Store
int main(){
	//Define Value
	//Message Queue Value
	key_t mq_key;
	int msgid;

	//pthread
	pthread_t thread_MQsnd;
	pthread_t thread_MQrcv;
    	int pth_MQsnd_status;
	int pth_MQrcv_status;


	//Set Initalization
	SetMsgQGetKey(&mq_key, &msgid);	


	//App 
	if ((pth_MQsnd_status = pthread_create(&thread_MQsnd, NULL, MQsend, (void *)&msgid)) < 0){
        	perror("thread create error:");
	        exit(0);
	}
	pthread_join(thread_MQsnd, (void **)&pth_MQsnd_status);

	return 0;
}



void SetMsgQGetKey(key_t *mq_key, int *msgid){
	mq_key = ftok(MQ_KEY_NAME, 1);
	msgid = msgget(mq_key, IPC_CREAT|0644);
	if(msgid < 0){
		perror("msegget");
		exit(1);
	}
}

int *MQsend(int *msgid){
	char snd_buf[MAX_BUFFER_SIZE];
	int len = 0;
        char rcv_buf[MAX_BUFFER_SIZE];
	while(1){
	printf("Send Message : ");
	scanf("%s", snd_buf);
	if(snd_buf == 'q' || snd_buf == 'Q'){
		printf("end of IPC-MQ\n");
		exit(1);
	}
	if(msgsnd(msgid, snd_buf, MAX_BUFFER_SIZE, IPC_NOWAIT) == -1) {
		perror("msgsnd");
		return 0;
	}
	len = msgrcv(msgid, rcv_buf, MAX_BUFFER_SIZE, 0, 0);
	printf("Received Msg = %s, Length = %d\n", rcv_buf, len);
	}
	return 0;
}


