
/**
  ******************************************************************************
  * @file    /modbus.c 
  * @author  MMY
  * @version V2.0.0
  * @date    4-July-2015
  * @brief   modbus function and var and 
  * 
@verbatim  
             20160520：只能进行普通io的读写和变量的读写
             20170411：增加组合命令函数，及读到变量函数
@endverbatim        
  *
  ******************************************************************************
  * @attention
  *
  * COPYRIGHT 2015 CQT Quartz. Co., Ltd.
  *
  ******************************************************************************
  */
	
	#include "modbus.h"



MODBUS_Coil g_modbusCoil={0};
MODBUS_Input_Coil g_modbusInCoil={0};
MODBUS_Register g_modbusReg={0};
MODBUS_Input_Register g_modbusInReg={0};

//int ReadCoil(u8* rxBuf,u8 rxLen, u8 uartNo);
//int ReadInCoil(u8* rxBuf,u8 rxLen, u8 uartNo);
//int ReadRegister(u8* rxBuf,u8 rxLen, u8 uartNo);
//int ReadInRegister(u8* rxBuf,u8 rxLen, u8 uartNo);
//int WriteCoil(u8* rxBuf,u8 rxLen, u8 uartNo);
//int WriteRegister(u8* rxBuf,u8 rxLen, u8 uartNo);
//int WriteMultiCoil(u8* rxBuf,u8 rxLen, u8 uartNo);
//int WriteMultiRegister(u8* rxBuf,u8 rxLen, u8 uartNo);

/**
  * @brief  modbus功能码出错回复
  *         
  *  
  * @param  no     功能码
  *         err     错误码
  *           
  *   
  * @retval 返回值说明
  *
  * @note   
  */
void ModbusRequest(u8 no,u8 errNo, CMD_ARG *arg) {
	u8 len , *txBuf =NULL;
//	OS_ERR err;
	switch(no) {
		case 65:
			len = 6;//tx总长度=8
			//txBuf = (u8*)malloc(sizeof(u8)*len);
			NEW08U(txBuf , len);
		  txBuf[0] = arg->buf[0];
			txBuf[1] = 0x41+0x80;
			txBuf[2] = 00;
		  txBuf[3] = 00;
		 // txBuf[4] = g_scriptInfo.no;
		  txBuf[5] = errNo;
//			len = 8;//tx总长度=8
//		USART_TX_ADD_WITH_LEN(len);//开辟空间
//		USART_TX_BUF(0) = CDV_ID;
//		USART_TX_BUF(1) = 0x41+0x80;
//		USART_TX_BUF(2) = 00;
//		USART_TX_BUF(3) = 00;
//		USART_TX_BUF(4) = g_scriptInfo.no;
//		USART_TX_BUF(5) = errNo;
//		RequestAdd(USART_TX_BUF_ADDR, len);
//		USART_TX_QUEUE_SELF_ADD;
//		OSTaskResume((OS_TCB*)&UsartSendTaskTCB,&err);
			break;
		default:
			len = 3;//tx总长度=5
		  //txBuf = (u8*)malloc(sizeof(u8)*len);
		  NEW08U(txBuf , len);
		  txBuf[0] = arg->buf[0];
			txBuf[1] = no+0x80;
			txBuf[2] = errNo;
			break;
	}
	AddTxPlus(txBuf , len , arg);
	DELETE(txBuf);
}
/**
  * @brief  modbus读线圈
  *         01
  *  
  * @param  rxBuf     待分析字符串指针
  *         rxLen     待分析字符串的长度
  *           
  *   
  * @retval 返回值说明
  *
  * @note   使用到全局变量,不能重入
  */
int ReadCoil(u8* rxBuf,u8 rxLen, CMD_ARG *arg){//获取位状态
	
	u16 i, len ;
	u16 addr, num, numCh;
	u16 sta, sf;
	u8 *txBuf = NULL;
	addr = (rxBuf[2]<<8) + rxBuf[3];

	num = (rxBuf[4]<<8) + rxBuf[5];
	
	if(0x0001 >num || num> 0x07D0){
		ModbusRequest(rxBuf[1] , 3, arg);
		return 3;
	} else if(addr > (COIL_CHN<<3) || addr + num > (COIL_CHN<<3)) {
		ModbusRequest(rxBuf[1] , 2, arg);
		return 2;
	}
	
	sta = (addr >> 3) /*+ 3*/;//读线圈的初始char
	sf = addr & 0x07;
	numCh = (num>>3)+((num & 0x07)?1:0);//读char数量
	
	
	len = 5 + numCh-2;//tx总长度
	//USART_TX_ADD_WITH_LEN(len);//开辟空间
	NEW08U(txBuf , len);
	txBuf[0] = rxBuf[0];
	txBuf[1] = rxBuf[1];
	txBuf[2] = numCh;//(num>>3);//numCh;
	//所处字节
	for(i=0;i<txBuf[2]+1;i++){//多操作一位，防止最后一个字节少读位
		txBuf[3+i] = MODBUS_COIL_CH(sta+i);
	}
	//偏移
	for(i=0;i<txBuf[2];i++){
		txBuf[3+i] = txBuf[3+i]>>sf | txBuf[4+i]<<(8-sf);
	}
	if(/*sf != 0 && */i!= 0)
		txBuf[3+i-1] &= (0xFF>>(8 - (0 ==(num & 0x07)?8:(num & 0x07))));// 补0
	
  AddTxPlus(txBuf , len, arg);
	DELETE(txBuf);
	return 0;
}

/**
  * @brief  modbus读输入线圈
  *         02
  *  
  * @param  rxBuf     待分析字符串指针
  *         rxLen     待分析字符串的长度
  *           
  *   
  * @retval 返回值说明
  *
  * @note   使用到全局变量,不能重入
  */
