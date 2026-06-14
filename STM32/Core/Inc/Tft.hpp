#ifndef TFT_DISPLAY_HPP
#define TFT_DISPLAY_HPP

#include "main.h"
#include <string>
#include "viet_font.h"
#include "font_VN.h"

// Cấu trúc dữ liệu thuốc để quản lý tập trung
struct MedicineInfo {
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
    void drawGlyph(uint16_t x, uint16_t y, int glyphIndex, uint16_t color, uint16_t bg); // Vẽ glyph từ font tiếng Việt
    void drawUTF8String(uint16_t x,uint16_t y,const std::string& str,uint16_t color, uint16_t bg);
    void drawVietnameseString(uint16_t x, uint16_t y, const char* str, uint16_t color);
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