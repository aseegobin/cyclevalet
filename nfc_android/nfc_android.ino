/** include library */
#include "nfc.h"

/** define a nfc class */
NFC_Module nfc;

void setup(void)
{
  Serial.begin(115200);
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
  Serial.print("Found chip PN5"); Serial.println((versiondata>>24) & 0xFF, HEX); 
  Serial.print("Firmware ver. "); Serial.print((versiondata>>16) & 0xFF, DEC); 
  Serial.print('.'); Serial.println((versiondata>>8) & 0xFF, DEC);
  
  /** Set normal mode, and disable SAM */
  nfc.SAMConfiguration();
}

void loop()
{
  bool success;
  
  uint8_t responseLength = 32;

  u8 buf[32],sta;
  
  /** Polling the mifar card, buf[0] is the length of the UID */
  sta = nfc.InListPassiveTarget(buf);
  
  Serial.println("Waiting for an ISO14443A card");
  
  // set shield to inListPassiveTarget
  success = sta;

  if(success) {
   
     Serial.println("Found something!");

     // AID given in APDU must match with Android app
     // in order to authenticate the message exchange
    u8 selectApdu[] = { 0x00, /* CLA */
                              0xA4, /* INS */
                              0x04, /* P1  */
                              0x00, /* P2  */
                              0x05, /* Length of AID  */
                              0xF2, 0x22, 0x22, 0x22, 0x22, /* AID defined on Android App */ };

                              
    u8 response[32];  
     
    success = nfc.P2PInitiatorTxRx(selectApdu, sizeof(selectApdu), response, &responseLength);
    
    if(success) {
      
      Serial.print("responseLength: "); Serial.println(responseLength);
      String UUID = "";
      
      UUID = nfc.ConvertHexId(response, responseLength); 
      nfc.PrintHexChar(response, responseLength);
      
      if (UUID == "20381955") {

        Serial.println("GOT THE PHONE DUDE!!");
      }
    }
    else {
     
      Serial.println("Failed sending SELECT AID"); 
    }
  }
  else {
   
    Serial.println("Didn't find anything!");
  }

  delay(1000);
}