int ReadInCoil(u8* rxBuf,u8 rxLen, CMD_ARG *arg){//获取位状态
	
	u16 i, len ;
	u16 addr, num, numCh;
	u16 sta, sf;
	u8 *txBuf = NULL;
	
	addr = (rxBuf[2]<<8) + rxBuf[3];
	num = (rxBuf[4]<<8) + rxBuf[5];//读i数量
	
	if(0x0001 >num || num> 0x07D0){
		ModbusRequest(rxBuf[1] , 3, arg);
		return 3;
	} else if(addr > (INCOIL_CHN<<3) || addr + num > (INCOIL_CHN<<3)) {
		ModbusRequest(rxBuf[1] , 2, arg);
		return 2;
	}
	
//	for(i = 0 ; i < I_NUM; i ++) {
//	  IRead(i);
//	}
	
	sta = (addr >> 3) /*+ 3*/;//读线圈的初始char
	sf = addr & 0x07;
	numCh = (num>>3)+((num & 0x07)?1:0);//读char数量
	len = 5 + numCh-2;//tx总长度
	
	NEW08U(txBuf , len);
	txBuf[0] = rxBuf[0];
	txBuf[1] = rxBuf[1];
	txBuf[2] = numCh;//(num>>3);//numCh;
	//所处字节
	for(i=0;i<txBuf[2]+1;i++){//多操作一位，防止最后一个字节少读位
				txBuf[3+i] = MODBUS_INCOIL_CH(sta+i);
			}
	//偏移
	for(i=0;i<txBuf[2];i++){
				txBuf[3+i] = txBuf[3+i]>>sf | txBuf[4+i]<<(8-sf);
			}
	if(/*sf != 0 &&*/ i!= 0)
		txBuf[3+i-1] &= (0xFF>>(8 - (0 ==(num & 0x07)?8:(num & 0x07))));// 补0
	
  AddTxPlus(txBuf , len, arg);	
	DELETE(txBuf);
	return 0;
}

/**
  * @brief  modbus读寄存器
  *         03
  *  
  * @param  rxBuf     待分析字符串指针
  *         rxLen     待分析字符串的长度
  *           
  *   
  * @retval 返回值说明
  *
  * @note   使用到全局变量,不能重入
  */
int ReadRegister(u8* rxBuf,u8 rxLen, CMD_ARG *arg){//获取位状态
	
	u16 i, len ;
	u16 addr, num;
	u8 *txBuf = NULL;
	
	addr = (rxBuf[2]<<8) + rxBuf[3];

	num = (rxBuf[4]<<8) + rxBuf[5];
	
	if(0x0001 >num || num> 0x007D){
		ModbusRequest(rxBuf[1] , 3, arg);
		return 3;
	} else if(addr > REG_N || addr + num > REG_N) {
		ModbusRequest(rxBuf[1] , 2, arg);
		return 2;
	}
	
	len = 5 + (num<<1)-2;//tx总长度
	
	NEW08U(txBuf , len);
	txBuf[0] = rxBuf[0];
	txBuf[1] = rxBuf[1];
	txBuf[2] = num<<1;
	//所处字节
	for(i=0;i<num;i++){
		txBuf[3 +(i<<1)] = HIGH16U(MODBUS_REG_ADDR(addr + i));
		txBuf[3 +(i<<1)+ 1] = LOW16U(MODBUS_REG_ADDR(addr + i));
	}
	
  AddTxPlus(txBuf , len, arg);
  DELETE(txBuf);
	return 0;	
}

/**
  * @brief  modbus读输入寄存器
  *         04
  *  
  * @param  rxBuf     待分析字符串指针
  *         rxLen     待分析字符串的长度
  *           
  *   
  * @retval 返回值说明
  *
  * @note   使用到全局变量,不能重入
  */
int ReadInRegister(u8* rxBuf,u8 rxLen, CMD_ARG *arg){//获取位状态
	
	u16 i, len ;
	u16 addr, num;
	u8 *txBuf = NULL;
	
	addr = (rxBuf[2]<<8) + rxBuf[3];

	num = (rxBuf[4]<<8) + rxBuf[5];
	
	if(0x0001 >num || num> 0x007D){
		ModbusRequest(rxBuf[1] , 3, arg);
		return 3;
	} else if(addr > INREG_N || addr + num > INREG_N) {
		ModbusRequest(rxBuf[1] , 2, arg);
		return 2;
	}
	
	len = 5 + (num<<1)-2;//tx总长度
	
	NEW08U(txBuf , len);
	txBuf[0] = rxBuf[0];
	txBuf[1] = rxBuf[1];
	txBuf[2] = num<<1;
	//所处字节
	for(i=0;i<num;i++){
		txBuf[3 +(i<<1)] = HIGH16U(MODBUS_INREG_ADDR(addr + i));
		txBuf[3 +(i<<1)+ 1] = LOW16U(MODBUS_INREG_ADDR(addr + i));
	}
	
  AddTxPlus(txBuf , len, arg);
	DELETE(txBuf);
	return 0;	
}

/**
  * @brief  modbus写单线圈
  *         05
  *  
  * @param  rxBuf     待分析字符串指针
  *         rxLen     待分析字符串的长度
  *           
  *   
  * @retval 返回值说明
  *
  * @note   使用到全局变量,不能重入
  */
int WriteCoil(u8* rxBuf,u8 rxLen, CMD_ARG *arg){//获取位状态
	
	//u8 len ;
	u16 addr, num;
	//u8 *txBuf = NULL;
//	u16 sta, sf;
	addr = (rxBuf[2]<<8) + rxBuf[3];
	
	num = (rxBuf[4]<<8) + rxBuf[5];
//	sta = (addr >> 3) /*+ 3*/;//读线圈的第几字节 0->*
//	sf = addr & 0x07;//读线圈的第几位 9->0
	if(addr > (COIL_CHN<<3) ) {
		ModbusRequest(rxBuf[1] , 2, arg);
		return 2;
	}else if(num != 0x0000 && num != 0xFF00) {
		ModbusRequest(rxBuf[1] , 3, arg);
		return 3;
	}
	
	if (0xFF00 == num){
		SET_COIL_ADDR(addr);
		//OWrite(addr,BIT_1);//定制
		
	} else if (0x0000 == num){
		RESET_COIL_ADDR(addr);
		//OWrite(addr,BIT_0);//定制
		
	} else {
		
	}
	
//		if(READ_O(addr))
//		{
//			OWrite(addr, BIT_1);
//		}
//		else
//		{
//			OWrite(addr, BIT_0);
//		}
	
	AddTxPlus(rxBuf , rxLen, arg);
	return 0;	
				
}
				
