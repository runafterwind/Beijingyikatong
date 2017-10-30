
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <errno.h>
#include <netinet/in.h>
#include <sys/socket.h>


#define portnumber 5800
#if 0

int main(int argc,char * argv[])
{
    int sockfd,new_fd;
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    int sin_size;
    int nbytes;
    char buffer[1024];
	int i;
	printf("------ welcom to use test server----\n");
	
//����sockfd��������AF_INET����IPv4��SOCK_STREAM����TCP    
    if ((sockfd = socket(AF_INET,SOCK_STREAM,0)) == -1)
    {
        fprintf(stderr,"socket error %s",strerror(errno));
        exit(1);
    }
//���������sockaddr�ṹ
    bzero(&server_addr,sizeof(struct sockaddr_in));//��ʼ������0��
    server_addr.sin_family = AF_INET;
//���������ϵ�long����ת��Ϊ�����ϵ�long����
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
//��������short����ת��Ϊ�����ϵ�short����
    server_addr.sin_port = htons(portnumber);
//����sockfd��������IP��ַ
    if (bind(sockfd,(struct sockaddr *)(&server_addr),sizeof(struct sockaddr)) == -1)
    {
        fprintf(stderr,"bind error %s\n\a",strerror(errno));
        exit(1);
    }
//������������ͻ���������
    if (listen(sockfd,5) == -1)
    {
        fprintf(stderr,"listen error %s\n\a",strerror(errno));
        exit(1);
    }
    while(1)
    {
//����������֪���ͻ�����������
        sin_size == sizeof(struct sockaddr_in);
        if ((new_fd = accept(sockfd,(struct sockaddr *)(&client_addr),&sin_size)) == -1)
        {
            fprintf(stderr,"accept error %s\n\a",strerror(errno));
            exit(1);
        }
        fprintf(stderr,"server get connection from %s\n\a",inet_ntoa(client_addr.sin_addr));

		while(1){
			//�ڲ�����select �����ͻ�������
        	if ((nbytes = read(new_fd,buffer,1024)) == -1)
        	{
            fprintf(stderr,"read error %s \a\n",strerror(errno));
            exit(1);
        	}
        	//buffer[nbytes] = '\0';
        	//printf("server received %s\n",buffer);
			printf("server recive %d bytes::",nbytes);
			for(i=0;i<nbytes;i++)
				printf("%02x ",buffer[i]);
			printf("\n");

			write(new_fd,"000111222333444555",18);
		}
		
		
        //close(new_fd);
    }
    exit(0);
} 
#endif

