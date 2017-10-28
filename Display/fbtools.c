#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <asm/page.h>
#include <linux/fb.h>
#include "fbtools.h"
#include "apparel.h"
#include "../gui/initsystem.h"

#define HZK16  		"./font/hzk16c_ASC.DZK"
#define HZK32  		"./font/hzk32c_ASC.DZK"
#define HZK32gbk  	"./font/GBK32c_ASC.DZK"
#define HZK48       "./font/hzk48c_ASC.DZK"
#define ASC48       "./font/ASC48.DZK"

//open & init a frame buffer
unsigned short Mcolor;
unsigned short Textcolor;
unsigned short TextSize;

//PIXEL _pix[76800]={0};
//PIXEL _pix1[76800]={0};
unsigned char use_curent_back=0;	//使用当前frambuffer中的数据来清除文字背景0x05表示使用当前背景

//char *background;		//[153600];
BMP_INFO  bmppicture1={0};
BMP_INFO  bmppicture2={0};



unsigned short int PixColor;
unsigned short int PixColor1;

static struct fb_fix_screeninfo fix;
static struct fb_var_screeninfo var;

static char *fbdevice = "/dev/fb0";
static char *hzkascii = NULL;
static char *hzkascii_gbk = NULL;
static char *hzkascii32 = NULL;
static char *hzkascii48 = NULL;
static char *ascii48=NULL;

static unsigned char *fbuffer = NULL;
static unsigned char *fbuffera = NULL;
static unsigned char *backbuf=NULL;				//背景图片buffer


static int fb_fd;
int _ciBitCount, _ciWidth, _ciHeight;
char Eflag;   //刷卡显示图片标志
char Bflag;   //背影图片标志
//static int bits_per_pixel = 0;


/*
static const char signal_BMP[] =
{0x70,0x00,0x78,0x00,0x7E,0x00,0x6F,0x00,0x67,0x80,0x7F,0xFF,0x7F,0xFF,0x7F,0xFF,
0x67,0x80,0x6F,0x00,0x7E,0x00,0x7C,0x00,0x78,0x00,0x70,0x00,0x00,0x00,0x00,0x7F,
0x00,0x7F,0x00,0x7F,0x00,0x00,0x03,0xFF,0x03,0xFF,0x03,0xFF,0x00,0x00,0x3F,0xFF,
0x3F,0xFF,0x3F,0xFF,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00};
*/
/*"signal.bmp*/






long chartolong( char * string, int length );


/*
*************************************************************************************************************
- 函数名称 : static int hzkopen(void)
- 函数说明 : 打开字库文件     16*16
- 输入参数 : 无
- 输出参数 : 无
*************************************************************************************************************
*/
static int hzkopen(void)
{
	int file_len;
	int file_fd;
	struct stat buf;

	/*打开并计算文件长度*/
	// printf("playsound 01 \n");
	file_fd = open(HZK16, O_RDONLY);
	if(fstat(file_fd,&buf) < 0)
	{
		perror("HZK16 fstat error");
		return -1;
	}
	file_len = buf.st_size;
	hzkascii = (char*)malloc(file_len);
	if(hzkascii == NULL)
	{
		perror("hzkascii malloc error ");
		return -1;
	}
	read(file_fd,hzkascii,file_len);
	close(file_fd);
	return 0;
}



/*
*************************************************************************************************************
- 函数名称 : static void hzkclose32(void)
- 函数说明 : 打开字库文件     32*32
- 输入参数 : 无
- 输出参数 : 无
*************************************************************************************************************
*/
static int hzkopen32(void)
{
	int file_len;
	int file_fd;
	struct stat buf;

	/*打开并计算文件长度*/
	// printf("playsound 01 \n");
	printf("#####%s\n", HZK32);
	file_fd = open(HZK32, O_RDONLY);
	if(fstat(file_fd,&buf) < 0)
	{
		perror("HZK32 fstat error");
		return -1;
	}
	file_len = buf.st_size;
	hzkascii32 = (char*)malloc(file_len);
	if(hzkascii32 == NULL)
	{
		perror("hzkascii32 malloc error ");
		return -1;
	}
	read(file_fd,hzkascii32,file_len);
	close(file_fd);
	return 0;

}

static int hzkopen16_gbk(void)
{
	int file_len;
	int file_fd;
	struct stat buf;

	/*打开并计算文件长度*/
	// printf("playsound 01 \n");
	file_fd = open(HZK32gbk, O_RDONLY);
	if(fstat(file_fd,&buf) < 0)
	{
		perror("HZK16 fstat error");
		return -1;
	}
	file_len = buf.st_size;
	hzkascii_gbk = (char*)malloc(file_len);
	if(hzkascii_gbk== NULL)
	{
		perror("hzkascii_gbk malloc error ");
		return -1;
	}
	read(file_fd,hzkascii_gbk,file_len);
	close(file_fd);
	return 0;
}

static int hzkopen48(void)
{
	int file_len;
	int file_fd;
	struct stat buf;

	/*打开并计算文件长度*/
	// printf("playsound 01 \n");
	printf("#####%s\n", HZK48);
	file_fd = open(HZK48, O_RDONLY);
	if(fstat(file_fd,&buf) < 0)
	{
		perror("HZK32 fstat error");
		return -1;
	}
	file_len = buf.st_size;
	hzkascii48 = (char*)malloc(file_len);
	if(hzkascii48 == NULL)
	{
		perror("hzkascii32 malloc error ");
		return -1;
	}
	read(file_fd,hzkascii48,file_len);
	close(file_fd);
	return 0;

}

static int ascopen48(void)
{
	int file_len;
	int file_fd;
	struct stat buf;

	/*打开并计算文件长度*/
	// printf("playsound 01 \n");
	printf("#####%s\n", ASC48);
	file_fd = open(ASC48, O_RDONLY);
	if(fstat(file_fd,&buf) < 0)
	{
		perror("ASC48 fstat error");
		return -1;
	}
	file_len = buf.st_size;
	ascii48 = (char*)malloc(file_len);
	if(ascii48 == NULL)
	{
		perror("ASC48 malloc error ");
		return -1;
	}
	read(file_fd,ascii48,file_len);
	close(file_fd);
	return 0;

}

