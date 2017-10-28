#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <sys/soundcard.h>
#include <sys/mman.h>
#include <linux/fb.h>
#include "sound.h"
#include "types.h"
#include "apparel.h"

#include "../gui/InitSystem.h"


volatile unsigned char Soundstatus;  //语音
unsigned char SoundIndex;  //语音
volatile unsigned char PlayMusicFinishFlag = 1;		//该标志位用来判断当前的声音是否已经播完
int soundfd, p_mixer;

volatile int sem_sound;
//sem_t sem_sound;
volatile int threadRunning;

VPInfo cachedVPInfo[MAX_FLOOR];
char snd_name[MAX_FLOOR][30];
unsigned char LoopPlayMusicFlag = 0;

extern int mf_fd;
extern volatile unsigned char showSunMoneyFLag;
extern volatile unsigned char showSunMoneyFinishFLag;

int my_sem_wait(volatile int *sem)
{
	while (*sem <= 0)
	{
		if (!threadRunning) break;
		usleep(1000);
	}
	*sem -= 1;

	return *sem;
}

int my_sem_post(volatile int *sem)
{
	*sem += 1;
	return *sem;
}

int my_sem_init(volatile int *sem, int value)
{
	*sem = value;
	return *sem;
}

/***********************************语音文件说明***************************/


/*********************************************************************************/
int PlaySoundOn(void)
{
	int format;
	int channels;
	int speed;
	int data;

	
	if((soundfd = open(SND_DEVICE_NAME, O_WRONLY)) == -1)
	{
		//printf("not open dsp\n");
		return -1;
	}
	p_mixer = open("/dev/mixer", O_RDWR);	
	if( p_mixer < 0 )
	{		
		close(soundfd);
		printf("Open mixer error\n");		
		return -1;	
	}

	data = 0x5050;
	ioctl(p_mixer, MIXER_WRITE(SOUND_MIXER_PCM), &data);
	
	format = AFMT_S16_LE;
	if(ioctl(soundfd, SNDCTL_DSP_SETFMT, &format) == -1)
	{
		printf("SNDCTL_DSP_SETFMT error\n");
		close(soundfd);
		close(p_mixer);
		return -1;
	}
	channels = SND_MONO;
	if(ioctl(soundfd, SNDCTL_DSP_CHANNELS, &channels) == -1)
	{
		printf("SNDCTL_DSP_CHANNELS error\n");
		close(soundfd);
		close(p_mixer);
		return -1;
	}
	speed = SPEED_22K;//SPEED_44K;
	if(ioctl(soundfd, SNDCTL_DSP_SPEED, &speed) == -1)
	{
		printf("SNDCTL_DSP_SPEED error\n");
		close(soundfd);
		close(p_mixer);
		return -1;
	}
	
	return 0;
}



int PlaySoundOff(void)
{
	close(soundfd);
	close(p_mixer);
	//free(snd_data);
	//snd_data = NULL;
	return 0;
}

