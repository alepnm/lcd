#ifndef __GLOBAL_H_
#define __GLOBAL_H_

#define SetBit(port,pin)        GPIO_SetBits(port, pin);
#define ClrBit(port,pin)        GPIO_ResetBits(port, pin);
#define AffBit(port,pin,state)  GPIO_WriteBit(port, pin, state);
#define ChangeBit(port,pin)     GPIO_ToggleBits(port, pin);

#endif