/*
*************************************************************************************************************
- 函数名称 : int open_framebuffer(void)
- 函数说明 : 打开FrameBuffer
- 输入参数 : 无
- 输出参数 : 无
*************************************************************************************************************
*/
int open_framebuffer(void)
{
	int result;

	result = hzkopen();
	if (result == -1) {
		perror("open hzk16.DZK error!\n");
		return -1;
	}

	if(access("/var/run/font/GBK32c_ASC.DZK",0)==0)
	{
		result = hzkopen16_gbk();
		if (result == -1) {
			perror("open hzk16_GBK.DZK error!\n");
			return -1;
		}
	}
	else
	{
		printf("ERROR! GBK16 hzk16_GBK.DZK can't find\n");
	}

	result = hzkopen32();
	if (result == -1) {
		perror("open hzk32.DZK error!\n");
		return -1;
	}
#ifdef NEW0409
	result = hzkopen48();
	if (result == -1) {
		perror("open hzk48.DZK error!\n");
		return -1;
	}

	result = ascopen48();
	if (result == -1) {
		perror("open asc48.DZK error!\n");
		return -1;
	}
#endif

	fb_fd = open(fbdevice, O_RDWR);
	if (fb_fd == -1) {
		perror("open fbdevice");
		close(fb_fd);
		return -1;
	}

	if (ioctl(fb_fd, FBIOGET_FSCREENINFO, &fix) < 0) {
		perror("ioctl FBIOGET_FSCREENINFO");
		close(fb_fd);
		return -1;
	}

	if (ioctl(fb_fd, FBIOGET_VSCREENINFO, &var) < 0) {
		perror("ioctl FBIOGET_VSCREENINFO");
		close(fb_fd);
		return -1;
	}

	//printf("var.yres = %d,var.xres = %d\n",var.yres, var.xres);
	fbuffera = mmap(NULL, fix.smem_len, PROT_READ | PROT_WRITE, MAP_FILE | MAP_SHARED, fb_fd, 0);
	if (fbuffer == (unsigned char *)-1) {
		perror("mmap a framebuffer");
		close(fb_fd);
		return -1;
	}

	//printf("var.yres = %d,var.xres = %d\n",var.yres, var.xres);
	fbuffer = (unsigned char *)malloc(fix.smem_len);
	if (fbuffer == NULL) {
		perror("mmap b framebuffer");
		return -1;
	}


	//memset(fbuffer,0xff,fix.smem_len);
	return 0;
}




void UpdataApp(int x1,int y1,int x2,int y2)
{
	int i,n,xa,ya;
	unsigned short *str1,*str2;

	xa = x2 - x1;
	ya = y2 - y1;

	for(i=0; i<ya; i++)
	{
        
		str1 = ((unsigned short *)fbuffera + (y1+i) * var.xres + x1);
		str2 = ((unsigned short *)fbuffer + (y1+i) * var.xres + x1);
		for(n =0; n<xa; n++)
		{
			if(str2[n] != str1[n])
			{
				str1[n] = str2[n];
			}
		}
	}
}


/*
*************************************************************************************************************
- 函数名称 :void close_framebuffer(void)
- 函数说明 ://关闭FrameBuffer
- 输入参数 : 无
- 输出参数 : 无
*************************************************************************************************************
*/
void close_framebuffer(void)
{
	munmap(fbuffera, fix.smem_len);
	close(fb_fd);
	free(hzkascii);
	free(hzkascii32);
	free(fbuffer);

}

/*
*************************************************************************************************************
- 函数名称 :static int pixel(int x,int y,unsigned short color)
- 函数说明 ://显示一个像素
- 输入参数 : 无
- 输出参数 : 无
*************************************************************************************************************
*/
static int pixel(int x,int y,unsigned short color)
{
	if((x >= var.xres)||(y >= var.yres))
		return (-1);
	unsigned short *dst = ((unsigned short *)fbuffer + y * var.xres + x);
	*dst = color;
	return (0);
}



/*
*************************************************************************************************************
- 函数名称 :void line (int x1, int y1, int x2, int y2, unsigned short colidx)
- 函数说明 ://画线函数
- 输入参数 : 无
- 输出参数 : 无
*************************************************************************************************************
*/
void line (int x1, int y1, int x2, int y2, unsigned short colidx)
{
	int tmp;
	int dx = x2 - x1;
	int dy = y2 - y1;

	if (abs (dx) < abs (dy)) {
		if (y1 > y2) {
			tmp = x1;
			x1 = x2;
			x2 = tmp;
			tmp = y1;
			y1 = y2;
			y2 = tmp;
			dx = -dx;
			dy = -dy;
		}
		x1 <<= 16;
		/* dy is apriori >0 */
		dx = (dx << 16) / dy;
		while (y1 <= y2) {
			pixel (x1 >> 16, y1, colidx);
			x1 += dx;
			y1++;
		}
	} else {
		if (x1 > x2) {
			tmp = x1;
			x1 = x2;
			x2 = tmp;
			tmp = y1;
			y1 = y2;
			y2 = tmp;
			dx = -dx;
			dy = -dy;
		}
		y1 <<= 16;
		dy = dx ? (dy << 16) / dx : 0;
		while (x1 <= x2) {
			pixel (x1, y1 >> 16, colidx);
			y1 += dy;
			x1++;
		}
	}
}

/*
*************************************************************************************************************
- 函数名称 :void rect (int x1, int y1, int x2, int y2, unsigned short colidx)
- 函数说明 ://矩形框
- 输入参数 : 无
- 输出参数 : 无
*************************************************************************************************************
*/
void rect (int x1, int y1, int x2, int y2, unsigned short colidx)
{
	line (x1, y1, x2, y1, colidx);
	line (x2, y1, x2, y2, colidx);
	line (x2, y2, x1, y2, colidx);
	line (x1, y2, x1, y1, colidx);
}

/*
*************************************************************************************************************
- 函数名称 :void fillrect (int x1, int y1, int x2, int y2, unsigned colidx)
- 函数说明 ://矩形面
- 输入参数 : 无
- 输出参数 : 无
*************************************************************************************************************
*/
void fillrectcoli (int x1, int y1, int x2, int y2, unsigned short colidx,unsigned char *fbs)
{

	int i,n,xa,ya;
	unsigned short *str2;

	xa = x2 - x1;

	ya = y2 - y1;
	//DBG_PRINTF("in func %s:\n",__func__);

	for(i=0; i<ya; i++)
	{
        
    		str2 = ((unsigned short *)fbs + (y1+i) * var.xres + x1);       
    		for(n=0; n<xa; n++)
    		{
    			str2[n] = colidx;
    		}
              
	}

}

void fillrectusebackground(int x1, int y1, int x2, int y2, unsigned char *fbs)
{
	int i,n,xa,ya;
	unsigned short *strdest,*strsrc;
	unsigned short colidx;
	xa = x2 - x1;
	ya = y2 - y1;
	
	for(i=0; i<ya; i++)
	{
		strdest = ((unsigned short *)fbs + (y1+i) * var.xres + x1);
		strsrc	= ((unsigned short *)backbuf + (y1+i) * var.xres + x1);
	
		for(n=0; n<xa; n++)
		{
			strdest[n] = strsrc[n];
		}
	}
}

int fillrectuseframbuffer(int x1,int y1,int x2,int y2,unsigned char *src,unsigned char *dest)
{
	int i,n,xa,ya;
	unsigned short *strdest,*strsrc;
	unsigned short colidx;
	xa = x2 - x1;
	ya = y2 - y1;
//	printf("----in func -- %s \n",__func__);
	for(i=0; i<ya; i++)
	{
		strdest = ((unsigned short *)dest + (y1+i) * var.xres + x1);
		strsrc	= ((unsigned short *)src + (y1+i) * var.xres + x1);
		
		for(n=0; n<xa; n++)
		{
			strdest[n] = strsrc[n];
			//if(strsrc[n]==Color_blue)
			//	printf(" wrong pixel \n");
		}
	}

	return 0;

}



