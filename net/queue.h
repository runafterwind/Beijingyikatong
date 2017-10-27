#ifndef __QUEUE__H__
#define __QUEUE__H__

#include <inttypes.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "list.h"




typedef struct mission_info_{
		int type;
		int status;				//0 ׼����1 �����У�2 ����� ��3 ʧ��
		int priority;			//0 ��ͨ ��1 ����ִ��
		unsigned char data[4096];		//�����Ǹ�������Э������涨������
		int datalen;
}mission_info;


typedef struct mission_list_{
	mission_info  missinfo;
	void * pravite;						//�������ݣ���һЩ�������ʹ��
	struct list_head list_node;			//����ڵ�
}mission_list;



extern mission_list * listhead;

int init_queue();
mission_list * new_mission(const mission_info* mission,void * arg);
void for_each_mission(const struct list_head *head);
void destroy_mission_list(struct list_head *head);
int add_mission(mission_list * newmisson);
int add_mission_tail(mission_list * newmisson);
int del_mission(struct list_head * head);
int del_mission_tail(struct list_head * head);
mission_list * get_first_node_of_list(const struct list_head * head);

#endif
