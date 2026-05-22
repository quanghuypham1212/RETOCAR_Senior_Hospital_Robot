#ifndef TFT_DISPLAY_HPP
#define TFT_DISPLAY_HPP

#include "main.h"
#include <string>

// Cấu trúc dữ liệu thuốc để quản lý tập trung
struct MedicineInfo {
    // 1. Thông tin định danh (Quan trọng nhất để xác nhận)
    // std::string patientName;    // Tên bệnh nhân (Ví dụ: "NGUYEN VAN A")
    // std::string bedNumber;      // Số giường (Ví dụ: "102-A")
    
    // // 2. Thông tin điều khiển robot
    // uint8_t compartmentID;      // Số ngăn thuốc (1, 2, 3...) để STM32 biết mở Servo nào
    // // 4. Trạng thái (Tùy chọn - giúp quản lý hành trình)
    // bool isDelivered;           // Đã giao thành công hay chưa
    int compartmentID;
    char patientName[32]; // Mảng cố định 32 byte
    char bedNumber[16];   // Mảng cố định 16 byte
};

class TFT_Display {
public:
    // Constructor: Khởi tạo với các chân cắm thực tế trên STM32
    TFT_Display(SPI_HandleTypeDef* hspi, GPIO_TypeDef* cs_port, uint16_t cs_pin,
                GPIO_TypeDef* dc_port, uint16_t dc_pin, GPIO_TypeDef* rst_port,uint16_t rst_pin);

    void init();                                      // Khởi động màn hình
    void fillScreen(uint16_t color);                  // Xóa/Tô màu toàn màn hình
    void drawString(uint16_t x, uint16_t y, const std::string& str, uint16_t color, uint16_t bg_color); // Vẽ chuỗi văn bản
    void draw_pixel(uint16_t x, uint16_t y, uint16_t color);
    void drawChar(uint16_t x, uint16_t y, char c, uint16_t color, uint16_t bg_color); // Vẽ ký tự đơn lẻ
    // Hàm quan trọng: Hiển thị hướng dẫn sử dụng thuốc
    void displayMedicineGuide(const MedicineInfo& med);
    void displayConfirmed();
    void Idle(); // Màn hình chờ khi không có lệnh nào (Có thể hiển thị logo hoặc thông tin hệ thống ở đây)

private:
    SPI_HandleTypeDef* _hspi;
    GPIO_TypeDef* _cs_port; uint16_t _cs_pin;
    GPIO_TypeDef* _dc_port; uint16_t _dc_pin;
	GPIO_TypeDef* _rst_port; uint16_t _rst_pin;


    void write_command(uint8_t cmd);
    void write_data(uint8_t data);
    void setWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
};

#endif