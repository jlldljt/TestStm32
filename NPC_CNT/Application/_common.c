/**
  ******************************************************************************
  * @file    /user_define.c 
  * @author  MMY
  * @version V2.0.0
  * @date    27-June-2015
  * @brief   global variable 
	*
@verbatim  

               
@endverbatim        
  *
  ******************************************************************************
  * @attention
  *
  * COPYRIGHT 2015 CQT Quartz. Co., Ltd.
  *
  ******************************************************************************
  */
	
#include "_common.h"


//THUMBָ�֧�ֻ������
//�������·���ʵ��ִ�л��ָ��WFI  
__asm void WFI_SET(void)
{
	WFI;		  
}
//�ر������ж�(���ǲ�����fault��NMI�ж�)
__asm void INTX_DISABLE(void)
{
	CPSID   I
	BX      LR	  
}
//���������ж�
__asm void INTX_ENABLE(void)
{
	CPSIE   I
	BX      LR  
}
//����ջ����ַ
//addr:ջ����ַ
__asm void MSR_MSP(u32 addr) 
{
	MSR MSP, r0 			//set Main Stack value
	BX r14
}
/**
  * @brief  ����
  * @param  
  * @retval 
  * @note   
  */
void ResetSys(void){
	CLI();
	NVIC_SystemReset();
}

/** @brief  
  * @param  
  * @retval 
  * @note   ��PVD�������ʹ��
  */
void Restart(void) {
//	__disable_irq();   // �ر����ж�
	
		//RCC_AHB1PeriphResetCmd(0X22E011FF, ENABLE);
		//RCC_APB1PeriphResetCmd(0xE6FEC9FF, ENABLE);
		//RCC_APB2PeriphResetCmd(0x00377F33, ENABLE);
	ResetSys();
}


/** @brief  Ψһʶ����
  * @param  
  * @retval 
  * @note   
  */
u32 GetOnlyID(void) {
	u32 sn0;
	sn0=*ONLY_ID_ADDR;
	return sn0;
}


/** @brief  
  * @param  
  * @retval 
  * @note   
  */
void NewError(void ) {
	u16 i = 0;
    u8	j = 0;
	u8 sw = 1;
	
	LED3 = LED_ON;
	//Log_Write("NEW ERROR" , LOG_ERROR);
	while(sw)
		if(0 == i++) {
			if(0 ==  j++)
			LED2 = ~LED2;		//������;
		}
}
/** @brief  
  * @param  
  * @retval 
  * @note   
  */
void WhileError(void ) {
	u16 i = 0;
  u8	j = 0;
	u8 sw = 1;
	
	LED3 = LED_ON;
	//Log_Write("WHILE ERROR" , LOG_ERROR);
	while(sw)
		if(0 == i++) {
			if(0 ==  j++)
			LED2 = ~LED2;		//������;
		}
}

/** @brief  
  * @param  
  * @retval 
  * @note   
  */
void NewMemory(void **p , size_t size ) {

	if(0 == size)
		return;
	
	if(*p!=NULL) { 
#if USE_MEMMNG == 1u
      memmng_free(*p);
#else
      free(*p);
#endif
      //free(*p);
      *p = NULL;
    }
#if USE_MEMMNG == 1u
    *p = (memmng_malloc(size));
#else
    *p = (malloc(size));
#endif
    if(NULL == *p) 
    {
      NewError();
    }
	memset(*p, 0, size);
}
/** @brief  
  * @param  
  * @retval 
  * @note   
  */
void ReNewMemory(void **p , size_t size ) {
	
	if(0 == size)
		return;
	
#if USE_MEMMNG == 1u
  *p = (memmng_realloc(*p ,size));
#else
  *p = (realloc(*p ,size));
#endif

	if(NULL == *p) 
	{
		NewError();
	}
	
}
/** @brief  
  * @param  
  * @retval 
  * @note   
  */
void DelMemory(void **p) {

	if(*p!=NULL) { 
#if USE_MEMMNG == 1u
    memmng_free(*p);
#else
    free(*p);
#endif
    *p = NULL;
  }
	
	
}
		
/** @brief  
  * @param  
  * @retval 
  * @note   
  */
void* MemCpy(void* dst, const void* src, size_t n)  
{  
	char *tmp;  
	char *s_src;  
	size_t s_n;
	
	ASSERT(dst);
	ASSERT(src);
	ASSERT(((u32)dst+n <= (u32)src) || ((u32)src+n <= (u32)dst));
	
memcpy_again:	
	s_n = n;
  tmp = (char*)dst;  
	s_src = (char*)src;  
	while(s_n--) {  
		*tmp++ = *s_src++;  
	}  
	
//ʹ������sram�з���
#if USE_MEMMNG == 1u
	s_n = n;
  tmp = (char*)dst;  
	s_src = (char*)src;  
	while(s_n--) {  
		//ASSERT(*tmp++ == *s_src++);  
		if(*tmp++ != *s_src++)
			goto memcpy_again;
	}  
#endif
	return dst;  
}  
		
/** @brief  
  * @param  
  * @retval 
  * @note   
  */
