/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "fatfs.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "sys.h"
#include "usart.h"
#include "lcd.h"
#include "malloc.h"
#include "MMC_SD.h"
#include "exfuns.h"
#include "piclib.h"
#include "remote.h"
#include "fattester.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint8_t pic[10][40];
uint8_t pnum = 0;
uint16_t index = 0;
extern UART_HandleTypeDef huart1;
extern uint8_t rxBuffer[20];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
u16 pic_get_tnum(u8 *path)
{
	u8 res;
	u16 rval=0;
 	DIR tdir;
	FILINFO *tfileinfo;
	tfileinfo=(FILINFO*)mymalloc(sizeof(FILINFO));
    res=f_opendir(&tdir,(const TCHAR*)path);
	if(res==FR_OK&&tfileinfo)
	{
		while(1)
		{
	        res=f_readdir(&tdir,tfileinfo);
	        if(res!=FR_OK||tfileinfo->fname[0]==0)break;
			res=f_typetell((u8*)tfileinfo->fname);
			if((res&0XF0)==0X50)
			{
				rval++;
			}
		}
	}
	myfree(tfileinfo);
	return rval;
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_SPI1_Init();
  MX_USART1_UART_Init();
  MX_FATFS_Init();
  MX_TIM5_Init();
  /* USER CODE BEGIN 2 */
  HAL_UART_Receive_IT(&huart1, (uint8_t *)rxBuffer, 1);
  LCD_Init();
  mem_init();
  exfuns_init();
  Remote_Init();

  uint8_t res;
  DIR picdir;
  uint16_t totpicnum;
  FILINFO *picfileinfo;
  uint8_t *pname;
  uint32_t *picoffsettbl;
  uint16_t curindex;
  uint16_t temp;
  uint8_t key;

  FRESULT fr = f_mount(fs[0], "0:", 1);
  if(fr == FR_OK){
      LCD_ShowString(30,170,240,16,16,"mount ok");
  }else if(fr == FR_NOT_READY) {
  	  LCD_ShowString(30,170,240,16,16,"not ready error");
  }
  while(f_opendir(&picdir, "0:/PICTURE")){
  		LCD_ShowString(30,40,240,16,16,"directory wrong!");
  		HAL_Delay(200);
  		LCD_Fill(30,40,240,56, WHITE);
  		HAL_Delay(200);
  }
  LCD_ShowString(30,40,240,16,16,"directory ok");
  totpicnum = pic_get_tnum("0:/PICTURE");
  while(totpicnum == NULL){
  		LCD_ShowString(30,60,240,16,16,"no picture!");
  		HAL_Delay(200);
  		LCD_Fill(30,60,240,76,WHITE);
  		HAL_Delay(200);
  }
  LCD_ShowString(30,60,240,16,16,"picture ok");

  picfileinfo = (FILINFO*)mymalloc(sizeof(FILINFO));
  pname = mymalloc(_MAX_LFN*2+1);
  picoffsettbl = mymalloc(4*totpicnum);

  while(!picfileinfo||!pname||!picoffsettbl){
  	  LCD_ShowString(30,80,240,16,16,"malloc fail!");
  	  HAL_Delay(200);
      LCD_Fill(30,80,240,96,WHITE);
  	  HAL_Delay(200);;
  }
  LCD_ShowString(30,80,240,16,16,"malloc ok");

  res = f_opendir(&picdir, "0:/PICTURE");
  if(res==FR_OK){
      curindex = 0;
  	  while(1){
  		  //temp = picdir.dptr;
  		  temp = picdir.index;
  	      res = f_readdir(&picdir, picfileinfo);
  	      if(res != FR_OK || picfileinfo->fname[0] == 0) break;
  		  res = f_typetell((u8*)picfileinfo->fname);
  		  if((res&0XF0)==0X50){
  		      picoffsettbl[curindex] = temp;
  			  curindex++;
  		  }
  	  }
  }
  piclib_init();
  curindex = 0;
  res = f_opendir(&picdir,(const TCHAR*)"0:/PICTURE");

  mf_scan_files((const TCHAR*)"0:/PICTURE");
  /*LCD_Clear(WHITE);
  LCD_ShowString(10,100,260,16,16,pic[0]);
  LCD_ShowString(10,120,260,16,16,pic[1]);
  LCD_ShowString(10,140,260,16,16,pic[2]);*/

  /*while(1){
	  LCD_Clear(BLACK);
	  ai_load_picfile(p[curindex], 0, 0, lcddev.width, lcddev.height, 1);
	  LCD_ShowString(2,2,lcddev.width,16,16,p[curindex]);
	  curindex = (curindex + 1) %3;
	  HAL_Delay(1000);
  }*/

  LCD_Clear(WHITE);
  LCD_ShowString(2,2,lcddev.width,16,16,pic[curindex]);
  char pn[20];
  switch(pnum){
  case 0: strcpy(pn, "number: 0"); break;
  case 1: strcpy(pn, "number: 1"); break;
  case 2: strcpy(pn, "number: 2"); break;
  case 3: strcpy(pn, "number: 3"); break;
  case 4: strcpy(pn, "number: 4"); break;
  case 5: strcpy(pn, "number: 5"); break;
  case 6: strcpy(pn, "number: 6"); break;
  case 7: strcpy(pn, "number: 7"); break;
  case 8: strcpy(pn, "number: 8"); break;
  case 9: strcpy(pn, "number: 9"); break;
  }
  LCD_ShowString(2,300,lcddev.width,16,16,pn);
  ai_load_picfile(pic[curindex], 0, 0, lcddev.width, lcddev.height, 1);
  //LCD_ShowString(30,130,200,16,16,"KEYVAL:");
  //LCD_ShowString(30,150,200,16,16,"KEYCNT:");
  while(1){
	  key=Remote_Scan();
	  HAL_Delay(1000);
	  //LCD_ShowNum(86,130,key,3,16);
	  //LCD_ShowNum(86,150,RmtCnt,3,16);
	  if(key == 194){
		  curindex = (curindex + 1) % pnum;
		  index = (index + 1) % pnum;
		  LCD_Clear(WHITE);
		  LCD_ShowString(2,300,lcddev.width,16,16,pn);
		  //LCD_ShowString(2,2,lcddev.width,16,16,pic[curindex]);
		  LCD_ShowString(2,2,lcddev.width,16,16,pic[index]);
		  //ai_load_picfile(pic[curindex], 0, 0, lcddev.width, lcddev.height, 1);
		  ai_load_picfile(pic[index], 0, 0, lcddev.width, lcddev.height, 1);
	  }
	  if(key == 34){
		  curindex = (curindex + pnum - 1) % pnum;
		  index = (index + pnum - 1) % pnum;
		  LCD_Clear(WHITE);
		  LCD_ShowString(2,300,lcddev.width,16,16,pn);
		  //LCD_ShowString(2,2,lcddev.width,16,16,pic[curindex]);
		  LCD_ShowString(2,2,lcddev.width,16,16,pic[index]);
		  //ai_load_picfile(pic[curindex], 0, 0, lcddev.width, lcddev.height, 1);
		  ai_load_picfile(pic[index], 0, 0, lcddev.width, lcddev.height, 1);
	  }
	  /*LCD_Clear(WHITE);
	  LCD_ShowString(2,300,lcddev.width,16,16,pn);
	  LCD_ShowString(2,2,lcddev.width,16,16,pic[curindex]);
	  ai_load_picfile(pic[curindex], 0, 0, lcddev.width, lcddev.height, 1);*/
  }




  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
