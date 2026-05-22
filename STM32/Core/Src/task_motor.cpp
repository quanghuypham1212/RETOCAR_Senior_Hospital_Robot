#include "task_motor.hpp"
#include "main.h"
#include "robotcontroller.hpp"
#include "task_comm_trans.hpp"

void TaskMotor::MotorTask_Execute() {
    // Vòng lặp chính của task Motor
   imu.update(); // Cập nhật dữ liệu từ IMU
   if (!imuFilter.Calibrated()) {
    imuFilter.calibrate(imu.getGyroZ());
    } else {
    imuFilter.update(imu.getGyroZ(), imu.getHeading());
    }
   	myRobot.move(targetVx, targetWz);
    myRobot.update(); // Cập nhật điều khiển PID và đọc encoder
    Telemetry::sendOdom(); // Gửi dữ liệu odometry về Pi (tạm thời yaw = 0.0f, sau này sẽ thay bằng góc thực tế từ IMU)
}
