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
int inPin = 3;
int inPin_2 = 2;

// Variable storage for input value
int val = 0;
int val_2 = 0;

void setup(void)
{
  Serial.begin(115200);


  nfc.begin();
  Serial.println("MF1S50 Reader Demo From Elechouse!");

  // attaches the servo on pin 9 to the servo object
  myservo_9.attach(9);

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

  Serial.println("Starting locking program");

  pinMode(inPin, INPUT);    // declare pushbutton as input
  pinMode(inPin_2, INPUT);    // declare pushbutton as input
  pinMode(13, OUTPUT);
  myservo_9.write(start_deg);

  /** Set normal mode, and disable SAM */
  nfc.SAMConfiguration();
}

void loop(void)
{

  u8 buf[32], sta;

  val = digitalRead(inPin);
  val_2 = digitalRead(inPin_2);

  if (val == HIGH) {
    Serial.println("read push button, going to read nfc...");
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

            val_2 = digitalRead(inPin_2);
            if (val_2 == HIGH) {

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