/**
  * @brief  modbus写单寄存器
  *         06
  *  
  * @param  rxBuf     待分析字符串指针
  *         rxLen     待分析字符串的长度
  *           
  *   
  * @retval 返回值说明
  *
  * @note   使用到全局变量,不能重入
  */
int WriteRegister(u8* rxBuf,u8 rxLen, CMD_ARG *arg){//获取位状态
	
	u16 addr, num;
	addr = (rxBuf[2]<<8) + rxBuf[3];

	num = (rxBuf[4]<<8) + rxBuf[5];
	
	if(addr > REG_N ) {
		ModbusRequest(rxBuf[1] , 2, arg);
		return 2;
	}
//	else if(num < 0x0000 || num > 0xFFFF) {
//		ModbusRequest(rxBuf[1] , 3, uartNo);
//		return;
//	}
	
	MODBUS_REG_ADDR(addr) = num;
	
  AddTxPlus(rxBuf , rxLen, arg);

//#if USE_PVD == 0u && USE_EXTI_POWER_OFF == 0u
//	SPI_Flash_Write((u8 *)&num, (VAL_STADDR + 2 * (addr)), 2);
//#endif
	return 0;
}
			
/**
  * @brief  modbus写多线圈
  *         15
  *  
  * @param  rxBuf     待分析字符串指针
  *         rxLen     待分析字符串的长度
  *           
  *   
  * @retval 返回值说明
  *
  * @note   使用到全局变量,不能重入
  */
int WriteMultiCoil(u8* rxBuf,u8 rxLen, CMD_ARG *arg){
	
	u16 i;
	u16 addr, num, numCh;
	u16 sta, sf, end, esf;
	addr = (rxBuf[2]<<8) + rxBuf[3];
	
	num = (rxBuf[4]<<8) + rxBuf[5];//写的数量，要处理多少字节关键要看这个，而不能看numch
	sta = (addr >> 3) /*+ 3*/;//初始char
	end = ((addr+num-1) >> 3) /*+ 3*/;//最终char
	sf = addr & 0x07;
	esf = ((addr+num-1) & 0x07) +1;
	
	numCh = end - sta;//间隔数
	
	if(0x0001 >num || num> 0x07B0 ||((num>>3)+(num&0x07?1:0)) != rxBuf[6]){
		ModbusRequest(rxBuf[1] , 3, arg);
		return 3;
	} else if(addr > (COIL_CHN<<3) || addr + num > (COIL_CHN<<3)) {
		ModbusRequest(rxBuf[1] , 2, arg);
		return 2;
	}
	
//	if(numCh > rxBuf[6])
//		return;//读char数量
	
	//第一个字节
	if(numCh != 0)
	  MODBUS_COIL_CH(sta) = (rxBuf[7]<<sf) | (MODBUS_COIL_CH(sta) & (0xFF>>(8-sf)));
	else
		MODBUS_COIL_CH(sta) = ((rxBuf[7]<<sf)& (0xFF>>(8-esf))) | (MODBUS_COIL_CH(sta) & ((0xFF>>(8-sf)) | (0xFF<<esf)));
	//中间字节
	for(i=1;i<numCh;i++){
		MODBUS_COIL_CH(sta+i) = (rxBuf[7+i]<<sf) | (rxBuf[7+i-1]>>(8-sf));
	}
	//最后字节
		//	if(i >= rxBuf[6])
	if(numCh != 0 )
		MODBUS_COIL_CH(sta+i) = (MODBUS_COIL_CH(sta+i) & (0xFF<<(esf))) 
	| (((i>=rxBuf[6]?0x00 : (rxBuf[7+i]<<sf)) | (rxBuf[7+i-1]>>(8-sf)))& (0xFF>>(8-esf)));
	
//	for(i = addr; i< addr + num ; i++)
//	if(READ_O(i))
//	{
//		OWrite(i, BIT_1);
//	}
//	else
//	{
//		OWrite(i, BIT_0);
//	}
  AddTxPlus(rxBuf , 6, arg);		
  return 0;	
}
			
/**
  * @brief  modbus写多寄存器
  *         16
  *  
  * @param  rxBuf     待分析字符串指针
  *         rxLen     待分析字符串的长度
  *           
  *   
  * @retval 返回值说明
  *
  * @note   使用到全局变量,不能重入
  */
int WriteMultiRegister(u8* rxBuf,u8 rxLen, CMD_ARG *arg){//获取位状态
	
	u16 i;
	u16 addr, num , numCh , data;
	addr = (rxBuf[2]<<8) + rxBuf[3];

	num = (rxBuf[4]<<8) + rxBuf[5];
	numCh = rxBuf[6];
	
	if(0x0001 >num || num> 0x007B ||numCh != num*2){
		ModbusRequest(rxBuf[1] , 3, arg);
		return 3;
	} else if(addr > REG_N || addr + num > REG_N) {
		ModbusRequest(rxBuf[1] , 2, arg);
		return 2;
	}
	
//	else if(num < 0x0000 || num > 0xFFFF) {
//		ModbusRequest(rxBuf[1] , 3);
//		return;
//	}
	for( i = 0; i< num ; i++) {
		data = (rxBuf[7+2*i]<<8) + rxBuf[8+2*i];
		MODBUS_REG_ADDR(addr+i) = data;
	}
	
	AddTxPlus(rxBuf , 6/*rxLen*/, arg);
//#if USE_PVD == 0u && USE_EXTI_POWER_OFF == 0u
//	ValToFlash((addr>>1), (num>>1));
//#endif
  return 0;	
}

/**
  * @brief  对字符串进行modbus分析
  *  
  * @param  rxBuf     待分析字符串指针
  *         len       待分析字符串的长度
  *         txBuff    应答字符串  
  *   
  * @retval 返回值说明
  *
  * @note   RecvParse中
  */

