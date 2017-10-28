#ifndef _SOUND_H_
#define _SOUND_H_

#define MAX_FLOOR		70

#define AUDIO_NBFRAGS_DEFAULT 8
#define AUDIO_FRAGSIZE_DEFAULT 8192 	//this for write
#define BUF_SIZE 		(AUDIO_FRAGSIZE_DEFAULT * AUDIO_NBFRAGS_DEFAULT)
//#define BUF_SIZE 		4096
#define SND_DEVICE_NAME 	"/dev/dsp"
#define SND_STEREO		2
#define SND_MONO		1
#define SPEED_8K		8000
#define SPEED_11K		11025
#define SPEED_22K		22050
#define SPEED_44K		44100

typedef  struct
{
	char *buffer;
	unsigned int size;
} VPInfo;

typedef struct tagWaveFormat
{ 
	char cRiffFlag[4]; 
	unsigned int nFileLen; 
	char cWaveFlag[4]; 
	char cFmtFlag[4]; 
	char cTransition[4]; 
	unsigned short nFormatTag ; 
	unsigned short nChannels; 
	unsigned int nSamplesPerSec; 
	unsigned int nAvgBytesperSec; 
	unsigned short nBlockAlign; 
	unsigned short nBitNumPerSample; 
	char cDataFlag[4]; 
	unsigned int nAudioLength; 
} WAVEFORMAT;

typedef struct tagWaveFormat2
{ 
	char cRiffFlag[4]; 
	unsigned int nFileLen; 
	char cWaveFlag[4]; 
	char cFmtFlag[4]; 
	char cTransition[4]; 
	unsigned short nFormatTag ; 
	unsigned short nChannels; 
	unsigned int nSamplesPerSec; 
	unsigned int nAvgBytesperSec; 
	unsigned short nBlockAlign; 
	unsigned short nBitNumPerSample; 
	char cReserverd[2]; 
	char cDataFlag[4]; 
	unsigned int nAudioLength; 
} WAVEFORMAT2;

extern VPInfo cachedVPInfo[MAX_FLOOR];

//extern sem_t sem_sound;
extern volatile int sem_sound;
extern int my_sem_wait(volatile int *sem);
extern int my_sem_post(volatile int *sem);
extern int my_sem_init(volatile int *sem, int value);

extern int PlaySoundOn(void);
extern int PlaySoundOff(void);
extern int playsound(char index);

extern void readsound(void);
extern  void * Sund_Pthread (void * args);

extern void PlayMusic(int index, int LoopFlag);
extern void StopMusic();
extern void StopAudioThread();
extern void MemoryTest();
#endif