/*
*************************************************************************************************************
- 函数名称 :void fillrect (int x1, int y1, int x2, int y2, unsigned colidx)
- 函数说明 ://矩形面
- 输入参数 : 无
- 输出参数 : 无
*************************************************************************************************************
*/
void fillrect (int x1, int y1, int x2, int y2, unsigned colidx)
{
	int tmp,i;
	/* Clipping and sanity checking */
	if (x1 > x2) {
		tmp = x1;
		x1 = x2;
		x2 = tmp;
	}
	if (y1 > y2) {
		tmp = y1;
		y1 = y2;
		y2 = tmp;
	}
	if (x1 < 0) x1 = 0;
	if ((__u32)x1 >= var.xres) x1 = var.xres - 1;
	if (x2 < 0) x2 = 0;
	if ((__u32)x2 >= var.xres) x2 = var.xres - 1;
	if (y1 < 0) y1 = 0;
	if ((__u32)y1 >= var.yres) y1 = var.yres - 1;
	if (y2 < 0) y2 = 0;
	if ((__u32)y2 >= var.yres) y2 = var.yres - 1;

	if ((x1 > x2) || (y1 > y2))
		return;
	tmp = y2 - y1;
	for(i = 0; i < tmp; i++)
	{
		line (x1, y1+i, x2, y1+i, colidx);
	}

}

/*
*************************************************************************************************************
- 函数名称 :void put_hz(unsigned char *dispBuf,unsigned int x1,unsigned int y1,unsigned short color)
- 函数说明 ://在x1,y1起开始显示一个16x16的汉字
- 输入参数 : 无
- 输出参数 : 无
*************************************************************************************************************
*/
void put_hz(unsigned char *dispBuf,unsigned int x1,unsigned int y1,unsigned short color)
{
	int i,j,n;
	unsigned char list;
	n = 0;
	for(i = 0; i < 16; i++,n++)
	{
		list = dispBuf[n];
		for(j=0; j<8; j++)
		{
			if(list&(1<<(7-j)))
			{
				pixel(y1+j,x1+i, color);
			}

		}
		n++;
		list = dispBuf[n];
		for(j=0; j<8; j++)
		{
			if(list&(1<<(7-j)))
			{
				pixel(y1+8+j,x1+i,color);
			}
		}
	}
}


/*
*************************************************************************************************************
- 函数名称 :void put_hz(unsigned char *dispBuf,unsigned int x1,unsigned int y1,unsigned short color)
- 函数说明 ://在x1,y1起开始显示一个32*32的汉字
- 输入参数 : 无
- 输出参数 : 无
*************************************************************************************************************
*/
void put_hz_32(unsigned char *dispBuf,unsigned int x1,unsigned int y1,unsigned short color)
{
	int i,j,n;
	unsigned char list;
	n = 0;
	for(i = 0; i < 32; i++,n++)
	{
		list = dispBuf[n];
		for(j=0; j<16; j++)
		{
			if(list&(1<<(15-j)))
			{
				pixel(y1+j,x1+i, color);
			}

		}
		n++;
		list = dispBuf[n];
		for(j=0; j<16; j++)
		{
			if(list&(1<<(15-j)))
			{
				pixel(y1+16+j,x1+i,color);
			}
		}
	}
}


/*
*************************************************************************************************************
- 函数名称 :void put_char(unsigned char *dispBuf,unsigned int x1,unsigned int y1,unsigned short color)
- 函数说明 ://在x1,y1起开始显示一个ASCII_8x16
- 输入参数 : 无
- 输出参数 : 无
*************************************************************************************************************
*/
void put_char(unsigned char *dispBuf,unsigned int x1,unsigned int y1,unsigned short color)
{
	int i,j,n;
	unsigned char list;
	n = 0;
	for(i = 0; i < 16; i++,n++)
	{
		list = dispBuf[n];
		for(j=0; j<8; j++)
		{
			if(list&(1<<(7-j)))
			{
				pixel(y1+j,x1+i, color);
			}

		}
	}
}

/*
*************************************************************************************************************
- 函数名称 :static void _GetPixel(unsigned char QM, unsigned char WM, unsigned char *pxBuff)
- 函数说明 : 根据区位码找字库数据返回
- 输入参数 : 无
- 输出参数 : 无
*************************************************************************************************************
*/
static void _GetPixel(unsigned char QM, unsigned char WM, unsigned char *pxBuff)
{
	unsigned int  Loff;
//unsigned char i;
//unsigned char loopt;

	if (WM == 0 )
	{
		Loff = (unsigned int)(QM) * (unsigned int)16 +(unsigned int)0x45080;
		memcpy(pxBuff,hzkascii+Loff,16);

		//fseek(hzk,Loff,SEEK_SET);
		//fread(pxBuff,16,1,hzk);

	}
	else
	{
		Loff = ( (unsigned int)(QM-0xa1) * (unsigned int)94 + (unsigned int)(WM-0xa1) ) * (unsigned int)32;
		//fseek(hzk,Loff,SEEK_SET);
		//fread(pxBuff,32,1,hzk);
		memcpy(pxBuff,hzkascii+Loff,32);
	}
}


/*
*************************************************************************************************************
- 函数名称 :void put_string(int x1, int y1, char *pzText, unsigned color)
- 函数说明 : //在x,y处显示一串字符  16 * 16
- 输入参数 : 无
- 输出参数 : 无
*************************************************************************************************************
*/
void put_string(int x1, int y1, char *pzText, unsigned color)
{
	unsigned char i,QM,WM;
	unsigned char hzkbuf[32];
	unsigned int x,y;
	x = x1;
	y = y1;
	i = 0;
	while ( *(pzText+i) > 0 )
	{
		QM = *(pzText+i);
		if ( QM <= 0x80 )
		{
			if((y < var.yres)&&(x < var.xres))
			{
				_GetPixel(QM,0,hzkbuf);
				put_char(hzkbuf,y,x,color);
				x = x + 8;
			}
			i++;
		}
		else
		{
			if (i+1)
			{
				WM = *(pzText+i+1);
				if (WM > 0 )
				{
					if((y < var.yres)&&(x < var.xres))
					{
						_GetPixel(QM,WM,hzkbuf);
						put_hz(hzkbuf,y,x,color);
						x = x + 16;
					}
					i++;
				}
				i++;
			}
			else i++;
		}
	}
}


/*
*************************************************************************************************************
- 函数名称 : void put_string_center(int x, int y, char *s, unsigned colidx)
- 函数说明 :
- 输入参数 : 无
- 输出参数 : 无
*************************************************************************************************************
*/
void put_string_center(int x, int y, char *s, unsigned colidx)
{
	size_t sl = strlen (s);
	//put_string (x - (sl / 2) * font_vga_8x8.width,
	//            y - font_vga_8x8.height / 2, s, colidx);
	put_string (x - (sl / 2) * 8,
	            y - 16 / 2, s, colidx);

}

