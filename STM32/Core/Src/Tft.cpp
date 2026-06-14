#include "Tft.hpp"
#include "stm32f4xx_hal.h"
#include"font.h"

// 1. CONSTRUCTOR: Khởi tạo các chân kết nối
TFT_Display::TFT_Display(SPI_HandleTypeDef* hspi, GPIO_TypeDef* cs_port, uint16_t cs_pin,
                         GPIO_TypeDef* dc_port, uint16_t dc_pin,
                         GPIO_TypeDef* rst_port, uint16_t rst_pin)
    : _hspi(hspi), _cs_port(cs_port), _cs_pin(cs_pin), 
      _dc_port(dc_port), _dc_pin(dc_pin), 
      _rst_port(rst_port), _rst_pin(rst_pin) {}

// 2. GIAO TIẾP TẦNG THẤP: Gửi lệnh và dữ liệu qua SPI
void TFT_Display::write_command(uint8_t cmd) {
    HAL_GPIO_WritePin(_dc_port, _dc_pin, GPIO_PIN_RESET); // DC = 0: Lệnh
    HAL_GPIO_WritePin(_cs_port, _cs_pin, GPIO_PIN_RESET); // Chọn chip
    HAL_SPI_Transmit(_hspi, &cmd, 1, 10);
    HAL_GPIO_WritePin(_cs_port, _cs_pin, GPIO_PIN_SET);   // Thả chip
}

void TFT_Display::write_data(uint8_t data) {
    HAL_GPIO_WritePin(_dc_port, _dc_pin, GPIO_PIN_SET);   // DC = 1: Dữ liệu
    HAL_GPIO_WritePin(_cs_port, _cs_pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(_hspi, &data, 1, 10);
    HAL_GPIO_WritePin(_cs_port, _cs_pin, GPIO_PIN_SET);
}

// 3. KHỞI TẠO MÀN HÌNH
void TFT_Display::init() {
    // Reset cứng màn hình
    HAL_GPIO_WritePin(_rst_port, _rst_pin, GPIO_PIN_RESET);
    HAL_Delay(100); // Đợi 100ms
    HAL_GPIO_WritePin(_rst_port, _rst_pin, GPIO_PIN_SET);
    HAL_Delay(100); 
    // Chuỗi lệnh khởi tạo cơ bản cho
    write_command(0x01); // Software reset
    HAL_Delay(100);
    write_command(0x11); // Sleep out
    HAL_Delay(100);
    write_command(0xB1); // Frame rate control
    HAL_Delay(100);
    write_data(0x01);
    write_data(0x2C);
    write_data(0x2D);
    HAL_Delay(100);
    write_command(0xB2); // Frame rate control
    HAL_Delay(100);
    write_data(0x01);
    write_data(0x2C);
    write_data(0x2D);
    HAL_Delay(100);
    write_command(0xB3); // Frame rate control
    HAL_Delay(100);
    write_data(0x01);
    write_data(0x2C);
    write_data(0x2D);
    write_data(0x01);
    write_data(0x2C);
    write_data(0x2D);
    HAL_Delay(100);
    write_command(0xB4); // Display inversion control
    HAL_Delay(100);
    write_data(0x07);
    HAL_Delay(100);
    write_command(0xC0); // Power control
    HAL_Delay(100);
    write_data(0xA2);
    write_data(0x02);
    write_data(0x84);
    HAL_Delay(100);
    write_command(0xC1); // Power control
    HAL_Delay(100);
    write_data(0xC5);
    HAL_Delay(100);
    write_command(0xC2); // Power control
    HAL_Delay(100);
    write_data(0x0A);
    write_data(0x00);
    HAL_Delay(100);
    write_command(0xC3); // Power control
    HAL_Delay(100);
    write_data(0x8A);
    write_data(0x2A);
    HAL_Delay(100);
    write_command(0xC4); // Power control
    HAL_Delay(100);
    write_data(0x8A);
    write_data(0xEE);
    HAL_Delay(100);
    write_command(0xC5); // VCOM control
    HAL_Delay(100);
    write_data(0x0E);
    HAL_Delay(100);

    write_command(0xE0); // Gamma correction
    HAL_Delay(100);
    write_data(0x02);
    write_data(0x1C);
    write_data(0x07);
    write_data(0x12);
    write_data(0x37);
    write_data(0x32);
    write_data(0x29);
    write_data(0x2D);
    write_data(0x29);
    write_data(0x25);
    write_data(0x2B);
    write_data(0x39);
    write_data(0x00);
    write_data(0x01);
    write_data(0x03);
    write_data(0x10);
    HAL_Delay(100);

    write_command(0xE1); // Gamma correction
    HAL_Delay(100);
    write_data(0x03);
    write_data(0x1D);
    write_data(0x07);
    write_data(0x06);
    write_data(0x2E);
    write_data(0x2C);
    write_data(0x29);
    write_data(0x2D);
    write_data(0x2E);
    write_data(0x2E);
    write_data(0x37);
    write_data(0x3F);
    write_data(0x00);
    write_data(0x00);
    write_data(0x02);
    write_data(0x10);
    HAL_Delay(100);

    write_command(0x36); // Memory Access Control
    HAL_Delay(100);
    write_data(0xC0);

    write_command(0x3A); // Interface Pixel Format
    HAL_Delay(100);
    write_data(0x05); // 16 bits per pixel

    write_command(0x20); // Display inversion off
    HAL_Delay(100);

    setWindow(0, 0, 128, 160); // Set the address window to the entire display
    write_command(0x29); // Display on
    HAL_Delay(100);
}

// 4. QUẢN LÝ VÙNG VẼ (Window)
void TFT_Display::setWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    write_command(0x2A); // Column Address Set
    write_data(x0 >> 8); write_data(x0 & 0xFF);
    write_data(x1 >> 8); write_data(x1 & 0xFF);

    write_command(0x2B); // Page Address Set
    write_data(y0 >> 8); write_data(y0 & 0xFF);
    write_data(y1 >> 8); write_data(y1 & 0xFF);

    write_command(0x2C); // Memory Write
}

