#ifndef __GLOBAL_H_
#define __GLOBAL_H_

#define SetBit(port,pin)        HAL_GPIO_WritePin(port, pin, GPIO_PIN_SET);
#define ClrBit(port,pin)        HAL_GPIO_WritePin(port, pin, GPIO_PIN_RESET);
#define AffBit(port,pin,state)  HAL_GPIO_WritePin(port, pin, state);
#define ChangeBit(port,pin)     HAL_GPIO_TogglePin(port, pin);

#endif