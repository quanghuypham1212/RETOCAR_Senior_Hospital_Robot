#include "robotcontroller.hpp"
#include "main.h"

void RobotController::setTargetVelocities(float v_left, float v_right) {
    velocity_target[0] = v_left;
    velocity_target[1] = v_right;
}

void RobotController::update(void) {
    const float max_v_change = ACCELERATION_LIMIT * SAMPLING_TIME_S;

	// 1. Đọc Encoder (Huy thay bằng Timer tương ứng của mình)
	int32_t deltaL = EncoderL.update();
	int32_t deltaR = EncoderR.update();

    total_pulseL += deltaL;
    total_pulseR += deltaR;

    last_deltaL = deltaL;
    last_deltaR = deltaR;
	int32_t deltas[2] = {deltaL, deltaR};

    for(int i = 0; i < 2; i++) {
       
        float actual_v = (float)deltas[i] * TICKS_TO_RAD_PER_S;
        filtered_actual[i] = (FILTER_ALPHA * filtered_actual[i]) + ((1.0f - FILTER_ALPHA) * actual_v);
    }
    
    // 1. Ramp Logic: Tăng/giảm tốc từ từ để xe không bị giật
    for(int i = 0; i < 2; i++) {
        float diff = velocity_target[i] - ramp_target[i];
        ramp_target[i] += fmaxf(-max_v_change, fminf(max_v_change, diff));
    }

    // 3. Tính toán PID Output (Đầu ra là rad/s mong muốn đã bù sai số)
    outputL = pid_l.compute(ramp_target[0], filtered_actual[0], SAMPLING_TIME_S);
    outputR = pid_r.compute(ramp_target[1], filtered_actual[1], SAMPLING_TIME_S);

    // 4. Điều khiển Motor thông qua hàm normalize
    // Giả sử xe Huy chạy tối đa 20 rad/s
    motorL.setVelocity(outputL);
	motorR.setVelocity(outputR);  
}

void RobotController::move(float linear_v, float angular_w)
{
    WheelVelocities target_speeds = kinematics.getWheelSpeeds(linear_v, angular_w);
    setTargetVelocities(target_speeds.left, target_speeds.right);
}

