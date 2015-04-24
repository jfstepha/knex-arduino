# Introduction #

This page serves as the documentation for the main branch of the Knex Arduino code.  (There is also a self\_test branch not documented here).

See http://code.google.com/p/knex-ros/ for more complete documentation on how to implement this on a real robot.

This code was developed for an Arduino Uno, though it may work on other models as well.  It depends on the ROS package rosserial.  See http://www.ros.org/wiki/rosserial/Tutorials for setup.

## Program Details ##

### Subscribed Topics: ###
  * lmotor\_cmd - Float32 - Left wheel motor drive.
  * rmotor\_cmd - Float32 - Right wheel motor drive.
    * 0 = coast
    * 1 to 255 = forward
    * -255 to -1 = reverse
    * > 255 or < -255 = brake
  * servo1\_cmd to servo5\_cmd - Int16 - Servo angle in degrees

### Published Messages: ###
  * lwheel - Int16 - Left wheel rotary encoder total number of ticks.
  * rwheel - Int16 - Right wheel rotary encoder total number of ticks.
  * range - Int16 - Analog value of the range finder pin.
  * scope - Int16 - Analog value of the scope pin.
  * arduino\_debug - String - Debug output from the arduino

### Pinout ###
| pin | I/O | Name | comments |
|:----|:----|:-----|:---------|
| A2  | Out | RFwd | Right wheel forward |
| 4   | Out | RRev | Right wheel reverse |
| 6   | Out | REna | Right wheel enable (PWM for speed control) |
| 8   | Out | LFwd | Left wheel forward |
| 7   | Out | LRev | Left wheel reverse |
| 5   | Out | LEna | Left wheel enable (PWM for speed control) |
| 10  | Out | Servo1 | Servo 1 |
| 11  | Out | Servo2 | Servo 2 |
| 12  | Out | Servo3 | Servo 3 |
| 13  | Out | Servo4 | Servo 4 |
| 14  | Out | Servo5 | Servo 5 |
| 2   | In  | LWheel | Left wheel rotary encoder |
| 3   | In  | RWheel | Right wheel rotary encoder |
| A5  | In  | Range  | IR Range finder input |
| A4  | In  | Scope  | Analog oscilloscope-like input |

Note that these pins were selected because there are restrictions on which pins can be connected to servos and which can be connected to as pulse width modulation.

### H-bridge truth table ###

| FWD | REV | ENA | Function |
|:----|:----|:----|:---------|
| L   | L   | L   | coast |
| L   | L   | H   | coast |
| H   | L   | L   | forward |
| H   | L   | H   | coast (for pwm) |
| L   | H   | L   | reverse |
| L   | H   | H   | coast (for pwm)  |
| H   | H   | L   | brake |