/** include library */
#include "nfc.h"
#include <Servo.h>

/** define a nfc class */
NFC_Module nfc;

// create servo object to control a servo
Servo myservo_9;

// Servo variables
int start_deg = 180;
int fully_down_deg = 90;
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

// IDs for the Fob, Card, and Android App
// ID for nfc uuid used to lock system
String FOB_ID = "23039104123";
String CARD_ID = "21216612187";
String PHONE_ID = "20381955";
String UNLOCK_ID = "";

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
  String UUID = "";

  pirReadVal = digitalRead(pirPin);

  if (pirReadVal == HIGH) {
    Serial.println("read prox sensor, going to read nfc...");
//    digitalWrite(13, HIGH);

    /** Polling the mifar card, buf[0] is the length of the UID */
    sta = nfc.InListPassiveTarget(buf);

    /** check state and UID length */
    if (sta) {

      /** the card may be Mifare Classic card, try to read the block */
      UUID = get_nfc_uuid(buf);

      if (lock_down) {
        Serial.println("LOCK IS DOWN, checking if uuid is correct");
        // can only be unlocked with UNLOCK_ID
        if (UUID == UNLOCK_ID) {
          unlock_bike();
          set_unlock_id("");
          delay(1000);
        } else {
          Serial.println("404, not your bike to unlock");
        }
      } else {
        if (lock_bike()) {
          lock_down = true;
          set_unlock_id(UUID); 
        }
        delay(1000);
      }

    } else {
       Serial.println("no NFC, loop back");
    }
  }

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

void unlock_bike() {
  Serial.println("Unlocking bike");
  for (int j = fully_down_deg; j <= start_deg; j++) {

    myservo_9.write(j);
    delay(50);
  }
  lock_down = false;
}

bool lock_bike() {
  Serial.println("Locking bike");
  for (int i = start_deg; i >= fully_down_deg; i--) {

    myservo_9.write(i);
    delay(50);

    intSwitchReadVal = digitalRead(intSwitchPin);
    if (intSwitchReadVal == HIGH) {

      Serial.println("----INTERFERENCE DETECTED-----");
      delay(100);
      reverse_lock();
      return false;
    }
  }

  return true;
}

String get_nfc_uuid(u8 buf[32]) {

  String readUUID = "";
  readUUID = nfc.getdec(buf + 1, buf[0]);
  
  uint8_t responseLength = 32;
  u8 response[32];
  bool didReadPhone;
  // AID given in APDU must match with Android app
  // in order to authenticate the message exchange
  u8 selectApdu[] = { 0x00, /* CLA */
                      0xA4, /* INS */
                      0x04, /* P1  */
                      0x00, /* P2  */
                      0x05, /* Length of AID  */
                      0xF2, 0x22, 0x22, 0x22, 0x22, /* AID defined on Android App */ };
  
  if (readUUID == FOB_ID) {
  
    Serial.println("GOT THE FOB");
    return FOB_ID;
 
  } else if (readUUID == CARD_ID) {
  
    Serial.println("CARD JUST READ");
    return CARD_ID;

  } else {
    // Not fob or card, check if phone
    didReadPhone = nfc.P2PInitiatorTxRx(selectApdu, sizeof(selectApdu), response, &responseLength);
    String convertedID = "";
  
    if (didReadPhone) {
  
      convertedID = nfc.ConvertHexId(response, responseLength);
      // nfc.PrintHexChar(response, responseLength);
  
      if (convertedID == PHONE_ID) {
  
        Serial.println("GOT THE PHONE DUDE!!");
        return PHONE_ID;
      }
    }
  }
}

void set_unlock_id(String unlock_id) {
  Serial.println("Changing unlock id to : " + unlock_id);
  UNLOCK_ID = unlock_id;
}

