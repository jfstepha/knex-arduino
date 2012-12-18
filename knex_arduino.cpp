// Do not remove the include below
//
#include "knex_arduino.h"
#include <ros.h>
#include <std_msgs/String.h>
#include <std_msgs/Int16.h>
#include <std_msgs/Float32.h>
#include <Time.h>
#include <Servo.h>

#define P_LFWD 8
#define P_LREV 11
#define P_LENA 9

#define P_RFWD 12
#define P_RREV 13
#define P_RENA 10

// EN_BAR: is the enable pin active low or active high?
//   EN_BAR 0 = pin is active high
//   EN_BAR 1 = pin is active low
//       (drive the pin low to make the motor spin)

#define EN_BAR 0

// define these to true to invert the direction of rotation of the wheel
#undef INVERT_LWHEEL
#undef INVERT_RWHEEL

// define this flag to true to use servos
#undef SERVOS

#ifdef SERVOS

#define SERVO1 4
#define SERVO2 5
#define SERVO3 7
#define SERVO4 10
#define SERVO5 14

#endif

// the A's are connected to interrupts, while the B's are just read with digital_reads
// interrupt 0 = pin 2
// interrupt 1 = pin 3
#define LWHEEL_A_INT 0
#define LWHEEL_B 4

#define RWHEEL_A_INT 1
#define RWHEEL_B 5

// define TWO_PHASE if the wheel encoder has two phases (ie an A and B pin).
// undef TWO_PHASE if there is only one signal to the wheel encoder (use the driven direction)

#define TWO_PHASE

#define LOOP_DLY 5  // in msec

#define FWD 1
#define REV -1

#define RANGE A5
#define SCOPE A4

#ifdef SERVOS
Servo servo1;
Servo servo2;
Servo servo3;
Servo servo4;
Servo servo5;
#endif

int lcoder = 0;
int rcoder = 0;
int lprev = 0;
int rprev = 0;
int ldir = 0;
int rdir = 0;

ros::NodeHandle nh;

std_msgs::String msg_debug;
ros::Publisher debug_pub("arduino_debug", &msg_debug);
std_msgs::Int16 msg_lwheel;
std_msgs::Int16 msg_rwheel;
std_msgs::Int16 msg_range;
std_msgs::Int16 msg_scope;
ros::Publisher lwheel_pub("lwheel", &msg_lwheel);
ros::Publisher rwheel_pub("rwheel", &msg_rwheel);
ros::Publisher range_pub("range", &msg_range);
ros::Publisher scope_pub("scope", &msg_scope);

char debug_str[80] = "blank";
int tick_no = 0;
int ticks_since_beat = 0;


//The setup function is called once at startup of the sketch

//////////////////////////////////////////////////////////////////////
void lfwd(int speed=255) {
//////////////////////////////////////////////////////////////////////
	sprintf(debug_str, "lfwd %d", speed);
	msg_debug.data = debug_str;
	debug_pub.publish( &msg_debug );

	digitalWrite( P_LFWD, LOW );
	digitalWrite( P_LREV, HIGH);
	if (EN_BAR) {
		analogWrite( P_LENA, constrain( 255-speed, 0, 255 ) );
	} else {
		analogWrite( P_LENA, constrain(speed, 0, 255 ) );
	}
#ifdef INVERT_LWHEEL
	ldir = REV;
#else
	ldir = FWD;
#endif

}

//////////////////////////////////////////////////////////////////////
void lrev(int speed=255) {
//////////////////////////////////////////////////////////////////////
	sprintf(debug_str, "lrev %d", speed);
	msg_debug.data = debug_str;
	debug_pub.publish( &msg_debug );

	digitalWrite( P_LFWD, HIGH);
	digitalWrite( P_LREV, LOW );
	if (EN_BAR) {
		analogWrite( P_LENA, constrain( 255-speed, 0, 255 ) );
	} else {
		analogWrite( P_LENA, constrain(speed, 0, 255 ) );
	}
#ifdef INVERT_LWHEEL
	ldir = FWD;
#else
	ldir = REV;
#endif
}

//////////////////////////////////////////////////////////////////////
void lcoast() {
//////////////////////////////////////////////////////////////////////
	sprintf(debug_str, "lcoast");
	msg_debug.data = debug_str;
	debug_pub.publish( &msg_debug );

	digitalWrite( P_LFWD, LOW );
	digitalWrite( P_LREV, LOW );
	digitalWrite( P_LENA, LOW );
	ldir=0;
}

//////////////////////////////////////////////////////////////////////
void lbrake() {
//////////////////////////////////////////////////////////////////////
	sprintf(debug_str, "lbrake");
	msg_debug.data = debug_str;
	debug_pub.publish( &msg_debug );

	digitalWrite( P_LFWD, HIGH );
	digitalWrite( P_LREV, HIGH );
	digitalWrite( P_LENA, LOW);
	ldir = 0;
}

