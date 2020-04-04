# Qwiic I2C Keypad STM32 C++ Library

This is a C++-14 library to ease communication between the [LPC 804](https://www.nxp.com/design/software/development-software/mcuxpresso-software-and-tools/lpcxpresso804-for-the-lpc804-family-of-mcus:OM40001) STM32 development board and the ATtiny85-powered [Sparkfun Qwiic I2C Keypad](https://www.sparkfun.com/products/15290).

View the documentation [here](https://htmlpreview.github.io/?https://github.com/richardrobinson0924/QwiicKeypad/blob/master/docs/html/class_i2_c_1_1_qwiic_keypad.html#pub-static-methods).

## Example
```cpp
#include "fsl_i2c.h"

// if user presses 0, 1, #, then button will be '0', '1', '#'
void onButtonPress(uint8_t button) {
  printf("Button %d was pressed!", buttonIndex);
}

int main(void) {
  InitializeI2C(); // initialize I2C first
  
  // register onButtonPress to execute when... a button is pressed on the IC20 interface
  I2C::QwiicKeypad::GetInstance(I2C0)->AddListener(onButtonPress);
  
  while (true) {
    // continuously poll the keypad and check for errors
    auto status = I2C::QwiicKeypad::GetInstance(I2C0)->Poll();
    if (status != kStatus_Success) {
      // handle error
    }
  }
}
```
