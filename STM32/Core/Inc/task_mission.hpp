#ifndef COMPARTMENT_H
#define COMPARTMENT_H

#include "main.h"
#include <string>
#include "Servo.hpp"
#include "Button.hpp"
#include "buzzer.h"
#include "Tft.hpp"
#include "usbd_cdc_if.h"
#include "usb_device.h"
#include "main.h"

// Enum trạng thái để quản lý FSM
enum State {
    LOCKED,
    OPENING,
    WAITING_CONFIRM,
    CLOSING
};

class Compartment {
private:
    State currentState;
    int id;
    ServoController* servo; 
    Button* btn_conf;
    Buzzer* buzzer;
    TFT_Display* tft;
    MedicineInfo currentMed; // Lưu thông tin thuốc để hiển thị khi mở nắp
    bool lastBtnState = false; // Biến lưu trạng thái nút ở lần cập nhật trước, dùng để phát hiện sườn xung

public:
    Compartment(int id, ServoController* servo, Button* btn_conf, Buzzer* buzzer, TFT_Display* tft); 

    // Hàm quan trọng nhất: Máy trạng thái của riêng ngăn này
    void update(bool btnPressed);
    
    void setData(const MedicineInfo& med); // Lưu thông tin thuốc vào ngăn (để hiển thị sau này)
    // Hàm để Pi kích hoạt mở nắp
    void triggerOpen();
    
    // Các hàm lấy thông tin để báo cáo lên Pi
    State getState();
    int getId();
};

#endif