#ifndef IMU_FILTER_HPP
#define IMU_FILTER_HPP

#include <stdint.h>
#include <math.h>
#include "Imu.hpp"
#include "main.h"
#include "robotcontroller.hpp"

class ImuFilter {
public:
    /**
     * @param dt Thời gian lấy mẫu (ví dụ 0.01f cho 10ms)
     * @param alpha Hệ số lọc (càng gần 1 thì càng tin Gyro, càng nhỏ thì càng tin Mag)
     */
    ImuFilter(float dt, float alpha = 0.998);

    // Hàm thực hiện lấy mẫu để triệt tiêu sai số tĩnh (gọi khi xe đứng yên lúc khởi động)
    void calibrate(float raw_gz);

    // Hàm cập nhật và trả về góc Yaw cuối cùng (Radian)
    float update(float raw_gz, float mag_heading_deg);

    float getYaw() const { return yaw; }
    bool Calibrated() const { return is_calibrated; }
    float get_final_yaw() const {return final_yaw; }

private:
    float _dt;
    float _alpha;
    float _gyro_bias;
    float yaw;
    bool is_calibrated;
    float final_yaw = 0.0f;

    // Chuẩn hóa góc về khoảng [-PI, PI] cho đúng chuẩn ROS 2
    float wrapToPi(float angle);
};

#endif