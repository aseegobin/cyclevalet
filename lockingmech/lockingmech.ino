/** include library */
#include "nfc.h"
#include <Servo.h>

/** define a nfc class */
NFC_Module nfc;

// create servo object to control a servo
Servo myservo_9;

int pos_9 = 0;    // variable to store the pin 9 servo position

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
  Serial.print("Found chip PN5"); Serial.println((versiondata>>24) & 0xFF, HEX);
  Serial.print("Firmware ver. "); Serial.print((versiondata>>16) & 0xFF, DEC);
  Serial.print('.'); Serial.println((versiondata>>8) & 0xFF, DEC);

  /** Set normal mode, and disable SAM */
  nfc.SAMConfiguration();
}

void loop(void)
{
  u8 buf[32],sta;

  /** Polling the mifar card, buf[0] is the length of the UID */
  sta = nfc.InListPassiveTarget(buf);

  /** check state and UID length */
  if(sta && buf[0] == 4){

    /** the card may be Mifare Classic card, try to read the block */
    Serial.println();
    String UUID = "";
    UUID = nfc.getdec(buf+1, buf[0]);
    if (UUID == "23039104123") {
      Serial.println("GOT THE FOB");
      myservo_9.write(20);
    } else if (UUID == "21216612187") {
      Serial.println("CARD JUST READ");
      myservo_9.write(0);
    } else {
      Serial.println(UUID);
    }

    // Delay a bit before reading again
    delay(1000);
  }
}
