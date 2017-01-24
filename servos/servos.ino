#include <Servo.h>

Servo myservo_9;  // create servo object to control a servo
Servo myservo_13;  // create servo object to control a servo
// twelve servo objects can be created on most boards

int pos = 0;    // variable to store the servo position
int pos_13 = 0;    // variable to store the servo position

int inPin = 2;   // choose the input pin (for a pushbutton)
int val = 0;     // variable for reading the pin status

void setup() {
  myservo_9.attach(9);  // attaches the servo on pin 9 to the servo object
  pinMode(inPin, INPUT);    // declare pushbutton as input
  pinMode(13, OUTPUT);
}

void loop() {
  
  val = digitalRead(inPin);  // read input value
  if (val == HIGH) {         // check if the input is HIGH (button released)
    digitalWrite(13, HIGH);
    myservo_9.write(90);              // tell servo to go to position in variable 'pos'
  } else {
    myservo_9.write(0);              // tell servo to go to position in variable 'pos'
    digitalWrite(13, LOW);
  }
}



