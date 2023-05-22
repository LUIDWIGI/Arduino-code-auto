#include <Arduino.h>		// Arduino library
#include "RF24.h"			// Comm module library
#include "printf.h"			// Enables printf by passing STDOUT to the Arduino serial library
#include "SPI.h"			// SPI Support
#include "pynq.h"			// PYNQ abstraction
#include "Servo.h"			// Servo motor support

typedef byte pin;			// Define pin data type as byte for better readability

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

const u16 servoDelay = 15;	// TODO: Find the right delay for the servo
u32 servoActivate;			// Hold the time at which the servo started moving

// Structs that holds the data of the single package that the controller sends
struct Package {
	u8 direction = 0;		// Byte that holds the value of the direction switch
	u16 speed = 0;			// Byte that holds the value of the speed joystick passthrough
	u16 steering = 0;		// Byte that holds the value of the steering joystick.
};
Package package; 			// Structs that holds the data of the single package that the controller sends

Servo turningServo;			// Create a servo object

// Wireless comm
RF24 radio(9, 10);  // using pin 9 for the CE pin, and pin 10 for the CSN pin
const byte addresses[][6] = {"1Node", "2Node"};	// Define addresses as a byte

void setup() {
	// Open transmission pipes
	radio.openWritingPipe(addresses[0]);
	radio.openReadingPipe(1,addresses[1]);

	turningServo.attach(servo);							// Attach the servo to the servo pin
}

void loop() {
// Read values of the wireless comms
radio.startListening();										// Start listening for data
if (radio.available()) {									// If data is available
	radio.read(&package, sizeof(Package)); 			// Read the controller data into Package struct
}

// Apply received data to car

// Apply direction switch state
	switch (package.direction) {
		case 0:												// Middle position (idle)
			analogWrite(VinADC, 0);					// Set speed to 0
			break;
		case 1:												// Up position
			analogWrite(package.direction, 1);		// Change H-bridge direction to forward
			analogWrite(VinADC, package.speed);		// Set speed to Joystick value
			break;
		case 2:												// Down position
			analogWrite(package.direction, 0);		// Change H-bridge direction to backward
			analogWrite(VinADC, package.speed);		// Set speed to Joystick value
			break;
	default:
		break;
	}

// Servo steering
if ((millis()-servoActivate) < servoDelay) {				// Check if the servo is done steering
	u8 steering = map(package.steering, 0, 1023, 0, 180);  	// Map the joystick steering value to degrees
	turningServo.write(steering);                    	// Write the angle to the servo
	servoActivate = millis();								// Note that the servo started turning
}

// Send PYNQ speed readout as a byte value
radio.stopListening();										// Stop listening for controller data
u8 pynqReading = readPynq((u8*)pynq);				// Save the pynq reading into a byte
radio.write(&pynqReading, sizeof(pynqReading));		// Send that byte
}