#include "task_comm.hpp"
#include <string.h>

UartParser::UartParser(RingBuffer* rb, Compartment* comps, float* targetVx, float* targetWz) 
    : _rb(rb), _compartments(comps), _targetVx(targetVx), _targetWz(targetWz) {
    
    _state = WAIT_HEADER;
    memset(_payloadBuffer, 0, sizeof(_payloadBuffer));

}

void UartParser::process() {
    uint8_t byte;

    // Đọc tất cả byte đang có trong RingBuffer
    while (_rb->dequeue(&byte)) {
        switch (_state)
        {
        case WAIT_HEADER:
            if (byte == PKT_HEADER) {
                _state = WAIT_TYPE;
                _calcChecksum = 0; // Reset checksum khi bắt đầu gói mới
            }
            break;

        case WAIT_TYPE:
            _pktType = byte;
            if(byte == 0x02 || byte == 0x03) {
                // Nếu là loại lệnh hợp lệ, tiếp tục nhận độ dài
                _calcChecksum += byte;
                _state = WAIT_LEN;
            } else {
                // Loại lệnh không hợp lệ, bỏ gói
                _state = WAIT_HEADER;
            }
            break;

        case WAIT_LEN:
            _pktLen = byte;
            if ((this->_pktType == 0x02 && byte == 11) || (this->_pktType == 0x03 && byte == 8)) {
                // Nếu độ dài hợp lệ với loại lệnh, tiếp tục nhận payload
                _calcChecksum += byte;
                _payloadIndex = 0;
                _state = WAIT_PAYLOAD;
            } else {
                // Độ dài không hợp lệ, bỏ gói
                _state = WAIT_HEADER;
            }
            break;
        
        case WAIT_PAYLOAD:
            _payloadBuffer[_payloadIndex] = byte;
            _calcChecksum += byte;
            _payloadIndex++;
            if (_payloadIndex >= _pktLen) {
                _state = WAIT_CHECKSUM; // Đã nhận đủ payload, chuyển sang nhận checksum
            }
            break;
        
        case WAIT_CHECKSUM:
            _recvChecksum = byte;
            if((_calcChecksum & 0xFF) == _recvChecksum) {
                this->_state = WAIT_FOOTER;
            }
            else {
                // Checksum không hợp lệ, có thể gửi phản hồi lỗi hoặc bỏ qua
                // const char* errChecksum = "ERR: Checksum khong hop le\r\n";
                // CDC_Transmit_FS((uint8_t*)errChecksum, (uint16_t)strlen(errChecksum));
                _state = WAIT_HEADER; // Bỏ gói và reset về chờ header
            }   

            break;
        
        case WAIT_FOOTER:
            if (byte == PKT_FOOTER) {
                // Kiểm tra checksum
                    // Nếu checksum hợp lệ, thực thi lệnh
                    executeCommand(_pktType, (uint8_t*)_payloadBuffer, _pktLen);
                }
        
            // Dù có nhận được footer đúng hay không, gói đã kết thúc nên reset về trạng thái chờ header
            _state = WAIT_HEADER;
            break;
        
        default:
            _state = WAIT_HEADER; // Nếu rơi vào trạng thái không xác định, reset về chờ header
            break;
        }
        
    }
}

void UartParser::executeCommand(uint8_t cmdType, uint8_t* payload, uint8_t len) {
   if(cmdType == CMD_OPEN_COMPARTMENT) {
        if (len != sizeof(PayloadOpen)) {
            const char* errLen = "ERR: Do dai payload khong hop le cho lenh OPEN_COMPARTMENT\r\n";
            CDC_Transmit_FS((uint8_t*)errLen, (uint16_t)strlen(errLen));
            return;
        }
        PayloadOpen* data = (PayloadOpen*)payload;
        uint8_t id = data->id;
        if (id < 1 || id > 4) {
            const char* errId = "ERR: ID ngan khong hop le (1-4)\r\n";
            CDC_Transmit_FS((uint8_t*)errId, (uint16_t)strlen(errId));
            return;
        }
        MedicineInfo medInfo;
        medInfo.compartmentID = id;
        strncpy(medInfo.patientName, data->patientName, sizeof(medInfo.patientName)-1);
        strncpy(medInfo.bedNumber, data->bedNumber, sizeof(medInfo.bedNumber)-1);
        _compartments[id-1].setData(medInfo); // Lưu thông tin thuốc vào ngăn tương ứng
        _compartments[id-1].triggerOpen(); // Kích hoạt mở ngăn
    } else if (cmdType == CMD_SET_VELOCITY) {
        if (len != sizeof(PayloadVelocity)) {
            const char* errLen = "ERR: Do dai payload khong hop le cho lenh SET_VELOCITY\r\n";
            CDC_Transmit_FS((uint8_t*)errLen, (uint16_t)strlen(errLen));
            return;
        }
        PayloadVelocity* velData = (PayloadVelocity*)payload;
        // Chuyển đổi từ int16_t sang float nếu cần thiết
        *_targetVx = velData->vx; // Ví dụ: nếu gửi vx=150 thì sẽ thành 1.5 m/s
        *_targetWz = velData->wz; // Ví dụ: nếu gửi wz=50 thì sẽ thành 0.5 rad/s
    } else {
        const char* errCmd = "ERR: Loai lenh khong ho tro\r\n";
        CDC_Transmit_FS((uint8_t*)errCmd, (uint16_t)strlen(errCmd));
    }
}