u8 ModbusParse(u8* rxBuf, u8 rxLen, CMD_ARG *arg/*, u8 uartNo*/){	

//	OS_ERR err;
	int ret = 0;
	switch(rxBuf[1]){ //命令码筛选
		case 1://读线圈
			ret = ReadCoil(rxBuf,rxLen, arg);
			break;
		case 2://读输入线圈
			ret = ReadInCoil(rxBuf,rxLen, arg);
			break;
		case 3://读寄存器
			ret = ReadRegister(rxBuf,rxLen, arg);
			break;
		case 4://读输入寄存器
			ret = ReadInRegister(rxBuf,rxLen, arg);
			break;
		case 5://写单线圈
			ret = WriteCoil(rxBuf,rxLen, arg);
			break;
		case 6://预置单寄存器
			ret = WriteRegister(rxBuf,rxLen, arg);
			break;
		case 7://读取异常状态
			break;
		case 8://回送诊断校验
			break;
		case 9://编程
			break;
		case 10://控询
			break;
		case 11://读取事件计数
			break;
		case 12://读取通信事件记录
			break;
		case 15://写多线圈
			ret = WriteMultiCoil(rxBuf,rxLen, arg);
			break;
		case 16://预置多寄存器
			ret = WriteMultiRegister(rxBuf,rxLen, arg);
			break;
		case 17://报告从机标志
			break;
		case 22://扩展功能------CDV功能
			break;
		case 65://扩展功能------CDV功能
			break;
		case 0xfe://自定义读
			break;
		default: //命令码无效不应答
			ModbusRequest(rxBuf[1], 1, arg);
			break;
	}
	
	return ret;
}


  /************************************************************
	********************发送命令组合函数*************************
	************************************************************/
/**
  * @brief  字节数据互换
  *  
  * @param  
  *   
  * @retval 
  *
  * @note   
  */
void SwapByte(u8* rbuf, u8* dbuf)
{
	u8 tmp;
	ASSERT(rbuf && dbuf);
	tmp = *rbuf;
	*rbuf = *dbuf;
	*dbuf = tmp;	
}
	

/**
  * @brief  16字节高低字节互换，用于大小端转换
  *  
  * @param  size  寄存器多少个字节，16位 取2
            num   buf有多少个字节
  *   
  * @retval u8*
  *
  * @note   
  */
u8* Endian_TF(u8* buf, const u16 num, const u16 size)
{
	int i, j;
	ASSERT(!(num%size));
	ASSERT(!(size%2));

	for (i = 0; i < num / size; i++)
	{
		for (j = 0; j < size / 2; j++)
		{
			SwapByte (buf + size*i + j , buf + size*i + size - 1 - j);
		}
	}
	
	
	return buf;
}


/** @brief  发送命令组合 写寄存器
  * @param  dev     对方设备号
            addr    地址
            num     数量
            cmdBuf  组合命令缓存
            cmdLen  组合命令长度
  * @retval 
  * @note   外部调用负责清理cmdBuf
  */
void WriteRegisterCmd(u8 dev, u16 addr, u16 val, 
     u8** cmdBuf,u8* cmdLen,BUF_OPT flag)
{
	ASSERT(NULL != cmdBuf);
	//ASSERT(NULL == *cmdBuf);
	
	ASSERT(NULL != cmdLen);
	
	*cmdLen = 6;
	if(BUF_NEW == flag)
	NEW08U(*cmdBuf, *cmdLen);
	(*cmdBuf)[0] = dev;
	(*cmdBuf)[1] = 0x06;
	(*cmdBuf)[2] = (addr>>8) & 0xff;
	(*cmdBuf)[3] = (addr) & 0xff;
	(*cmdBuf)[4] = (val>>8) & 0xff;
	(*cmdBuf)[5] = (val) & 0xff;
	
}
/** @brief  发送命令组合 写多寄存器
  * @param  dev     对方设备号
            addr    地址
            num     数量
            cmdBuf  组合命令缓存
            cmdLen  组合命令长度
  * @retval 
  * @note   外部调用负责清理cmdBuf
  */
void WriteMultiRegisterCmd(u8 dev, u16 addr, u16 num, 
     u8* regVal, u8** cmdBuf,u8* cmdLen,BUF_OPT flag)
{
	ASSERT(NULL != cmdBuf);
	//ASSERT(NULL == *cmdBuf);
	
	ASSERT(NULL != cmdLen);
	ASSERT(NULL != regVal);
	ASSERT(0x7D >= num);
	
	*cmdLen = 7 + (num << 1);
	if(BUF_NEW == flag)
	NEW08U(*cmdBuf, *cmdLen);
	(*cmdBuf)[0] = dev;
	(*cmdBuf)[1] = 0x10;
	(*cmdBuf)[2] = (addr>>8) & 0xff;
	(*cmdBuf)[3] = (addr) & 0xff;
	(*cmdBuf)[4] = (num>>8) & 0xff;
	(*cmdBuf)[5] = (num) & 0xff;
  (*cmdBuf)[6] = num << 1;
	
	MemCpy((*cmdBuf) + 7, Endian_TF(regVal, num << 1, 2), (*cmdBuf)[6]);
}
/** @brief  发送命令组合 读寄存器
  * @param  dev     对方设备号
            addr    地址
            num     数量
            cmdBuf  组合命令缓存
            cmdLen  组合命令长度
  * @retval 
  * @note   外部调用负责清理cmdBuf
  */
void ReadRegisterCmd(u8 dev, u16 addr, u16 num, 
     u8** cmdBuf,u8* cmdLen,BUF_OPT flag)
{
	ASSERT(NULL != cmdBuf);
	//ASSERT(NULL == *cmdBuf);
	ASSERT(NULL != cmdLen);
	ASSERT(0x7D >= num);
	
	*cmdLen = 6;
	if(BUF_NEW == flag)
	NEW08U(*cmdBuf, *cmdLen);
	(*cmdBuf)[0] = dev;
	(*cmdBuf)[1] = 0x03;
	(*cmdBuf)[2] = (addr>>8) & 0xff;
	(*cmdBuf)[3] = (addr) & 0xff;
	(*cmdBuf)[4] = (num>>8) & 0xff;
	(*cmdBuf)[5] = (num) & 0xff;
}
/** @brief  发送命令组合 读只读寄存器
  * @param  dev     对方设备号
            addr    地址
            num     数量
            cmdBuf  组合命令缓存
            cmdLen  组合命令长度
  * @retval 
  * @note   外部调用负责清理cmdBuf
  */
