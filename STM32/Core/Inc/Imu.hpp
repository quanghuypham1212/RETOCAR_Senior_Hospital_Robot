#ifndef SRC_IMU_HPP_
#define SRC_IMU_HPP_

#include "main.h"

// Địa chỉ mặc định của các chip trên GY-87
#define MPU6050_ADDR    (0x68 << 1)
#define HMC5883L_ADDR   (0x1E << 1)

class Imu {
private:
    I2C_HandleTypeDef* hi2c;
    
    // Lưu trữ giá trị thô (Raw data) từ cảm biến
    int16_t ax, ay, az;
    int16_t gx, gy, gz;
    int16_t mx, my, mz;

    // Biến lưu trữ sau khi đã quy đổi sang đơn vị vật lý (m/s2, deg/s)
    float accel_x, accel_y, accel_z;
    float gyro_x, gyro_y, gyro_z;
    float mag_x, mag_y, mag_z, heading;

public:
    Imu(I2C_HandleTypeDef* hi2c);
    
    void init();
    
    // Các hàm đọc dữ liệu từ thanh ghi
    void update(); // Hàm tổng để đọc tất cả cảm biến
    void readMPU6050(); 
    void readHMC5883L();
    
    // Bật Bypass Mode để STM32 có thể thấy được HMC5883L
    void enableBypass();
    
    // Hàm bổ trợ giao tiếp I2C
    HAL_StatusTypeDef writeRegister(uint16_t addr, uint8_t reg, uint8_t data);
    HAL_StatusTypeDef readRegisters(uint16_t addr, uint8_t reg, uint8_t* buffer, uint16_t size);

    float getGyroZ() const { return gyro_z; } // Hàm lấy giá trị góc quay quanh trục Z (yaw)
    float getHeading() const { return heading; } // Hàm lấy giá trị góc hướng từ cảm biến từ kế
};

#endif