//
// Created by empir on 19-5-2023.
//

#include "pynq.h"
#define sizeof(uint8_t) 8 // Totally not sus at all

uint8_t readPynq(uint8_t* pynqPins) {
	uint8_t val = 0;
	for (uint8_t i = 0; i < sizeof(uint8_t); ++i) {
		val += (uint8_t)digitalRead(pynqPins[i]) << i;	// I love bitshifts
		}
	return val;

}
