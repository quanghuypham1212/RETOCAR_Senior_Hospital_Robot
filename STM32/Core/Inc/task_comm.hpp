#ifndef UART_PARSER_HPP
#define UART_PARSER_HPP

#include "main.h"
#include "usb_ring_buffer.hpp"
#include "task_mission.hpp"
#include <string>
#include <stdio.h>

#define PKT_HEADER 0xAA
#define PKT_FOOTER 0x55
#define CMD_OPEN_COMPARTMENT 0x02
#define CMD_SET_VELOCITY     0x03
#define CMD_LOADING_COMPARTMENT 0x07 // Lệnh mở 4 ngăn thuốc tại vị trí xuất phát
#define CMD_CLOSE_COMPARTMENT 0x08 // Lệnh đóng ngăn thuốc tại vị trí xuất phát

#pragma pack(push, 1)

struct PayloadOpen {
    uint8_t id;            // 1 byte (1-4)
    char bedNumber[5];     // 5 byte
    char patientName[20];  // 5 byte
};

struct PayloadVelocity {
    float vx; // Vận tốc thẳng V_x
    float wz; // Vận tốc góc Omega_z
};

struct PayloadLoading {
    uint8_t id;
};

struct PayloadClose {
    uint8_t id;
};

#pragma pack(pop)

class UartParser {
public:
    // Constructor nhận vào RingBuffer và mảng các ngăn thuốc
    UartParser(RingBuffer* rb, Compartment* comps, float* targetVx, float* targetWz); 
    
    // Hàm xử lý chính, gọi trong Communication_Task (20ms/lần)
    void process();

private:
    RingBuffer* _rb;
    Compartment* _compartments;

    float* _targetVx; // Con trỏ đến biến lưu vận tốc thẳng mục tiêu
    float* _targetWz; // Con trỏ đến biến lưu vận tốc góc mục tiêu

    char _payloadBuffer[256]; // Bộ nhớ đệm để chứa chuỗi lệnh tạm thời

    enum ParseState {
        WAIT_HEADER,
        WAIT_TYPE,
        WAIT_LEN,
        WAIT_PAYLOAD,
        WAIT_CHECKSUM,
        WAIT_FOOTER
    };

    ParseState _state;
    uint8_t _pktType;
    uint8_t _pktLen;
    uint8_t _payloadIndex;
    uint8_t _calcChecksum;
    uint8_t _recvChecksum;
    
    // Hàm thực thi lệnh sau khi bóc tách thành công
    void executeCommand(uint8_t cmdType, uint8_t* payload, uint8_t len);
};

#endif