/*
******************************put_char32*******************************************************************************
- 函数名称 : void _draw_model(char *pdata, int w, int h, int x, int y, int color)
- 函数说明 : //在x,y处显示一个字符
- 输入参数 : 无
- 输出参数 : 无
*************************************************************************************************************
*/
void _draw_model(char *pdata, int w, int h, int x, int y, int color)
{
	int     i;    /* 控制行 */
	int     j;    /* 控制一行中的8个点 */
	int     k;    /* 一行中的第几个"8个点"了 */
	int     nc;   /* 到点阵数据的第几个字节了 */
	int     cols; /* 控制列 */

	w = (w + 7) / 8 * 8; /* 重新计算w */
	nc = 0;
	for (i=0; i< h; i++)
	{
		cols = 0;
		for (k=0; k<w/8; k++)
		{
			for (j=0; j<8; j++)
			{
				if(pdata[nc]&(1<<(7-j)))
				{
					pixel(x+cols, y+i, color);
				}
				cols++;
			}
			nc++;
		}
	}
}

/*
*************************************************************************************************************
- 函数名称 : void _draw_char(char *hz, int x, int y, int w, int h, int color)
- 函数说明 : //在x,y处显示一个字符
- 输入参数 : 无
- 输出参数 : 无
*************************************************************************************************************
*/
void _draw_char(char hz, int x, int y, int w, int h, int color)
{
	char fontbuf[128];   /* 足够大的缓冲区,也可以动态分配 */
	/* 计算偏移 */
	long offset = (long)(hz*64 + 0x114200);

// fseek(hzk32, offset, SEEK_SET); /* 进行寻址 */
// fread(fontbuf,1,64, hzk32);   /* 读入点阵数据 */
	memcpy(fontbuf,hzkascii32+offset,64);
	_draw_model(fontbuf, w, h, x, y, color);    /* 绘制字模 */
}

/*
*************************************************************************************************************
- 函数名称 : void _draw_char(char *hz, int x, int y, int w, int h, int color)
- 函数说明 : //在x,y处显示一个字符
- 输入参数 : 无
- 输出参数 : 无
*************************************************************************************************************
*/
void _draw_char48(char hz, int x, int y, int w, int h, int color)
{
	char fontbuf[288];   /* 足够大的缓冲区,也可以动态分配 */
	/* 计算偏移 */
	long offset = (long)(hz*144+0);

// fseek(hzk32, offset, SEEK_SET); /* 进行寻址 */
// fread(fontbuf,1,64, hzk32);   /* 读入点阵数据 */
	memcpy(fontbuf,ascii48+offset,144);
	_draw_model(fontbuf, w, h, x, y, color);    /* 绘制字模 */
}


/*
*************************************************************************************************************
- 函数名称 : void _draw_hz(char *hz, int x, int y, int w, int h, int color)
- 函数说明 : //在x,y处显示一个字符
- 输入参数 : 无
- 输出参数 : 无
*************************************************************************************************************
*/
void _draw_hz(char *hz, int x, int y, int w, int h, int color)
{

	char fontbuf[128];   /* 足够大的缓冲区,也可以动态分配 */
	int ch0 = hz[0]-0xA0; /* 区码 */
	int ch1 = hz[1]-0xA0; /* 位码 */

	/* 计算偏移 */
	long offset = (long)(128 * ((ch0 - 1) * 94 + ch1 - 1));
	//fseek(hzk32, offset, SEEK_SET);              /* 进行寻址 */
	//fread(fontbuf,1,(w + 7) / 8 * h, hzk32);   /* 读入点阵数据 */
	memcpy(fontbuf,hzkascii32+offset,(w + 7) / 8 * h);
	_draw_model(fontbuf, w, h, x, y, color);    /* 绘制字模 */

}

/*
*************************************************************************************************************
- 函数名称 : void _draw_hz(char *hz, int x, int y, int w, int h, int color)
- 函数说明 : //在x,y处显示一个字符
- 输入参数 : 无
- 输出参数 : 无
*************************************************************************************************************
*/
void _draw_hz48(char *hz, int x, int y, int w, int h, int color)
{

	char fontbuf[288];   /* 足够大的缓冲区,也可以动态分配 */
	int ch0 = hz[0]-0xA0; /* 区码 */
	int ch1 = hz[1]-0xA0; /* 位码 */

	/* 计算偏移 */
    
	long offset = (long)(288 * ((ch0 - 1) * 94 + ch1 - 1));
	//fseek(hzk32, offset, SEEK_SET);              /* 进行寻址 */
	//fread(fontbuf,1,(w + 7) / 8 * h, hzk32);   /* 读入点阵数据 */
	memcpy(fontbuf,hzkascii48+offset,(w + 7) / 8 * h);
	_draw_model(fontbuf, w, h, x, y, color);    /* 绘制字模 */

}


/*
*************************************************************************************************************
- 函数名称 : void put_string32(int x1, int y1, char *pzText, unsigned color)
- 函数说明 : //在x,y处显示一串字符
- 输入参数 : 无
- 输出参数 : 无
*************************************************************************************************************
*/
void put_string32(int x1, int y1, char *pzText, unsigned color)
{
	unsigned char i,QM,WM;
//unsigned char hzkbuf[128];
	unsigned int x,y;
	x = x1;
	y = y1;
	i = 0;
	while ( *(pzText+i) > 0 )
	{
		QM = *(pzText+i);
		if ( QM <= 0x80 )
		{
			if((y < var.yres)&&(x < var.xres))
			{
				_draw_char(QM,x,y,16,32,color);
				x = x + 16;
			}
			i++;
		}
		else
		{
			if(i+1)
			{
				WM = *(pzText+i+1);
				if (WM > 0 )
				{
					if((y < var.yres)&&(x < var.xres))
					{
						char buff[2];
						buff[0] = QM;
						buff[1] = WM;
						//put_hz32(hzkbuf,y,x,color);
						
						_draw_hz(buff,x,y,32,32,color);
						x = x + 32;
					}
					i++;
				}
				i++;
			}
			else i++;
		}
	}
}


/*
*************************************************************************************************************
- 函数名称 : void put_string32(int x1, int y1, char *pzText, unsigned color)
- 函数说明 : //在x,y处显示一串字符
- 输入参数 : 无
- 输出参数 : 无
*************************************************************************************************************
*/
void put_string48(int x1, int y1, char *pzText, unsigned color)
{
	unsigned char i,QM,WM;
//unsigned char hzkbuf[128];
	unsigned int x,y;
	x = x1;
	y = y1;
	i = 0;
	while ( *(pzText+i) > 0 )
	{
		QM = *(pzText+i);
      
		if ( QM <= 0x80 )
		{
			if((y < var.yres)&&(x < var.xres))
			{
				_draw_char48(QM,x,y,24,48,color);
				x = x + 24;
			}
			i++;
		}
		else
		{
			if(i+1)
			{
				WM = *(pzText+i+1);
               
				if (WM > 0 )
				{
					if((y < var.yres)&&(x < var.xres))
					{
						char buff[2];
						buff[0] = QM;
						buff[1] = WM;
						//put_hz32(hzkbuf,y,x,color);
						
						_draw_hz48(buff,x,y,48,48,color);
						x = x + 48;
					}
					i++;
				}
				i++;
			}
			else i++;
		}
	}
}

/*
*************************************************************************************************************
- 函数名称 : void InitDisplay(void)
- 函数说明 :
- 输入参数 : 无
- 输出参数 : 无
*************************************************************************************************************
*/
void InitDisplay(void)
{
	open_framebuffer();
	SetColor(Color_blue);
	Mcolor = Color_blue;
	Textcolor = Color_white;
	TextSize = 16;
	
}