void ReadInRegisterCmd(u8 dev, u16 addr, u16 num, 
     u8** cmdBuf,u8* cmdLen,BUF_OPT flag)
{
	ASSERT(NULL != cmdBuf);
	//ASSERT(NULL == *cmdBuf);
	ASSERT(NULL != cmdLen);
	ASSERT(0x7D >= num);
	
	*cmdLen = 6;
	if(BUF_NEW == flag)
	NEW08U(*cmdBuf, *cmdLen);
	(*cmdBuf)[0] = dev;
	(*cmdBuf)[1] = 0x04;
	(*cmdBuf)[2] = (addr>>8) & 0xff;
	(*cmdBuf)[3] = (addr) & 0xff;
	(*cmdBuf)[4] = (num>>8) & 0xff;
	(*cmdBuf)[5] = (num) & 0xff;
}
/** @brief  发送命令组合 写线圈
  * @param  dev     对方设备号
            addr    地址
            num     数量
            cmdBuf  组合命令缓存
            cmdLen  组合命令长度
  * @retval 
  * @note   外部调用负责清理cmdBuf
  */
void WriteCoilCmd(u8 dev, u16 addr, u16 val, 
     u8** cmdBuf,u8* cmdLen,BUF_OPT flag)
{
	ASSERT(NULL != cmdBuf);
	//ASSERT(NULL == *cmdBuf);
	ASSERT(NULL != cmdLen);
	
	*cmdLen = 6;
	if(BUF_NEW == flag)
	NEW08U(*cmdBuf, *cmdLen);
	(*cmdBuf)[0] = dev;
	(*cmdBuf)[1] = 0x05;
	(*cmdBuf)[2] = (addr>>8) & 0xff;
	(*cmdBuf)[3] = (addr) & 0xff;
	(*cmdBuf)[4] = val ? 0xFF : 0x00;
	(*cmdBuf)[5] = 0x00;
}
/** @brief  发送命令组合 写多线圈
  * @param  dev     对方设备号
            addr    地址
            num     数量
            cmdBuf  组合命令缓存
            cmdLen  组合命令长度
  * @retval 
  * @note   外部调用负责清理cmdBuf
  */
void WriteMultiCoilCmd(u8 dev, u16 addr, u16 num, 
     u8* coilVal, u8** cmdBuf,u8* cmdLen,BUF_OPT flag) {
	u16 numCh = (num>>3)+((num & 0x07)?1:0);
	ASSERT(NULL != cmdBuf);
	//ASSERT(NULL == *cmdBuf);
	ASSERT(NULL != cmdLen);
	
	*cmdLen = numCh + 7;
			 if(BUF_NEW == flag)
	NEW08U(*cmdBuf, *cmdLen);
	(*cmdBuf)[0] = dev;
	(*cmdBuf)[1] = 0x0F;
	(*cmdBuf)[2] = (addr>>8) & 0xff;
	(*cmdBuf)[3] = (addr) & 0xff;
	(*cmdBuf)[4] = (num>>8) & 0xff;
	(*cmdBuf)[5] = (num) & 0xff;
  (*cmdBuf)[6] = numCh;
	MemCpy((*cmdBuf) + 7, coilVal, (*cmdBuf)[6]);
	
}
/** @brief  发送命令组合 读只读线圈
  * @param  dev     对方设备号
            addr    地址
            num     数量
            cmdBuf  组合命令缓存
            cmdLen  组合命令长度
  * @retval 
  * @note   外部调用负责清理cmdBuf
  */
void ReadInCoilCmd(u8 dev, u16 addr, u16 num, 
     u8** cmdBuf,u8* cmdLen,BUF_OPT flag)
{
	ASSERT(NULL != cmdBuf);
	//ASSERT(NULL == *cmdBuf);
	ASSERT(NULL != cmdLen);
	ASSERT(0x7D0 >= num);
	
	*cmdLen = 6;
	if(BUF_NEW == flag)
	NEW08U(*cmdBuf, *cmdLen);
	(*cmdBuf)[0] = dev;
	(*cmdBuf)[1] = 0x02;
	(*cmdBuf)[2] = (addr>>8) & 0xff;
	(*cmdBuf)[3] = (addr) & 0xff;
	(*cmdBuf)[4] = (num>>8) & 0xff;
	(*cmdBuf)[5] = (num) & 0xff;
}
/** @brief  发送命令组合 读线圈
  * @param  dev     对方设备号
            addr    地址
            num     数量
            cmdBuf  组合命令缓存
            cmdLen  组合命令长度
  * @retval 
  * @note   外部调用负责清理cmdBuf
  */
void ReadCoilCmd(u8 dev, u16 addr, u16 num, 
     u8** cmdBuf,u8* cmdLen,BUF_OPT flag)
{
	ASSERT(NULL != cmdBuf);
	//ASSERT(NULL == *cmdBuf);
	ASSERT(NULL != cmdLen);
	ASSERT(0x7D0 >= num);
	
	*cmdLen = 6;
	if(BUF_NEW == flag)
	  NEW08U(*cmdBuf, *cmdLen);
	(*cmdBuf)[0] = dev;
	(*cmdBuf)[1] = 0x01;
	(*cmdBuf)[2] = (addr>>8) & 0xff;
	(*cmdBuf)[3] = (addr) & 0xff;
	(*cmdBuf)[4] = (num>>8) & 0xff;
	(*cmdBuf)[5] = (num) & 0xff;
}
  /************************************************************
	********************反馈命令组合函数*************************
	************************************************************/
/** @brief  反馈命令组合 读线圈 01
  * @param  rxBuf     待分析字符串指针
  *         rxLen     待分析字符串的长度
  * @retval 返回值说明
  * @note   外部调用负责清理cmdBuf
  */
