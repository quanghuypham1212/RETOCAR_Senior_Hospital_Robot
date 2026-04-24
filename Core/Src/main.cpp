/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "usb_device.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#define PCA9685_MODE1 0x00
#define PCA9685_PRESCALE 0xFE
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
ADC_HandleTypeDef hadc1;

I2C_HandleTypeDef hi2c1;

SPI_HandleTypeDef hspi1;

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
uint8_t pca_addr = 0xAA;
HAL_StatusTypeDef test_status;
uint32_t encoder1_count = 0 ;
uint32_t encoder2_count = 0 ;
uint8_t btn_rst = 1;
uint8_t btn_conf = 1;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC1_Init(void);
static void MX_I2C1_Init(void);
static void MX_SPI1_Init(void);
static void MX_TIM1_Init(void);
static void MX_TIM3_Init(void);
static void MX_TIM4_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */
void TFT_WriteCommand(uint8_t cmd);
void TFT_WriteData(uint8_t data);
void PCA9685_Init_50Hz(void);
void PCA9685_SetServoAngle(uint8_t channel, uint16_t value);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void PCA9685_Init_50Hz(void) {
    uint8_t data;

    // 1. Reset chip hoàn toàn
    data = 0x80;
    HAL_I2C_Mem_Write(&hi2c1, 0x80, 0x00, 1, &data, 1, 100);
    HAL_Delay(50);

    // 2. Cho chip ngủ để cài Prescale
    data = 0x10;
    HAL_I2C_Mem_Write(&hi2c1, 0x80, 0x00, 1, &data, 1, 100);

    // 3. Cài 50Hz (Prescale = 121)
    data = 121;
    HAL_I2C_Mem_Write(&hi2c1, 0x80, 0xFE, 1, &data, 1, 100);

    // 4. Thức dậy và cho phép Restart
    data = 0xA1; // 0xA1 = 1010 0001 (Restart + Auto-Increment + AllCall)
    HAL_I2C_Mem_Write(&hi2c1, 0x80, 0x00, 1, &data, 1, 100);
    HAL_Delay(5);
}

