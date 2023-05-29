#include <Arduino.h>		// Arduino library
#include "RF24.h"			// Comm module library
#include "printf.h"			// Enables printf by passing STDOUT to the Arduino serial library
#include "SPI.h"			// SPI Support
#include "pynq.h"			// PYNQ abstraction
#include "Servo.h"			// Servo motor support

typedef uint8_t pin;		// Define pin data type as byte for better readability

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

const uint16_t servoDelay = 15;
uint32_t servoActivate;			// Hold the time at which the servo started moving
uint32_t lastInRange;
const uint16_t inRangeTimeout = 1000;


// Structs that holds the data of the single package that the controller sends
struct Package {
	uint8_t flipSwitch = 0;		// Byte that holds the value of the direction switch
	uint16_t speed = 0;			// Byte that holds the value of the speed joystick passthrough
	uint16_t steering = 0;		// Byte that holds the value of the steering joystick.
	bool inRange = false;		// Boolean for checking if the controller is still in range
};
Package package; 			// Structs that holds the data of the single package that the controller sends

Servo turningServo;			// Create a servo object

// Wireless comm
RF24 radio(9, 10);  // using pin 9 for the CE pin, and pin 10 for the CSN pin
const uint8_t addresses[][6] = {"1Node", "2Node"};	// Define addresses as a byte

void setup() {
	// Open transmission pipes
	radio.openWritingPipe(addresses[0]);
	radio.openReadingPipe(1,addresses[1]);

	turningServo.attach(servo);							// Attach the servo to the servo pin
}

void loop() {
	package.inRange = false;
// Read values of the wireless comms
radio.startListening();										// Start listening for data
if (radio.available()) {									// If data is available
	radio.read(&package, sizeof(Package)); 			// Read the controller data into Package struct
}

// Apply received data to car
int16_t speed = map(package.speed, 0, 1023, -1023, 1023);

if(package.inRange) {
	lastInRange = millis();
}

if(millis()-lastInRange >= inRangeTimeout) {
	speed = 0;
}

if (package.flipSwitch) {
	if (speed < 0) {
		analogWrite(direction, 0);
		analogWrite(VinADC, abs(speed));
	} else if (speed > 0) {
		analogWrite(direction, 1023);
		analogWrite(VinADC, speed);
	} else if (speed == 0) {
		analogWrite(VinADC, 0);
	}
}
else if (package.flipSwitch == 2) {
	analogWrite(direction, 0);
	analogWrite(VinADC, 1023);
}
else {
	analogWrite(VinADC, 0);
}

// Servo steering
if ((millis()-servoActivate) < servoDelay) {				// Check if the servo is done steering
	uint8_t steering = map(package.steering, 0, 1023, 30, 150);  	// Map the joystick steering value to degrees
	turningServo.write(steering);                    	// Write the angle to the servo
	servoActivate = millis();								// Note that the servo started turning
}

// Send PYNQ speed readout as a byte value
radio.stopListening();										// Stop listening for controller data
uint8_t pynqReading = readPynq((uint8_t*)pynq);				// Save the pynq reading into a byte
radio.write(&pynqReading, sizeof(pynqReading));		// Send that byte
}