void* MemCpyUnsafe(void* dst, const void* src, size_t n)  
{  
	char *tmp;  
	char *s_src;  
	size_t s_n;
	
	ASSERT(dst);
	ASSERT(src);
	
	s_n = n;
  tmp = (char*)dst;  
	s_src = (char*)src;  
	while(s_n--) {  
		*tmp++ = *s_src++;  
	}  
	
	return dst;  
}  

/** @brief  
  * @param  
  * @retval 
  * @note   
  */
void* MemMove(void* dst, const void* src, size_t n)  
{  
	char* s_dst;  
	char* s_src;  
	size_t s_n = n;
	ASSERT(dst);
	ASSERT(src);
	s_dst = (char*)dst;  
	s_src = (char*)src;  
	if(s_dst>s_src && (s_src+n>s_dst)) { 
		s_dst = s_dst+n-1;  
		s_src = s_src+n-1;  
		while(n--) {  
			*s_dst-- = *s_src--;  
		}  
	} else {  
		while(n--) {  
			*s_dst++ = *s_src++;  
		}  
	}  
	return dst;  
}  


void DelayUS(u32 cnt)
{
	u32 j;
	cnt=cnt*18;//cnt=cnt*42;
	for(j=0;j<cnt;j++);
}

	
/** @brief  
  * @param  
  * @retval 
  * @note   
  */
void assert(uint8_t* file, uint8_t* function, uint32_t line)
{ 
  char tmp[100]={0};
	char loop = 1;
	
	__disable_irq();   // �ر����ж�
	
	//g_cdvStat = CDV_ASSERT;
	
	sprintf(tmp , "npc assert occur! file:%s\r\nfunction:%s\r\nline:%d\r\n" 
	,file 
	,function
	,line
	);
	
  while (loop)
  {
	//	USARTSend((u8*)tmp, strlen(tmp), MAIN_COM);
		DelayUS(5000000);
  }
	
	__enable_irq();
}
/** @brief  
  * @param  
  * @retval 
  * @note   
  */
void assert_plus(uint8_t* file, uint8_t* function, uint32_t line, char* str, int len)
{ 
	#define NUM_ASSERT 200
  char tmp[NUM_ASSERT]={0};
	char loop = 1;
	
	__disable_irq();   // �ر����ж�
	
	//g_cdvStat = CDV_ASSERT;
	
	sprintf(tmp , "npc assert occur! file:%s\r\nfunction:%s\r\nline:%d\r\n" 
	,file 
	,function
	,line
	);
	
	
	if(len && str)
	  if(strlen(tmp)+len < NUM_ASSERT) {
		  MemCpy(tmp+strlen(tmp),str,len);
			MemCpy(tmp+strlen(tmp)+len,"\r\n",2);
		}
	
  while (loop)
  {
		//USARTSend((u8*)tmp, strlen(tmp), MAIN_COM);
		DelayUS(5000000);
  }
	
	__enable_irq();
}
/** @brief  
  * @param  
  * @retval 
  * @note   ����������봥�����豸�������ܿ�������
  */
void com_print(const char* str)
{
		//USARTSend((u8*)str, strlen(str), MAIN_COM);
}
//////////crc 

const unsigned char chCRCHTalbe[] =                                 // CRC ��λ�ֽ�ֵ��
{
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40
};

const unsigned char chCRCLTalbe[] =                                 // CRC ��λ�ֽ�ֵ��
{
0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7,
0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E,
0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09, 0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9,
0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC,
0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32,
0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D,
0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A, 0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38,
0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF,
0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1,
0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4,
0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F, 0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB,
0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA,
0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0,
0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97,
0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C, 0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E,
0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89,
0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83,
0x41, 0x81, 0x80, 0x40
};

// crc = MODBUS_CRC16(buf,len, 0xFFFF);
unsigned short MODBUS_CRC16(unsigned char* pchMsg, unsigned short wDataLen, unsigned short crc)
{
        unsigned char chCRCHi = (crc >> 8) & 0xFF; // ��CRC�ֽڳ�ʼ��
        unsigned char chCRCLo = crc & 0xFF; // ��CRC�ֽڳ�ʼ��
        unsigned short wIndex;            // CRCѭ���е�����

        while (wDataLen--)
        {
                // ����CRC
                wIndex = chCRCLo ^ *pchMsg++ ;
                chCRCLo = chCRCHi ^ chCRCHTalbe[wIndex];
                chCRCHi = chCRCLTalbe[wIndex] ;
        }

        return ((chCRCHi << 8) | chCRCLo) ;
}
   
/////////////////////////////////////ticks

volatile uint32_t sys_ticks;                       /* timeTicks counter */
void SysTick_Handler(void) {
  sys_ticks++;                                     /* increment timeTicks counter */
}

//__INLINE static void Delay (uint32_t dlyTicks) {
//  uint32_t curTicks = msTicks;
//  while ((msTicks - curTicks) < dlyTicks);
//}

//#define COUNT_TICK // 1��tick �� 1ms
u32 CalcCount(u32 end_count , u32 start_count) {
	u32 count = end_count >= start_count ? end_count - start_count : ((u32)0xFFFFFFFF) - start_count + end_count; 
	return (count);
}