int load_backm(int x, int y,uint8_t *buf,char * Path)	//x,y代表其实位置，buf即为freambuffer的映射
{
	FILE *fp;
	BITMAPFILEHEADER FileHead;
	BITMAPINFOHEADER InfoHead;
	int rc;
	int ciBitCount, ciWidth, ciHeight;
	int line_x, line_y;
	long int location = 0;

#if !_F26
	if((x >= var.xres)||(y >= var.yres))
		return (-1);
#endif
	/* 打开位图文件 */
	fp = fopen(Path, "rb" );
	if (fp == NULL)
	{
		return( -1 );
	}

	/* 读取位图文件头 */
	rc = fread( &FileHead, sizeof(BITMAPFILEHEADER),1, fp );
	if ( rc != 1)
	{
		printf("read header error!\n");
		fclose( fp );
		return( -2 );
	}

	/* 判断位图的类型 */
	if (memcmp(FileHead.cfType, "BM", 2) != 0)
	{
		printf("it's not a BMP file\n");
		fclose( fp );
		return( -3 );
	}

	/* 读取位图信息头 */
	rc = fread( (char *)&InfoHead, sizeof(BITMAPINFOHEADER),1, fp );
	if ( rc != 1)
	{
		printf("read infoheader error!\n");
		fclose( fp );
		return( -4 );
	}

	ciWidth    = (int) chartolong( InfoHead.ciWidth,	4 );
	ciHeight   = (int) chartolong( InfoHead.ciHeight,	4 );
	ciBitCount = (int) chartolong( InfoHead.ciBitCount, 2 );	

	fseek(fp, (int)chartolong(FileHead.cfoffBits, 4), SEEK_SET);

	line_x = line_y = 0;
	while(!feof(fp))
	{

		PIXEL pix;
		PIXEL24 pix24;
		
		unsigned short int tmp;
		if(ciBitCount==16)
		{
			rc = fread( (char *)&pix, 1, sizeof(unsigned short int), fp );	//读出一个像素数据
			if (rc != sizeof(unsigned short int) )
			{
				break;
			}
		}
		else if(ciBitCount==24)
		{
			rc = fread( (char *)&pix24, 1, sizeof(PIXEL24), fp );
			if (rc != sizeof(PIXEL24) )
			{
				break;
			}
			//将24位像素转换为16位像素
			pix.blue=pix24.re>>3;
			pix.green=pix24.gr>>3;
			pix.red=pix24.bl>>3;
		}

		
		location = line_x * var.bits_per_pixel / 8 + (ciHeight - line_y - 1) * var.xres * var.bits_per_pixel / 8;

		tmp=pix.red<<11 | pix.green<<6 | pix.blue;
		*((unsigned short int*)(buf + y * var.xres + x + location)) = tmp;

		line_x++;
		if (line_x == ciWidth )
		{
			line_x = 0;
			line_y++;

			if(line_y==ciHeight-1)
			{
				break;
			}
		}
	}
	fclose( fp );
		//DBG_PRINTF("show bmp.\n");
		/*UpdataApp(var.xres-x,var.yres-y,100,y+32);*/
		return( 0 );
	
}


void uninit_backgroud(void)
{
	if (backbuf) {
		free(backbuf);
		backbuf = NULL;
	}
}

void init_backgroud(void)
{
	backbuf = (unsigned char *)malloc(fix.smem_len);	//fix.smem_len为framebuffer的大小
	if (backbuf == NULL) {
		perror("mmap b backbuf error");
		return ;
	}
	if(access("/var/run/logo.bmp",0)==0)
	{
		if (load_backm(0,0,backbuf,"/var/run/logo.bmp") == 0) {
			memcpy((char *)fbuffera, (char *)backbuf,fix.smem_len);
		} else {
			uninit_backgroud();
		}
	}
	else{
		printf("背景图片不存在\n");
		return ;
	}

	if(backbuf!=NULL)
		memcpy(fbuffera,backbuf,fix.smem_len);
}



/*
*************************************************************************************************************
- 函数名称 : void CloseDisplay(void)
- 函数说明 :
- 输入参数 : 无
- 输出参数 : 无
*************************************************************************************************************
*/
void CloseDisplay(void)
{
	close_framebuffer();
}


static void _GetPixelgbk(unsigned char QM, unsigned char WM, unsigned char *pxBuff)
{
	unsigned int  Loff;
	unsigned char line;
	if(WM<=0X7f)
	{
		line=0x40;
	}
	else
	{
		line=0x41;
	}
	Loff =  ((unsigned int)(QM-0x81)* (unsigned int)190  + (unsigned int)(WM-line))* (unsigned int)128;
	memcpy(pxBuff, hzkascii_gbk+Loff, 128);
}



/*
*************************************************************************************************************
- 函数名称 :void put_string(int x1, int y1, char *pzText, unsigned color)
- 函数说明 : //在x,y处显示一串字符  16 * 16
- 输入参数 : 无
- 输出参数 : 无
*************************************************************************************************************
*/
void put_stringgbk(int x1, int y1, char *pzText, unsigned color)
{
	unsigned char i,QM,WM;
	unsigned char hzkbuf[128];
	unsigned int x,y;
	x = x1;
	y = y1;
	i = 0;

	memset(hzkbuf, 0, sizeof(hzkbuf));
	
	while ( *(pzText+i) > 0 )
	{
		QM = *(pzText+i);
		if ( QM <= 0x80 )
		{
			if((y < var.yres)&&(x < var.xres))
			{
				_draw_char(QM,x,y,16,32,color);
				x = x + 16;
			}
			i++;
		}
		else
		{
			if (i+1)
			{
				WM = *(pzText+i+1);
				if (WM > 0)
				{
					if((y < var.yres)&&(x < var.xres))
					{
						_GetPixelgbk(QM,WM,hzkbuf);
						_draw_model(hzkbuf, 32, 32, x, y, color);    /* 绘制字模 */
						x = x + 32;
					}
					i++;
				}
				i++;
			}
			else i++;
		}
	}
}

void TextOut_CMD(int x1, int y1, char *pzText,int cmd)
{
		switch(cmd)
			{
				case 0:
				     break;
				default:
					TextOut( x1,y1,pzText);
					break;
				
			}
		
}