void MemoryTest()
{
	char index = 1;
	int file_len;
	char *buffer = NULL;
	audio_buf_info param;
	int ret;
	int i, j;
	int from, to, diff;
	int total;

	DBG_AUDIO_PRINTF("MemoryTest() is called, index = %d\n", index);

	file_len = cachedVPInfo[index].size;
	if (cachedVPInfo[index].buffer == NULL)
	{
		perror("no sound data.");
		return -1;
	}
	
	buffer = (char*)malloc(BUF_SIZE);
	if(buffer == NULL)
	{
		perror("sound malloc");
		return -1;
	}

	ioctl(soundfd, SNDCTL_DSP_RESET);

	memset(buffer, 0, BUF_SIZE);
	param.bytes = cachedVPInfo[index].buffer;
	param.fragments = buffer;
	param.fragsize = BUF_SIZE;

	ret = ioctl(soundfd, SNDCTL_DSP_POST, &param);
	DBG_AUDIO_PRINTF("MemoryTest(): SNDCTL_DSP_POST, memory size = %d, ret = %d\n", BUF_SIZE, ret);

	from = 0;
	to = 0;
	diff = 0;
	total = 0;
	for (i = 0; i < BUF_SIZE; i++)
	{
		if (buffer[i] != cachedVPInfo[index].buffer[i])
		{
			if (!diff)
			{
				// found difference
				diff = 1;
				from = i;
				total++;
			}
			else
			{
				// continuous difference
				to = i;
				total++;
			}
		}
		else
		{
			if (diff)
			{
				// end of difference
				DBG_AUDIO_PRINTF("Found error at offset 0x%08X, size = %d:\n", from, to - from + 1);
				
				DBG_AUDIO_PRINTF("Source: \n");
				for (j = from; j <= to; j++)
				{
					DBG_AUDIO_PRINTF("%02X ", cachedVPInfo[index].buffer[j]);
					if (j && (j % 16 == 0)) DBG_AUDIO_PRINTF("\n");
				}

				DBG_AUDIO_PRINTF("Read back: \n");
				for (j = from; j <= to; j++)
				{
					DBG_AUDIO_PRINTF("%02X ", buffer[j]);
					if (j && (j % 16 == 0)) DBG_AUDIO_PRINTF("\n");
				}

				diff = 0;
			}
		}
	}	

	if (diff)
	{
		// end of difference
		DBG_AUDIO_PRINTF("Found error at offset 0x%08X, size = %d:\n", from, to - from + 1);
		
		DBG_AUDIO_PRINTF("Source: \n");
		for (j = from; j <= to; j++)
		{
			DBG_AUDIO_PRINTF("%02X ", cachedVPInfo[index].buffer[j]);
			if (j && (j % 16 == 0)) DBG_AUDIO_PRINTF("\n");
		}

		DBG_AUDIO_PRINTF("Read back: \n");
		for (j = from; j <= to; j++)
		{
			DBG_AUDIO_PRINTF("%02X ", buffer[j]);
			if (j && (j % 16 == 0)) DBG_AUDIO_PRINTF("\n");
		}

		diff = 0;
	}

	free(buffer);
	DBG_AUDIO_PRINTF("MemoryTest() finished, total diff = %d.\n", total);
}

