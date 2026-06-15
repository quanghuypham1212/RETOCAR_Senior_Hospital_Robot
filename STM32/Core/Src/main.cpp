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
#include "cmsis_os.h"
#include "usb_device.h"
#include "usbd_cdc_if.h"
#include "motor.hpp"
#include "Encoder.hpp"
#include "Pid.hpp"
#include "robotcontroller.hpp"
#include "Servo.hpp"
#include "Batery.hpp"
#include "Acs712.hpp"
#include "Tft.hpp"
#include "usb_ring_buffer.hpp"
#include "Odometry.hpp"
#include "Imu.hpp"
#include "Button.hpp"
#include "buzzer.h"
#include "task_mission.hpp"
#include "task_comm.hpp"
#include "task_motor.hpp"
#include "task_comm_trans.hpp"
#include "filter_imu.hpp"
#include "viet_font.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

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
DMA_HandleTypeDef hdma_adc1;
I2C_HandleTypeDef hi2c1;
SPI_HandleTypeDef hspi1;
TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;
TIM_HandleTypeDef htim10;
UART_HandleTypeDef huart2;

/* Definitions for Motor_Task */
osThreadId_t Motor_TaskHandle;
const osThreadAttr_t Motor_Task_attributes = {
  .name = "Motor_Task",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityHigh,
};
/* Definitions for Comm_Task */
osThreadId_t Comm_TaskHandle;
const osThreadAttr_t Comm_Task_attributes = {
  .name = "Comm_Task",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityAboveNormal,
};
/* Definitions for Mission_Task */
osThreadId_t Mission_TaskHandle;
const osThreadAttr_t Mission_Task_attributes = {
  .name = "Mission_Task",
  .stack_size = 1024 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for Health_Task */
osThreadId_t Health_TaskHandle;
const osThreadAttr_t Health_Task_attributes = {
  .name = "Health_Task",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* USER CODE BEGIN PV */
ImuFilter imuFilter(0.01f, 0.998f); // Khởi tạo bộ lọc IMU với dt=10ms và alpha=0.98
RingBuffer usbBuffer; // Khởi tạo vòng đệm USB với kích thước 256 byte
Button btn_conf(GPIOB, GPIO_PIN_10, 50); // Khởi tạo nút cấu hình
Buzzer buzzer(GPIOB, GPIO_PIN_3); // Khởi tạo buzzer
ServoController servo(&hi2c1, (0x40 << 1));
// volatile bool btn_conf_state = false; // Biến trạng thái cấu hình
RobotController myRobot;
// Odometry odometry;
TFT_Display tft(&hspi1, GPIOA, GPIO_PIN_10, GPIOB, GPIO_PIN_0, GPIOB, GPIO_PIN_1); // Khởi tạo TFT với chân CS, DC, RST
Imu imu(&hi2c1); // Khởi tạo IMU
volatile uint16_t adc_buffer[3] = {0}; // Buffer DMA cho 3 kênh ADC
Battery robot_batery(NULL, 12.0f, 0.0f, 4.1f);
// CurrentSensor Motor_right(&adc_buffer[1], 0.1f, 1.6667f);
// CurrentSensor Motor_left(&adc_buffer[2], 0.1f, 1.6667f);
// volatile float cur_right = 0;
// volatile float cur_left = 0;

Compartment compartment[4] = {
    Compartment(1, &servo, &btn_conf, &buzzer, &tft),
    Compartment(2, &servo, &btn_conf, &buzzer, &tft),
    Compartment(3, &servo, &btn_conf, &buzzer, &tft),
    Compartment(4, &servo, &btn_conf, &buzzer, &tft)
};

float targetVx = 0.0f;
float targetWz = 0.0f;
UartParser uartParser(&usbBuffer, compartment, &targetVx, &targetWz); // Khởi tạo parser với vòng đệm và mảng ngăn thuốc
TaskMotor myMotorTask;
Telemetry telemetry;


/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_ADC1_Init(void);
static void MX_I2C1_Init(void);
static void MX_SPI1_Init(void);
static void MX_TIM1_Init(void);
static void MX_TIM3_Init(void);
static void MX_TIM4_Init(void);
static void MX_TIM10_Init(void);
static void MX_USART2_UART_Init(void);
#ifdef __cplusplus
extern "C" {
#endif

void StartMotorTask(void *argument);
void StartCommTask(void *argument);
void StartMissionTask(void *argument);
void StartHealthTask(void *argument);

#ifdef __cplusplus
}
#endif
/* USER CODE BEGIN PFP */
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
// void I2C_Scan_To_Array(I2C_HandleTypeDef *hi2c) {
//     devices_count = 0;
//     // Xóa dữ liệu cũ trong mảng
//     for(int i=0; i<10; i++) i2c_addresses[i] = 0;

//     for (uint16_t i = 1; i < 128; i++) {
//         // Kiểm tra thiết bị
//         if (HAL_I2C_IsDeviceReady(hi2c, (uint16_t)(i << 1), 3, 5) == HAL_OK) {
//             if (devices_count < 10) {
//                 i2c_addresses[devices_count] = (uint8_t)i; // Lưu địa chỉ 7-bit
//                 devices_count++;
//             }
//         }
//     }
// }
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
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_I2C1_Init();
  MX_SPI1_Init();
  MX_TIM1_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_TIM10_Init();
  MX_USB_DEVICE_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 0);
  __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 0);
  HAL_TIM_Encoder_Start(&htim1, TIM_CHANNEL_ALL); // Bắt đầu Encoder R
  HAL_TIM_Encoder_Start(&htim4, TIM_CHANNEL_ALL); // Bắt đầu Encoder L
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
  HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_buffer, 3); // Bắt đầu ADC với DMA cho 3 kênh
  // Motor_right.calibrate();
  // Motor_left.calibrate();
  tft.init();
  imu.init();
  servo.init();
  servo.stopPWM(0);
  servo.stopPWM(1);
  servo.stopPWM(2);
  servo.stopPWM(3);
  tft.Idle();
