/*
 * Filterimu.cpp
 *
 *  Created on: Apr 24, 2026
 *      Author: huy
 */

#include "Filter_imu.hpp"

ImuFilter::ImuFilter(float dt, float alpha) 
	: _dt(dt), _alpha(alpha), _gyro_bias(0.0f), yaw(0.0f), is_calibrated(false) {}

void ImuFilter::calibrate(float raw_gz) {
    static int samples = 0;
    static float sum = 0;
    const int MAX_SAMPLES = 200; // Calibrate trong khoảng 2 giây nếu chạy 100Hz

    if (samples < MAX_SAMPLES) {
        sum += raw_gz;
        samples++;
    } else {
        _gyro_bias = sum / MAX_SAMPLES;
        is_calibrated = true;
    }
}

float ImuFilter::update(float raw_gz, float mag_heading_deg) {
    if (!is_calibrated) return 0.0f;

    // 1. Chuyển đổi 303 độ sang Radian (~5.28 rad) và ép về [-PI, PI]
    float mag_yaw_rad = wrapToPi(mag_heading_deg * (M_PI / 180.0f));

    // 2. Khử Bias cho Gyro và tính phần dự đoán
    float gz_fixed = (raw_gz - _gyro_bias) * (M_PI / 180.0f);
    
    // 3. Tính toán sai lệch góc (diff) để tránh nhảy số khi qua điểm 180 độ
    float diff = mag_yaw_rad - yaw;
    if (diff > M_PI)  diff -= 2.0f * M_PI;
    if (diff < -M_PI) diff += 2.0f * M_PI;

    // 4. Cập nhật Yaw bằng bộ lọc bù (Complementary Filter)
    // yaw = yaw + (gz_fixed * _dt) + (1.0f - _alpha) * diff;
    yaw = yaw + (gz_fixed * _dt);
    yaw = wrapToPi(yaw);

    // 5. Logic Zeroing: Lần đầu tiên chạy xong calibrate, coi hướng đó là 0
    static bool offset_captured = false;
    static float yaw_offset = 0.0f;
    
    if (!offset_captured) {
        yaw_offset = yaw; // Lưu lại cái số 303 độ (đã đổi sang rad) lúc khởi động
        offset_captured = true;
    }
    final_yaw = wrapToPi(yaw - yaw_offset);
    // Trả về giá trị đã trừ offset để luôn bắt đầu từ 0
    return final_yaw;
}

float ImuFilter::wrapToPi(float angle) {
    while (angle > PI)  angle -= 2.0f * M_PI;
    while (angle < -PI) angle += 2.0f * M_PI;
    return angle;
}