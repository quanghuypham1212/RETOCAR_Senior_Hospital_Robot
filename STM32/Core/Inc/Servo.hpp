#ifndef SERVO_CONTROLLER_HPP
#define SERVO_CONTROLLER_HPP

#include "main.h"

class ServoController {
public:
    // Constructor truyền vào bộ I2C và địa chỉ PCA (thường là 0x40 << 1)
    ServoController(I2C_HandleTypeDef* hi2c, uint16_t devAddr);

    void init();
    void setAngle(uint8_t channel, uint16_t angle); // Góc từ 0 - 180
    void openCompartment(uint8_t id);              // Mở ngăn thuốc 1-4
    void closeCompartment(uint8_t id);             // Đóng ngăn thuốc 1-4
    void setPWM(uint8_t channel, uint16_t on, uint16_t off);
    void stopPWM(uint8_t channel); // Tắt tín hiệu PWM để servo không bị nóng khi giữ ở vị trí cố định

private:
    I2C_HandleTypeDef* _hi2c;
    uint16_t _devAddr;
    
    void writeRegister(uint8_t reg, uint8_t data);
//    void setPWM(uint8_t channel, uint16_t on, uint16_t off);
};

#endif
