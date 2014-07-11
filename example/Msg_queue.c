#include "Msg_queue.h"

const uint8_t *nullString = "";

void initQueue(){
	first = 0;
	last = QUEUE_SIZE-1;
	count = 0;
}
void enqeue(uint8_t *msg){
	int msgLength = strlen(msg);
	if(count >= QUEUE_SIZE){
		dequeue();
	}
	last = (last+1) % QUEUE_SIZE;

	if(msgLength>35){
		memcpy(msg_string[last],msg,35);
		memset((uint8_t*)(msg_string[last]+35),'.',3);
	}else{
		memcpy(msg_string[last],msg,msgLength);
	}
	count++;
}

void dequeue(){
	if(count >0){
		first = (first+1) % QUEUE_SIZE;
		count--;
	}
}
uint8_t* getMsg(int pos){
	if(pos > last && pos < QUEUE_SIZE){
		return msg_string[QUEUE_SIZE-(pos-last)];
	} else if(pos <= last){
		return msg_string[(last-pos)];
	} else{
		return nullString;
	}

}

