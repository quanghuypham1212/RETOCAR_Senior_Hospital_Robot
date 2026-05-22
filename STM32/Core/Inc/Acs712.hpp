/*
 * Acs712.hpp
 *
 *  Created on: Apr 24, 2026
 *      Author: huy
 */

#ifndef SRC_ACS712_HPP_
#define SRC_ACS712_HPP_

#include "stm32f4xx_hal.h"

class CurrentSensor {
private:
    volatile uint16_t* adc_raw_ptr; // Con trỏ đến mảng DMA
    float sensitivity;              // Độ nhạy (V/A)
    float v_offset;                 // Điện áp khi dòng bằng 0 (thường là 2.5V)
    float divider_ratio;            // Tỉ lệ cầu phân áp (1.6667 cho 10k/15k)
    
    float current;
    float samples[10];              // Bộ lọc trung bình trượt nhỏ cho dòng điện
    int sample_index;

public:
    /**
     * @brief Khởi tạo cảm biến dòng ACS712
     * @param adc_ptr: Địa chỉ vùng nhớ DMA của kênh ADC tương ứng
     * @param sens: Độ nhạy (5A: 0.185, 20A: 0.100, 30A: 0.066)
     * @param ratio: Tỉ lệ cầu phân áp (V_sensor / V_pin). 10k/15k là 1.6667
     */
    CurrentSensor(volatile uint16_t* adc_ptr, float sens, float ratio = 1.6667f);

    /**
     * @brief Tính toán và trả về dòng điện hiện tại (Ampe)
     */
    float getCurrent();

    /**
     * @brief Hiệu chỉnh điểm 0 (gọi khi motor không chạy)
     */
    void calibrate();
};

#endif /* SRC_ACS712_HPP_ */
