#ifndef __TYPES_H__
#define __TYPES_H__


#ifndef NULL
#define NULL 		((void*)0)
#endif

#define DEBUG				0
#define USE_STDINT_TYPES	1
#define DBG_AUDIO			0

#if DEBUG
#include "stdio.h"
# define DBG_PRINTF					printf
#else
# define DBG_PRINTF(...)  
#endif

#if DBG_AUDIO
#define DBG_AUDIO_PRINTF			DBG_PRINTF
#else
#define DBG_AUDIO_PRINTF(...)  
#endif

#if USE_STDINT_TYPES
#include "stdint.h"
#else
// Data types
#ifndef uint8_t
#define uint8_t  	unsigned char
#define uint16_t 	unsigned short
#define uint32_t 	unsigned int
#endif

#ifndef int16_t
typedef signed short int16_t;
typedef signed int int32_t;
#endif
#endif

#ifndef TRUE
#define TRUE		1
#define FALSE	0
#endif


#endif