int playsound(char index)
{
	int file_len;
	//int loops;
	//int file_fd;
	struct stat buf;//,fdbuf;
	char *buffer = NULL;
	int ret;
	unsigned int leftSize, readSize;
	int readCount, writeCount;
	unsigned int MusicCurrentPos;	
	WAVEFORMAT *pWavFormat;
	WAVEFORMAT2 *pWavFormat2;
	audio_buf_info streamInfo;
	struct timeval now;
	int frag;
	audio_buf_info info;
	
	DBG_AUDIO_PRINTF("playsound() is called, index = %d\n", index);


	file_len = cachedVPInfo[index].size;
	if (cachedVPInfo[index].buffer == NULL)
	{
		printf("no sound data.\n");
		return -1;
	}
		
	readSize = sizeof(WAVEFORMAT2);
	//readCount = read(file_fd, buffer, readSize);
	buffer = cachedVPInfo[index].buffer;
	readCount = readSize;
	pWavFormat = (WAVEFORMAT *)buffer;
	pWavFormat2 = (WAVEFORMAT2 *)buffer;

	if ((pWavFormat->cRiffFlag[0] == 'R')
		&& (pWavFormat->cRiffFlag[1] == 'I')
		&& (pWavFormat->cRiffFlag[2] == 'F')
		&& (pWavFormat->cRiffFlag[3] == 'F')
		&& (pWavFormat->cWaveFlag[0] == 'W')
		&& (pWavFormat->cWaveFlag[1] == 'A')
		&& (pWavFormat->cWaveFlag[2] == 'V')
		&& (pWavFormat->cWaveFlag[3] == 'E'))
	{

		if ((pWavFormat->cDataFlag[0] == 'd')
			&& (pWavFormat->cDataFlag[1] == 'a')
			&& (pWavFormat->cDataFlag[2] == 't')
			&& (pWavFormat->cDataFlag[3] == 'a'))
		{
			// wav file with header
			DBG_AUDIO_PRINTF("Wav file, audio length = %d bytes\n", pWavFormat->nAudioLength);

			MusicCurrentPos = sizeof(WAVEFORMAT);
			leftSize = pWavFormat->nAudioLength;
		}
		else if ((pWavFormat2->cDataFlag[0] == 'd')
			&& (pWavFormat2->cDataFlag[1] == 'a')
			&& (pWavFormat2->cDataFlag[2] == 't')
			&& (pWavFormat2->cDataFlag[3] == 'a'))
		{
			// wav file with header
			DBG_AUDIO_PRINTF("Wav2 file, audio length = %d bytes\n", pWavFormat2->nAudioLength);

			MusicCurrentPos = sizeof(WAVEFORMAT2);
			leftSize = pWavFormat2->nAudioLength;
		}
		else
		{
			DBG_AUDIO_PRINTF("Wav file, unknown data length.\n");
			MusicCurrentPos = 200;
			leftSize = file_len - MusicCurrentPos;
		}
	}
	else
	{
		// RAW PCM file
		DBG_AUDIO_PRINTF("Raw PCM, file size = %d bytes\n", file_len);

		MusicCurrentPos = 200;
		leftSize = file_len - MusicCurrentPos;
	}

	if (leftSize > BUF_SIZE)
	{
		readSize = BUF_SIZE;
	}
	else
	{
		readSize = leftSize;
	}

	//ret = lseek(file_fd, MusicCurrentPos, SEEK_SET);
	//readCount = read(file_fd, buffer, readSize);
	buffer = cachedVPInfo[index].buffer + MusicCurrentPos;
	readCount = readSize;

	MusicCurrentPos += readCount;
	leftSize -= readCount;
	
	//SoundIndex = 0;
	Soundstatus = 1;
	ioctl(soundfd, SNDCTL_DSP_RESET);
	//non-block mode
	//ioctl(soundfd, SNDCTL_DSP_NONBLOCK);
	ioctl(soundfd, SNDCTL_DSP_GETBLKSIZE, &frag);

	DBG_AUDIO_PRINTF("playback started.frag:%d,readCount:%d \n",frag,readCount);
	while (readCount)
	{							
		do
		{
			ioctl(soundfd , SNDCTL_DSP_GETOSPACE , &info);
			usleep(100);		
		}while(info.bytes < frag);	
		//printf("----info.bytes:%d \n",info.bytes);

		/*
		//SNDCTL_DSP_GETOSPACE
		memset(&streamInfo, 0, sizeof(streamInfo));
		ret = ioctl(soundfd, SNDCTL_DSP_GETOSPACE, &streamInfo);
		DBG_AUDIO_PRINTF("playsound(): ret = %d, t=%d, f=%d, s=%d, b=%d\n", 
			ret,
			streamInfo.fragstotal,
			streamInfo.fragments,
			streamInfo.fragsize,
			streamInfo.bytes);
		while (streamInfo.fragstotal * streamInfo.fragsize - streamInfo.bytes > 512)
		{
			usleep(1000);
			ioctl(soundfd, SNDCTL_DSP_GETOSPACE, &streamInfo);
			if(Soundstatus == 0) {
				DBG_AUDIO_PRINTF("playsound(): playback interrupted, offset = %d\n", MusicCurrentPos);
				break;
			}
		}
		*/

		//gettimeofday(&now,0);
		//DBG_AUDIO_PRINTF("write begin, buffer = 0x%08X, time = %ld\"%06ld.\n", buffer, now.tv_sec, now.tv_usec);
		writeCount = 0;
	//	printf("----start write frag: %d \n",frag);

		if(readCount >= frag)
			writeCount = write(soundfd, buffer, frag);
		else
			writeCount = write(soundfd, buffer, readCount);
		// check data if write successfully
		if (writeCount != readCount)
		{
			// soudn driver will make DWORD align
			if ((readCount >= 4) && (writeCount >= 4) && ((readCount - writeCount) >= 4))
			{
				MusicCurrentPos = MusicCurrentPos - (readCount - writeCount);
				leftSize = leftSize + (readCount - writeCount);
			}
		}
		usleep(100);
		//printf("----after write writeCount:%d \n",writeCount);
		//gettimeofday(&now,0);
		//DBG_AUDIO_PRINTF("write end, time = %ld\"%06ld, write = %d.\n", now.tv_sec, now.tv_usec, writeCount);
		//printf("--play bytes : %d \n",bytes);
		if(Soundstatus == 0) {
		//ioctl(soundfd, SNDCTL_DSP_RESET);
		//DBG_AUDIO_PRINTF("playsound(): playback interrupted, offset = %d\n", MusicCurrentPos);
			break;
		}
		//printf("-------MusicCurrentPos : %d ,writeCount:%d\n",MusicCurrentPos,writeCount);
		if(leftSize)
		{
			if (leftSize > BUF_SIZE)
			{
				readSize = BUF_SIZE;
			}
			else
			{
				readSize = leftSize;
			}

			//readCount = read(file_fd, buffer, readSize);
			//memcpy(buffer, cachedVPInfo[index].buffer + MusicCurrentPos, readSize);
			buffer = cachedVPInfo[index].buffer + MusicCurrentPos;
			readCount = readSize;

			MusicCurrentPos += readCount;
			leftSize -= readCount;
			DBG_AUDIO_PRINTF(".");
		}
		else
		{
			// reset readCount to exit the loop
			readCount = 0;
		}

		//usleep(1000);

		if(Soundstatus == 0) {
//			ioctl(soundfd, SNDCTL_DSP_RESET);
//			DBG_AUDIO_PRINTF("playsound(): playback interrupted, offset = %d\n", MusicCurrentPos);
			break;
		}
	}
#if 0	
	int bytes;	
	ioctl(soundfd,SNDCTL_DSP_GETODELAY,&bytes);	
	int delay = (bytes*1000)/ (SPEED_22K * 2 * SND_MONO);		
	usleep(delay*1000);	
#endif
	//usleep(100);
	//close(file_fd);
	//free(buffer);
	//buffer = NULL;
	int bytes;	
	ioctl(soundfd,SNDCTL_DSP_GETODELAY,&bytes);	
	int delay = (bytes*1000)/ (SPEED_22K * 2 * SND_MONO);
	if(delay)
		usleep(delay*1000);	
	do
	{
		ioctl(soundfd , SNDCTL_DSP_GETOSPACE , &info);
		usleep(100);		
	}while(info.bytes < frag);	
	
	ioctl(soundfd, SNDCTL_DSP_RESET);
	Soundstatus = 0;
	//printf("(((((((((((((((((((((((((((( \n");
	return 0;
}


