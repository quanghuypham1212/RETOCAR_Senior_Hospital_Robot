#include "Acs712.hpp"

CurrentSensor::CurrentSensor(volatile uint16_t* adc_ptr, float sens, float ratio) 
    : adc_raw_ptr(adc_ptr), sensitivity(sens), divider_ratio(ratio), v_offset(2.8f), sample_index(0) {
    for(int i = 0; i < 10; i++) samples[i] = 0;
}

void CurrentSensor::calibrate() {
    float sum_v = 0;
    for(int i = 0; i < 50; i++) {
        // Đọc trực tiếp giá trị ADC
        float v_pin = ((float)(*adc_raw_ptr) / 4095.0f) * 3.3f;
        sum_v += v_pin * divider_ratio;

        // Thay HAL_Delay(1) bằng vòng lặp chờ thô
        for(volatile uint32_t wait = 0; wait < 10000; wait++);
    }
    v_offset = sum_v / 50.0f;
}

float CurrentSensor::getCurrent() {
     //1. Đọc điện áp tại chân Pin (0 - 3.3V)
     float v_pin = ((float)(*adc_raw_ptr) / 4095.0f) * 3.3f;
    
     // 2. Quy đổi ngược về điện áp thực tại ngõ ra ACS712 (0 - 5V)
     float v_sensor = v_pin * divider_ratio;
    if (v_sensor < 0.5f) return 0.0f;
     // 3. Tính dòng điện dựa trên độ nhạy
     float instant_current = (v_sensor - v_offset) / sensitivity;

    // 4. Bộ lọc trung bình trượt đơn giản để khử nhiễu
    samples[sample_index] = instant_current;
    sample_index = (sample_index + 1) % 10;
    
    float sum = 0;
    for(int i = 0; i < 10; i++) sum += samples[i];
    current = sum / 10.0f;
	if (current > 1.0f) { 
        this->v_offset = current;
    }

     return current;
	

}
