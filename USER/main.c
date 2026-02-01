#include "stdio.h"
#include "delay.h"
#include "sys.h"
#include "malloc.h"
#include "MMC_SD.h"
#include "ff.h"
#include "exfuns.h"
#include "stm32f10x.h"
#include "stm32f10x_dma.h"
#include "stm32f10x_dac.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "math.h"
#include "string.h"

#include "delay.h"
#include "usart.h"
#include "led.h"
#include "sys.h"
#include "key.h"

#include "sim900a.h"
#include "usart2.h"

//#include "exti.h"

#define ADC_VREF 3.3

void errorLog(int num);
void parseGpsBuffer(void);
void printGpsBuffer(void);
// ADC
//char filename[] = "LOG.txt";  // ???
char data[] = "[UTC Time], [Latitude(North)], [Longitude(East)]\n";     // ?????

char dis_buff[60]={0};
char dis_buf2[15]={0};
char dis_buf3[10]={0};
char dis_buf4[10]={0};
//char hour[2] = {0,0};
char hour[] = "-";
// ?????SD?
void blink_led(){
		LED1 = 0;
		delay_ms(1000);
		LED1 = 1;
		delay_ms(1000);
}

void blink_led0(){
		LED0 = 0;
		delay_ms(1000);
		LED0 = 1;
		delay_ms(1000);
}

void WriteToSDCard(const char *filename, const char *data) {
    FATFS fs;
    FIL fil;
    UINT bw;
		printf("File Name: %s\r\n", filename);
    // Attempt to mount the SD card
    if (f_mount(&fs, "0:", 1) == FR_OK) {
			  
				//blink_led();
        //printf("SD card mounted successfully.\r\n");
        // Attempt to open the file
        if (f_open(&fil, filename, FA_WRITE | FA_OPEN_ALWAYS) == FR_OK) {
			  		//blink_led();
            //printf("File opened successfully: %s\r\n", filename);

            // Move to the end of the file to append data
            f_lseek(&fil, f_size(&fil));

            // Attempt to write data
            if (f_write(&fil, data, strlen(data), &bw) == FR_OK && bw == strlen(data)) {
								//blink_led();
                //printf("Data written successfully: %s\r\n", data);
            } else {
                //printf("File write failed!\r\n");
            }

            f_close(&fil);  // Close the file
        } else {
            //printf("File open failed!\r\n");
        }

        // Attempt to unmount the SD card
        if (f_mount(NULL, "", 1) == FR_OK) {
            //printf("SD card unmounted successfully.\r\n");
        } else {
            //printf("SD card unmount failed!\r\n");
        }
    } else {
        //printf("SD card mount failed!\r\n");
    }
}

// ???

void File_Init(){
	int res;
	printf("-----Power On-----\r\n");
	printf("Initializing File Name\r\n");
  
	res=1;           //sim800c配置
	while(res)
	{
		printf("Detecting GSM\r\n");
		res=GSM_Dect();
		delay_ms(2000);
	}	
	res=1;//获取时间
	
	res=1;//获取时间
	while(res)
	{
		printf("Connecting to Time Server\r\n");
		res=SIM900A_CONNECT_SERVER_SEND_INFOR((u8*)"time.nist.gov",(u8*)"13");
	}
	UART3SendString("1111111111\r\n",strlen("1111111111\r\n"));
	delay_ms(1000);	
	res=1;
	while(res)
	{		Flag_Rec_TCP=1;
		if(Flag_Rec_TCP==1)	//收到服务器下发数据
		{
			Flag_Rec_TCP=0;
			//UART3SendString("1111111111\r\n",strlen("1111111111\r\n"));	
			strcat(dis_buff,USART2_RX_BUF);
			strncpy(dis_buf3, dis_buff + 8 - 1, 8);
			strncpy(dis_buf4, dis_buff +16, 8);
			strcat(dis_buf2,dis_buf3);
			
			//hour[0] = dis_buf4[0];
			//hour[1] = dis_buf4[1];
			//printf("%s\r\n",hour);
			
			//strcat(dis_buf2,hour);
			strcat(dis_buf2,".txt");//存储文件名
			printf(":%s\r\n",dis_buf2);
			printf("%s\r\n",dis_buf4);

			
			memset(dis_buff,'\0',sizeof(dis_buff));
			
			memset(dis_buf3,'\0',sizeof(dis_buf3));
			res=0;

			//}
		}
	}
}

int main(void) {
    u8 t=0;
		u8 res=1;//Part of GPRS Setup
	  int mode = 0; //Defult to Manual
	  int press = 0;
	  LED_Init();
		uart_init(9600);	 //串口初始化为9600
	  
    // ?????
	  delay_init();    
    SystemInit();       // ???????
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); // ???????
	  blink_led0();
	  USART2_Init(115200);
	  USART3_Init(9600);
	  File_Init();
////////////////////////////////////////////////// GPRS Setup:
	  
	  USART2_Init(115200);	//初始化串口2 
	  USART3_Init(9600);	//LOG信息

	  res=1;
	  while(res)
	  { printf("Detecting GSM\r\n");	
		  res=GSM_Dect();
		  delay_ms(2000);
	  }	
	  res=1;
    //Attempt to Establish Connection    
	  while(res)
	  {
		 printf("Establishing Server Connection\r\n");
	   res=SIM900A_CONNECT_SERVER_SEND_INFOR((u8*)"<YOUR_LOGGING_SERVER_IP_ADDRESS>",(u8*)"<PORT>");
	   //res=SIM900A_CONNECT_SERVER_SEND_INFOR((u8*)"127.0.0.1",(u8*)"5000");
	  }
		res = 1;
		while(res){
			printf("Sending Test Message\r\n");
			res = SIM900A_GPRS_SEND_DATA((u8 *)"Test");
		}
