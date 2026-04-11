#include <SPI.h>
#include <RF24.h>
#include <DHT.h>

#define DHTPIN 6
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

RF24 radio(7, 8); 
const byte address[6] = "00001";

//----
//hadi spicial nta3 motor driver
#define IN3B 2
#define IN4B 4
#define PWMB 3
//--------
//hadi nta3 alarm 
#define greenLED  A0
#define yellowLED A1
#define redLED    A2
#define buzzer    5
//----
//hadi n ta3 ultra son
#define trigPin 9
#define echoPin 10
//---
long duration;
float distance;
char command = 'S'; 
int motorSpeed = 0;

void setup() {
  Serial.begin(9600);
  dht.begin();
  
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  
  pinMode(greenLED, OUTPUT);
  pinMode(yellowLED, OUTPUT);
  pinMode(redLED, OUTPUT);
  pinMode(buzzer, OUTPUT);
  
  pinMode(IN3B, OUTPUT);
  pinMode(IN4B, OUTPUT);
  pinMode(PWMB, OUTPUT);

  if (!radio.begin()) { Serial.println("NRF Error!"); }
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_LOW);
  radio.startListening();
  
  Serial.println("UNO Receiver Ready");
}

void loop() {
float temp = dht.readTemperature();
  if (isnan(temp)) temp = 25; 
  float speedSound = (331.3 + 0.606 * temp) / 10000.0;

  digitalWrite(trigPin, LOW); 
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH); 
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH, 25000); 

  if (duration == 0) {
    distance = 400; 
  } else {
    distance = (duration * speedSound) / 2.0; 
  }

  Serial.print("Distance: "); Serial.println(distance);
  byte receivedData[2];
  if (radio.available()) {
    radio.read(&receivedData, sizeof(receivedData));
    command = (char)receivedData[0];
    motorSpeed = receivedData[1]; 
  }


  if (command == 'B') { 
    digitalWrite(IN3B, HIGH);
    digitalWrite(IN4B, LOW);
    analogWrite(PWMB, motorSpeed);
    noTone(buzzer);
   
    digitalWrite(greenLED, LOW);
    digitalWrite(yellowLED, LOW); 
    digitalWrite(redLED, LOW);
  } 
  else if (command == 'F') { 
    if (distance > 30 || distance == 0) {
      digitalWrite(greenLED, HIGH); 
      digitalWrite(yellowLED, LOW);
      digitalWrite(redLED, LOW);
      noTone(buzzer);
      
      digitalWrite(IN3B, LOW); 
      digitalWrite(IN4B, HIGH);
      analogWrite(PWMB, motorSpeed); 
    } 
    else if (distance > 15) {
      digitalWrite(greenLED, LOW); 
      digitalWrite(yellowLED, HIGH); 
      digitalWrite(redLED, LOW);
      tone(buzzer, 1000, 100);
      
      digitalWrite(IN3B, LOW); 
      digitalWrite(IN4B, HIGH);
      analogWrite(PWMB, motorSpeed / 2); 
    } 
    else { 
      digitalWrite(greenLED, LOW); 
      digitalWrite(yellowLED, LOW); 
      digitalWrite(redLED, HIGH);
      tone(buzzer, 2000);
      
      digitalWrite(IN3B, LOW); 
      digitalWrite(IN4B, LOW);
      analogWrite(PWMB, 0); 
    }
  } 
  else {
    digitalWrite(IN3B, LOW); 
    digitalWrite(IN4B, LOW);
    analogWrite(PWMB, 0);
    
    digitalWrite(greenLED, LOW); 
    digitalWrite(yellowLED, LOW); 
    digitalWrite(redLED, LOW);
    noTone(buzzer);
  }

  delay(50);
}