//////////////////////////////////////////////////////////////////////
void rfwd( int speed=255) {
//////////////////////////////////////////////////////////////////////
	sprintf(debug_str, "rfwd %d", speed);
	msg_debug.data = debug_str;
	debug_pub.publish( &msg_debug );

	digitalWrite( P_RFWD, LOW  );
	digitalWrite( P_RREV, HIGH);
	if (EN_BAR) {
		analogWrite( P_RENA, constrain( 255-speed, 0, 255 ) );
	} else {
		analogWrite( P_RENA, constrain(speed, 0, 255 ) );
	}
#ifdef INVERT_RWHEEL
	rdir = REV;
#else
	rdir = FWD;
#endif
}

//////////////////////////////////////////////////////////////////////
void rrev(int speed=255) {
//////////////////////////////////////////////////////////////////////
	sprintf(debug_str, "rrev %d", speed);
	msg_debug.data = debug_str;
	debug_pub.publish( &msg_debug );

	digitalWrite( P_RFWD, HIGH);
	digitalWrite( P_RREV, LOW  );
	if (EN_BAR) {
		analogWrite( P_RENA, constrain( 255-speed, 0, 255 ) );
	} else {
		analogWrite( P_RENA, constrain(speed, 0, 255 ) );
	}
#ifdef INVERT_RWHEEL
	rdir = FWD;
#else
	rdir = REV;
#endif
}


//////////////////////////////////////////////////////////////////////
void rcoast() {
//////////////////////////////////////////////////////////////////////
	sprintf(debug_str, "rcoast");
	msg_debug.data = debug_str;
	debug_pub.publish( &msg_debug );

	digitalWrite( P_RFWD, LOW );
	digitalWrite( P_RREV, LOW );
	digitalWrite( P_RENA, LOW );
	rdir = 0;
}

//////////////////////////////////////////////////////////////////////
void rbrake() {
//////////////////////////////////////////////////////////////////////
	sprintf(debug_str, "rbrake");
	msg_debug.data = debug_str;
	debug_pub.publish( &msg_debug );

	digitalWrite( P_RFWD, HIGH );
	digitalWrite( P_RREV, HIGH );
	digitalWrite( P_RENA, LOW);
	rdir = 0;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// callbacks
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
void RMotorCallBack( const std_msgs::Float32& motor_msg) {
//////////////////////////////////////////////////////////////////////
	sprintf(debug_str, "LMotorCallback %0.3f", motor_msg.data);
	msg_debug.data = debug_str;
	debug_pub.publish( &msg_debug );

    if (motor_msg.data > 255 || motor_msg.data < -255) {
    	rbrake();
    } else if (motor_msg.data == 0) {
    	rcoast();
#ifdef INVERT_RWHEEL
    } else if (motor_msg.data < 0) {
    	rfwd(abs(motor_msg.data));
    } else {
    	rrev(motor_msg.data);
    }
#else
    } else if (motor_msg.data < 0) {
    	rrev(abs(motor_msg.data));
    } else {
    	rfwd(motor_msg.data);
    }
#endif
}

//////////////////////////////////////////////////////////////////////
void LMotorCallBack( const std_msgs::Float32& motor_msg) {
//////////////////////////////////////////////////////////////////////

    if (motor_msg.data > 255 || motor_msg.data < -255) {
    	lbrake();
    } else if (motor_msg.data == 0) {
    	lcoast();
#ifdef INVERT_LWHEEL
    } else if (motor_msg.data < 0) {
    	lfwd(abs(motor_msg.data));
    } else {
    	lrev(motor_msg.data);
    }
#else
    } else if (motor_msg.data < 0) {
    	lrev(abs(motor_msg.data));
    } else {
    	lfwd(motor_msg.data);
    }
#endif
}

#ifdef SERVOS
//////////////////////////////////////////////////////////////////////////
void Servo1CallBack( const std_msgs::Int16& servo_msg) {
//////////////////////////////////////////////////////////////////////////
	sprintf(debug_str, "servo1 %d", servo_msg.data);
	msg_debug.data = debug_str;
	debug_pub.publish( &msg_debug );

	servo1.write( constrain( servo_msg.data, 0, 179) );
}
//////////////////////////////////////////////////////////////////////////
void Servo2CallBack( const std_msgs::Int16& servo_msg) {
//////////////////////////////////////////////////////////////////////////
	sprintf(debug_str, "servo2 %d", servo_msg.data);
	msg_debug.data = debug_str;
	debug_pub.publish( &msg_debug );

	servo2.write( constrain( servo_msg.data, 0, 179) );
}
////////////////////////////////////////////////////////////////////////
void Servo3CallBack( const std_msgs::Int16& servo_msg) {
////////////////////////////////////////////////////////////////////////
	sprintf(debug_str, "servo3 %d", servo_msg.data);
	msg_debug.data = debug_str;
	debug_pub.publish( &msg_debug );

	servo3.write( constrain( servo_msg.data, 0, 179) );
}
////////////////////////////////////////////////////////////////////////
void Servo4CallBack( const std_msgs::Int16& servo_msg) {
////////////////////////////////////////////////////////////////////////
	sprintf(debug_str, "servo4 %d", servo_msg.data);
	msg_debug.data = debug_str;
	debug_pub.publish( &msg_debug );

	servo4.write( constrain( servo_msg.data, 0, 179) );
}
////////////////////////////////////////////////////////////////////////
void Servo5CallBack( const std_msgs::Int16& servo_msg) {
////////////////////////////////////////////////////////////////////////
	sprintf(debug_str, "servo5 %d", servo_msg.data);
	msg_debug.data = debug_str;
	debug_pub.publish( &msg_debug );

	servo5.write( constrain( servo_msg.data, 0, 179) );
}
#endif

