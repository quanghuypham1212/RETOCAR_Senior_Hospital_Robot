#include "task_comm_trans.hpp"
#include <string.h>
#include "main.h"
#include "usb_device.h"
#include "usbd_cdc_if.h"


// Gửi dữ liệu Pin
void Telemetry::sendBattery() {
    BatteryPacket_t packet;
    packet.header = 0xAA;
    packet.type = MSG_TYPE_BATTERY;
    packet.len = 2; // 2 byte cho battery 
    packet.battery = robot_batery.getPercentage();

    // Checksum tính từ byte Type đến hết Battery (4 byte)
    packet.checksum = calculateChecksum((uint8_t*)&packet + 1, 4); 
    packet.footer = 0x55;
    CDC_Transmit_FS((uint8_t*)&packet, sizeof(BatteryPacket_t));
    
}

void Telemetry::sendButton(bool button_state) {
    ButtonPacket_t packet;
    packet.header = 0xAA;
    packet.type = MSG_TYPE_BUTTON;
    packet.len = 1; // 1 byte cho trạng thái nút
    packet.button_state = button_state;

    // Checksum tính từ byte Type đến hết button_state (2 byte)
    packet.checksum = calculateChecksum((uint8_t*)&packet + 1, 3); 
    packet.footer = 0x55;
    CDC_Transmit_FS((uint8_t*)&packet, sizeof(ButtonPacket_t));
}

// Gửi dữ liệu Encoder và Yaw
void Telemetry::sendOdom() {
    OdomPacket_t packet;
    packet.header = 0xAA;
    packet.type = MSG_TYPE_ODOM;
    packet.len = 12; // 4 byte cho yaw + 4 byte cho encL + 4 byte cho encR
    packet.yaw = imuFilter.get_final_yaw(); 
    packet.encL = myRobot.getLastDeltaL(); // Lấy delta encoder trái từ RobotController
    packet.encR = myRobot.getLastDeltaR(); // Lấy delta encoder phải từ RobotController 
    // packet.veloc_L = myRobot.getFilteredVelocityL();
    // packet.veloc_R = myRobot.getFilteredVelocityR();

    // Checksum tính từ byte Type đến hết Encoder R (11 byte)
    packet.checksum = calculateChecksum((uint8_t*)&packet + 1, 14); 
    packet.footer = 0x55;

    CDC_Transmit_FS((uint8_t*)&packet, sizeof(OdomPacket_t));
}

uint8_t Telemetry::calculateChecksum(uint8_t *data, uint8_t len) {
    uint8_t sum = 0;
    for (uint8_t i = 0; i < len; i++) {
        sum += data[i];
    }
    return sum;
}