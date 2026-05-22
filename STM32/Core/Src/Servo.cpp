#include "Servo.hpp"
#include "cmsis_os.h"  // Thư viện quản lý các hàm os của FreeRTOS"

ServoController::ServoController(I2C_HandleTypeDef* hi2c, uint16_t devAddr)
    : _hi2c(hi2c), _devAddr(devAddr) {}

void ServoController::writeRegister(uint8_t reg, uint8_t data) {
    HAL_I2C_Mem_Write(_hi2c, _devAddr, reg, 1, &data, 1, 100);
}

void ServoController::init() {
	 uint8_t data;
	    // 1. Reset chip hoàn toàn
	    data = 0x80;
	    HAL_I2C_Mem_Write(_hi2c, _devAddr, 0x00, 1, &data, 1, 100);
	    HAL_Delay(50);

	    // 2. Cho chip ngủ để cài Prescale
	    data = 0x10;
	    HAL_I2C_Mem_Write(_hi2c, _devAddr, 0x00, 1, &data, 1, 100);

	    // 3. Cài 50Hz (Prescale = 121)
	    data = 121;
	    HAL_I2C_Mem_Write(_hi2c, _devAddr, 0xFE, 1, &data, 1, 100);

	    // 4. Thức dậy và cho phép Restart
	    data = 0xA1; // 0xA1 = 1010 0001 (Restart + Auto-Increment + AllCall)
	    HAL_I2C_Mem_Write(_hi2c, _devAddr, 0x00, 1, &data, 1, 100);
	    HAL_Delay(5);
	    // 5. Cấu hình MODE2 (Totem Pole) - Cực kỳ quan trọng để Servo quay
	        data = 0x04;
	        HAL_I2C_Mem_Write(_hi2c, _devAddr, 0x01, 1, &data, 1, 100);
}

void ServoController::setPWM(uint8_t channel, uint16_t on, uint16_t off) {
    uint8_t data[4];
    data[0] = 0x00; // ON Low (Bật ở vạch 0)
    data[1] = 0x00; // ON High
    data[2] = off & 0xFF; // OFF Low (Tắt ở vạch 'value')
    data[3] = off >> 8;   // OFF High
    HAL_I2C_Mem_Write(_hi2c, _devAddr, 0x06 + (4 * channel), 1, data, 4, 100);
}

void ServoController::setAngle(uint8_t channel, uint16_t angle) {
    // Chuyển đổi góc 0-180 sang độ rộng xung PWM (thường từ 150 đến 600 trên PCA9685)
    uint16_t offValue = (uint16_t)(120.0 + (angle * (620.0 - 120.0) / 180.0));
    setPWM(channel, 0, offValue);
}

void ServoController::openCompartment(uint8_t id) {
    setAngle(id - 1, 90); // Giả sử 90 độ là mở
}

void ServoController::closeCompartment(uint8_t id) {
    setAngle(id - 1, 0);  // Giả sử 0 độ là đóng
	osDelay(500); // Đợi 500ms để servo quay về vị trí đóng hoàn toàn
	stopPWM(id - 1); // Tắt tín hiệu PWM để tránh servo bị nóng khi giữ ở vị trí đóng

}

void ServoController::stopPWM(uint8_t channel) {
    uint8_t data[4] = {0, 0, 0, 0}; // Cả ON và OFF đều bằng 0
    HAL_I2C_Mem_Write(_hi2c, _devAddr, 0x06 + (4 * channel), 1, data, 4, 100);
}
