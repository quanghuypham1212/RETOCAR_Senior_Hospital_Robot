/*
 * task_health.hpp
 *
 *  Created on: May 6, 2026
 *      Author: huy
 */

#ifndef INC_TASK_COMM_TRANS_HPP_
#define INC_TASK_COMM_TRANS_HPP_

#ifndef TELEMETRY_HPP
#define TELEMETRY_HPP

#include "main.h"
#include "usb_device.h"
#include "usbd_cdc_if.h"
#include "Batery.hpp"
#include "robotcontroller.hpp"
#include "Imu.hpp"
#include "Filter_imu.hpp"

// Định nghĩa ID để phía Raspberry Pi phân biệt gói tin
#define MSG_TYPE_BATTERY  0x04
#define MSG_TYPE_ODOM     0x05
#define MSG_TYPE_BUTTON   0x06

extern Battery robot_batery; // Khai báo biến robot_batery để sử dụng trong Telemetry
extern Imu imu; // Khai báo biến imu để sử dụng trong Telemetry
extern ImuFilter imuFilter; // Khai báo biến imuFilter để sử dụng trong Telemetry
extern RobotController myRobot; // Khai báo biến myRobot để sử dụng trong Telemetry
// Gói tin Pin - Gửi 1 giây/lần
typedef struct __attribute__((packed)) {
    uint8_t header;    // 0xAA
    uint8_t type;      // 0x04
    uint8_t len;
    int16_t battery;   // Pin (V * 100)
    uint8_t checksum;
    uint8_t footer;    // 0x55
} BatteryPacket_t;

typedef struct __attribute__((packed)) {
    uint8_t header;       // 0xAA
    uint8_t type;         // 0x06
    uint8_t len;          // 1 (1 byte cho trạng thái nút)
    uint8_t button_state; // 1 là nhấn/giữ, 0 là thả
    uint8_t checksum;
    uint8_t footer;       // 0x55
} ButtonPacket_t;


// Gói tin Odometry - Gửi 10ms hoặc 20ms/lần (trong Task Motor)
typedef struct __attribute__((packed)) {
    uint8_t header;    // 0xAA
    uint8_t type;      // 0x05
    uint8_t len;
    float yaw;       // Góc Yaw (Degree * 100)
    // int32_t veloc_L;     
    // int32_t veloc_R;     
    int32_t encL;      // Tổng xung Encoder Trái tích lũy
    int32_t encR;      // Tổng xung Encoder Phải tích lũy
    uint8_t checksum;
    uint8_t footer;    // 0x55
} OdomPacket_t;

class Telemetry {
public:
    // Hàm gửi Pin (Gọi trong Task Battery)
    static void sendBattery();
    
    // Hàm gửi Odom (Gọi trong Task Motor)
    static void sendOdom();
    
    // Hàm gửi trạng thái nút (Gọi trong Task Mission)
    static void sendButton(bool button_state);

private:
    static uint8_t calculateChecksum(uint8_t *data, uint8_t len);
};

#endif



#endif /* INC_TASK_COMM_TRANS_HPP_ */