/*
*************************************************************************************************************
- 函数名称 : void TextOut_Font(int x1, int y1, char *pzText, char fontSize)
- 函数说明 :
- 输入参数 : 无
- 输出参数 : 无
*************************************************************************************************************
*/
void TextOut_fontSize(int x1, int y1, char *pzText, char fontSize)
{
   
	int lenx;
	if(fontSize!=16 && fontSize!=32)
		fontSize = 16;

	lenx = strlen(pzText);
	if(fontSize <= 16)
	{
		lenx = lenx*8;
		if(x1==0)
		{
			x1=(MAXLEN-lenx)/2;
		}
		if(use_curent_back)
			{
              //  printf("使用当前背影11\n");
                fillrectuseframbuffer(x1,y1,x1+lenx,y1+16,fbuffera,fbuffer);
          }
		else if(backbuf!=NULL)
			{
            //    printf("使用图片背影22\n");
                fillrectusebackground(x1,y1,x1+lenx,y1+16,fbuffer);
          }
		else
			fillrectcoli(x1,y1,x1+lenx,y1+16,Mcolor,fbuffer);
		//fillrect(0, y1, var.xres - 1, y1+16, Mcolor);
		put_string(x1,y1,pzText,Textcolor);
		UpdataApp(x1,y1,x1+lenx,y1+16);
	}
	else
	{
		lenx = lenx*16;
		if(x1==0)
		{
			x1=(MAXLEN-lenx)/2;
		}
		if(use_curent_back)
			fillrectuseframbuffer(x1,y1,x1+lenx,y1+32,fbuffera,fbuffer);
		else if(backbuf!=NULL)
			fillrectusebackground(x1,y1,x1+lenx,y1+32,fbuffer);
		else
			fillrectcoli(x1,y1,x1+lenx,y1+32,Mcolor,fbuffer);
		//fillrect(0, y1, var.xres - 1, y1+32, Mcolor);
		put_string32(x1,y1,pzText,Textcolor);
		UpdataApp(x1,y1,x1+lenx,y1+32);
	}
//  UpdataApp();
	// usleep(1000);
}



/*
*************************************************************************************************************
- 函数名称 : void TextOut(int x1, int y1, char *pzText)
- 函数说明 :
- 输入参数 : 无
- 输出参数 : 无
//put_string(50,100,"卡联科技有限公司",0);
//put_string32(50,100,"卡联科技有限公司",0);
*************************************************************************************************************
*/
void TextOut(int x1, int y1, char *pzText)
{

    
	int lenx;
	lenx = strlen(pzText);
	if(TextSize <= 16)
	{
		lenx = lenx*8;
		if(x1==0)
		{
			x1=(MAXLEN-lenx)/2;
		}
		if(use_curent_back == 0x05)
		{
				
			fillrectuseframbuffer(x1,y1,x1+lenx,y1+16,fbuffera,fbuffer);
			//memcpy(fbuffer,fbuffera,fix.smem_len);
		}
		else if(backbuf!=NULL){
            
			fillrectusebackground(x1,y1,x1+lenx,y1+16,fbuffer);
			}
		else
			fillrectcoli(x1,y1,x1+lenx,y1+16,Mcolor,fbuffer);
		//fillrect(0, y1, var.xres - 1, y1+16, Mcolor);
		
		put_string(x1,y1,pzText,Textcolor);
		UpdataApp(x1,y1,x1+lenx,y1+16);
	}	
	else if(TextSize == 17)
	{
		lenx = lenx*16;
		if(x1==0)
		{
			x1=(MAXLEN-lenx)/2;
		}
		if(use_curent_back==0x05)
		{
			fillrectuseframbuffer(x1,y1,x1+lenx,y1+32,fbuffera,fbuffer);
		}
		else if(backbuf!=NULL)
				fillrectusebackground(x1,y1,x1+lenx,y1+32,fbuffer);
		else
				fillrectcoli(x1,y1,x1+lenx,y1+32,Mcolor,fbuffer);
		//fillrect(0, y1, var.xres - 1, y1+16, Mcolor);
		put_stringgbk(x1,y1,pzText,Textcolor);
		UpdataApp(x1,y1,x1+lenx,y1+32);
	}
	else if(TextSize == 48)
	{
        
		lenx = lenx*24;
		if(x1==0)
		{
			x1=(MAXLEN-lenx)/2;
		}
		if(use_curent_back==0x05){
					fillrectuseframbuffer(x1,y1,x1+lenx,y1+48,fbuffera,fbuffer);					
		
			}			
		else if(backbuf!=NULL)
				fillrectusebackground(x1,y1,x1+lenx,y1+48,fbuffer);
		else
				fillrectcoli(x1,y1,x1+lenx,y1+48,Mcolor,fbuffer);
		//fillrect(0, y1, var.xres - 1, y1+32, Mcolor);
		put_string48(x1,y1,pzText,Textcolor);
		UpdataApp(x1,y1,x1+lenx,y1+48);
	
        }

    else
	{
		lenx = lenx*16;
		if(x1==0)
		{
			x1=(MAXLEN-lenx)/2;
		}
		if(use_curent_back==0x05){
					fillrectuseframbuffer(x1,y1,x1+lenx,y1+32,fbuffera,fbuffer);					
		
			}			
		else if(backbuf!=NULL)
				fillrectusebackground(x1,y1,x1+lenx,y1+32,fbuffer);
		else
				fillrectcoli(x1,y1,x1+lenx,y1+32,Mcolor,fbuffer);
		//fillrect(0, y1, var.xres - 1, y1+32, Mcolor);
		put_string32(x1,y1,pzText,Textcolor);
		UpdataApp(x1,y1,x1+lenx,y1+32);
	}
//  UpdataApp();
	// usleep(1000);
}



void TextOut_1(int x1, int y1, char *pzText,unsigned short Colour,unsigned short TColour)
{
#ifdef NEW0409
    x1 = 0;
#endif    
	int lenx;
	lenx = strlen(pzText);
	if(TextSize <= 16)
	{
		lenx = lenx*8;
		if(x1==0)
		{
			x1=(MAXLEN-lenx)/2;
		}
		fillrectcoli(x1,y1,x1+lenx,y1+16,Colour,fbuffer);
		put_string(x1,y1,pzText,TColour);
		UpdataApp(x1,y1,x1+lenx,y1+16);
	}
	else
	{
		lenx = lenx*16;
		if(x1==0)
		{
			x1=(MAXLEN-lenx)/2;
		}
		fillrectcoli(x1,y1,x1+lenx,y1+32,Colour,fbuffer);
		put_string32(x1,y1,pzText,TColour);
		UpdataApp(x1,y1,x1+lenx,y1+32);
	}
}


/*
*************************************************************************************************************
- 函数名称 : void SetTextSize(unsigned short size)
- 函数说明 :
- 输入参数 : 无
- 输出参数 : 无
*************************************************************************************************************
*/
void SetTextSize(unsigned short size)
{
	TextSize = size;
}

/*
*************************************************************************************************************
- 函数名称 : void SetTextColor(unsigned short color)
- 函数说明 :
- 输入参数 : 无
- 输出参数 : 无
*************************************************************************************************************
*/
void SetTextColor(unsigned short color)
{
	Textcolor = color;
}

/*
*************************************************************************************************************
- 函数名称 : unsigned short RGB565(unsigned char r,unsigned g,unsigned char b)
- 函数说明 : //调色函数
- 输入参数 : 无
- 输出参数 : 无
*************************************************************************************************************
*/
unsigned short RGB565(unsigned char r,unsigned g,unsigned char b)
{
//RGB---------565
	unsigned short rgb;
	rgb = (unsigned short)(((r&0x1f) << 11)|((g&0x3f) << 5)|(b&0x1f));
	return rgb;
}

