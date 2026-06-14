/*
 * Batery.cpp
 *
 *  Created on: Apr 24, 2026
 *      Author: huy
 */

#include "Batery.hpp"

Battery::Battery(volatile uint16_t* adc_ptr, float max, float min, float ratio) : 
	adc_raw_ptr(adc_ptr), v_max(max), v_min(min), divider_ratio(ratio), sample_index(0) {
}
extern uint16_t adc_buffer[];
float Battery::getVoltage() {
	// // Đọc giá trị ADC thô
	// // uint16_t adc_raw = *adc_raw_ptr;
	// uint16_t adc_raw = adc_buffer[0]; // Giả sử kênh ADC đầu tiên là pin đo điện áp
	// // // Chuyển đổi sang điện áp thực tế
	//  float voltage = ((float)adc_raw / 4095.0f) * 3.3f * divider_ratio;
	// //Lọc trung bình
	// samples[sample_index] = voltage;
	// sample_index = (sample_index + 1) % SAMPLES;
	
	// float sum = 0;
	// for(int i=0; i<SAMPLES; i++) sum += samples[i];
	
	// return sum / SAMPLES; // Trả về điện áp đã qua lọc
	// //  return voltage; // Trả về điện áp chưa qua lọc
	uint16_t adc_raw = adc_buffer[0];
    float voltage = ((float)adc_raw / 4095.0f) * 3.3f * divider_ratio;
    samples[sample_index] = voltage;
    sample_index = (sample_index + 1) % SAMPLES;
    if(validSamples < SAMPLES)
    {
        validSamples++;
        return voltage;
    }

    float sum = 0;

    for(int i=0; i<SAMPLES; i++)
    {
        sum += samples[i];
    }

    return sum / SAMPLES;
}

int Battery::getPercentage() {
	float voltage = getVoltage();
	if(voltage >= v_max) return 100;
	if(voltage <= v_min) return 0;
	return (int)(((voltage - v_min) / (v_max - v_min)) * 100);
}

