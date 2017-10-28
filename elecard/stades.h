/*
These code were got from Internet,I have no any work on it.
*/
#ifndef _STADES_H
#define _STADES_H

#ifndef uint8
#define uint8  unsigned char
#endif

#ifndef uint32
#define uint32 unsigned int
#endif

#define   true    ((char)0)
#define   false   ((char)-1)



typedef struct
{
    uint32 esk[32];     /* DES encryption subkeys */
    uint32 dsk[32];     /* DES decryption subkeys */
} des_context;

typedef struct
{
    uint32 esk[96];     /* Triple-DES encryption subkeys */
    uint32 dsk[96];     /* Triple-DES decryption subkeys */
} des3_context;

int  des_set_key( des_context *ctx, uint8 key[8] );
void des_encrypt( des_context *ctx, uint8 input[8], uint8 output[8] );
void des_decrypt( des_context *ctx, uint8 input[8], uint8 output[8] );

int  des3_set_2keys( des3_context *ctx, uint8 key1[8], uint8 key2[8] );
int  des3_set_3keys( des3_context *ctx, uint8 key1[8], uint8 key2[8], uint8 key3[8] );

void des3_encrypt( des3_context *ctx, uint8 input[8], uint8 output[8] );
void des3_decrypt( des3_context *ctx, uint8 input[8], uint8 output[8] );

#define  ENCRYPT 	 0	//����
#define  DECRYPT 	 1	//����
#define  ECB 	     0	//ECBģʽ
#define  CBC 		 1	//CBCģʽ
#define  PAD_ISO_1   0	//ISO_1��䣺���ݳ��Ȳ���8���صı�������0x00���㣬���Ϊ8���صı�������8��0x00
#define  PAD_ISO_2   1	//ISO_2��䣺���ݳ��Ȳ���8���صı�������0x80,0x00..���㣬���Ϊ8���صı�������0x80,0x00..0x00
#define  PAD_PKCS_7  2 	//PKCS7��䣺���ݳ��ȳ�8����Ϊn,��(8-n)����Ϊ8�ı���
#define  PAD_PBOC    3

/*******************************************************************/
/*
  �� �� �� ��:	RunPad
  �� �� �� ����	����Э��Լ���ǰ�����ݽ������
  �� �� ˵ ����	bType	:���ͣ�PAD����
				In		:���ݴ�ָ��
				Out		:��������ָ��
				datalen	:���ݵĳ���
				padlen	:(in,out)���buffer�ĳ��ȣ�����ĳ���

  ����ֵ ˵����	bool	:�Ƿ����ɹ�
*/
/*******************************************************************/
char RunPad(char nType,const char* In,unsigned int datalen,char* Out,unsigned int* padlen);

/*******************************************************************/
/*
  �� �� �� ��:	RunDes
  �� �� �� ����	ִ��DES�㷨���ı��ӽ���
  �� �� ˵ ����	bType	:���ͣ�����ENCRYPT������DECRYPT
				bMode	:ģʽ��ECB,CBC
				In		:�����ܴ�ָ��
				Out		:�������ָ��
				datalen	:�����ܴ��ĳ��ȣ�ͬʱOut�Ļ�������СӦ���ڻ��ߵ���datalen
				Key		:��Կ(��Ϊ8λ,16λ,24λ)֧��3��Կ
				keylen	:��Կ���ȣ����24λ���ֽ����Զ��ü�

  ����ֵ ˵����	bool	:�Ƿ���ܳɹ�
*/
/*******************************************************************/
char RunDes(char bType,char bMode,char* In,char* Out,unsigned int datalen,const char* Key,const unsigned char keylen);

//��λȡ������
void Not(char *Out, const char *In, int len);

//��λ������
void XOR(char *Out, const char *In, int len);


#endif /* des.h */

