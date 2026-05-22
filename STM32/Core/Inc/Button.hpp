/*
 * Button.hpp
 *
 *  Created on: Apr 24, 2026
 *      Author: huy
 */

#ifndef SRC_BUTTON_HPP_
#define SRC_BUTTON_HPP_

#include "main.h"
class Button {
private:
	GPIO_TypeDef* GPIO_Port;
	uint16_t GPIO_Pin;
	uint32_t lastDebounceTime; // Thời gian cuối cùng nút được thay đổi trạng thái
	uint32_t debounceDelay; // Thời gian chống rung (debounce) tính bằng ms
	bool lastButtonState; // Trạng thái nút trước đó
	bool isPressed; // Trạng thái hiện tại của nút sau khi đã qua xử lý chống rung
public:
	Button(GPIO_TypeDef* GPIO_Port, uint16_t GPIO_Pin, uint32_t debounceDelay = 50);
	bool reading(); // Kiểm tra nếu nút đã được nhấn (đã qua xử lý chống rung)
	bool isHolding(); // Kiểm tra nếu nút đang được giữ (đã nhấn và chưa thả)
};

#endif /* SRC_BUTTON_HPP_ */
