#include "Int_SI24R1.h"
#include "Com_debug.h"

uint8_t TX_ADDRESS[TX_ADR_WIDTH] = {0x0A,0x01,0x08,0x0E,0x01};  // 定义一个静态发送地址
//测试用数据存储
uint8_t TEST_BUFFER[5]= {0};  

//使用hal库函数读取一个字节
static uint8_t SPI_RW(uint8_t byte)
{
	uint8_t rx_data;
	HAL_SPI_TransmitReceive(&hspi1, &byte, &rx_data, 1, 1000);
	return rx_data;
}

uint8_t SI24R1_Test_Error(void)
{
	//SI24R1需要先读取一次，保证SPI通信正常才能写入
	//读取发送地址寄存器的值，验证SPI通信是否正常
	SI24R1_Read_Buf(SI24R1_READ_REG + TX_ADDR, TEST_BUFFER, TX_ADR_WIDTH);
	//测试SPI通信能否读写寄存器
	SI24R1_Write_Buf(SI24R1_WRITE_REG + TX_ADDR, TX_ADDRESS, TX_ADR_WIDTH);     // 写入发送地址
	//读取发送地址寄存器的值，验证SPI通信是否正常
	SI24R1_Read_Buf(SI24R1_READ_REG + TX_ADDR, TEST_BUFFER, TX_ADR_WIDTH);
	//计算是否正确写入寄存器
	for(int i = 0; i < TX_ADR_WIDTH; i++) {
		if(TEST_BUFFER[i] != TX_ADDRESS[i]) {
			return 1;
		}
	}
	return 0;
}

/********************************************************
函数功能：SI24R1引脚初始化                
入口参数：无
返回  值：无
*********************************************************/
void SI24R1_Init(void)
{
	//上电后等待SI24R1稳定，至少等待200ms
	HAL_Delay(200);
	//测试直到SPI通信正常，如果通信异常则每50ms重试一次，直到通信正常
	while(SI24R1_Test_Error()!= 0) {
		LOG_DEBUG("SI24R1 communication error, retrying...\n");
		HAL_Delay(50);
	}

	//默认接收模式，发送时切换到发送模式，发送完成后切换回接收模式
	SI24R1_RX_Mode();
	LOG_DEBUG("SI24R1 communication success\n");
}

/********************************************************
函数功能：写寄存器的值（单字节）                
入口参数：reg:寄存器映射地址（格式：SI24R1_WRITE_REG｜reg）
					value:寄存器的值
返回  值：状态寄存器的值
*********************************************************/
uint8_t SI24R1_Write_Reg(uint8_t reg, uint8_t value)
{
	uint8_t status;

	CS_LOW();               
	status = SPI_RW(reg);				
	SPI_RW(value);
	CS_HIGH();  
	
	return(status);
}


/********************************************************
函数功能：写寄存器的值（多字节）                  
入口参数：reg:寄存器映射地址（格式：SI24R1_WRITE_REG｜reg）
					pBuf:写数据首地址
					bytes:写数据字节数
返回  值：状态寄存器的值
*********************************************************/
uint8_t SI24R1_Write_Buf(uint8_t reg, const uint8_t *pBuf, uint8_t bytes)
{
	uint8_t status,byte_ctr;

  CS_LOW();                                  			
  status = SPI_RW(reg);                          
  for(byte_ctr=0; byte_ctr<bytes; byte_ctr++)     
    SPI_RW(*pBuf++);
  CS_HIGH();                                      	

  return(status);       
}							  					   


/********************************************************
函数功能：读取寄存器的值（单字节）                  
入口参数：reg:寄存器映射地址（格式：SI24R1_READ_REG｜reg）
返回  值：寄存器值
*********************************************************/
uint8_t SI24R1_Read_Reg(uint8_t reg)
{
 	uint8_t value;

	CS_LOW();    
	SPI_RW(reg);			
	value = SPI_RW(0);
	CS_HIGH();              

	return(value);
}


/********************************************************
函数功能：读取寄存器的值（多字节）                  
入口参数：reg:寄存器映射地址（SI24R1_READ_REG｜reg）
					pBuf:接收缓冲区的首地址
					bytes:读取字节数
返回  值：状态寄存器的值
*********************************************************/
uint8_t SI24R1_Read_Buf(uint8_t reg, uint8_t *pBuf, uint8_t bytes)
{
	uint8_t status,byte_ctr;

  CS_LOW();                                        
  status = SPI_RW(reg);                           
  for(byte_ctr=0;byte_ctr<bytes;byte_ctr++)
    pBuf[byte_ctr] = SPI_RW(0);                   //读取数据，低字节在前
  CS_HIGH();                                        

  return(status);    
}