// 5. CÁC HÀM VẼ CƠ BẢN
void TFT_Display::draw_pixel(uint16_t x, uint16_t y, uint16_t color) {
    if (x >= 240 || y >= 320) return; // Kiểm tra tràn màn hình , Sửa hàm này
    setWindow(x, y, x, y);
    write_data(color >> 8);
    write_data(color & 0xFF);
}

void TFT_Display::fillScreen(uint16_t color) {
    write_command(0x2C); // Memory Write
    for (uint16_t i = 0; i < 128 * 160; i++) {
        write_data(color >> 8); // Send high byte
        write_data(color & 0xFF); // Send low byte
    }
}

// 6. HIỂN THỊ CHỮ VÀ DỮ LIỆU THUỐC
void TFT_Display::drawChar(uint16_t x, uint16_t y, char c, uint16_t color, uint16_t bg_color) {
    if (c < 32 || c > 127) return; // Kiểm tra ký tự hợp lệ Sửa hàm này
    const uint16_t* char_Data = Font7x10 + (c - 32) * 10; // Lấy dữ liệu bitmap của ký tự
    for(uint8_t i = 0; i < 10; i++) { // Loop through each row of the character
        uint16_t row = char_Data[i];
        for(uint8_t j = 0; j < 7; j++) { // Loop through each column of the character
            if((row << j) & 0x8000) { // Check if the pixel should be drawn
                draw_pixel(x + j, y + i, color); // Draw the pixel
            } else {
                draw_pixel(x + j, y + i, bg_color); // Draw the background pixel
            }
        }
    }
}

void TFT_Display::drawString(uint16_t x, uint16_t y, const std::string& str, uint16_t color, uint16_t bg_color) {
   for (char c : str) {
    drawChar(x, y,  c, color, bg_color); // Sửa hàm này
    x += 8; // Di chuyển tọa độ x sau mỗi ký tự
}
}