ros::Subscriber<std_msgs::Float32> rmotor_sub("rmotor_cmd", &RMotorCallBack);
ros::Subscriber<std_msgs::Float32> lmotor_sub("lmotor_cmd", &LMotorCallBack);

#ifdef SERVOS
ros::Subscriber<std_msgs::Int16> servo1_sub("servo1_cmd", &Servo1CallBack);
ros::Subscriber<std_msgs::Int16> servo2_sub("servo2_cmd", &Servo2CallBack);
ros::Subscriber<std_msgs::Int16> servo3_sub("servo3_cmd", &Servo3CallBack);
ros::Subscriber<std_msgs::Int16> servo4_sub("servo4_cmd", &Servo4CallBack);
ros::Subscriber<std_msgs::Int16> servo5_sub("servo5_cmd", &Servo5CallBack);
#endif

//////////////////////////////////////////////////////////////////////
void doLEncoder(){
//////////////////////////////////////////////////////////////////////
	// note that you could get 4x the resolution by catching rising and fallling edges of A and B.
#ifdef TWO_PHASE
	if (digitalRead(LWHEEL_B)) {
		lcoder += 1;
	} else {
		lcoder -= 1;
	}
#else
   lcoder += ldir;
#endif

}

//////////////////////////////////////////////////////////////////////
void doREncoder(){
//////////////////////////////////////////////////////////////////////
#ifdef TWO_PHASE
	if (digitalRead(RWHEEL_B)){
		rcoder += 1;
	} else {
		rcoder -= 1;
	}
#else
    rcoder += rdir;
#endif
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
void setup()
//////////////////////////////////////////////////////////////////////
{
	nh.initNode();
	nh.advertise(debug_pub);
	nh.advertise(lwheel_pub);
	nh.advertise(rwheel_pub);
	nh.advertise(range_pub);
	nh.advertise(scope_pub);
	nh.subscribe(rmotor_sub);
	nh.subscribe(lmotor_sub);

#ifdef SERVOS
	nh.subscribe(servo1_sub);
	nh.subscribe(servo2_sub);
	nh.subscribe(servo3_sub);
	nh.subscribe(servo4_sub);
	nh.subscribe(servo5_sub);

	servo1.attach(SERVO1);
	servo2.attach(SERVO2);
	servo3.attach(SERVO3);
	servo4.attach(SERVO4);
	servo5.attach(SERVO5);
#endif

	  pinMode(P_LFWD, OUTPUT);
	  pinMode(P_LREV, OUTPUT);
	  pinMode(P_LENA, OUTPUT);

	  pinMode(P_RFWD, OUTPUT);
	  pinMode(P_RREV, OUTPUT);
	  pinMode(P_RENA, OUTPUT);


	  //
	  // FWD REV ENA
	  // L   L   L    coast
	  // L   L   H    coast
	  // H   L   L    fwd
	  // H   L   H    coast (for pwm)
	  // L   H   L    reverse
	  // L   H   H    coast (for pwm)
	  // H   H   L    brake`


	  pinMode( RANGE, INPUT );

	  pinMode(LWHEEL_B, INPUT);
	  pinMode(RWHEEL_B, INPUT);

	  attachInterrupt(LWHEEL_A_INT, doLEncoder, RISING);   //init the interrupt mode
	  attachInterrupt(RWHEEL_A_INT, doREncoder, RISING);

}


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void loop()
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
{
	nh.spinOnce();
	ticks_since_beat++;

	if(ticks_since_beat > (1000 / LOOP_DLY) ) {
    	tick_no++;
    	sprintf(debug_str, "tick %d", tick_no);
    	msg_debug.data = debug_str;
    	debug_pub.publish( &msg_debug );
    	ticks_since_beat = 0;
	}

	// try without interrupts

//    if (rprev != digitalRead(RWHEEL)) {
//    	rcoder += rdir;
//    	rprev = digitalRead(RWHEEL);
//    }
//    if (lprev != digitalRead(LWHEEL)){
//    	lcoder += ldir;
//    	lprev = digitalRead(LWHEEL);
//    }
	msg_lwheel.data = lcoder;
	msg_rwheel.data = rcoder;
	msg_range.data = analogRead( RANGE );
	msg_scope.data = analogRead( SCOPE );
	lwheel_pub.publish( &msg_lwheel );
	rwheel_pub.publish( &msg_rwheel );
	range_pub.publish( &msg_range );
	scope_pub.publish( &msg_scope );


	nh.spinOnce();

    delay(LOOP_DLY);
}