/*
*************************************************************************************************************
- 函数名称 : void SetColor(unsigned short color)
- 函数说明 : //控制整体屏幕的颜色函数
- 输入参数 : 无
- 输出参数 : 无
*************************************************************************************************************
*/
void SetColor(unsigned short color)
{
	unsigned short *cor;
	unsigned int i,ch;

	if(backbuf!=NULL){
			memcpy(fbuffera,backbuf,fix.smem_len);
		}
	else{
			i = 0;
			ch = fix.smem_len / 2;
			cor = (unsigned short *)fbuffera;
			do
			{
				*cor++ = color;
				i++;
			} while(i < ch);
		}
}


//设置背景颜色
//posx-X坐标，posy-Y坐标
//L-长，W-宽
//color-填充的背景色
void SetBKColorAnywhere(int posx,int posy,int L,int W,unsigned short color)
{
	unsigned short* cor;
	unsigned int i;
	unsigned int pixel_num;

	if(posx<0||(posx+L)>319)
		return;
	if(posy<0||(posy+W)>239)
		return;
	pixel_num = fix.smem_len / 2;
	
	
}


/******************************************************************************
*
******************************************************************************/
long chartolong( char * string, int length )
{
	long number;

	if (length <= 4)
	{
		memset( &number, 0x00, sizeof(long) );
		memcpy( &number, string, length );
	}

	return( number );
}

/******************************************************************************
*
******************************************************************************/

int Show_BMP(int x,int y,char *bmpfile)
{
	//DebugPrintf("\n");
	FILE *fp;
	BITMAPFILEHEADER FileHead;
	BITMAPINFOHEADER InfoHead;
	int rc;
	int ciBitCount, ciWidth, ciHeight;
	int line_x, line_y;
	long int location = 0;


	if((x >= var.xres)||(y >= var.yres))
		return (-1);

	/* 打开位图文件 */
	fp = fopen( bmpfile, "rb" );
	if (fp == NULL)
	{
		return( -1 );
	}

	/* 读取位图文件头 */
	rc = fread( &FileHead, sizeof(BITMAPFILEHEADER),1, fp );
	if ( rc != 1)
	{
		printf("read header error!\n");
		fclose( fp );
		return( -2 );
	}

	/* 判断位图的类型 */
	if (memcmp(FileHead.cfType, "BM", 2) != 0)
	{
		printf("it's not a BMP file\n");
		fclose( fp );
		return( -3 );
	}

	/* 读取位图信息头 */
	rc = fread( (char *)&InfoHead, sizeof(BITMAPINFOHEADER),1, fp );
	if ( rc != 1)
	{
		printf("read infoheader error!\n");
		fclose( fp );
		return( -4 );
	}

	ciWidth    = (int) chartolong( InfoHead.ciWidth,	4 );
	ciHeight   = (int) chartolong( InfoHead.ciHeight,	4 );
	ciBitCount = (int) chartolong( InfoHead.ciBitCount, 4 );

	fseek(fp, (int)chartolong(FileHead.cfoffBits, 4), SEEK_SET);


	//printf("width=%d, height=%d, bitCount=%d, offset=%d\n", ciWidth, ciHeight, ciBitCount, (int)chartolong(FileHead.cfoffBits, 4));

	line_x = line_y = 0;
	while(!feof(fp))
	{
		PIXEL pix;
		unsigned short int tmp;
		rc = fread( (char *)&pix, 1, sizeof(unsigned short int), fp );
		if (rc != sizeof(unsigned short int) )
		{
			break;
		}
		location = line_x * var.bits_per_pixel / 8 + (ciHeight - line_y - 1) * var.xres * var.bits_per_pixel / 8;

		tmp=pix.red<<11 | pix.green<<6 | pix.blue;
		*((unsigned short int*)(fbuffera + y * var.xres + x + location)) = tmp;

		line_x++;
		if (line_x == ciWidth )
		{
			line_x = 0;
			line_y++;

			if(line_y==ciHeight-1)
			{
				break;
			}
		}
	}
	fclose( fp );
	return( 0 );
}


//int load_back_Img(int x, int y,char *buf,char * Path)	

int Load_Bmp_Img(char *bmpfile,BMP_INFO *buff)
{

	FILE *fp;
	unsigned short * pos=NULL;
	BITMAPFILEHEADER FileHead;
	BITMAPINFOHEADER InfoHead;
	int rc;
	int ciBitCount, ciWidth, ciHeight;
	int line_x, line_y;
	long int location = 0;

	DBG_PRINTF("-------------in func %s \n",__func__);

	if(buff->data!=NULL){
		printf("please realese the data of this buffer");
		return -1;
	}

	/* 打开位图文件 */
	fp = fopen( bmpfile, "rb" );			//以二进制读
	if (fp == NULL)
	{
		DBG_PRINTF("open file failed\n");
		return( -1 );
	}	

	
	/* 读取位图文件头 */
	rc = fread( &FileHead, sizeof(BITMAPFILEHEADER),1, fp );
	if ( rc != 1)
	{
		printf("read header error!\n");
		fclose( fp );
		return( -2 );
	}


	/* 判断位图的类型 */
	if (memcmp(FileHead.cfType, "BM", 2) != 0)
	{
		printf("it's not a BMP file\n");
		fclose( fp );
		return( -3 );
	}

	/* 读取位图信息头 */
	rc = fread( (char *)&InfoHead, sizeof(BITMAPINFOHEADER),1, fp );
	if ( rc != 1)
	{
		printf("read infoheader error!\n");
		fclose( fp );
		return( -4 );
	}

	
	DBG_PRINTF("图像文件大小:%d\n",(int)chartolong(FileHead.cfSize,4));
	DBG_PRINTF("图像阵列偏移量:%d\n",(int)chartolong(FileHead.cfoffBits,4));
		
	ciWidth    = (int) chartolong( InfoHead.ciWidth,	4 );
	ciHeight   = (int) chartolong( InfoHead.ciHeight,	4 );
	ciBitCount = (int) chartolong( InfoHead.ciBitCount, 2 );
	
	buff->width=ciWidth;
	buff->hieght=ciHeight;
	buff->bitcount=ciBitCount;
	buff->sz=(int)chartolong(FileHead.cfSize,4);
	buff->data=(char *)malloc((int)chartolong(FileHead.cfSize,4));
	if(buff->data==NULL)
		{
			perror("malloc for picture");
			fclose(fp);
			return -1;
		}
	
	pos=buff->data;
	DBG_PRINTF("位图宽度:%d 位图高度:%d 颜色深度:%d \n",ciWidth,ciHeight,ciBitCount);
	DBG_PRINTF("图像大小:%d\n",(int)chartolong(InfoHead.ciSizeImage,4));
	fseek(fp, (int)chartolong(FileHead.cfoffBits, 4), SEEK_SET);

	
	line_x = line_y = 0;
	while(!feof(fp))
	{
		PIXEL pix;
		PIXEL24 pix24;
		
		unsigned int tmp;
		if(ciBitCount==16)
		{
			rc = fread( (char *)&pix, 1, sizeof(unsigned short ), fp ); //读出一个像素数据
			if (rc != sizeof(unsigned short ) )
			{
				break;
			}
		}
		else if(ciBitCount==24)
		{
			rc = fread( (char *)&pix24, 1, sizeof(PIXEL24), fp );
			if (rc != sizeof(PIXEL24) )
			{
				break;
			}
			//将24位像素转换为16位像素
			pix.blue=pix24.re>>3;
			pix.green=pix24.gr>>3;
			pix.red=pix24.bl>>3;
		}

		
			//location = line_x * Var.bits_per_pixel / 8 + (ciHeight - line_y - 1) * Var.xres * Var.bits_per_pixel / 8;

			tmp=pix.red<<11 | pix.green<<6 | pix.blue;		
			*pos=(unsigned short)tmp;
			pos++;
			
	
	}
	
	fclose( fp );
	return( 0 );
}


