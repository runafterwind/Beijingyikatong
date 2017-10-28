#ifndef __FBTOOLS_H
#define __FBTOOLS_H

//#define  Mcolor   RGB565(50,50,60)

#define Color_black        0       // 黑色
#define Color_blue    RGB565(0,0,255)  //篮色
#define Color_red     RGB565(255,0,0)  //红色
#define Color_green   RGB565(0,255,0)  //绿色
#define Color_purple  RGB565(255,0,255)  //紫色
#define Color_yellow  RGB565(255,255,0)  //黄色
#define Color_white   RGB565(255,255,255) //白色

//14byte
typedef struct
{
  char cfType[2];         /* 文件类型, 必须为 "BM" (0x4D42)*/
  char cfSize[4];         /* 文件的大小(字节) */
  char cfReserved[4];     /* 保留, 必须为 0 */
  char cfoffBits[4];      /* 位图阵列相对于文件头的偏移量(字节)*/
}__attribute__((packed)) BITMAPFILEHEADER;       /* 文件头结构 */

//40byte
typedef struct
{
  char ciSize[4];         /* size of BITMAPINFOHEADER */
  char ciWidth[4];        /* 位图宽度(像素) */
  char ciHeight[4];       /* 位图高度(像素) */
  char ciPlanes[2];       /* 目标设备的位平面数, 必须置为1 */
  char ciBitCount[2];     /* 每个像素的位数, 1,4,8或24 */
  char ciCompress[4];     /* 位图阵列的压缩方法,0=不压缩 */
  char ciSizeImage[4];    /* 图像大小(字节) */
  char ciXPelsPerMeter[4];/* 目标设备水平每米像素个数 */
  char ciYPelsPerMeter[4];/* 目标设备垂直每米像素个数 */
  char ciClrUsed[4];      /* 位图实际使用的颜色表的颜色数 */
  char ciClrImportant[4]; /* 重要颜色索引的个数 */
}__attribute__((packed)) BITMAPINFOHEADER;       /* 位图信息头结构 */



typedef struct
{
  unsigned short blue:5;
  unsigned short green:5;
  unsigned short red:5;
  unsigned short rev:1;
}__attribute__((packed)) PIXEL;
typedef struct
{
	 unsigned char re;
	 unsigned char gr;
	 unsigned char bl;

}__attribute__((packed)) PIXEL24;//24位像素



typedef struct MYBMPFILE {
	int ciWidth;
 	int ciHeight;
    char *bmpfile;
}MYBMPFILE;


typedef struct 
{
	short width;
	short hieght;
	char bitcount;
	long int sz;
	char * data;

}BMP_INFO;


//extern int Lond_BMP(char *bmpfile,MYBMPFILE*fpb);
//extern int reLond_BMP(MYBMPFILE *fpb);
//extern int Show_bmp_file(int x,int y,MYBMPFILE *fpb);



extern unsigned short Mcolor;
extern unsigned short Textcolor;
extern unsigned short TextSize;




extern int open_framebuffer(void);
extern void close_framebuffer(void);
extern void line (int x1, int y1, int x2, int y2, unsigned short colidx);
extern void rect (int x1, int y1, int x2, int y2, unsigned short colidx);
extern void fillrect (int x1, int y1, int x2, int y2, unsigned colidx);
extern void put_hz(unsigned char *dispBuf,unsigned int x1,unsigned int y1,unsigned short color);
extern void put_char(unsigned char *dispBuf,unsigned int x1,unsigned int y1,unsigned short color);
extern void put_string(int x1, int y1, char *pzText, unsigned color);
extern void put_string_center(int x, int y, char *s, unsigned colidx);
extern unsigned short RGB565(unsigned char r,unsigned g,unsigned char b);
extern void SetColor(unsigned short color);


extern void _draw_model(char *pdata, int w, int h, int x, int y, int color);
extern void _draw_char(char hz, int x, int y, int w, int h, int color);
extern void _draw_hz(char *hz, int x, int y, int w, int h, int color);
extern void put_string32(int x1, int y1, char *pzText, unsigned color);

extern void InitDisplay(void);
extern void CloseDisplay(void);
extern void ClearAll(void);
extern void ClearWin(int x,int y,int x1,int y1);
extern void SetTextColor(unsigned short color);
extern void SetTextSize(unsigned short size);
extern void TextOut(int x1, int y1, char *pzText);
extern int Show_BMP(int x,int y,char *bmpfile);
//extern int load_back_Img(int x,int y,char * buf,char * Path);
extern int Load_Bmp_Img(char * bmpfile,BMP_INFO * buff);
extern int Show_Bmp(int x1,int y1,BMP_INFO * bmp);
extern int Set_Clear_Mode(unsigned char mode);
extern int change_background(int backindex);
extern void init_backgroud(void);
extern void TextOut_CMD(int x1,int y1,char * pzText,int cmd);
#endif