void WaitPlayMusic(void)
{
	while(!PlayMusicFinishFlag)
		usleep(5000);
}

void PlayMusic(int index, int LoopFlag)
{
	DebugPrintf("PlayMusic() is called, index = %d, Soundstatus:%d\n", index,Soundstatus);
	
#ifdef SHAOYANG_PARK
	if(!LoopFlag)
	{
		StopMusic();
	}
	else
	{
		WaitPlayMusic();
	}
#else
	// 停止语音播放
	StopMusic();
#endif	
	if(LoopFlag)
		LoopPlayMusicFlag = 1;
	else
		LoopPlayMusicFlag = 0;

	// 开始语音
	SoundIndex = index;
	my_sem_post(&sem_sound);
	//sem_post(&sem_sound);

	usleep(100 * 1000);
	return ;
}





unsigned char JudgeIsPlayMusic(int x)
{
	DebugPrintf("\n");
	static int flag = 0;

	if(x)
		flag = 0;	
		
	if((ioctl(mf_fd, RC531_M1_CSN) == MI_OK) && (flag == 0))
		flag = 1;
	else if((ioctl(mf_fd, RC531_M1_CSN) != MI_OK) && ((flag == 1)||(flag == 0)))
		flag = 2;
	else if((ioctl(mf_fd, RC531_M1_CSN) == MI_OK) && (flag == 2))
		flag = 0;


	return flag;
}


