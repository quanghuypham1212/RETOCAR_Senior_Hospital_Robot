#ifndef RING_BUFFER_HPP
#define RING_BUFFER_HPP

#include <stdint.h>

class RingBuffer {
private:    
    uint8_t buffer[256];          // Mảng dữ liệu
    uint16_t size = 256;            // Kích thước buffer
    volatile uint16_t head = 0;   // Chỉ số ghi vào
    volatile uint16_t tail = 0;   // Chỉ số đọc ra

public:
    // Constructor: Khởi tạo buffer với kích thước tùy chọn
    RingBuffer();
    
    // Thêm dữ liệu vào (tương đương uart_rx_enqueue)
    bool enqueue(uint8_t byte);

    // Lấy dữ liệu ra (tương đương CDC_Receive_Byte)
    bool dequeue(uint8_t* byte);

    // Kiểm tra buffer có trống không
    bool isEmpty() const;

    // Xóa sạch buffer
    void flush();

    uint16_t getHead() const { return head; }
    uint16_t getTail() const { return tail; }
};

#endif