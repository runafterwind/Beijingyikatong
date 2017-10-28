#ifndef _PRINTER_H_
#define _PRINTER_H_

#define  PRINT_START     "/var/run/start.txt"
#define  PRINT_END       "/var/run/end.txt"

#define PRINTER_MOMTOR    0xA0
#define PRINTER_TEM	  0xA1

int open_printer(void);
int close_printer(void);
int PrintText(char *pzText);
void Print_Txt(const char *filename,char big);
void Start_Name(const char *filename);
int Big_PrintText(char *pzText);

int readline(const char *filename,int line,char *date);
int Write_Start(const char *namefile);
int Write_File(const char *namefile,char *FileTxt,char TxtLen);

#endif
