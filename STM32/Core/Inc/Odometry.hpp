#ifndef ODOMETRY_HPP
#define ODOMETRY_HPP

#include "main.h"
#include <stdio.h>
#include <string.h>

class Odometry {
private:
    char tx_buffer[128]; // Bộ đệm để đóng gói chuỗi gửi đi

public:
    Odometry() {}

    // Hàm đóng gói dữ liệu thô để gửi lên Pi
    // Gói tin: d,delta_left,delta_right,yaw_rad\n
    void sendRawData(int32_t d_left, int32_t d_right, float imu_yaw);
};

#endif