int ReadCoilReqCmd(u8* rxBuf,u8 rxLen, u8** cmdBuf,u8* cmdLen){//获取位状态
	
	u8 i;
	u16 addr, num, numCh;
	u16 sta, sf;
	
	ASSERT(NULL != cmdBuf);
	ASSERT(NULL == *cmdBuf);
	ASSERT(NULL != cmdLen);
	
	addr = (rxBuf[2]<<8) + rxBuf[3];

	num = (rxBuf[4]<<8) + rxBuf[5];
	
	if(0x0001 >num || num> 0x07D0){
		return 3;
	} else if(addr > (COIL_CHN<<3) || addr + num > (COIL_CHN<<3)) {
		return 2;
	}
	
	sta = (addr >> 3) /*+ 3*/;//读线圈的初始char
	sf = addr & 0x07;
	numCh = (num>>3)+((num & 0x07)?1:0);//读char数量
	
	
	*cmdLen = 5 + numCh-2;//tx总长度
	NEW08U(*cmdBuf , *cmdLen);
	(*cmdBuf)[0] = rxBuf[0];
	(*cmdBuf)[1] = rxBuf[1];
	(*cmdBuf)[2] = numCh;//;(num>>3)
	//所处字节
	for(i=0;i<(*cmdBuf)[2]+1;i++){//多操作一位，防止最后一个字节少读位
		(*cmdBuf)[3+i] = MODBUS_COIL_CH(sta+i);
	}
	//偏移
	for(i=0;i<(*cmdBuf)[2];i++){
		(*cmdBuf)[3+i] = (*cmdBuf)[3+i]>>sf | (*cmdBuf)[4+i]<<(8-sf);
	}
	if(/*sf != 0 && */i!= 0)
		(*cmdBuf)[3+i-1] &= (0xFF>>(8 - (0 ==(num & 0x07)?8:(num & 0x07))));// 补0
	
	return 0;
}

/** @brief  反馈命令组合 读输入线圈 02
  * @param  rxBuf     待分析字符串指针
  *         rxLen     待分析字符串的长度
  * @retval 返回值说明
  * @note   外部调用负责清理cmdBuf
  */
int ReadInCoilReqCmd(u8* rxBuf,u8 rxLen, u8** cmdBuf,u8* cmdLen){//获取位状态
	
	u8 i;
	u16 addr, num, numCh;
	u16 sta, sf;
	
	ASSERT(NULL != cmdBuf);
	ASSERT(NULL == *cmdBuf);
	ASSERT(NULL != cmdLen);
	
	addr = (rxBuf[2]<<8) + rxBuf[3];
	num = (rxBuf[4]<<8) + rxBuf[5];//读i数量
	
	if(0x0001 >num || num> 0x07D0){
		return 3;
	} else if(addr > (INCOIL_CHN<<3) || addr + num > (INCOIL_CHN<<3)) {
		return 2;
	}
	
//	for(i = 0 ; i < CDV_I_NUM; i ++) {
//	  IRead(i);
//	}
	
	sta = (addr >> 3) /*+ 3*/;//读线圈的初始char
	sf = addr & 0x07;
	numCh = (num>>3)+((num & 0x07)?1:0);//读char数量
	*cmdLen = 5 + numCh-2;//tx总长度
	
	NEW08U((*cmdBuf) , *cmdLen);
	(*cmdBuf)[0] = rxBuf[0];
	(*cmdBuf)[1] = rxBuf[1];
	(*cmdBuf)[2] = numCh;//;(num>>3)
	//所处字节
	for(i=0;i<(*cmdBuf)[2]+1;i++){//多操作一位，防止最后一个字节少读位
				(*cmdBuf)[3+i] = MODBUS_INCOIL_CH(sta+i);
			}
	//偏移
	for(i=0;i<(*cmdBuf)[2];i++){
				(*cmdBuf)[3+i] = (*cmdBuf)[3+i]>>sf | (*cmdBuf)[4+i]<<(8-sf);
			}
	if(/*sf != 0 &&*/ i!= 0)
		(*cmdBuf)[3+i-1] &= (0xFF>>(8 - (0 ==(num & 0x07)?8:(num & 0x07))));// 补0
	
	return 0;
}

/** @brief  反馈命令组合 读寄存器 03
  * @param  rxBuf     待分析字符串指针
  *         rxLen     待分析字符串的长度
  * @retval 返回值说明
  * @note   外部调用负责清理cmdBuf
  */
int ReadRegisterReqCmd(u8* rxBuf,u8 rxLen, u8** cmdBuf,u8* cmdLen){//获取位状态
	
	u8 i;
	u16 addr, num;
		
	ASSERT(NULL != cmdBuf);
	ASSERT(NULL == *cmdBuf);
	ASSERT(NULL != cmdLen);
	addr = (rxBuf[2]<<8) + rxBuf[3];

	num = (rxBuf[4]<<8) + rxBuf[5];
	
	if(0x0001 >num || num> 0x007D){
		return 3;
	} else if(addr > REG_N || addr + num > REG_N) {
		return 2;
	}
	
	*cmdLen = 5 + (num<<1)-2;//tx总长度
	
	NEW08U((*cmdBuf) , *cmdLen);
	(*cmdBuf)[0] = rxBuf[0];
	(*cmdBuf)[1] = rxBuf[1];
	(*cmdBuf)[2] = num<<1;
	//所处字节
	for(i=0;i<num;i++){
		(*cmdBuf)[3 +(i<<1)] = HIGH16U(MODBUS_REG_ADDR(addr + i));
		(*cmdBuf)[3 +(i<<1)+ 1] = LOW16U(MODBUS_REG_ADDR(addr + i));
	}

	return 0;	
}

/** @brief  反馈命令组合 读输入寄存器 04
  * @param  rxBuf     待分析字符串指针
  *         rxLen     待分析字符串的长度
  * @retval 返回值说明
  * @note   外部调用负责清理cmdBuf
  */
