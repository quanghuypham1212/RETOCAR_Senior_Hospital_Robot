#ifndef ROBOT_CONTROLLER_HPP
#define ROBOT_CONTROLLER_HPP

#include "PID.hpp"
#include "main.h"
#include "Encoder.hpp"
#include "motor.hpp"
#include "Kinematics.hpp"
#define ACCELERATION_LIMIT 10.0f // rad/s^2, giới hạn gia tốc để tránh sốc cơ học
#define FILTER_ALPHA 0.8f // Hệ số lọc cho bộ lọc thông thấp (Low-pass filter)
#define SAMPLING_TIME_S  0.01f // 20ms = 0.02s, tương ứng với tần số điều khiển 50Hz   
#define PI               3.14159265359f
#define PPR              330.0f // Pulse Per Revolution - Số xung/vòng của encoder
// Hằng số để chuyển từ xung/thời gian lấy mẫu sang rad/s
#define TICKS_TO_RAD_PER_S ((2.0f * PI) / (PPR * SAMPLING_TIME_S))

class RobotController {
private:
    // PID cho 2 bánh: Trái (Left) và Phải (Right)
	Encoder EncoderL;
	Encoder EncoderR;
	Motor motorL;
	Motor motorR;
    PID pid_l, pid_r;
    Kinematics kinematics{0.292f, 0.0425f}; // L=0.3m, R=0.05m (Cần điều chỉnh theo thực tế)
    uint8_t _startup_counter = 0; // Dùng để đếm số lần gọi update() sau khi khởi động, tránh tính toán PID quá sớm
    static constexpr uint8_t STARTUP_TICKS = 5; // 5 x 10ms = 50ms, thời gian chờ để encoder ổn định trước khi tính PID

    float velocity_target[2] = {0, 0};   // 0: Left, 1: Right
    float ramp_target[2] = {0, 0};
    float filtered_actual[2] = {0, 0};
    int16_t prev_enc[2] = {0, 0};
    int32_t last_deltaL;
    int32_t last_deltaR;
    int32_t total_pulseL = 0;
    int32_t total_pulseR = 0;
    float outputL = 0.0f;
    float outputR = 0.0f;
public:
    RobotController() : 
		EncoderL(&htim4, false), EncoderR(&htim1, true),
		motorL(&htim3, TIM_CHANNEL_2, GPIOB, GPIO_PIN_14, GPIOB, GPIO_PIN_15),
		motorR(&htim3, TIM_CHANNEL_1, GPIOB, GPIO_PIN_12, GPIOB, GPIO_PIN_13),
        // Kp, Ki, Kd, Out_Limit (W_MAX), Int_Limit
        
        // pid_l(8.0f, 0.0f, 0.3f, 30.0f, 20.0f),
        // pid_r(8.0f, 0.0f, 0.3f, 30.0f, 20.0f) {}
        pid_l(0.5f, 1.5f, 0.01f, 30.0f, 20.0f),
        pid_r(0.5f, 1.5f, 0.01f, 30.0f, 20.0f) {}
        void move(float linear_v, float angular_w);

    void setTargetVelocities(float v_left, float v_right);
    void update(void); // Gọi trong ngắt Timer 20ms
    
    // Hàm lấy vận tốc thực tế đã lọc (dùng cho Odometry sau này)
    float getFilteredVelocityL() { return filtered_actual[0]; }
    float getFilteredVelocityR() { return filtered_actual[1]; }
    int32_t getLastDeltaL() { return last_deltaL; }
    int32_t getLastDeltaR() { return last_deltaR; }
    int32_t getTotalPulseL() { return total_pulseL; }
    int32_t getTotalPulseR() { return total_pulseR; }
};

#endif