void PCA9685_SetServoAngle(uint8_t channel, uint16_t value) {
    // channel: 0 đến 15
    // value: 102 (0 độ) đến 512 (180 độ)

    uint8_t buf[4];
    buf[0] = 0x00; // ON Low (Bật ở vạch 0)
    buf[1] = 0x00; // ON High
    buf[2] = value & 0xFF; // OFF Low (Tắt ở vạch 'value')
    buf[3] = value >> 8;   // OFF High

    // Ghi vào địa chỉ thanh ghi của chân tương ứng
    // Công thức: 0x06 (LED0) + 4 * số kênh
    HAL_I2C_Mem_Write(&hi2c1,(0x40 << 1), 0x06 + (4 * channel), 1, buf, 4, 100);
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
  MX_ADC1_Init();
  MX_I2C1_Init();
  MX_SPI1_Init();
  MX_TIM1_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_USB_DEVICE_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  // Start Encoders
    HAL_TIM_Encoder_Start(&htim1, TIM_CHANNEL_1);
    HAL_TIM_Encoder_Start(&htim1, TIM_CHANNEL_2);
    HAL_TIM_Encoder_Start(&htim4, TIM_CHANNEL_1);
    HAL_TIM_Encoder_Start(&htim4, TIM_CHANNEL_2);
    // Start PWM
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
    // Đọc địa chỉ I2C
//    for (uint16_t i = 0; i < 128; i++) {
//         test_status = HAL_I2C_IsDeviceReady(&hi2c1, (i << 1), 3, 10);
//         if (test_status == HAL_OK) {
//             pca_addr = i;
//         }
//     }

//    PCA9685_SetServoAngle(0, 512);
//    PCA9685_SetServoAngle(1, 512);
//    PCA9685_SetServoAngle(2, 512);
//    PCA9685_SetServoAngle(3, 512);
//    HAL_Delay(2000);
//    PCA9685_SetServoAngle(0, 307);
//    PCA9685_SetServoAngle(1, 307);
//    PCA9685_SetServoAngle(2, 307);
//    PCA9685_SetServoAngle(3, 307);
//    HAL_Delay(2000);
//    PCA9685_SetServoAngle(0, 102);
//    PCA9685_SetServoAngle(1, 102);
//    PCA9685_SetServoAngle(2, 102);
//    PCA9685_SetServoAngle(3, 102);
   // Đọc encoder
    encoder1_count = (uint32_t)__HAL_TIM_GET_COUNTER(&htim1);
    encoder2_count = __HAL_TIM_GET_COUNTER(&htim4);


     //2. Thiết lập hướng quay TIẾN cho cả 2 động cơ
     //Động cơ A (PB12, PB13)
//    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);
//    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET);
    // Động cơ B (PB14, PB15)
//    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);
//    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_RESET);

    // 3. Vòng lặp tăng tốc dần từ 0% đến 100% (mỗi bước 10%)
//    for (int i = 0; i <= 10; i++) {
//        uint32_t duty_value = (i * 65535) / 10; // Tính toán giá trị PWM

//        __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 65535);
        //__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 65535);
//
//        //HAL_Delay(1000); // Đợi 1 giây ở mỗi mức tốc độ
//    //}
//
    // 4. Giữ tốc độ tối đa trong 2 giây
//    HAL_Delay(5000);
//
//    // 5. DỪNG HẲN CẢ 2 ĐỘNG CƠ
//    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 0);
//    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 0);
//
//    // Đưa tất cả chân hướng về 0 (Thả trôi bánh xe)
//    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15, GPIO_PIN_RESET);
//
//    // Tắt PWM để tiết kiệm năng lượng
//    HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_1);
//    HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_2);

    // Test màn hình
    /* USER CODE BEGIN 2 */
    HAL_Delay(500);

        // Đảm bảo các chân ở trạng thái nghỉ
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, 1); // CS = 1 (Tạm ngắt)
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, 1);  // RES = 1
        HAL_Delay(100);

        // Bắt đầu quy trình Reset cứng
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, 0); // RES = 0 (Bắt đầu Reset)
        HAL_Delay(200);                          // Giữ 200ms cho chắc
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, 1); // RES = 1 (Thả Reset)
        HAL_Delay(200);                          // Đợi chip màn hình ổn định

        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, 0); // Bây giờ mới CS = 0 để làm việc

        // Gửi lệnh Software Reset để "làm sạch" nội bộ màn hình
        TFT_WriteCommand(0x01);
        HAL_Delay(150);

    TFT_WriteCommand(0x11); // Exit Sleep
    HAL_Delay(200);

    // Thiết lập chế độ màu 16-bit (5-6-5)
    TFT_WriteCommand(0x3A);
    TFT_WriteData(0x05);

    // Cấu hình hướng quét (Quan trọng để không bị sọc)
    TFT_WriteCommand(0x36);
    TFT_WriteData(0xC0);

    // Bật màn hình
    TFT_WriteCommand(0x29);
    HAL_Delay(100);

    // 3. XÓA NHIỄU - Đổ màu đỏ toàn màn hình
    TFT_WriteCommand(0x2A); // Column addr
    TFT_WriteData(0x00); TFT_WriteData(0x00);
    TFT_WriteData(0x00); TFT_WriteData(0x9F); // 127

    TFT_WriteCommand(0x2B); // Row addr
    TFT_WriteData(0x00); TFT_WriteData(0x00);
    TFT_WriteData(0x00); TFT_WriteData(0x9F); // 159

    TFT_WriteCommand(0x2C);

        // Nửa trên: Xanh lá
        for (int i = 0; i < 10240; i++) { // 10240 là một nửa của 20480
            TFT_WriteData(0x07);
            TFT_WriteData(0xE0);
        }

        // Nửa dưới: Đỏ
        for (int i = 0; i < 10240; i++) {
            TFT_WriteData(0xF8);
            TFT_WriteData(0x00);
        }
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
//	  char *usb_msg = "STM32 USB OK!\r\n";
//	  CDC_Transmit_FS((uint8_t*)usb_msg, strlen(usb_msg)); // Giao tiếp USB
	  btn_rst = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_3);
	  btn_conf = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_10);

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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 336;
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
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSE;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
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
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_HIGH;
  hspi1.Init.CLKPhase = SPI_PHASE_2EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_Encoder_InitTypeDef sConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 0;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 65535;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  sConfig.EncoderMode = TIM_ENCODERMODE_TI1;
  sConfig.IC1Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC1Filter = 0;
  sConfig.IC2Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC2Filter = 0;
  if (HAL_TIM_Encoder_Init(&htim1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 0;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 65535;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */
  HAL_TIM_MspPostInit(&htim3);

}

/**
  * @brief TIM4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM4_Init(void)
{

  /* USER CODE BEGIN TIM4_Init 0 */

  /* USER CODE END TIM4_Init 0 */

  TIM_Encoder_InitTypeDef sConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM4_Init 1 */

  /* USER CODE END TIM4_Init 1 */
  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 0;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 65535;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  sConfig.EncoderMode = TIM_ENCODERMODE_TI1;
  sConfig.IC1Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC1Filter = 15;
  sConfig.IC2Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC2Filter = 0;
  if (HAL_TIM_Encoder_Init(&htim4, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM4_Init 2 */

  /* USER CODE END TIM4_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, TFT_DC_Pin|TFT_RST_Pin|MPU_INT_Pin|Motor_A1_Pin
                          |Motor_A2_Pin|Motor_B1_Pin|Motor_B2_Pin|BUZZER_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(TFT_CS_GPIO_Port, TFT_CS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : TFT_DC_Pin TFT_RST_Pin MPU_INT_Pin Motor_A1_Pin
                           Motor_A2_Pin Motor_B1_Pin Motor_B2_Pin BUZZER_Pin */
  GPIO_InitStruct.Pin = TFT_DC_Pin|TFT_RST_Pin|MPU_INT_Pin|Motor_A1_Pin
                          |Motor_A2_Pin|Motor_B1_Pin|Motor_B2_Pin|BUZZER_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PB10 */
  GPIO_InitStruct.Pin = GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : TFT_CS_Pin */
  GPIO_InitStruct.Pin = TFT_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(TFT_CS_GPIO_Port, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void TFT_WriteCommand(uint8_t cmd) {
       HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, 0); // DC = 0 (Chế độ gửi lệnh)
       HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, 0); // CS = 0 (Chọn màn hình)
       HAL_SPI_Transmit(&hspi1, &cmd, 1, 10);   // Bắn dữ liệu qua SPI
       HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, 1); // CS = 1 (Ngắt chọn)
   }

   // Hàm gửi dữ liệu (Data) qua SPI
   void TFT_WriteData(uint8_t data) {
       HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, 1); // DC = 1 (Chế độ gửi dữ liệu)
       HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, 0); // CS = 0
       HAL_SPI_Transmit(&hspi1, &data, 1, 10);
       HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, 1); // CS = 1
   }
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