//HAL_TIM_Base_Start_IT(&htim10); // Bật ngắt định kỳ 20ms để đọc encoder và điều khiển PID

    //HAL_Delay(1000);
  osKernelInitialize();
  /* Create the thread(s) */
  /* creation of Motor_Task */
 Motor_TaskHandle = osThreadNew(StartMotorTask, NULL, &Motor_Task_attributes);

 /* creation of Comm_Task */
 Comm_TaskHandle = osThreadNew(StartCommTask, NULL, &Comm_Task_attributes);
//
//  /* creation of Mission_Task */
 Mission_TaskHandle = osThreadNew(StartMissionTask, NULL, &Mission_Task_attributes);
//
//  /* creation of Health_Task */
 Health_TaskHandle = osThreadNew(StartHealthTask, NULL, &Health_Task_attributes);

//  /* USER CODE BEGIN RTOS_THREADS */
//  /* add threads, ... */
//  /* USER CODE END RTOS_THREADS */
//
//  /* USER CODE BEGIN RTOS_EVENTS */
//  /* add events, ... */
//  /* USER CODE END RTOS_EVENTS */
//
//  /* Start scheduler */
  osKernelStart();
  /* USER CODE END 2 */

  /* Infinite loop */

  // HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET); // Bật LED để kiểm tra Task này có chạy không
  // HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET); // Tắt LED để kiểm tra Task này có chạy không
  // __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 65535); // PWM cho Motor A (giả sử 30000 là giá trị phù hợp để đạt ~30 rad/s)
  // HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET); // Bật LED để kiểm tra Task này có chạy không
  // HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_RESET); // Tắt LED để kiểm tra Task này có chạy không
  // __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 65535); // PWM cho Motor A (giả sử 30000 là giá trị phù hợp để đạt ~30 rad/s)
  // HAL_Delay(20000);
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  // CDC_Transmit_FS((uint8_t*)usb_msg, strlen(usb_msg)); // Giao tiếp USB
    // HAL_Delay(2000); // Đợi 1 giây trước khi gửi tiếp
      // debug_deltaL = EncoderL.update();
      // debug_deltaR = EncoderR.update();
      // total_pulseL += debug_deltaL;
      // total_pulseR += debug_deltaR;
      // HAL_Delay(20); // Nghỉ 10ms để xung kịp tăng lên
    // myRobot.setTargetVelocities(20.0f, 20.0f); // Đặt mục tiêu vận tốc cho robot (ví dụ: 15 rad/s cho cả 2 bánh)
