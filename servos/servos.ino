#include <Servo.h>

Servo myservo_9;  // create servo object to control a servo
Servo myservo_13;  // create servo object to control a servo
// twelve servo objects can be created on most boards

int pos = 0;    // variable to store the servo position
int pos_13 = 0;    // variable to store the servo position

int start_deg = 180;
int last_deg = 0;
bool lock_down = false;

int inPin = 3;   // choose the input pin (for a pushbutton)
int inPin_2 = 2;   // choose the input pin (for a pushbutton)
int val = 0;     // variable for reading the pin status
int val_2 = 0;     // variable for reading the pin status

void setup() {
  // initialize the serial communication:
  Serial.begin(115200);
  myservo_9.attach(9);  // attaches the servo on pin 9 to the servo object
  pinMode(inPin, INPUT);    // declare pushbutton as input
  pinMode(inPin_2, INPUT);    // declare pushbutton as input
  pinMode(13, OUTPUT);
  myservo_9.write(start_deg);
}

void loop() {
  
  val = digitalRead(inPin);
  val_2 = digitalRead(inPin_2);
  
  if (val == HIGH) {
    digitalWrite(13, HIGH);
    if (lock_down) {
      for (int j = 0; j <= start_deg; j++) {
        myservo_9.write(j);
        delay(50);
      }
      lock_down = false;
    } else {
      int i = 0;
      for (i = start_deg; i >= 0; i--) {
        myservo_9.write(i);
        delay(50);
      
        val_2 = digitalRead(inPin_2);
        if (val_2 == HIGH) {
          reverse_lock();
          break;
        } 
      }
      
      int down_deg = myservo_9.read();
      Serial.println((String)down_deg);
      if (down_deg == 0) {
        lock_down = true; 
      }
    }
  }
  digitalWrite(13, LOW);

//  val = digitalRead(inPin);
//  if (val == LOW) {
    delay(200);
//    myservo_9.write(start_deg); 
//  }
}

void reverse_lock() {
  last_deg = myservo_9.read();

  for (int k = last_deg; k <= start_deg; k++) {
    myservo_9.write(k);
    delay(50);
  }
  
  lock_down = false;
}

