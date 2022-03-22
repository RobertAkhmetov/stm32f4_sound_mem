/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************

  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "main_page.h"
#include "main_page_desactive.h"
#include "call_page.h"
#include "preview_page.h"
#include "112call_page.h"
//#include "callODS_page.h"
#include "concierge_page.h"
//#include "dispetcher_page.h"
#include "spi.h"
#include "gpio.h"
#include "flash.h"
#include "MY_CS43L22.h"
#include <math.h>
#include "stm32f4xx_hal_i2s.h"
#include "at45db161d.h" // spi2 память
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
I2C_HandleTypeDef hi2c1;
I2S_HandleTypeDef hi2s3;
DMA_HandleTypeDef hdma_spi3_tx;

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
RNG_HandleTypeDef hrng;
SPI_HandleTypeDef hspi1;
ADC_HandleTypeDef hadc1;

//добавили таймеры прерываний
TIM_HandleTypeDef htim6; //++++++++
uint8_t tim6_counter; //++++++
static void MX_TIM6_Init(void);//++++++++
static void MX_ADC1_Init(void);
#define PI 3.14159f

//Sample rate and Output freq
#define F_SAMPLE		48000.0f
#define F_OUT				1500.0f


/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_RNG_Init(void);
static void MX_SPI1_Init(void);

static void MX_DMA_Init(void);
static void MX_I2C1_Init(void);
static void MX_I2S3_Init(void);
/* USER CODE BEGIN PFP */

#define USE_EXTI
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/


/* USER CODE BEGIN 0 */
float mySinVal;
float sample_dt;
uint16_t sample_N;
uint16_t i_t;

uint32_t myDacVal;

int16_t dataI2S[100];


__STATIC_INLINE void DelayMicro(uint32_t __IO micros)
{
  micros *=(SystemCoreClock/1000000)/6;
  while(micros--);
}

/* USER CODE END 0 */

/*
  * @brief  The application entry point.
  */

void Draw_c112call_page ()
{
ILI9341_Draw_Image((const char*)c112call_page);
	CS_ACTIVE();
}
/*
void Draw_dispetcher_page ()
{
ILI9341_Draw_Image((const char*)dispetcher_page);
	CS_ACTIVE();
}
*/
void Draw_concierge_page ()
{
ILI9341_Draw_Image((const char*)concierge_page);
CS_ACTIVE();
}

/*
void Draw_callODS_page ()
{
ILI9341_Draw_Image((const char*)callODS_page);
}
*/
void Draw_main_page ()
{
 ILI9341_Draw_Image((const char*)main_page);
CS_ACTIVE();
}

void Draw_main_page_desactive ()
{
 ILI9341_Draw_Image((const char*)main_page_desactive);
	CS_ACTIVE();
}



void Draw_call_page ()
{
 ILI9341_Draw_Image((const char*)call_page);
CS_ACTIVE();
}

void FlatCall()
{
	int i=0;
	for(i;i<4;i++)
	{	
		
	int j=0;	
	while(j<50)
	{
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET);
		DelayMicro(744);
		
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET);
		DelayMicro(756);
		
	 j++;
	}
	
	j=0;
	
	while(j<50)
	{
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET);
		DelayMicro(604);
		
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET);
		DelayMicro(592);
		
	 j++;
	}
	j=0;
	
	}
	
}

/*-------------------------------------------
								Sys Sound Zone
-------------------------------------------*/
void ClickSound()
{
	for(int i=0;i<40;i++)
			{
				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_SET);
				DelayMicro(505);
				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_RESET);
				DelayMicro(505);
			}
}

void ResetSound()
{
	for(int i=0;i<40;i++)
			{
				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_SET);
				DelayMicro(290);
				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_RESET);
				DelayMicro(290);
			}
	
}

void OpenDoorSound()
{
	for(int i=0;i<14000;i++)
			{
				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_SET);
				DelayMicro(220);
				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_RESET);
				DelayMicro(220);
			}
}

void CallingSound()
{
	for(int j=0;j<2;j++)
	{
		for(int i=0;i<400;i++)
			{
				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_SET);
				DelayMicro(800);
				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_RESET);
				DelayMicro(800);
			}
		for(int i=0;i<500;i++)
			{
				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_SET);
				DelayMicro(1100);
				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_RESET);
				DelayMicro(1100);
			}
		DelayMicro(900000);
	}
}

void FailedOperationSound()
{
	for(int i=0;i<400;i++)
			{
				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_SET);
				DelayMicro(675);
				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_RESET);
				DelayMicro(675);
			}
			
	for(int i=0;i<400;i++)
			{
				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_SET);
				DelayMicro(805);
				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_RESET);
				DelayMicro(805);
			}
			
	for(int i=0;i<400;i++)
			{
				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_SET);
				DelayMicro(940);
				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_RESET);
				DelayMicro(940);
			}
	
}