//    HAL_Delay(100);
    //Quay từ 0 đến 180 độ
//     for(int angle = 0; angle <= 180; angle += 5) {
//         servo.setAngle(0, angle);
//         HAL_Delay(20);
//     }
//     HAL_Delay(1000); // Đợi 1 giây
//    if (current_time - last_time_battery >= 5000) { // Cập nhật mỗi 1 giây
    // realVoltage = robot_batery.getVoltage();
    // pinPercentage = robot_batery.getPercentage();
    // cur_right = Motor_right.getCurrent();
    // cur_left = Motor_left.getCurrent();
//    last_time_battery = current_time;
//    }
    // if (usbBuffer.dequeue(&rx_byte)) {
        
    //     // 2. Nếu là ký tự Enter (\r hoặc \n)
    //     if (rx_byte == '\r' || rx_byte == '\n') {
    //         if (idx > 0) { // Chỉ gửi khi đã có chữ trong buffer
    //             uart_buffer[idx] = '\0'; // Thêm ký tự kết thúc chuỗi
                
    //             // 3. Gửi ngược lại toàn bộ chuỗi cho Pi
    //             // Thêm \r\n để Pi xuống dòng cho đẹp
    //             strcat(uart_buffer, "\r\n"); 
    //             CDC_Transmit_FS((uint8_t*)uart_buffer, strlen(uart_buffer));
                
    //             // 4. Reset chỉ số để đợi lệnh tiếp theo
    //             idx = 0; 
    //         }
    //     } 
    //     // 3. Nếu chưa phải Enter, cứ gom vào mảng
    //     else {
    //         if (idx < sizeof(uart_buffer) - 2) { // Chống tràn mảng
    //             uart_buffer[idx++] = rx_byte;
    //         }
    //     }
    // }
    // imu.update(); // Cập nhật dữ liệu từ IMU
    // HAL_Delay(1000); // Đợi 2000ms trước khi cập nhật lại (tần số 10Hz)
    // if(btn_conf.reading()) {

    //         tft.displayConfirmed(); // Hiển thị thông báo đã xác nhận
    //         buzzer.turnOn(); // Kêu buzzer trong 1 giây 
    //     }

  }
}
  // uint32_t dL = myRobot.getLastDeltaL();
  // uint32_t dR = myRobot.getLastDeltaR();
  // odometry.sendRawData(dL, dR, 0.0f); // Gửi dữ liệu về Pi (tạm thời để yaw = 0.0f, sau này sẽ thay bằng góc thực tế từ IMU)
  
  /* USER CODE END 3 */


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
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
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
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = ENABLE;
  hadc1.Init.ContinuousConvMode = ENABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 3;
  hadc1.Init.DMAContinuousRequests = ENABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SEQ_CONV;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_480CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_1;
  sConfig.Rank = 2;
  sConfig.SamplingTime = ADC_SAMPLETIME_56CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_4;
  sConfig.Rank = 3;
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
  hi2c1.Init.ClockSpeed = 400000;
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
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
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
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
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

static void MX_TIM10_Init(void)
{

  /* USER CODE BEGIN TIM10_Init 0 */

  /* USER CODE END TIM10_Init 0 */

  /* USER CODE BEGIN TIM10_Init 1 */

  /* USER CODE END TIM10_Init 1 */
  htim10.Instance = TIM10;
  htim10.Init.Prescaler = 249;
  htim10.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim10.Init.Period = 1999;
  htim10.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim10.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim10) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM10_Init 2 */

  /* USER CODE END TIM10_Init 2 */

}
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

static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA2_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA2_Stream0_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);

}