void PlaySum(unsigned int sum)
{
	DebugPrintf("sum = %u\n", sum);
	
	unsigned char pTempBuf[8], pSumBuf[8], i;
	int index = 0, sumLen = 0, zeroFlag = 1;

	memset(pTempBuf, 0, sizeof(pTempBuf));
	memset(pSumBuf, 0, sizeof(pSumBuf));
	
	sprintf(pTempBuf, "%08u", sum);
	DebugPrintf("pTempBuf = %s\n", pTempBuf);

	PlayMusic(33, 0);		/* 播报余额为 */

	/* 余额到元，所以把总长度减去2 */
	for(i=0; i<6; i++)
	{
		if(pTempBuf[i] == '0' && zeroFlag )	
		{
			continue;
		}
		else
		{
			zeroFlag = 0;
			/* 前面17种是卡类型，数字语言是从17之后开始 */
			index = pTempBuf[i] - '0' + 18;		
			
			PlayMusic(index, 0);
			
			if(!showSunMoneyFLag)  return 0;
		}
	}

	if(zeroFlag)
		PlayMusic(18, 0);
	
	if(!showSunMoneyFLag)  return 0;

	PlayMusic(34, 0);		/* 播报 元 */
	
	DebugPrintf("pSumBuf = %s\n", pTempBuf);
	return 0;
}

void StopMusic(void)
{
	DebugPrintf("\n");
	LoopPlayMusicFlag = 0;
	Soundstatus = 0;
	showSunMoneyFLag = 0;
	
	while(!PlayMusicFinishFlag || showSunMoneyFinishFLag)
		usleep(5000);
}


/* 马上停止语言的播放 */
void StopMusic_Immediately()
{
	if(!PlayMusicFinishFlag)
	{
		Soundstatus = 0;
		showSunMoneyFLag = 0;
	}
}


void StopAudioThread()
{
	threadRunning = 0;
}

/*
*************************************************************************************************************
- 函数名称 : void readsound(void)
- 函数说明 : 读语音文件
- 输入参数 : 无
- 输出参数 : 无
*************************************************************************************************************
*/
void readsound(void)
{
	unsigned char i;
	char name[30];
	int file_fd;
	int readCount;
	struct stat buf;
	char *buffer;

	memset(cachedVPInfo, 0, sizeof(cachedVPInfo));

	for(i = 0; i< MAX_FLOOR; i++)
	{
		sprintf(name,"/var/run/sound/music%02d.wav",i); //music01.wav
		if(access(name, 0) == 0)
		{
			memcpy(snd_name[i], name, strlen(name));

			file_fd = open(name, O_RDONLY);
			if(fstat(file_fd, &buf) < 0)
			{
				perror("fstat error");
				return -1;
			}

			buffer = (char*)malloc(buf.st_size);
			if(buffer == NULL)
			{
				perror("sound malloc");
				break;
			}

			readCount = read(file_fd, buffer, buf.st_size);
			if(readCount != buf.st_size)
			{
				perror("read sound data error!");
				break;
			}

			cachedVPInfo[i].buffer = buffer;
			cachedVPInfo[i].size = buf.st_size;

			close(file_fd);

#if UARTDIS
			printf("music  %s \n",snd_name[i]);
#endif
		}
	}
}

void AudioRelease()
{
	int i;
	
	for(i = 0; i< MAX_FLOOR; i++)
	{
		if (cachedVPInfo[i].buffer)
		{
			free(cachedVPInfo[i].buffer);	
		}
	}
}

/*
*************************************************************************************************************
- 函数名称 : void * Sund_Pthread (void * args)
- 函数说明 : 播放语音
- 输入参数 : 无
- 输出参数 : 无
*************************************************************************************************************
*/
void * Sund_Pthread (void * args)
{
	int PlayCount = 1,ret;

	Soundstatus = 0;
	SoundIndex = 0;
	threadRunning = 1;

	readsound();

	while(1)
	{
		ret = PlaySoundOn();
		if(ret < 0)
			usleep(100000);
		else
			break;
	}

	while(threadRunning)
	{
		ret = my_sem_wait(&sem_sound);  //my_sem_post(&sem_sound);
		PlayMusicFinishFlag = 0;
		PlayCount = 1;
		DebugPrintf("####SoundIndex = [%02X] PlayCount[%d]\n", SoundIndex, PlayCount);

		if (!threadRunning) break;

		if((snd_name[SoundIndex][0] != 0)&&(snd_name[SoundIndex][1] != 0))
		{
			while(SoundIndex && (LoopPlayMusicFlag || (PlayCount > 0)))
			{
				playsound(SoundIndex);
				PlayCount--;
				usleep(300*1000);
			}
		}
		SoundIndex = 0;

		PlayMusicFinishFlag = 1;
	}
	PlaySoundOff();

	AudioRelease();
	
	return 0;
}



