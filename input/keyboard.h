#ifndef __KEYBOARD_H
#define __KEYBOARD_H


#define __KEYBOARD_1					0
#define __KEYBOARD_2					1

#if __KEYBOARD_1
#define SCANCODE_ESCAPE                 1
#define SCANCODE_BACKSPACE              14
#define SCANCODE_ENTER                  96
#define SCANCODE_PERIOD                 83

#define SCANCODE_1                      79
#define SCANCODE_2                      80
#define SCANCODE_3                      81
#define SCANCODE_4                      75
#define SCANCODE_5                      76
#define SCANCODE_6                      77
#define SCANCODE_7                      71
#define SCANCODE_8                      72
#define SCANCODE_9                      73
#define SCANCODE_0                      82

#define SCANCODE_F1                     59
#define SCANCODE_F2                     60
#define SCANCODE_F3                     61
#define SCANCODE_F4                     62
#define SCANCODE_F5                     63
#define SCANCODE_F6                     64

#define SCANCODE2_FUN6					77		//新增子功能，设定消费固定值

#elif	__KEYBOARD_2
#define SCANCODE_ESCAPE                 1
#define SCANCODE_BACKSPACE              14
#define SCANCODE_ENTER                  96
#define SCANCODE_PERIOD                 83

#define SCANCODE_1                      79
#define SCANCODE_2                      80
#define SCANCODE_3                      81
#define SCANCODE_4                      75
#define SCANCODE_5                      76
#define SCANCODE_6                      77
#define SCANCODE_7                      71
#define SCANCODE_8                      72
#define SCANCODE_9                      73
#define SCANCODE_0                      82

#define SCANCODE_F1                     83
#define SCANCODE_F2                     1
#define SCANCODE_F3                     61

#define SCANCODE2_FUN1					79
#define SCANCODE2_FUN2					80
#define SCANCODE2_FUN3					81
#define SCANCODE2_FUN4					75
#define SCANCODE2_FUN5					76
#define SCANCODE2_FUN6					77		//新增子功能，设定消费固定值


#define SCANCODE2_CHL					72
#define SCANCODE2_PRE					71
#define SCANCODE2_NEXT					73
#endif


//侧面按键
#define SCANCODE2_OK                 3
#define SCANCODE2_C                  4
#define SCANCODE2_UP                  1
#define SCANCODE2_DOWN                  2



int updatekey(void);
void closekey(void);





#endif
