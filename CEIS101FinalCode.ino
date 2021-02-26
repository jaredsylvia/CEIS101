//Libraries
#include "DHT.h"

//define LED pins, not sure whe we use define instead of int
#define rLED 12
#define yLED 11
#define gLED 10
#define bLED 9

//passive and active buzzer pins
#define pBuz 6
#define aBuz 5

//buzzer selection switch
#define buzSwitch 13

//magnetic (reed?) switch emulator button
#define dSensor 2

//ultrasonic sensor - from tutorial (https://create.arduino.cc/projecthub/Isaac100/getting-started-with-the-hc-sr04-ultrasonic-sensor-036380)
#define trigPin 30
#define echoPin 31

//photoresistor
#define photoResistor A0

//DHT11
#define DHTPin 50
#define DHTType DHT11

//Moisture sensor
#define moisturePin A1

//timer duration variables
const long delayTimer = 1000; // Adjust to change length between status reporting
const long toneTimer = 500; // Adjust to change length between tone changes of buzzer
unsigned long distTimer;

unsigned long previousDistTime;
unsigned long previousTime;
unsigned long previousToneTime;

//human input sensors
int buzSwitchState;
int doorStateInt;
int selectedBuzzer;

//buzzer tone
int lastTone = 450;

//ultrasonic sensor
float duration, distance;

//used by functions
int alarmState;
int highLow;
int lightLevel;
String doorStateStr;

//DHT variables
DHT dht(DHTPin, DHTType);
float humidity;
float tempC;

//Moisture sensor
int moisture;
int moistAlarm;

void setup() {
Serial.begin(9600);
Serial.println("CEIS101 Course Project Module 5");
Serial.println("Programmed by: Jared Sylvia");


//Set pin modes to OUTPUT for LEDs and Buzzers
pinMode(rLED, OUTPUT);
pinMode(yLED, OUTPUT);
pinMode(gLED, OUTPUT);
pinMode(bLED, OUTPUT);
pinMode(pBuz, OUTPUT);
pinMode(aBuz, OUTPUT);
Serial.println("Finished initial set up of digital output pins.");

//Set pin modes to INPUT for switch and sensor
pinMode(buzSwitch, INPUT);
pinMode(dSensor, INPUT);
Serial.println("Finished initial set up of digital input pins.");

//Set pin modes for Ultrasonic sensor
pinMode(trigPin, OUTPUT);
pinMode(echoPin, INPUT);
Serial.println("Set trigger to output and echo to input.");

//Initialize DHT
dht.begin();

//CSV header
Serial.println("Door,Distance,Alarm,Light,Temp,Humidity,Moisture,Moisture Alarm");


}

void readSensors() { //read human input sensors

  doorStateInt = digitalRead(dSensor); //set variables for open/closed door
if(doorStateInt == 0){ 
  doorStateStr = "closed";
  alarmState = 0;
} else if(doorStateInt == 1){
  doorStateStr = "open";
  alarmState = 1;
}
buzSwitchState = digitalRead(buzSwitch); // set variables for buzzer switch
if(buzSwitchState == 1){
    selectedBuzzer = pBuz;
} else if(buzSwitchState == 0){
    selectedBuzzer = aBuz;
}
lightLevel = analogRead(photoResistor);
moisture = analogRead(moisturePin);
}

void toneTimeGo() {

if(millis() - previousToneTime >= toneTimer) { //timer equal to toneTimer
  previousToneTime = millis();
  if(lastTone == 450){
    lastTone = 540;
    highLow = HIGH;
  } else if(lastTone == 540){
    lastTone = 450;
    highLow = LOW;
  }
}
  
}

void distTimeGo() {
if(distance <=5) {
  
  alarmState = 1;  // if distance too low trigger Alarm state.
  
  } else if(distance <= 30 && alarmState == 0){
  distTimer = map(distance, 0, 30, 50, 250); // distance defined by readDistance() in cm
     if(millis() - previousDistTime >= distTimer) { //timer equal to distance under 30cm scaled to 50 - 250
       previousDistTime = millis();
       tone(selectedBuzzer, 1000 - distTimer); //subtract scaled distance from current tone, tone will start lower and grow in pitch
       delay(map(distance, 0, 30, 50, 125)); //tiny delay up to 1/4 of a second, delay lessens with distance
       noTone(selectedBuzzer);
        
     }
  } else if(distance > 30 && alarmState == 0) {
    noTone(selectedBuzzer);
  }
}



void distanceCheck() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  duration = pulseIn(echoPin, HIGH);
  distance = (duration*.034)/2; //math to centimeters outlined in distance sensor tutorial referenced
 
}

void lightLevelCheck(){
  if(lightLevel < 150){
    digitalWrite(bLED, HIGH);
  } else {
    digitalWrite(bLED, LOW);
  }
}

void tempCheck() {
  tempC = dht.readTemperature();
  humidity = dht.readHumidity();
}

void moistCheck() {
  if(moisture > 100){
    moistAlarm = 1;
  } else if(moisture < 100){
    moistAlarm = 0;
  }
}

void delayTimeGo() {

if(millis() - previousTime >= delayTimer){ // timer equal to delayTimer
  previousTime = millis();

  Serial.print(doorStateInt);
  Serial.print(",");
  Serial.print(distance);
  Serial.print(",");
  Serial.print(alarmState);
  Serial.print(",");
  Serial.print(lightLevel);
  Serial.print(",");
  Serial.print(tempC);
  Serial.print(",");
  Serial.print(humidity);
  Serial.print(",");
  Serial.print(moisture);
  Serial.print(",");
  Serial.println(moistAlarm);
    
  }
}

void alarmStateCheck() {
  if(moistAlarm == 1){
    alarmState == 1;
  }
  if(alarmState == 1){
  digitalWrite(rLED, highLow);
  digitalWrite(yLED, LOW);
  digitalWrite(gLED, LOW);
   
  tone(selectedBuzzer, lastTone);
} else if(alarmState == 0){
  if(distance < 20) {
  digitalWrite(rLED, LOW);
  digitalWrite(yLED, highLow);
  digitalWrite(gLED, LOW);    
  } else if(distance > 20){
  digitalWrite(rLED, LOW);
  digitalWrite(yLED, LOW);
  digitalWrite(gLED, HIGH);
  }
  noTone(aBuz);
  noTone(pBuz);
}
}
void loop() {

readSensors();
distanceCheck();
distTimeGo();
toneTimeGo();
delayTimeGo();
lightLevelCheck();
tempCheck();
moistCheck();
alarmStateCheck();

}
