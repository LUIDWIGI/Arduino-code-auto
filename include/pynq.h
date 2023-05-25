//
// Created by empir on 19-5-2023.
//
#include <Arduino.h>
#ifndef ARDUINO_CODE_AUTO_PYNQ_H
#define ARDUINO_CODE_AUTO_PYNQ_H
/* Reads the 8 pynq pins as a byte of data
 * @param
 * u8 pynqPins[8]: Byte array of pins which holds the pins where the PYNQ is connected to from LSB[0] to MSB[7]
 * Outputs a byte which is value that the pynq outputs
 */
uint8_t readPynq(uint8_t pynqPins[8]);
#endif //ARDUINO_CODE_AUTO_PYNQ_H