int ReadInRegisterReqCmd(u8* rxBuf,u8 rxLen, u8** cmdBuf,u8* cmdLen){//获取位状态
	
	u8 i;
	u16 addr, num;
		
	ASSERT(NULL != cmdBuf);
	ASSERT(NULL == *cmdBuf);
	ASSERT(NULL != cmdLen);
	addr = (rxBuf[2]<<8) + rxBuf[3];

	num = (rxBuf[4]<<8) + rxBuf[5];
	
	if(0x0001 >num || num> 0x007D){
		return 3;
	} else if(addr > INREG_N || addr + num > INREG_N) {
		return 2;
	}
	
	*cmdLen = 5 + (num<<1)-2;//tx总长度
	
	NEW08U((*cmdBuf) , *cmdLen);
	(*cmdBuf)[0] = rxBuf[0];
	(*cmdBuf)[1] = rxBuf[1];
	(*cmdBuf)[2] = num<<1;
	//所处字节
	for(i=0;i<num;i++){
		(*cmdBuf)[3 +(i<<1)] = HIGH16U(MODBUS_INREG_ADDR(addr + i));
		(*cmdBuf)[3 +(i<<1)+ 1] = LOW16U(MODBUS_INREG_ADDR(addr + i));
	}
	
	return 0;	
}

/** @brief  反馈命令组合 写单线圈05
  * @param  rxBuf     待分析字符串指针
  *         rxLen     待分析字符串的长度
  * @retval 返回值说明
  * @note   外部调用负责清理cmdBuf
  */
int WriteCoilReqCmd(u8* rxBuf,u8 rxLen, u8** cmdBuf,u8* cmdLen){//获取位状态
	u16 addr, num;
	ASSERT(NULL != cmdBuf);
	ASSERT(NULL == *cmdBuf);
	ASSERT(NULL != cmdLen);
	addr = (rxBuf[2]<<8) + rxBuf[3];
	
	num = (rxBuf[4]<<8) + rxBuf[5];
	if(addr > (COIL_CHN<<3) ) {
		return 2;
	}else if(num != 0x0000 && num != 0xFF00) {
		return 3;
	}
	
	if (0xFF00 == num){
		SET_COIL_ADDR(addr);
		
	} else if (0x0000 == num){
		RESET_COIL_ADDR(addr);
		
	} else {
		
	}
	
//	if(READ_O(addr))
//	{
//		OWrite(addr, BIT_1);
//		
//		if(addr == 200)
//			ChangeToCdvStat(CDV_RUN);
//		else if(addr == 201)
//			ChangeToCdvStat(CDV_PAUSE);
//	}
//	else
//	{
//		OWrite(addr, BIT_0);
//		
//		if(addr == 200)
//			ChangeToCdvStat(CDV_ONLINE);
//		else if(addr == 201)
//			ChangeToCdvStat(CDV_RUN);
//	}
	*cmdLen = 6;
	NEW08U((*cmdBuf) , *cmdLen);
	MemCpy((*cmdBuf), rxBuf , 6);
	return 0;	
				
}
				
/** @brief  反馈命令组合 写单寄存器0x06
  * @param  rxBuf     待分析字符串指针
  *         rxLen     待分析字符串的长度
  * @retval 返回值说明
  * @note   外部调用负责清理cmdBuf
  */
int WriteRegisterReqCmd(u8* rxBuf,u8 rxLen, u8** cmdBuf,u8* cmdLen){//获取位状态
	
	u16 addr, num;
		
	ASSERT(NULL != cmdBuf);
	ASSERT(NULL == *cmdBuf);
	ASSERT(NULL != cmdLen);
	addr = (rxBuf[2]<<8) + rxBuf[3];

	num = (rxBuf[4]<<8) + rxBuf[5];
	
	if(addr > REG_N ) {
		return 2;
	}
	
	MODBUS_REG_ADDR(addr) = num;
	
	*cmdLen = 6;
	NEW08U((*cmdBuf) , *cmdLen);
	MemCpy((*cmdBuf), rxBuf , 6);
//#if USE_PVD == 0u && USE_EXTI_POWER_OFF == 0u
//	SPI_Flash_Write((u8 *)&num, (VAL_STADDR + 2 * (addr)), 2);
//#endif
	return 0;
}
			
/** @brief  反馈命令组合 写多线圈0x0F
  * @param  rxBuf     待分析字符串指针
  *         rxLen     待分析字符串的长度
  * @retval 返回值说明
  * @note   外部调用负责清理cmdBuf
  */
int WriteMultiCoilReqCmd(u8* rxBuf,u8 rxLen, u8** cmdBuf,u8* cmdLen){
	
	u8 i;
	u16 addr, num, numCh;
	u16 sta, sf, end, esf;
		
	ASSERT(NULL != cmdBuf);
	ASSERT(NULL == *cmdBuf);
	ASSERT(NULL != cmdLen);
	addr = (rxBuf[2]<<8) + rxBuf[3];
	
	num = (rxBuf[4]<<8) + rxBuf[5];//写的数量，要处理多少字节关键要看这个，而不能看numch
	sta = (addr >> 3) /*+ 3*/;//初始char
	end = ((addr+num-1) >> 3) /*+ 3*/;//最终char
	sf = addr & 0x07;
	esf = ((addr+num-1) & 0x07) +1;
	
	numCh = end - sta;//间隔数
	
	if(0x0001 >num || num> 0x07B0 ||((num>>3)+(num&0x07?1:0)) != rxBuf[6]){
		return 3;
	} else if(addr > (COIL_CHN<<3) || addr + num > (COIL_CHN<<3)) {
		return 2;
	}

	//第一个字节
	if(numCh != 0)
	  MODBUS_COIL_CH(sta) = (rxBuf[7]<<sf) | (MODBUS_COIL_CH(sta) & (0xFF>>(8-sf)));
	else
		MODBUS_COIL_CH(sta) = ((rxBuf[7]<<sf)& (0xFF>>(8-esf))) | (MODBUS_COIL_CH(sta) & ((0xFF>>(8-sf)) | (0xFF<<esf)));
	//中间字节
	for(i=1;i<numCh;i++){
		MODBUS_COIL_CH(sta+i) = (rxBuf[7+i]<<sf) | (rxBuf[7+i-1]>>(8-sf));
	}
	//最后字节
	if(numCh != 0 )
		MODBUS_COIL_CH(sta+i) = (MODBUS_COIL_CH(sta+i) & (0xFF<<(esf))) 
	| (((i>=rxBuf[6]?0x00 : (rxBuf[7+i]<<sf)) | (rxBuf[7+i-1]>>(8-sf)))& (0xFF>>(8-esf)));
	
	for(i = addr; i< addr + num ; i++)
//	if(READ_O(i))
//	{
//		OWrite(i, BIT_1);
//	}
//	else
//	{
//		OWrite(i, BIT_0);
//	}
	*cmdLen = 6;
	NEW08U((*cmdBuf) , *cmdLen);
	MemCpy((*cmdBuf), rxBuf , 6);
  return 0;	
}
			
