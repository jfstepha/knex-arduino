// Do not remove the include below
/// some extra line
//
#include "knex_arduino.h"

#define P_RFWD A2
#define P_RREV 4
#define P_RENA 6

#define P_LFWD A0
#define P_LREV 7
#define P_LENA 5

	  // FWD REV ENA
	  // L   L   L    coast
	  // L   L   H    coast
	  // H   L   L    fwd
	  // H   L   H    coast (for pwm)
	  // L   H   L    reverse
	  // L   H   H    coast (for pwm)
	  // H   H   L    brake`

void lfwd(int speed=255) {
	digitalWrite( P_LFWD, HIGH );
	digitalWrite( P_LREV, LOW );
	analogWrite( P_LENA, constrain( 255-speed, 0, 255 ) );
}

void lrev(int speed=255) {
	digitalWrite( P_LFWD, LOW );
	digitalWrite( P_LREV, HIGH );
	analogWrite( P_LENA, constrain( 255-speed, 0, 255 ) );
}

void lcoast() {
	digitalWrite( P_LFWD, LOW );
	digitalWrite( P_LREV, LOW );
	digitalWrite( P_LENA, LOW );
}

void lbrake() {
	digitalWrite( P_LFWD, HIGH );
	digitalWrite( P_LREV, HIGH );
	digitalWrite( P_LENA, LOW);
}

void rfwd( int speed=255) {
	digitalWrite( P_RFWD, HIGH );
	digitalWrite( P_RREV, LOW );
	analogWrite( P_RENA, constrain( 255-speed, 0, 255 ) );
}

void rrev(int speed=255) {
	digitalWrite( P_RFWD, LOW );
	digitalWrite( P_RREV, HIGH );
	analogWrite( P_RENA, constrain( 255-speed, 0, 255 ) );
}


void rcoast() {
	digitalWrite( P_RFWD, LOW );
	digitalWrite( P_RREV, LOW );
	digitalWrite( P_RENA, LOW );
}

void rbrake() {
	digitalWrite( P_RFWD, HIGH );
	digitalWrite( P_RREV, HIGH );
	digitalWrite( P_RENA, LOW);
}

void setup()
{
	  pinMode(P_LFWD, OUTPUT);
	  pinMode(P_LREV, OUTPUT);
	  pinMode(P_LENA, OUTPUT);

	  pinMode(P_RFWD, OUTPUT);
	  pinMode(P_RREV, OUTPUT);
	  pinMode(P_RENA, OUTPUT);
}



// The loop function is called in an endless loop
void loop()
{
	digitalWrite( 13, HIGH );
	lfwd();
	delay(1000);

	digitalWrite( 13, LOW );
	lbrake();
	delay(1000);

	lrev();
	delay(1000);

	lbrake();
	delay(1000);

	rfwd();
	delay(1000);

	rbrake();
	delay(1000);

	rrev();
	delay(1000);

	rbrake();
	delay(1000);


}
