//
// Created by empir on 19-5-2023.
//

#include "pynq.h"

u8 readPynq(u8* pynqPins) {
	u8 val = 0;
	for (u8 i = 0; i < sizeof(u8); ++i) {
		val += (u8)digitalRead(pynqPins[i]) << i;
		}
	return val;
}
