//the time we give the sensor to calibrate (10-60 secs according to the datasheet)
int calibrationTime = 30;

int pirPin = 2;    //the digital pin connected to the PIR sensor's output
int ledPin = 13;
int readVal = 0;


/////////////////////////////
//SETUP
void setup(){
  Serial.begin(9600);
  pinMode(pirPin, INPUT);
  pinMode(ledPin, OUTPUT);
  digitalWrite(pirPin, LOW);

  //give the sensor some time to calibrate
  Serial.print("calibrating sensor ");
    for(int i = 0; i < calibrationTime; i++){
      Serial.print(".");
      delay(1000);
    }
  Serial.println(" done");
  Serial.println("SENSOR ACTIVE");
  delay(50);
}

void loop(){
  digitalWrite(ledPin, LOW);  //the led visualizes the sensors output pin state
  readVal = digitalRead(pirPin);

  while(readVal == HIGH){
    digitalWrite(ledPin, HIGH);   //the led visualizes the sensors output pin state
    Serial.println("DETECT");
    readVal = digitalRead(pirPin);
  }
  delay(50);
  Serial.println("NONE");
}