////////////////////////////////////////////////////
		res = 1;    
	  KEY_Init();
	  clrStruct();
		
		blink_led();
		printf("dis_buf2 = %s/r/n", dis_buf2);
		WriteToSDCard(dis_buf2, data);
	  USART2_Init(115200);	//初始化串口2 
		blink_led();
		
	  printf("-Startup Complete-\r\n");
		//printf("[UTC Time], [Latitude(North)], [Longitude(East)]:\r\n");		
		LED0 = 0; //Red - Manual
	  LED1 = 1; //Green - Automatic
	  while(1)
	  {
		  t=KEY_Scan(0);
		
		  switch(t)
		  {				 
			  case KEY0_PRES://Manual Localization
				  mode = 0;
				  LED1 = 1;
				  LED0 = 0;
				  break;
			  case KEY1_PRES://Automatic Localization
				  mode = 1;
				  LED1 = 0;
				  LED0 = 1;
				  break;
			  case WKUP_PRES:				
				  press = 1;
				  break;
		 }
		 if(mode == 1){//In automatic mode
			 LED1 = 1;
			 //delay_ms(100);
			
			 //parseGpsBuffer();
		   printGpsBuffer();
			 
		   //res = SIM900A_GPRS_SEND_DATA((u8 *)Save_Data.GPS_Buffer);

			 LED1 = 0;
			 delay_ms(1000);
		 }
		 else if(press==1){//In manual mode and pressed
			 LED0 = 1;
			 press = 0;
			 //parseGpsBuffer();
		   
			 printGpsBuffer();

		   //res = SIM900A_GPRS_SEND_DATA((u8 *)Save_Data.GPS_Buffer);
			 
			 LED0 = 0;
		 }
		
		
		/*
		parseGpsBuffer();
		printGpsBuffer();
		*/
    
	}
}


void errorLog(int num)
{	
	while (1)
	{
	  	printf("ERROR%d\r\n",num);
	}
}

void parseGpsBuffer()
{
	char *subString;
	char *subStringNext;
	char i = 0;
	if (Save_Data.isGetData)
	{
		Save_Data.isGetData = false;
		printf("**************\r\n");
		printf(Save_Data.GPS_Buffer);

		
		for (i = 0 ; i <= 6 ; i++)
		{
			if (i == 0)
			{
				if ((subString = strstr(Save_Data.GPS_Buffer, ",")) == NULL)
					errorLog(1);	//解析错误
			}
			else
			{
				subString++;
				if ((subStringNext = strstr(subString, ",")) != NULL)
				{
					char usefullBuffer[2]; 
					switch(i)
					{
						case 1:memcpy(Save_Data.UTCTime, subString, subStringNext - subString);break;	//获取UTC时间
						case 2:memcpy(usefullBuffer, subString, subStringNext - subString);break;	//获取UTC时间
						case 3:memcpy(Save_Data.latitude, subString, subStringNext - subString);break;	//获取纬度信息
						case 4:memcpy(Save_Data.N_S, subString, subStringNext - subString);break;	//获取N/S
						case 5:memcpy(Save_Data.longitude, subString, subStringNext - subString);break;	//获取经度信息
						case 6:memcpy(Save_Data.E_W, subString, subStringNext - subString);break;	//获取E/W

						default:break;
					}

					subString = subStringNext;
					Save_Data.isParseData = true;
					if(usefullBuffer[0] == 'A')
						Save_Data.isUsefull = true;
					else if(usefullBuffer[0] == 'V')
						Save_Data.isUsefull = false;

				}
				else
				{
					errorLog(2);	//解析错误
				}
			}


		}
		
	}
}

void printGpsBuffer()
{
	  
    WriteToSDCard(dis_buf2, Save_Data.GPS_Buffer);
    USART2_Init(115200);	//初始化串口2 
	  SIM900A_GPRS_SEND_DATA((u8 *)Save_Data.GPS_Buffer);
		
	  
	/*
	if (Save_Data.isParseData)
	{
		Save_Data.isParseData = false;
		
		//printf(Save_Data.UTCTime);
		//printf(", ");
		snprintf(data, sizeof(data), Save_Data.UTCTime);
    WriteToSDCard(filename, data);
		SIM900A_GPRS_SEND_DATA(Save_Data.UTCTime);
		snprintf(data, sizeof(data), ", ");
    WriteToSDCard(filename, data);
    SIM900A_GPRS_SEND_DATA((u8*)", ");
		
		if(Save_Data.isUsefull)
		{
			Save_Data.isUsefull = false;

			//printf(Save_Data.latitude);
			//printf(", ");
		  snprintf(data, sizeof(data), Save_Data.latitude);
      WriteToSDCard(filename, data);
			SIM900A_GPRS_SEND_DATA(Save_Data.latitude);
		  snprintf(data, sizeof(data), ", ");
      WriteToSDCard(filename, data);

			//printf(Save_Data.longitude);
		  snprintf(data, sizeof(data), Save_Data.longitude);
      WriteToSDCard(filename, data);
			SIM900A_GPRS_SEND_DATA(Save_Data.longitude);
			//printf("\r\n");
		  snprintf(data, sizeof(data), "\r\n");
      WriteToSDCard(filename, data);
			SIM900A_GPRS_SEND_DATA((u8*)"\r\n");
		}
		else
		{
			printf("GPS DATA is not usefull!\r\n");
		}
		
	}
	*/
}





