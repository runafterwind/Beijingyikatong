#include <ctype.h>
#include <string.h>
char strnum(char *strn , int len)
{
int i;
	for(i =0 ;i < len;i++)
	{
		if(isdigit(strn[i]) == 0)break;
	}
	if(i != len)return -1;
	return 0;
}

unsigned int myatoi(char *src)
{
unsigned int total = 0;
    while (*src)
    {
       total = total*10 + (unsigned int)(*src - '0');
       src++;
    }
    return total;
}

char strnumdot(char *strn , int len)
{
int i;
int err;
	for(i =0 ;i < len;i++)
	{
		if((isdigit(strn[i]) == 0)&&(strn[i] != '.'))break;
	}
	if(i != len)return -1;
	for(i =0 ,err = 0 ;i < len;i++)
	{
		if(strn[i] == '.')err++;
	}
	if(err > 1)return -1;
	return 0;
}

unsigned int struint(char *date)
{
char ch;
char strbuf[200];
int len,i;
	len = strlen(date);
	memset(strbuf,0,sizeof(strbuf));
	for(i = 0;i < len; i++)
	{
		if(date[i] == '.')break;
		strbuf[i] = date[i];
	}
	ch = i;
	len = len - i;
	if(len > 3)len = 3;
	switch(len)
	{
	 case 0:
	 case 1:
		strbuf[i++] = '0';
		strbuf[i++] = '0';
		strbuf[i] = 0;
		break;
	 case 2:
		strbuf[i++] = date[ch+1];
		strbuf[i++] = '0';
		strbuf[i] = 0;
		break;
	 case 3:
		strbuf[i++] = date[ch+1];
		strbuf[i++] = date[ch+2];
		strbuf[i] = 0;
		break;
	}
	len = myatoi(strbuf);
	return len;
}

void strmoney(char *date,char *monet)
{
char ch,cl;
int len;
	
	len = strlen(date);
	ch = date[len - 2];
	cl = date[len - 1];
	memcpy(monet,date,(len - 2));
	monet[len - 2] = '.';
	monet[len - 1] = ch;
	monet[len - 0] = cl;
}
