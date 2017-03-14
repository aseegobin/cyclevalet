/** include library */
#include "nfc.h"
#include <Servo.h>

/** define a nfc class */
NFC_Module nfc;

// create servo object to control a servo
Servo myservo_9;

int start_deg = 180;
int fully_down_deg = 0;
int last_deg = 0;
bool lock_down = false;

// Input pin declaration
int pirPin = 3;
int intSwitchPin = 2;

// Variable storage for input value
int pirReadVal = 0;
int intSwitchReadVal = 0;

//the time we give the sensor to calibrate (10-60 secs according to the datasheet)
int calibrationTime = 30;

void setup(void)
{
  Serial.begin(115200);

  pinMode(pirPin, INPUT);    // declare pirPin as input
  pinMode(intSwitchPin, INPUT);    // declare pushbutton as input
  nfc.begin();
  Serial.println("MF1S50 Reader Demo From Elechouse!");

  uint32_t versiondata = nfc.get_version();
  while (!versiondata) {
    Serial.println("Didn't find PN53x board");
    Serial.println("trying again...");
    delay(1000);
    nfc.begin();
  }

  // Got ok data, print it out!
  Serial.print("Found chip PN5"); Serial.println((versiondata >> 24) & 0xFF, HEX);
  Serial.print("Firmware ver. "); Serial.print((versiondata >> 16) & 0xFF, DEC);
  Serial.print('.'); Serial.println((versiondata >> 8) & 0xFF, DEC);

  //give the sensor some time to calibrate
  Serial.print("calibrating sensor ");
    for(int i = 0; i < calibrationTime; i++){
      Serial.print(".");
      delay(1000);
    }
  Serial.println(" done");
  Serial.println("SENSOR ACTIVE");

  Serial.println("Starting locking program");

  pinMode(13, OUTPUT);
  // attaches the servo on pin 9 to the servo object
  myservo_9.attach(9);
  myservo_9.write(start_deg);

  /** Set normal mode, and disable SAM */
  nfc.SAMConfiguration();
}

void loop(void)
{

  u8 buf[32], sta;

  pirReadVal = digitalRead(pirPin);
  intSwitchReadVal = digitalRead(intSwitchPin);

  if (pirReadVal == HIGH) {
    Serial.println("read prox sensor, going to read nfc...");
    digitalWrite(13, HIGH);

    /** Polling the mifar card, buf[0] is the length of the UID */
    sta = nfc.InListPassiveTarget(buf);

    /** check state and UID length */
    if (sta && buf[0] == 4) {

      /** the card may be Mifare Classic card, try to read the block */
      String UUID = "";
      UUID = nfc.getdec(buf + 1, buf[0]);

      if (UUID == "23039104123") {

        Serial.println("GOT THE FOB, engaging system");

        if (lock_down) {

          Serial.println("Unlocking bike");
          for (int j = 0; j <= start_deg; j++) {

            myservo_9.write(j);
            delay(50);
          }
          lock_down = false;
        } else {

          Serial.println("Locking bike");
          for (int i = start_deg; i >= 0; i--) {

            myservo_9.write(i);
            delay(50);

            intSwitchReadVal = digitalRead(intSwitchPin);
            if (intSwitchReadVal == HIGH) {

              Serial.println("----INTERFERENCE DETECTED-----");
              delay(100);
              reverse_lock();
              break;
            }
          }

          int down_deg = myservo_9.read();
          //          Serial.println((String)down_deg);
          if (down_deg == fully_down_deg) {

            lock_down = true;
          }
        }
      } else if (UUID == "21216612187") {

        Serial.println("CARD JUST READ");
      } else {

        Serial.println(UUID);
      }

    } else {
      Serial.println("no NFC, loop back");
    }
  }

  digitalWrite(13, LOW);
  delay(200);
}

void reverse_lock() {
  last_deg = myservo_9.read();

  for (int k = last_deg; k <= start_deg; k++) {
    myservo_9.write(k);
    delay(50);
  }

  lock_down = false;
}
