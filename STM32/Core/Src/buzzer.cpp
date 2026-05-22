#include "Buzzer.h"

Buzzer::Buzzer(GPIO_TypeDef* port, uint16_t pin) {
    this->GPIOx = port;
    this->GPIO_Pin = pin;
    this->isActive = false;
    this->turnOff();
}

void Buzzer::turnOn() {
    HAL_GPIO_WritePin(GPIOx, GPIO_Pin, GPIO_PIN_SET);
}

void Buzzer::turnOff() {
    HAL_GPIO_WritePin(GPIOx, GPIO_Pin, GPIO_PIN_RESET);
}

void Buzzer::beep(uint32_t ms) {
    this->duration = ms;
    this->startTime = HAL_GetTick();
    this->isActive = true;
    this->turnOn();
}

void Buzzer::update() {
    if (isActive) {
        // Nếu đã đủ thời gian yêu cầu thì tắt còi
        if (HAL_GetTick() - startTime >= duration) {
            this->turnOff();
            this->isActive = false;
        }
    }
}