static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOB, TFT_DC_Pin|TFT_RST_Pin|MPU_INT_Pin|Motor_A1_Pin
                          |Motor_A2_Pin|Motor_B1_Pin|Motor_B2_Pin|BUZZER_Pin, GPIO_PIN_RESET);
  /*Configure GPIO pin : PC13 */
   GPIO_InitStruct.Pin = GPIO_PIN_13;
   GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
   GPIO_InitStruct.Pull = GPIO_NOPULL;
   GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
   HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

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
// extern "C" void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
//     if (htim->Instance == TIM10) {
//       myRobot.update(); // Gọi hàm cập nhật điều khiển robot mỗi 20ms
//       debug_deltaL = myRobot.getFilteredVelocityL(); // Lấy giá trị thực tế đã qua lọc để debug
//       debug_deltaR = myRobot.getFilteredVelocityR();

//     }
// }
extern "C"{
/* USER CODE END Header_StartMotorTask */
void StartMotorTask(void *argument)
{

  /* USER CODE BEGIN 5 */
  /* Infinite loop */
  for(;;)
  {

    myMotorTask.MotorTask_Execute();
    // myRobot.setTargetVelocities(30.0f, 30.0f); // Đặt mục tiêu vận tốc cho robot (ví dụ: 15 rad/s cho cả 2 bánh)
    // myRobot.update(); // Cập nhật điều khiển robot mỗi 20ms
    // HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET); // Bật LED để kiểm tra Task này có chạy không
    // HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET); // Tắt LED để kiểm tra Task này có chạy không
    // __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 32767); // PWM cho Motor A (giả sử 30000 là giá trị phù hợp để đạt ~30 rad/s)
    // HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET); // Bật LED để kiểm tra Task này có chạy không
    // HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_RESET); // Tắt LED để kiểm tra Task này có chạy không
    // __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 32767); // PWM cho Motor A (giả sử 30000 là giá trị phù hợp để đạt ~30 rad/s)
    buzzer.update(); // Cập nhật trạng thái buzzer (tự tắt sau 1 giây nếu đã bật)
    osDelay(10);
  }
  /* USER CODE END 5 */
}

/* USER CODE BEGIN Header_StartCommTask */
/**
* @brief Function implementing the Comm_Task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartCommTask */
void StartCommTask(void *argument)
{
  /* USER CODE BEGIN StartCommTask */

  /* Infinite loop */
  for(;;)
  {
    uartParser.process();
    // Task ngủ 20ms để nhường CPU cho Motor_Task tính toán PID
    osDelay(20);
  }
  /* USER CODE END StartCommTask */
}
/* USER CODE BEGIN Header_StartMissionTask */
/**
* @brief Function implementing the Mission_Task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartMissionTask */


void StartMissionTask(void *argument)
{
  /* USER CODE BEGIN StartMissionTask */
  bool last_button_state = 0;
  /* Infinite loop */
  for(;;)
  {
   // 1. Đọc nút nhấn (Debounce 50ms nhờ chu kỳ Task)
      bool okPressed = btn_conf.reading();

      if(okPressed != last_button_state) {
          last_button_state = okPressed;
          telemetry.sendButton(okPressed); // Gửi trạng thái nút cấu hình ngay khi có sự thay đổi
      }
//
//        // 2. Cập nhật FSM cho từng ngăn
      for(int i = 0; i < 4; i++) {
          compartment[i].update(okPressed);
       }
    
    //buzzer.update(); // Cập nhật trạng thái buzzer (tự tắt sau 1 giây nếu đã bật)
    osDelay(50); // Đọc nút mỗi 50ms để debounce và đủ thời gian cho người dùng nhấn
  }
  /* USER CODE END StartMissionTask */
}


/* USER CODE BEGIN Header_StartHealthTask */
/**
* @brief Function implementing the Health_Task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartHealthTask */
void StartHealthTask(void *argument)
{
  /* USER CODE BEGIN StartHealthTask */
  /* Infinite loop */
  for(;;)
  { 
    telemetry.sendBattery();
    osDelay(1000);
  }
  /* USER CODE END StartHealthTask */
}

}
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM9) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
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
