/*
 * task_motor.hpp
 *
 *  Created on: May 6, 2026
 *      Author: huy
 */

#ifndef INC_TASK_MOTOR_HPP_
#define INC_TASK_MOTOR_HPP_


#include "main.h"
#include "RobotController.hpp"
#include "Imu.hpp"
#include "Encoder.hpp"
#include "Filter_imu.hpp"

// Mang các khai báo extern ra ngoài Class
extern RobotController myRobot;
extern Imu imu;
extern ImuFilter imuFilter;
extern Encoder encoderL;
extern Encoder encoderR;

extern float targetVx;
extern float targetWz;

class TaskMotor {
public:
    // Entry point cho FreeRTOS (phải là static)
    static void MotorTask_Execute(void); 
};


#endif /* INC_TASK_MOTOR_HPP_ */