void TFT_Display::displayMedicineGuide(const MedicineInfo& med) {
    setWindow(0, 0, 128, 160);
    fillScreen(0x0000); // Nền đen

    // 1. PHẦN TIÊU ĐỀ (Header) - Dùng màu sắc để phân cấp
    // Thay vì thanh bar, ta dùng màu Cyan cho nhãn tĩnh
    //drawString(0, 5, "---NHAN THUOC---", 0x07FF, 0x0000); 
    // drawUTF8String(0, 5, "---NHẬN THUỐC---", 0x07E0, 0x0000);
    drawVietnameseString(0, 5, "-NHẬN THUỐC-", 0x07FF);

    // 2. THÔNG TIN BỆNH NHÂN

    //drawUTF8String(5, 30, "Họ tên:", 0x07FF, 0x0000);
    drawVietnameseString(18, 30, "Bệnh nhân", 0xFFE0);
    // Tên bệnh nhân viết Trắng để nổi bật nhất
    //drawString(5, 45, med.patientName, 0xFFFF, 0x0000); 
    drawVietnameseString(0, 52, med.patientName, 0xFFFF);
    
    //drawUTF8String(5, 70, "Giường:", 0x07FF, 0x0000);
    drawVietnameseString(18, 85, "Giường:", 0x07FF);
    //drawString(65, 70, med.bedNumber, 0xFFFF, 0x0000);
    drawVietnameseString(75, 85, med.bedNumber, 0xFFFF);

    // 3. ĐƯỜNG GẠCH NGANG (Thẩm mỹ & Rõ ràng)
    // Giúp tách biệt phần "Ai nhận" và "Nhận ở đâu"
    // for(int i=10; i<118; i++) draw_pixel(i, 90, 0x7BEF); 

    // 4. CHỈ DẪN NGĂN THUỐC (Màu Xanh Lá - Quan trọng nhất)
    char buf[30];
    sprintf(buf, "MỞ NGĂN SỐ:%d", med.compartmentID); // Dùng sprintf để trộn số ngăn
    //drawString(5, 115, buf, 0x07E0, 0x0000); 
    //drawUTF8String(5, 115, buf, 0x07E0, 0x0000);
    
    
    // 5. HƯỚNG DẪN HÀNH ĐỘNG (Màu Đỏ/Vàng để gây chú ý)
    // Chia làm 2 dòng để chữ không bị tràn viền màn hình
    //drawString(13, 135, "[NHAN NUT OK]", 0xF800, 0x0000); 
    //drawUTF8String(13, 135, "[NHẤN NÚT OK]", 0xF800, 0x0000);
    drawVietnameseString(20, 120, "NHẤN NÚT", 0xF800);
    drawVietnameseString(20, 136, "XÁC NHẬN", 0xF800);
}

void TFT_Display::displayConfirmed() {
    setWindow(0, 0, 128, 160);
    fillScreen(0x0000); // Nền đen
    // Vẽ một khung đơn giản cho thẩm mỹ
    //drawString(25, 80, "DA XAC NHAN", 0x07E0, 0x0000);
    //drawUTF8String(25, 80, "ĐÃ XÁC NHẬN", 0x07E0, 0x0000);
    drawVietnameseString(5, 80, "ĐÃ XÁC NHẬN", 0x07E0);
}

void TFT_Display::Idle() {
    setWindow(0, 0, 128, 160);
    fillScreen(0x0000); // Nền đen
    // drawString(26, 70, "MED-ROBOT", 0x07FF, 0x0000); // Viết chữ đen lên nền trắng
    // drawString(20, 90, "IN PROGRESS", 0xFFE0, 0x0000);
    drawVietnameseString(14, 70, "MED-ROBOT", 0x07FF);
    drawVietnameseString(10, 100, "IN PROGRESS", 0xFFE0);
}