int Show_Bmp(int x1,int y1,BMP_INFO * bmp)
{
	int w,h;
	int line_x,line_y;
	long int i;
	unsigned short *pos=NULL;
	long int location;

	DBG_PRINTF("in func -------------------%s\n",__func__);
	if(bmp->data==NULL)
		{
			printf("this pictrue is empty\n");
			return -1;
		}
	if((x1>=var.xres)||(y1>=var.yres))
		{
			printf("invalid x1 y1\n");
			return -1;
		}
	
	w=bmp->width;
	h=bmp->hieght;
	line_x = line_y = 0;
	pos=bmp->data;
	for(i=0;i<(bmp->sz/2);i++)
		{

			if((line_x+x1)<=var.xres && (h- line_y +y1)<=var.yres )
			{
				location = line_x * var.bits_per_pixel / 8 + (h- line_y - 1) * var.xres * var.bits_per_pixel / 8;

				*((unsigned short int*)(fbuffera+ y1 * var.xres + x1 + location)) = (pos[i]);
			}
		
		
			line_x++;
			if (line_x == w) 
			{
				line_x = 0;
				line_y++;

				if(line_y==h-1) 
				{
					break;
				}
			}
		}

	return 0;
	
}






#if 0
int load_BMP(char *bmpfile,PIXEL *_pix)
{
	FILE *fp;
	BITMAPFILEHEADER FileHead;
	BITMAPINFOHEADER InfoHead;
	int rc,i=0;
	//int ciBitCount, ciWidth, ciHeight;
	int line_x, line_y;
	long int location = 0;
	//PIXEL _pix;

	

	/* 打开位图文件 */
	fp = fopen( bmpfile, "rb" );
	if (fp == NULL)
	{
		return( -1 );
	}

	/* 读取位图文件头 */
	rc = fread( &FileHead, sizeof(BITMAPFILEHEADER),1, fp );
	if ( rc != 1)
	{
		printf("read header error!\n");
		fclose( fp );
		return( -2 );
	}

	/* 判断位图的类型 */
	if (memcmp(FileHead.cfType, "BM", 2) != 0)
	{
		printf("it's not a BMP file\n");
		fclose( fp );
		return( -3 );
	}

	/* 读取位图信息头 */
	rc = fread( (char *)&InfoHead, sizeof(BITMAPINFOHEADER),1, fp );
	if ( rc != 1)
	{
		printf("read infoheader error!\n");
		fclose( fp );
		return( -4 );
	}

	_ciWidth    = (int) chartolong( InfoHead.ciWidth,	4 );
	_ciHeight   = (int) chartolong( InfoHead.ciHeight,	4 );
	_ciBitCount = (int) chartolong( InfoHead.ciBitCount, 2 );

	fseek(fp, (int)chartolong(FileHead.cfoffBits, 4), SEEK_SET);


	//printf("width=%d, height=%d, bitCount=%d, offset=%d\n", ciWidth, ciHeight, ciBitCount, (int)chartolong(FileHead.cfoffBits, 4));

	line_x = line_y = 0;
	while(!feof(fp))
	{
		
		unsigned short int tmp;
		rc = fread( (char *)&_pix[i], 1, sizeof(unsigned short int), fp );
		
		if (rc != sizeof(unsigned short int) )
		{
			break;
		}
		
		i++;
	}
	fclose( fp );
	return( 0 );
}
#endif

#if 0
int Show_BMP_(int x,int y)
{

	BITMAPFILEHEADER FileHead;
	BITMAPINFOHEADER InfoHead;
	int rc,i=0;
	//int ciBitCount, ciWidth, ciHeight;
	int line_x, line_y;
	long int location = 0;


	//printf("width=%d, height=%d, bitCount=%d, offset=%d\n", ciWidth, ciHeight, ciBitCount, (int)chartolong(FileHead.cfoffBits, 4));

	line_x = line_y = i=0;
	while(i!=76800)
	{
		//PIXEL pix;
		unsigned short int tmp;
		
		location = line_x * var.bits_per_pixel / 8 + (_ciHeight - line_y - 1) * var.xres * var.bits_per_pixel / 8;
		tmp=_pix[i].red<<11 | _pix[i].green<<6 | _pix[i].blue;
		*((unsigned short int*)(fbuffera + y * var.xres + x + location)) = tmp;
		if(i==5000)
			PixColor1=tmp;	

		line_x++;
		if (line_x == _ciWidth )
		{
			line_x = 0;
			line_y++;

			if(line_y==_ciHeight-1)
			{
				break;
			}
		}
		i++;
	}

	return( 0 );
}
#endif

#if 0
int Show_BMP_1(int x,int y)
{

	BITMAPFILEHEADER FileHead;
	BITMAPINFOHEADER InfoHead;
	int rc,i=0;
	//int ciBitCount, ciWidth, ciHeight;
	int line_x, line_y;
	long int location = 0;

	DBG_PRINTF("in func %s:\n",__func__);
	line_x = line_y = i=0;
	while(i!=76800)
	{
		//PIXEL pix;
		unsigned short int tmp;
		location = line_x * var.bits_per_pixel / 8 + (_ciHeight - line_y - 1) * var.xres * var.bits_per_pixel / 8;
		tmp=_pix1[i].red<<11 | _pix1[i].green<<6 | _pix1[i].blue;
		*((unsigned short int*)(fbuffera + y * var.xres + x + location)) = tmp;
		if(i==20000)
			PixColor=tmp;
		line_x++;
		if (line_x == _ciWidth )
		{
			line_x = 0;
			line_y++;

			if(line_y==_ciHeight-1)
			{
				break;
			}
		}
		
		i++;
	}
	DBG_PRINTF("in func %s:\n",__func__);
	return( 0 );
}

#endif




//若mode =0x05   则显示文字时使用当前framebuffer中的数据清楚文字背景
int Set_Clear_Mode(unsigned char mode)
{
	use_curent_back=mode;
	return 0;
}

#if 0
int change_background(int backindex)
{

	DBG_PRINTF("in func %s:\n",__func__);
	if(backindex==1){
		if(background==NULL)
			{
				printf("the background is null\n");
				return -1;
			}
		memcpy(background,background1,fix.smem_len);
	}
	else if (backindex==2){
		if(background==NULL)
			{
				printf("the background is null\n");
				return -1;
			}
			memcpy(background,background2,fix.smem_len);
	}
	else{
		printf("invalid back index\n");
		return -1;
	}
	DBG_PRINTF("in func %s LINE %d\n",__func__,__LINE__);
	return 0;
}

#endif














