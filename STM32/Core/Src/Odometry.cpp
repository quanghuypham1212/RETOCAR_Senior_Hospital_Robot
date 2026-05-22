/*
 * Odometry.cpp
 *
 *  Created on: Apr 24, 2026
 *      Author: huy
 */

#include "Odometry.hpp"
#include "usbd_cdc_if.h" 

void Odometry::sendRawData(int32_t d_left, int32_t d_right, float imu_yaw) {
    // Đóng gói dữ liệu theo định dạng chuỗi: d (delta), left, right, yaw
    // Chúng ta dùng float cho yaw để Pi nhận được góc chính xác ngay
    int len = sprintf(tx_buffer, "d,%ld,%ld,%.4f\n", d_left, d_right, imu_yaw);
    
    // Gửi trực tiếp qua cổng USB CDC
    CDC_Transmit_FS((uint8_t*)tx_buffer, len);
}