int findGlyph(uint32_t unicode)
{
    for(int i = 0; i < FONT_NUM_CHARS; i++)
    {
        if(font_unicode[i] == unicode)
            return i;
    }

    return -1;
}

void TFT_Display::drawGlyph( uint16_t x, uint16_t y, int glyphIndex, uint16_t color, uint16_t bg)
{
    if(glyphIndex < 0)
        return;

    for(int row = 0; row < FONT_CHAR_H; row++)
    {
        uint8_t bits = font_bitmaps[glyphIndex][row];

        for(int col = 0; col < FONT_CHAR_W; col++)
        {
            if(bits & (0x80 >> col))
            {
                draw_pixel(x + col, y + row, color);
            }
            else
            {
                draw_pixel(x + col, y + row, bg);
            }
        }
    }
}

uint32_t utf8_next(const char*& p)
{
    uint8_t c = (uint8_t)*p++;

    if(c < 0x80)
    {
        return c;
    }

    if((c & 0xE0) == 0xC0)
    {
        uint8_t c2 = (uint8_t)*p++;

        return ((c & 0x1F) << 6)
             | (c2 & 0x3F);
    }

    if((c & 0xF0) == 0xE0)
    {
        uint8_t c2 = (uint8_t)*p++;
        uint8_t c3 = (uint8_t)*p++;

        return ((c & 0x0F) << 12)
             | ((c2 & 0x3F) << 6)
             | (c3 & 0x3F);
    }

    return '?';
}

void TFT_Display::drawUTF8String(uint16_t x, uint16_t y, const std::string& str, uint16_t color, uint16_t bg)
{
    const char* p = str.c_str();

    while(*p)
    {
        uint32_t unicode = utf8_next(p);

        int idx = findGlyph(unicode);

        if(idx >= 0)
        {
            drawGlyph(x, y, idx, color, bg);
        }
        else
        {
            drawChar(x, y, '?', color, bg);
        }

        x += FONT_CHAR_W;
    }
}

// void TFT_Display::drawVietnameseString(uint16_t x, uint16_t y, char* str, uint16_t color) {
//     uint16_t cursor_x = x;
//     unsigned char char_offset;
    
//     while (*str) {
//         // Lấy index ký tự từ bảng tra của IOT47
//         unsigned char index = UTF8_GetAddr((unsigned char*)str, &char_offset);
        
//         if (index != '?') {
//             // Duyệt qua 16 hàng của font
//             for (int row = 0; row < 16; row++) {
//                 // Duyệt qua 8 cột (font 8x16)
//                 for (int col = 0; col < 8; col++) {
//                     // Kiểm tra pixel tại vị trí (col, row) của ký tự index
//                     if (read_font16(col, row, index)) {
//                         // Gọi hàm vẽ pixel của chính class TFT_Display này
//                         draw_pixel(cursor_x + col, y + row, color);
//                     }
//                 }
//             }
//             cursor_x += 8; // Dịch con trỏ sang phải 8 pixel
//         }
//         str += char_offset; // Nhảy sang ký tự tiếp theo trong chuỗi UTF-8
//     }
// }

void TFT_Display::drawVietnameseString(uint16_t x, uint16_t y, const char* str, uint16_t color) {
    uint16_t cursor_x = x;
    unsigned char char_offset;
    
    while (*str) {
        unsigned char index = UTF8_GetAddr((unsigned char*)str, &char_offset);
        
        if (index != '?') {
            // Lấy độ rộng thực tế của ký tự từ bảng size_font16
            unsigned char char_width = size_font16[index]; 
            
            for (int row = 0; row < 16; row++) {
                // Duyệt theo độ rộng thực tế (char_width) thay vì số 8 cố định
                for (int col = 0; col < char_width; col++) {
                    if (read_font16(col, row, index)) {
                        draw_pixel(cursor_x + col, y + row, color);
                    }
                }
            }
            cursor_x += char_width; // Dịch con trỏ theo độ rộng thực
        }
        str += char_offset;
    }
}