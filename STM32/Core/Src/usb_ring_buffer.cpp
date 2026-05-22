#include "usb_ring_buffer.hpp"

// RingBuffer::RingBuffer(uint16_t buffer_size ) 
//     : size(buffer_size), head(0), tail(0) {
//     buffer = uint8_t[size]; // Cấp phát bộ nhớ động
// }
RingBuffer::RingBuffer() : size(256), head(0), tail(0) {
    // buffer đã được khởi tạo sẵn với kích thước 256 byte
}


bool RingBuffer::enqueue(uint8_t byte) {
    uint16_t next_head = (head + 1) % size;
    if (next_head != tail) { // Kiểm tra nếu buffer chưa đầy
        buffer[head] = byte;
        head = next_head;
        return true;
    }
    return false; // Buffer đầy, không thể thêm
}

bool RingBuffer::dequeue(uint8_t* byte) {
    if (head == tail) {
        return false; // Buffer rỗng
    }
    *byte = buffer[tail];    tail = (tail + 1) % size;
    return true;
}

bool RingBuffer::isEmpty() const {
    return (head == tail);
}

void RingBuffer::flush() {
    head = tail = 0;
}