/** @brief  反馈命令组合 写多寄存器
  * @param  rxBuf     待分析字符串指针
  *         rxLen     待分析字符串的长度
  * @retval 
  * @note   外部调用负责清理cmdBuf
  */
int WriteMultiRegisterReqCmd(u8* rxBuf,u8 rxLen, u8** cmdBuf,u8* cmdLen){//获取位状态
	
	u8 i;
	u16 addr, num , numCh , data;
		
	ASSERT(NULL != cmdBuf);
	ASSERT(NULL == *cmdBuf);
	ASSERT(NULL != cmdLen);
	addr = (rxBuf[2]<<8) + rxBuf[3];

	num = (rxBuf[4]<<8) + rxBuf[5];
	numCh = rxBuf[6];
	
	if(0x0001 >num || num> 0x007B ||numCh != num*2){
		return 3;
	} else if(addr > REG_N || addr + num > REG_N) {
		return 2;
	}
	
	for( i = 0; i< num ; i++) {
		data = (rxBuf[7+2*i]<<8) + rxBuf[8+2*i];
		MODBUS_REG_ADDR(addr+i) = data;
	}
	
	*cmdLen = 6;
	NEW08U((*cmdBuf) , *cmdLen);
	MemCpy((*cmdBuf), rxBuf , rxLen-2);
//#if USE_PVD == 0u && USE_EXTI_POWER_OFF == 0u
//	ValToFlash((addr>>1), (num>>1));
//#endif
  return 0;	
}


  /************************************************************
	********************串口反馈解析函数*************************
	************************************************************/

/** @brief  读到变量 线圈
  * @param  buf     待分析字符串指针
            len     待分析字符串的长度
            bit     提取第几个线圈
            var     到哪好变量
  * @retval 
  * @note
  */
RET_STATUS ReadCoilReqToVar(u8* buf,u8 len, u8 bit, s32 *var){//获取位状态
	u8 sta, sf;
	u8 fc = buf[1];
	u8 numCh = buf[2];
	u8* p_bit = buf + 3;
	RET_STATUS ret = OPT_FAILURE;
	
	if(0x01 != fc || len < numCh + 3)
		return ret;
	
	sta = (bit >> 3);
	sf = bit & 0x07;
	
	//ValSet(var, (p_bit[sta] >> sf) & 0x01);
	*var = (p_bit[sta] >> sf) & 0x01;
	
	ret = OPT_SUCCESS;
	return ret;
}
/** @brief  读到变量 只读线圈
  * @param  buf     待分析字符串指针
            len     待分析字符串的长度
            bit     提取第几个线圈
            var     到哪好变量
  * @retval 
  * @note
  */
RET_STATUS ReadInCoilReqToVar(u8* buf,u8 len, u8 bit, s32 *var){//获取位状态
	u8 sta, sf;
	u8 fc = buf[1];
	u8 numCh = buf[2];
	u8* p_bit = buf + 3;
	RET_STATUS ret = OPT_FAILURE;
	
	if(0x02 != fc || len < numCh + 3)
		return ret;
	
	sta = (bit >> 3);
	sf = bit & 0x07;
	
	//ValSet(var, (p_bit[sta] >> sf) & 0x01);
	*var = (p_bit[sta] >> sf) & 0x01;
	
	ret = OPT_SUCCESS;
	return ret;
}
/** @brief  读到变量 寄存器
  * @param  buf     待分析字符串指针
            len     待分析字符串的长度
            reg     提取第几个寄存器
            var     到哪好变量
  * @retval 
  * @note
  */
RET_STATUS ReadRegReqToVar(u8* buf,u8 len, u8 reg, s32 *var){//获取位状态
	u8 fc = buf[1];
	u8 numCh = buf[2];
	u16* p_reg = (u16*)(buf + 3);
	RET_STATUS ret = OPT_FAILURE;
	s32 num;
	
	if(0x03 != fc || len < numCh + 3)
		return ret;
	
	if(numCh == 4)
	{
		num = ((ENDIAN_TF16U(p_reg[reg + 1]))<<16)| ENDIAN_TF16U(p_reg[reg]);
	}
	else if(numCh == 2)
	{
		num = ENDIAN_TF16U(p_reg[reg]);
	}
	else // 有些设备会回复numCH = 0
	{
		return OPT_SUCCESS;
	}
	
	//ValSet(var, num);
	*var = num;
	
	ret = OPT_SUCCESS;
	return ret;
}
/** @brief  读到变量 只读寄存器
  * @param  buf     待分析字符串指针
            len     待分析字符串的长度
            reg     提取第几个寄存器
            var     到哪好变量
  * @retval 
  * @note
  */
RET_STATUS ReadInRegReqToVar(u8* buf,u8 len, u8 reg, s32 *var){//获取位状态
	u8 fc = buf[1];
	u8 numCh = buf[2];
	u16* p_reg = (u16*)(buf + 3);
	RET_STATUS ret = OPT_FAILURE;
	
	if(0x04 != fc || len < numCh + 3)
		return ret;
	
	//ValSet(var, ENDIAN_TF16U(p_reg[reg]));
	*var = ENDIAN_TF16U(p_reg[reg]);
	
	ret = OPT_SUCCESS;
	return ret;
}
