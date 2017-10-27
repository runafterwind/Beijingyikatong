#ifndef __QUEUE__H__
#define __QUEUE__H__

#include <inttypes.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "list.h"




typedef struct mission_info_{
		int type;
		int status;				//0 准备，1 进行中，2 已完成 ，3 失败
		int priority;			//0 普通 ，1 立即执行
		unsigned char data[4096];		//这里是根据网络协议里面规定的限制
		int datalen;
}mission_info;


typedef struct mission_list_{
	mission_info  missinfo;
	void * pravite;						//特有数据，做一些特殊操作使用
	struct list_head list_node;			//链表节点
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