/* Tube Zone */

void TubeStateControlCheck()
{
uint16_t adc = 0;	
	
while (1)
  {
					
          HAL_ADC_Start(&hadc1); // запускаем преобразование сигнала АЦП
          HAL_ADC_PollForConversion(&hadc1, 500); // ожидаем окончания преобразования
					DelayMicro(100000);
          adc = HAL_ADC_GetValue(&hadc1); // читаем полученное значение в переменную adc
         // HAL_ADC_Stop(&hadc1); // останавливаем АЦП (не обязательно)
					
					
					if (adc<100) //НЕТ НА ЛИНИИ
					{
						HAL_GPIO_WritePin(GPIOD,GPIO_PIN_13,GPIO_PIN_SET); //зажигаем ОРАНЖЕВЫЙ светодиод
					}
					
					if (adc>100 && adc<1000) //НА ЛИНИИ, ТРУБКА НЕ СНЯТА
					{
						HAL_GPIO_WritePin(GPIOD,GPIO_PIN_12,GPIO_PIN_SET); //зажигаем ЗЕЛЕНЫЙ светодиод
					}
					else if(adc>1000 && adc<2495) //ТРУБКА СНЯТА
					{
						HAL_GPIO_WritePin(GPIOD,GPIO_PIN_14,GPIO_PIN_SET); //зажигаем КРАСНЫЙ светодиод	
					}
					else if(adc>2495)//КНОПКА НАЖАТА
					{
						HAL_GPIO_WritePin(GPIOD,GPIO_PIN_15,GPIO_PIN_SET); //зажигаем СИНИЙ светодиод	
						
					}
					
  }
}


int main(void)
{
  sample_dt = F_OUT/F_SAMPLE;
	sample_N = F_SAMPLE/F_OUT;
	
	
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
	MX_GPIO_Init1();
	
	MX_DMA_Init();
  MX_I2C1_Init();
  MX_I2S3_Init();
	MX_ADC1_Init();
	
  MX_RNG_Init();
  MX_SPI1_Init();
	MX_SPI2_Init();
	
	//инициализируем таймеры
	MX_TIM6_Init();//++++++
	
	HAL_GPIO_WritePin(FLASH_RES_GPIO_Port, FLASH_RES_Pin, GPIO_PIN_RESET); // Сброс чипа
	HAL_GPIO_WritePin(FLASH_RES_GPIO_Port, FLASH_RES_Pin, GPIO_PIN_SET);
	
	
	CS43_Init(hi2c1, MODE_I2S);
	CS43_SetVolume(40); //0 - 100,, 40
	CS43_Enable_RightLeft(CS43_RIGHT_LEFT);
	CS43_Start();
	
	
	//Build Sine wave
	for(uint16_t i=0; i<sample_N; i++)
	{
		mySinVal = sinf(i*2*PI*sample_dt);
		dataI2S[i*2] = (mySinVal )*8000;    //Right data (0 2 4 6 8 10 12)
		dataI2S[i*2 + 1] =(mySinVal )*8000; //Left data  (1 3 5 7 9 11 13)
	}
	
	//HAL_I2S_Transmit_DMA(&hi2s3, (uint16_t *)dataI2S, sample_N*2); //ggg
	
//	//Start DAC
//	HAL_DAC_Start(&hdac, DAC_CHANNEL_1);
//	//Start TIM2
//	HAL_TIM_Base_Start_IT(&htim2);
	

	TFT9341_ini(320,240); // инициализируем дисплей 240х320
	TFT9341_SetRotation(3); // устанавливаем горизонтальную ориентацию для домофона
	
	
	uint8_t buff[10];
	uint8_t byte;
	
	
	uint16_t page=0; uint16_t offset=0; uint32_t razmer=0; uint32_t temp=0;
	
	while(1)
	{
		AT45_ReadMainMemoryPage( page, offset);
		HAL_SPI_Receive(&hspi2, &byte, 1, 100);	buff[0] = byte;
		
		if (buff[0]==preview_page[0])
		{
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_SET);
		}
	}
	
	
	
	
	ILI9341_Draw_Image((const char*)preview_page);
	
	
	
	//HAL_ADCEx_Calibration_Start(&hadc1);
	
	TubeStateControlCheck();
	
	 
	
	
	DelayMicro(2000000); //10^-6 сек
	
	/* Выведем текст на дисплей  */
	CS_ACTIVE(); //команды типа TFT9341_  FillScreen,StringRus,String и тп требуют CS 0, чтобы вывестись 
	//TFT9341_FillScreen(TFT9341_WHITE);
	//TFT9341_StringRus(45,200,TFT9341_WHITE,TFT9341_BLUE,"Инициализация"); //Русская надпись выведится поверх изображения 
	// (х,у, цвет букв, цвет фона, содержимое)
	
	
	// -----------------------------------------------
	
	//DelayMicro(2000000); //10^-6 сек
	
	Draw_main_page_desactive();
	
	//+++++
	HAL_TIM_Base_Start(&htim6);
	HAL_TIM_Base_Start_IT(&htim6); //запуск таймвера
	tim6_counter=0;
	
	
	//HAL_TIM_Base_Stop(&htim6);
	//HAL_TIM_Base_Stop_IT(&htim6);
	
	
	
	CheckFirstButtonPressed ();

	Draw_main_page ();
	
	while (1)
	{
		
		
		DelayMicro(500000);
		keyboard_default_waiting();
	}

}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
	RCC_PeriphCLKInitTypeDef PeriphClkInitStruct;

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
	
	PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_I2S;
  PeriphClkInitStruct.PLLI2S.PLLI2SN = 50;
  PeriphClkInitStruct.PLLI2S.PLLI2SR = 2;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
	
	 HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);

}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */

static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */
  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_8;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/* I2C1 init function */
static void MX_I2C1_Init(void)
{

  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  /*if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
	*/

}

/* I2S3 init function */
static void MX_I2S3_Init(void)
{

  hi2s3.Instance = SPI3;
  hi2s3.Init.Mode = I2S_MODE_MASTER_TX;
  hi2s3.Init.Standard = I2S_STANDARD_PHILIPS;
  hi2s3.Init.DataFormat = I2S_DATAFORMAT_16B;
  hi2s3.Init.MCLKOutput = I2S_MCLKOUTPUT_ENABLE;
  hi2s3.Init.AudioFreq = I2S_AUDIOFREQ_48K;
  hi2s3.Init.CPOL = I2S_CPOL_LOW;
  hi2s3.Init.ClockSource = I2S_CLOCK_PLL;
  hi2s3.Init.FullDuplexMode = I2S_FULLDUPLEXMODE_DISABLE;
  /*if (HAL_I2S_Init(&hi2s3) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
	*/
}

/** 
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void) 
{
  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Stream5_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);

}

/**
  * @brief RNG Initialization Function
  * @param None
  * @retval None
  */
static void MX_RNG_Init(void)
{

  /* USER CODE BEGIN RNG_Init 0 */

  /* USER CODE END RNG_Init 0 */

  /* USER CODE BEGIN RNG_Init 1 */

  /* USER CODE END RNG_Init 1 */
  hrng.Instance = RNG;
  if (HAL_RNG_Init(&hrng) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RNG_Init 2 */

  /* USER CODE END RNG_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM6_Init(void) //++++++++++++++
{

	__TIM6_CLK_ENABLE();
  /* USER CODE BEGIN TIM6_Init 0 */

  /* USER CODE END TIM6_Init 0 */

   TIM_MasterConfigTypeDef sMasterConfig;

  /* USER CODE BEGIN TIM6_Init 1 */

  /* USER CODE END TIM6_Init 1 */
  htim6.Instance = TIM6;
  htim6.Init.Prescaler = 15999;
  htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim6.Init.Period = 499;
  htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim6) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM6_Init 2 */

  /* USER CODE END TIM6_Init 2 */
	//активируем прерывания таймера
	HAL_NVIC_SetPriority(TIM6_DAC_IRQn,0,0);
	HAL_NVIC_EnableIRQ(TIM6_DAC_IRQn);
}



static void MX_SPI1_Init(void)
{
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 7;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }

}

/**
  * @brief GPIO Initialization Function
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  
  __HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOH_CLK_ENABLE();
  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4, GPIO_PIN_RESET);

  /*Configure GPIO pins : PA2 PA3 PA4 */
  GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	//Конфигурируем GPIOD для клавиатуры
	
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4, GPIO_PIN_RESET);
	GPIO_InitStruct.Pin = GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
	
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_8, GPIO_PIN_RESET);
	GPIO_InitStruct.Pin = GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT; //ранее под прерывания GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct); 
	
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_0, GPIO_PIN_RESET);
	GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
	
	 /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15 
                          |GPIO_PIN_4, GPIO_PIN_RESET);

  /*Configure GPIO pins : PD12 PD13 PD14 PD15 
                           PD4 */
  GPIO_InitStruct.Pin = GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15 
                          |GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
	
	HAL_NVIC_SetPriority(EXTI0_IRQn,0,0);
	HAL_NVIC_EnableIRQ(EXTI0_IRQn);
	
	//активируем прерывания таймера
	HAL_NVIC_SetPriority(TIM6_DAC_IRQn,0,0);
	HAL_NVIC_EnableIRQ(TIM6_DAC_IRQn);
	
	//конфигурируем gpioC для трубки и системных звуков
	 
	GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_7;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	

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


void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	main();
}


void HAL_TIM_IC_Callback(TIM_HandleTypeDef *htim)
{
	main();
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

