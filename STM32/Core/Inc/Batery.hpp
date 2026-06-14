#ifndef BATTERY_H
#define BATTERY_H

#include "stm32f4xx_hal.h"

class Battery {
private:
    volatile uint16_t* adc_raw_ptr;    // Con trỏ trỏ tới phần tử trong mảng DMA
    float v_max;              // Điện áp khi đầy (vd: 8.4V cho pin 2S)
    float v_min;              // Điện áp khi cạn (vd: 6.4V cho pin 2S)
    float divider_ratio;      // Tỉ lệ cầu phân áp (vd: (R1+R2)/R2)
    
    // Lọc trung bình
    static const int SAMPLES = 20;
    float samples[SAMPLES];
    int sample_index = 0;
    int validSamples = 0; // Số lượng mẫu đã thu thập, tối đa là SAMPLES

public:
    // Khởi tạo: Truyền con trỏ mảng ADC, Vmax, Vmin và tỉ lệ phân áp
    Battery(volatile uint16_t* adc_ptr, float max, float min, float ratio);

    // Đọc điện áp thực tế (Volt)
    float getVoltage();
    // Tính phần trăm Pin (0 - 100%)
    int getPercentage();
};

#endif