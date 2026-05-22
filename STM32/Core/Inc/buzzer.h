#ifndef BUZZER_H
#define BUZZER_H

#include "main.h"

class Buzzer {
private:
    GPIO_TypeDef* GPIOx;
    uint16_t GPIO_Pin;
    uint32_t startTime;
    uint32_t duration;
    bool isActive;

public:
    Buzzer(GPIO_TypeDef* port, uint16_t pin);
    
    // Kêu một khoảng thời gian rồi tắt (ms)
    void beep(uint32_t ms);
    
    // Hàm cập nhật - Cần gọi trong while(1)
    void update();
    
    // Bật/Tắt trực tiếp
    void turnOn();
    void turnOff();
};

#endif