#include <Arduino.h>	// Arduino library
#include "RF24.h"		// Comm module library
#include "printf.h"		// Enables printf by passing STDOUT to the Arduino serial library
#include "SPI.h"		// SPI Support
#include "pynq.h"		// PYNQ abstraction

typedef byte pin;

// Pin definition
const pin servo = 6;		// Pin for controlling the pwn of the servo
const pin direction = A1;	// Pin for changing the direction of the H-bridge
const pin VinADC = A0;		// Passthrough ADC value of speed potentiometer
const pin pynq[] = {		// Pins that are connected to the pynq (LSB -> MSB)
		0,
		1,
		2,
		3,
		4,
		5,
		7,
		8};

// Structs that holds the data of the single package that the controller sends
struct Package {
	u8 direction = 0;		// Byte that holds the value of the direction switch
	u8 speed = 0;			// Byte that holds the value of the speed joystick passthrough
	u8 stearing = 0;		// Byte that holds the value of the stearing joystick.
};
Package package; 			// Create a package that has the structure of package

// Wireless comm
RF24 radio(9, 10);  // using pin 9 for the CE pin, and pin 10 for the CSN pin
const byte addresses[][6] = {"1Node", "2Node"};	// Define addresses as a byte

void setup() {
	// Open transmission pipes
	radio.openWritingPipe(addresses[0]);
	radio.openReadingPipe(1,addresses[1]);
}

void loop() {
// Read values of the wireless comms
radio.startListening();
if (radio.available()) {
	radio.read(&package, sizeof(Package)); // Read the controller data into Package struct
}

// Send PYNQ speed readout as a byte value
radio.stopListening();									// Stop listening for controller data
u8 pynqReading = readPynq((u8*)pynq);			// Save the pynq reading into a byte
radio.write(&pynqReading, sizeof(pynqReading));	// Send that byte
}