/********************************************************
函数功能：SI24R1接收模式初始化                      
入口参数：无
返回  值：无
*********************************************************/
void SI24R1_RX_Mode(void)
{
	CE_LOW();  //拉低CE引脚，使能SI24R1配置
	SI24R1_Write_Buf(SI24R1_WRITE_REG + RX_ADDR_P0, TX_ADDRESS, TX_ADR_WIDTH);	// 接收设备接收通道0使用和发送设备相同的发送地址
	SI24R1_Write_Reg(SI24R1_WRITE_REG + EN_AA, 0x01);               			// 使能接收通道0自动应答
	SI24R1_Write_Reg(SI24R1_WRITE_REG + EN_RXADDR, 0x01);           			// 使能接收通道0
	SI24R1_Write_Reg(SI24R1_WRITE_REG + RF_CH, RF_CHANNEL);                 	// 选择射频通道0x40
	SI24R1_Write_Reg(SI24R1_WRITE_REG + RX_PW_P0, TX_PLOAD_WIDTH);  			// 接收通道0选择和发送通道相同有效数据宽度
	SI24R1_Write_Reg(SI24R1_WRITE_REG + RF_SETUP, 0x06);            			// 数据传输率1Mbps，发射功率4dBm
	SI24R1_Write_Reg(SI24R1_WRITE_REG + CONFIG, 0x0f);              			// CRC使能，16位CRC校验，上电，接收模式
	SI24R1_Write_Reg(SI24R1_WRITE_REG + STATUS, 0xff);  						// 清除所有的中断标志位
	CE_HIGH();                                        							// 拉高CE启动接收设备
}						


/********************************************************
函数功能：SI24R1发送模式初始化                      
入口参数：无
返回  值：无
*********************************************************/
void SI24R1_TX_Mode(void)
{
	CE_LOW();  //拉低CE引脚
	SI24R1_Write_Buf(SI24R1_WRITE_REG + TX_ADDR, TX_ADDRESS, TX_ADR_WIDTH);     // 写入发送地址
	SI24R1_Write_Buf(SI24R1_WRITE_REG + RX_ADDR_P0, TX_ADDRESS, TX_ADR_WIDTH);  // 为了应答接收设备，接收通道0地址和发送地址相同
	SI24R1_Write_Reg(SI24R1_WRITE_REG + EN_AA, 0x01);       					// 使能接收通道0自动应答
	SI24R1_Write_Reg(SI24R1_WRITE_REG + EN_RXADDR, 0x01);   					// 使能接收通道0
	SI24R1_Write_Reg(SI24R1_WRITE_REG + SETUP_RETR, 0x0a);  					// 自动重发延时等待250us+86us，自动重发10次
	SI24R1_Write_Reg(SI24R1_WRITE_REG + RF_CH, RF_CHANNEL);                 	// 选择射频通道
	SI24R1_Write_Reg(SI24R1_WRITE_REG + RF_SETUP, 0x06);    					// 数据传输率1Mbps，发射功率4dBm
	SI24R1_Write_Reg(SI24R1_WRITE_REG + CONFIG, 0x0e);      					// CRC使能，16位CRC校验，上电
	CE_HIGH();                                        							// 拉高CE启动发送设备
}


/********************************************************
函数功能：读取接收数据                       
入口参数：rxbuf:接收数据存放首地址
返回  值：0:接收到数据
          1:没有接收到数据
*********************************************************/
uint8_t SI24R1_RxPacket(uint8_t *rxbuf)
{
	uint8_t state;
	state = SI24R1_Read_Reg(SI24R1_READ_REG + STATUS);  				//读取状态寄存器的值    	  
	SI24R1_Write_Reg(SI24R1_WRITE_REG + STATUS, state);               	//再写回去，写1清除RX_DS中断标志
	if(state & RX_DR)								                  	//接收到数据
	{
		SI24R1_Read_Buf(SI24R1_RD_RX_PLOAD,rxbuf,TX_PLOAD_WIDTH);     	//读取数据
		SI24R1_Write_Reg(SI24R1_FLUSH_RX,0xff);					      	//清除RX FIFO寄存器
		return 0; 
	}	   
	return 1;                                            			 	//没收到任何数据
}


/********************************************************
函数功能：	发送一个数据包                      
入口参数：	txbuf:要发送的数据
返回值：	0x10:达到最大重发次数，发送失败 
          	0x20:发送成功            
          	0xff:发送失败                  
*********************************************************/
uint8_t SI24R1_TxPacket(uint8_t *txbuf)
{
	uint8_t state;
	CE_LOW();															//CE拉低，使能SI24R1配置
  	SI24R1_Write_Buf(SI24R1_WR_TX_PLOAD, txbuf, TX_PLOAD_WIDTH);	    //写数据到TX FIFO,32个字节
 	CE_HIGH();															//CE置高，使能发送	   
	/*不使用中断等待发送完成
	while(IRQ == 1);
	*/																	
	state = SI24R1_Read_Reg(STATUS);  									//读取状态寄存器的值	   
	while((state & TX_DS) == 0 && (state & MAX_RT) == 0)					//等待发送完成或达到最大重发次数
	{
		state = SI24R1_Read_Reg(STATUS);  									//读取状态寄存器的值
	}
	SI24R1_Write_Reg(SI24R1_WRITE_REG+STATUS, state); 					//清除TX_DS或MAX_RT中断标志
	if(state&MAX_RT)													//达到最大重发次数
	{
		SI24R1_Write_Reg(SI24R1_FLUSH_TX,0xff);							//清除TX FIFO寄存器 
		return MAX_RT; 
	}
	if(state&TX_DS)														//发送完成
	{
		return TX_DS;
	}
	return 0XFF;														//发送失败
}
