#include "motor.hpp"

Motor::Motor(TIM_HandleTypeDef* htim, uint32_t channel, 
             GPIO_TypeDef* port1, uint16_t pin1, 
             GPIO_TypeDef* port2, uint16_t pin2)
    : _htim(htim), _channel(channel), _port1(port1), _pin1(pin1), _port2(port2), _pin2(pin2) {
    
    // Khởi động bộ xuất xung PWM (Chân ENA)
    HAL_TIM_PWM_Start(_htim, _channel);
    stop(); 
}

float Motor::normalize(float w) {
    float s = w / W_MAX;
    if (s > 1.0f) return 1.0f;
    if (s < -1.0f) return -1.0f;
    return s;
}

void Motor::setVelocity(float w_rad_s) {
    float speed = normalize(w_rad_s);

    if (speed > 0) {
        // Quay thuận: IN1=HIGH, IN2=LOW
        HAL_GPIO_WritePin(_port1, _pin1, GPIO_PIN_SET);
        HAL_GPIO_WritePin(_port2, _pin2, GPIO_PIN_RESET);
        
    } 
    else if (speed < 0) {
        // Quay nghịch: IN1=LOW, IN2=HIGH
        HAL_GPIO_WritePin(_port1, _pin1, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(_port2, _pin2, GPIO_PIN_SET);
        speed = -speed; // Lấy giá trị tuyệt đối để tính PWM
    } 
    else {
        stop();
        return;
    }

    // Tính toán và xuất xung PWM ra chân ENA
    uint32_t pulse = (uint32_t)((float)speed * PWM_MAX);
    __HAL_TIM_SET_COMPARE(_htim, _channel, pulse);
}

void Motor::stop() {
    // Dừng tự do (Coast): IN1=LOW, IN2=LOW, PWM=0
    HAL_GPIO_WritePin(_port1, _pin1, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(_port2, _pin2, GPIO_PIN_RESET);
    __HAL_TIM_SET_COMPARE(_htim, _channel, 0);	
}

