#include "task_mission.hpp"


Compartment::Compartment(int id, ServoController* servo, Button* btn_conf, Buzzer* buzzer, TFT_Display* tft) {
    this->id = id;
    this->servo = servo;
    this->btn_conf = btn_conf;
    this->buzzer = buzzer;
    this->tft = tft;
    this->currentState = LOCKED;
    this->confirmStartTime = 0;    // Khởi tạo = 0 để tránh giá trị rác

}

void Compartment::setData(const MedicineInfo& med) {
     // Lưu thông tin để hiển thị khi mở nắp
     this->currentMed = med;
}

// void Compartment::update(bool btnPressed) {
//     switch (currentState) {
//         case LOCKED:
//             // Đứng yên, chờ trigger từ Pi để mở
//             // Đứng yên ở closeAngle
//             break;

//         case OPENING:
//             // servo->setAngle(openAngle);
//             // Sau khi servo quay xong (hoặc delay đủ)
//             tft->displayMedicineGuide(currentMed); // Hiển thị thông tin thuốc trên TFT
//             servo->openCompartment(id); // Mở ngăn
//             currentState = WAITING_CONFIRM;
//             break;

//         case WAITING_CONFIRM:
//             if (btnPressed) {
//                 tft->displayConfirmed(); // Hiển thị thông báo đã xác nhận
//                 //buzzer->beep(100); // Kêu buzzer để báo đã nhận được xác nhận
//                 // Sau khi nhận được xác nhận từ người dùng, chuyển sang trạng thái đóng
//                 // buzzer->beep(100);
//                 currentState = CLOSING;
//             }
//             break;

//        case CLOSING:
//         // 1. Thực thi đóng nắp vật lý
//         servo->closeCompartment(id); 

//         // 2. Chuẩn bị chuỗi thông báo
//         char status_msg[100];
        
//         // snprintf giúp định dạng chuỗi có ID và ngăn tràn bộ nhớ
//         int len = snprintf(status_msg, sizeof(status_msg), 
//                         "\r\n[SYSTEM] Ngan %d: DA DONG (Xac nhan thanh cong)\r\n", 
//                         this->id);

//         // 3. Gửi xuống laptop qua cổng USB
//         CDC_Transmit_FS((uint8_t*)status_msg, (uint16_t)len);

//         // 4. Chuyển về trạng thái chờ lệnh tiếp theo
//         currentState = LOCKED;
//         break;
//     }
// }

void Compartment::update(bool btnPressed) {
    // Bắt sườn xung lên: Nút chu kỳ này bằng 1, chu kỳ trước bằng 0
    bool isEdgeRising = (btnPressed == true && lastBtnState == false);
    
    // Lưu lại trạng thái nút chu kỳ này để làm quá khứ cho chu kỳ sau
    lastBtnState = btnPressed;

    switch (currentState) {
        case LOCKED:
            break;

        case OPENING:
            // tft->displayMedicineGuide(currentMed); 
            // servo->openCompartment(id); 
            
            // // CHỐT CHẶN VÀNG: Ngay khi mở ngăn, ép biến quá khứ bằng true 
            // // Điều này đánh lừa Class là "nút đang được đè sẵn rồi, đừng dính sườn xung nữa"
            // lastBtnState = true; 
            
            // currentState = WAITING_CONFIRM;
            // break;
            servo->openCompartment(id);
            lastBtnState = true; 
            if(openMode == PATIENT_MODE)
            {
                tft->displayMedicineGuide(currentMed);
                currentState = WAITING_CONFIRM;
            }
            else
            {
                currentState = WAITING_LOADING_CONFIRM;
            }

            break;

        case WAITING_CONFIRM:
            // CHỈ KHI CÓ SƯỜN XUNG LÊN THỰC SỰ (Thả tay ra rồi bấm lại phát nữa) THÌ MỚI ĐÓNG NẮP
            if (isEdgeRising) { 
                buzzer->beep(200);
                tft->displayConfirmed(); 
                servo->closeCompartment(id); 
                confirmStartTime = xTaskGetTickCount(); // Lưu thời điểm bắt đầu đóng nắp
                currentState = CLOSING;
            }
            break;
        
        case WAITING_LOADING_CONFIRM:

            if(isEdgeRising)
            {
                servo->closeCompartment(id);
                currentState = LOCKED;
            }

            break;

        case CLOSING:
            //servo->closeCompartment(id); 
            if ((xTaskGetTickCount() - confirmStartTime) >= pdMS_TO_TICKS(1000)) { 
                tft->Idle();
                currentState = LOCKED; // 4. Hết 3 giây, chuyển về trạng thái khóa (màn hình Idle)
            }
            break;
    }
}

void Compartment::triggerOpen() {
    openMode = PATIENT_MODE;
    if (currentState == LOCKED) {
        currentState = OPENING;
    }
}

void Compartment::triggerLoadingOpen() {
    openMode = LOADING_MODE;
    if (currentState == LOCKED) {
        currentState = OPENING;
    }
}

void Compartment::triggerClose() {
        servo->closeCompartment(id); 
        currentState = LOCKED;   
        tft->Idle();
}

State Compartment::getState() {
    return currentState;
}

int Compartment::getId() {
    return id;
}
