/*
 * Imu.cpp
 *
 *  Created on: Apr 24, 2026
 *      Author: huy
 */

#include "Imu.hpp"
#include "math.h"

Imu::Imu(I2C_HandleTypeDef* hi2c) : hi2c(hi2c) {}

HAL_StatusTypeDef Imu::writeRegister(uint16_t addr, uint8_t reg, uint8_t data) {
	return HAL_I2C_Mem_Write(hi2c, addr, reg, I2C_MEMADD_SIZE_8BIT, &data, 1, 100);
}

HAL_StatusTypeDef Imu::readRegisters(uint16_t addr, uint8_t reg, uint8_t* buffer, uint16_t size) {
	return HAL_I2C_Mem_Read(hi2c, addr, reg, I2C_MEMADD_SIZE_8BIT, buffer, size, 100);
}

void Imu::enableBypass() {
	// Đọc giá trị hiện tại của INT_PIN_CFG
	writeRegister(MPU6050_ADDR, 0x6A, 0x00);
	uint8_t intPinCfg;
	readRegisters(MPU6050_ADDR, 0x37, &intPinCfg, 1);

	// Bật bit BYPASS_EN (bit 1)
	intPinCfg |= 0x02;

	// Ghi lại giá trị đã chỉnh sửa
	writeRegister(MPU6050_ADDR, 0x37, intPinCfg);
}

void Imu::init() {
	// Khởi tạo MPU6050
	writeRegister(MPU6050_ADDR, 0x6B, 0x00); // Wake up MPU6050
	writeRegister(MPU6050_ADDR, 0x1B, 0x00); // Gyro full scale ±250 deg/s
	writeRegister(MPU6050_ADDR, 0x1C, 0x00); // Accel full scale ±2g

	// Bật Bypass Mode để STM32 có thể giao tiếp trực tiếp với HMC5883L
	enableBypass();

	// Khởi tạo HMC5883L
	writeRegister(HMC5883L_ADDR, 0x00, 0x70); // Configuration Register A: 8-average, 15 Hz default, normal measurement
	writeRegister(HMC5883L_ADDR, 0x01, 0x20); // Configuration Register B: Gain = 1.3 Ga (default)
	writeRegister(HMC5883L_ADDR, 0x02, 0x00); // Mode Register: Continuous-measurement mode
}

void Imu::readMPU6050() {
    uint8_t buffer[14];
    // Kiểm tra nếu đọc thành công mới cập nhật biến
    if (readRegisters(MPU6050_ADDR, 0x3B, buffer, 14) == HAL_OK) {
        ax = (int16_t)((buffer[0] << 8) | buffer[1]);
        ay = (int16_t)((buffer[2] << 8) | buffer[3]);
        az = (int16_t)((buffer[4] << 8) | buffer[5]);
        // buffer[6,7] là nhiệt độ, chúng ta bỏ qua
        gx = (int16_t)((buffer[8] << 8) | buffer[9]);
        gy = (int16_t)((buffer[10] << 8) | buffer[11]);
        gz = (int16_t)((buffer[12] << 8) | buffer[13]);

        // Quy đổi (Sử dụng các hằng số nhạy của Huy là đúng rồi)
        accel_x = ax / 16384.0f * 9.81f;
        accel_y = ay / 16384.0f * 9.81f;
        accel_z = az / 16384.0f * 9.81f;
		gyro_x  = gx / 131.0f;
		gyro_y  = gy / 131.0f;
        gyro_z  = gz / 131.0f; // Trục quan trọng nhất để tính Yaw cho robot
    }
}

void Imu::readHMC5883L() {
    uint8_t buffer[6];
    // HMC5883L lưu theo thứ tự: X_High, X_Low, Z_High, Z_Low, Y_High, Y_Low
    if (readRegisters(HMC5883L_ADDR, 0x03, buffer, 6) == HAL_OK) {
        mx = (int16_t)((buffer[0] << 8) | buffer[1]);
        mz = (int16_t)((buffer[2] << 8) | buffer[3]); // Lưu ý: Trục Z ở giữa
        my = (int16_t)((buffer[4] << 8) | buffer[5]);

        mag_x = mx * 0.92f;
        mag_y = my * 0.92f;
        mag_z = mz * 0.92f;

		heading = atan2f(-mag_y, mag_x) * 180.0f / 3.14159265359f;
		if (heading < 0) heading += 360.0f; // Đảm bảo góc luôn dương
    }
}

void Imu::update() {
	readMPU6050();
	readHMC5883L();
}



