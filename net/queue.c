#include "queue.h"
#include "stdio.h"
#include <stdio.h>



mission_list * listhead;



int init_queue()
{

	listhead=(mission_list *)malloc(sizeof(mission_list));
	if(listhead==NULL)
	{
	    fprintf(stderr, "Failed to malloc memory, errno:%u, reason:%s\n",errno, strerror(errno));
	    printf(" malloc err for new space \n");
    	return -1;
     }
	init_list_head(&listhead->list_node);
	return 0;
}



/*
	根据任务信息创建任务节点
*/

mission_list * new_mission(const mission_info *mission,void * arg)
{
	mission_list * templist=NULL;
	
	if(mission==NULL)
		{
				printf("1st paramiter is null \n");
				return NULL;
		}

	
	templist = (mission_list*)malloc(sizeof(mission_list));
	
	if (templist== NULL)
	  {
	    fprintf(stderr, "Failed to malloc memory, errno:%u, reason:%s\n",errno, strerror(errno));
	    printf(" malloc err for new space \n");
    	return NULL;
     }

	
	memcpy(&(templist->missinfo),mission,sizeof(mission_info));
	templist->pravite=arg;
	return templist;

}


/*
	
*/
void for_each_mission(const struct list_head *head)
{
     struct list_head *pos;
     mission_list * mission;
		
	 //先检查链表是否为空	
	 if(list_empty( head))
	 	{
	 		printf("queue is empty can not operate\n");
			return ;
	 	}
	
     //遍历链表
     list_for_each(pos, head)
     {
     mission = list_entry(pos, mission_list, list_node);
     printf("mission type=%d  status=%d \n",mission->missinfo.type,mission->missinfo.status);
     }
}



void destroy_mission_list(struct list_head *head)
{
     struct list_head *pos = head->next;
     struct list_head *tmp = NULL;
     while (pos != head)
     {
     tmp = pos->next;
     list_del(pos);
     pos = tmp;     
	 }
}


int add_mission(mission_list * newmisson)
{

	if(newmisson==NULL)
		{
			printf("the peramiter is null\n");
			return -1;
		}
	list_add(&(newmisson->list_node),&(listhead->list_node));
	return 0;
}


int add_mission_tail(mission_list * newmisson)
{
	if(newmisson==NULL)
		{
			printf("the peramiter is null\n");
			return -1;
		}
	list_add(&(newmisson->list_node),listhead->list_node.prev);
	return 0;

}


int del_mission(struct list_head * head)
{

	 struct list_head *pos = head->next;
     struct list_head *tmp = NULL;
     if (pos != head)
     {
	     list_del(pos);
		 return 0;
	 }
	 else{
	 	printf("this list is already empty\n");
		return 1;
	 }
}


int del_mission_tail(struct list_head * head)
{
	 struct list_head *pos = head->prev;
     struct list_head *tmp = NULL;
     if (pos != head)
     {
	     list_del(pos);
		 return 0;
	 }
	  else{
	 	printf("this list is already empty\n");
		return 1;
	 }
	 
}


mission_list * get_first_node_of_list(const struct list_head * head)
{
	mission_list * mission;

	if(list_empty(head))
		return NULL;
		
	mission=list_first_entry(head,mission_list,list_node);
	return mission;
}











/*  队列测试程序 */
/*
int main()
{

	listhead=(mission_list *)malloc(sizeof(mission_list));
	if(listhead==NULL)
	{
	    fprintf(stderr, "Failed to malloc memory, errno:%u, reason:%s\n",errno, strerror(errno));
	    printf(" malloc err for new space \n");
    	return -1;
     }

	
	mission_info missinfo;
	mission_list * tempmission;


	init_list_head(&listhead->list_node);
	
	missinfo.priority=0;
	missinfo.status=0;
	missinfo.type=101;
	tempmission=new_mission(&missinfo,NULL);
	if(tempmission!=NULL)
		if(tempmission->missinfo.priority)
			add_mission( tempmission);
		else
			add_mission_tail(tempmission);

	missinfo.priority=1;
	missinfo.status=0;
	missinfo.type=102;
	tempmission=new_mission(&missinfo,NULL);
	if(tempmission!=NULL)
		if(tempmission->missinfo.priority)
			add_mission( tempmission);
		else
			add_mission_tail(tempmission);

	missinfo.priority=0;
	missinfo.status=0;
	missinfo.type=103;
	tempmission=new_mission(&missinfo,NULL);
	if(tempmission!=NULL)
		if(tempmission->missinfo.priority)
			add_mission( tempmission);
		else
			add_mission_tail(tempmission);

	missinfo.priority=1;
	missinfo.status=0;
	missinfo.type=104;
	tempmission=new_mission(&missinfo,NULL);
	if(tempmission!=NULL)
		if(tempmission->missinfo.priority)
			add_mission( tempmission);
		else
			add_mission_tail(tempmission);


	for_each_mission(&listhead->list_node);


	tempmission=get_first_node_of_list(&listhead->list_node);
	if(tempmission!=NULL)
		{
			printf("get node success \n");
			printf("mission type=%d status=%d priority=%d \n",tempmission->missinfo.type,tempmission->missinfo.status,tempmission->missinfo.priority);
		}

	del_mission(&listhead->list_node);
	del_mission_tail(&listhead->list_node);
	
	for_each_mission(&listhead->list_node);

	
	destroy_mission_list(&listhead->list_node);
	

}
*/
