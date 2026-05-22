/*
 * Button.cpp
 *
 *  Created on: Apr 24, 2026
 *      Author: huy
 */

#include "Button.hpp"

Button::Button(GPIO_TypeDef* GPIO_Port, uint16_t GPIO_Pin, uint32_t debounceDelay) {
	this->GPIO_Port = GPIO_Port;
	this->GPIO_Pin = GPIO_Pin;
	this->debounceDelay = debounceDelay;
	this->lastDebounceTime = 0;
	this->lastButtonState = HAL_GPIO_ReadPin(this->GPIO_Port, this->GPIO_Pin);
	this->isPressed = false;
}

bool Button::reading() {
	bool currentButtonState = HAL_GPIO_ReadPin(this->GPIO_Port, this->GPIO_Pin);
	bool pressed = false;

	if (currentButtonState != this->lastButtonState) {
		this->lastDebounceTime = HAL_GetTick();
	}

	if ((HAL_GetTick() - this->lastDebounceTime) > this->debounceDelay) {
		if (currentButtonState == GPIO_PIN_RESET ) {
			// if(!this->isPressed) 
			{
				this->isPressed = true;
				pressed = true; // Nút vừa được nhấn
			}
		}
		else {
		this->isPressed = false; // Nút đã được thả
	}
}

	this->lastButtonState = currentButtonState;
	return this->isPressed;
}

bool Button::isHolding() {
	return HAL_GPIO_ReadPin(this->GPIO_Port, this->GPIO_Pin) == GPIO_PIN_RESET;
}


