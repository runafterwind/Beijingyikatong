
#include "eletric.h"


/*
	���õ�10����ӿ�����Կ
*/
char keyarry[10][16]={
			{},
			{},
			{},
			{},
			{},
			{},
			{},
			{},
			{},
			{}	
};

int check_eletron_head(eletron_info info)
{		
	ELETRON_ERROCODE err;
	int len;
	
	/*check code id ,if err return  */
	

	/*check key id ,if err return */


	/*check data len*/
	len=((info.head.lenth[0]<<8)&0xff00) | info.head.lenth[1]&0x00ff;
	if(len!=ELETRON_MESG_LEN-ELETRON_HEAD_LEN)		
		{
			err=DATA_LEN_ERR;
			return err;
		}
	
	err=MESG_OK;
	return err;
}


/*
	body:�����壬����	32�ֽ�

*/
int decode_mesgbody(eletron_info * eleinfo, unsigned char *body)
{

	des3_context ctx3;
	unsigned char key[16];
	unsigned char outdata[ELETRON_MESG_LEN-ELETRON_HEAD_LEN]={0};
	int index;
	int len;
	
	len=((eleinfo->head.lenth[0]<<8)&0xff00) | eleinfo->head.lenth[1]&0x00ff;
	index=eleinfo->head.keyid;

	des3_set_2keys(&ctx3,keyarry[index],keyarry[index]+8);
	des3_encrypt(&ctx3,(char *)&eleinfo->head,key);
	

	/*���ܺ�����ݴ�ŵ�elinfo��*/	
	
	des3_set_2keys(&ctx3,key,key+8);
	int i;
	for(i=0;i<(sizeof(outdata)/8);i++)
		des3_decrypt(&ctx3,body+i*8,outdata+i*8);
		
	memcpy(&eleinfo->body,outdata,sizeof(outdata));
	return 0;	
}



/*
	��鿨�źϷ���
*/
int check_codeno(eletron_info eleinfo)
{
	

	return 0;
}

/*
	��鿨��
*/

int check_cardtype(eletron_info eleinfo)
{

	return 0;
}


/*
	����ѻ������޶��Ƿ�����ۺ�Ʊ��
*/

int check_over_offlinelimit(eletron_info eleinfo)
{

	unsigned short transvalue;
	unsigned short afterdiscont;
	
	transvalue=(eleinfo.body.offlinelimit[0]<<8)&0xff00| eleinfo.body.offlinelimit[1];

	if(transvalue>afterdiscont)
		return -1;
	return 0;
}


/*
	��֤��֤��
*/
int check_Verification_code(eletron_info eleinfo)
{

	unsigned char vrifydata[8];
	unsigned recivedata[8];
	int backlen,status;
	
	vrifydata[0]=eleinfo.body.cardno[0];
	vrifydata[1]=eleinfo.body.cardtype;
	memcpy(vrifydata+2,eleinfo.body.invalidtime,6)��

	/*
		ѡ��1003Ŀ¼
	*/

	unsigned char selectcmd[16]={0};
	selectcmd[0]=0x00;
	selectcmd[1]=0xA4;
	selectcmd[2]=0x00;		//p1
	selectcmd[3]=0x00;		//p2
	selectcmd[4]=0x02;		//lc
	selectcmd[5]=0x10;		//0x1003Ŀ¼
	selectcmd[6]=0x03;
	status=PsamCos(selectcmd,recivedata,&backlen);

	if(status!=0 ||(recivedata[backlen-2] != 0x90) || (recivedata[backlen-1] != 0x00) )
		return -1;
	/*
		������֤
	*/

	unsigned char vericmd[24];
	unsigned char KID;		//���ⲿ����

	vericmd[0]=0x80;
	vericmd[1]=0xfc;
	vericmd[2]=0x00;		//��׼ģʽ
	vericmd[3]=KID;
	vericmd[4]=0x0d;		//datalen

	memcpy(vericmd+5,vrifydata,8);		//Ҫ��֤������
	memcpy(vericmd+13,eleinfo.body.Authentication,4);		//��֤��
	vericmd[17]=0x01;
	PsamCos(vericmd,recivedata,&backlen);
	if((status == 0)&&(recivedata[backlen-2] == 0x90)&&(recivedata[backlen-1] == 0x00))
		return 0;
	else
		return -1;
	
}


/*
	�����ӿ�����Ч��
*/

int check_valid_time(eletron_info eleinfo)
{

	
	
}


/*
	mesg:��ά��ɨ����ԭʼ���ݣ�lenΪ�����ݳ���
*/

int eletron_card_process(unsigned char * mesg,int len)
{

	int loop,step;
	eletron_info eletinfo;
	ELETRON_ERROCODE err;
	
	loop=1;
	step=1;

	while(loop)
	{
		switch(step)
		{
			case 1:		//����Ƿ�Ϊ�״�ˢ��

					step++;
					break;
			case 2:		
					if(len<ELETRON_MESG_LEN)
						{
							err=MESG_LEN_ERR;
							loop=0;
							break;
						}
					memcpy(&eletinfo.head,mesg,sizeof(eletro_head));
					err=check_eletron_head(eletinfo);
					if(err!=MESG_OK)
						{
							loop=0;
							break;
						}
					step++;	
					break;
					
			case 3:		//
					decode_mesgbody(&eletinfo,mesg+ELETRON_MESG_LEN);
					step++;
					break;
					
			case 4:
				   if(check_codeno(eletinfo))
				   	{
				   		err=CARD_NO_ERR;
						loop=0;
						break;
				   	}
				   step++;
				   break;
			case 5:
				  if(check_cardtype(eletinfo))
						{
				   		err=CARD_TYPE_ERR;
						loop=0;
						break;
				   	}
				  step++;
				  break;
				  
			case 6:
				if(check_over_offlinelimit(eletinfo))
					{
						err=OVERUN_ERR;
						loop=0;
						break;
					}
				step++;
				break;

			case 7:
				if(check_Verification_code(eletinfo))
				 	{
						err=VERIFY_ERR;
						loop=0;
						break;
					}
				step++;
				break;
			case 8:
				if(check_valid_time(eletinfo))
					{
						err=INVALID_TIME_ERR;
						loop=0;
						break;
					}
				step++;
				break;

			case 9:
					err=MESG_OK;
					loop=0;
					break;
		}

	}

	return err;
}

