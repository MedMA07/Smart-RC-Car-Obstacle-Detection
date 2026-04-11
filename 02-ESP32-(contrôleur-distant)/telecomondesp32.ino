#include <SPI.h>
#include <RF24.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
//------------------------------
//hadi partiya nta3 radia bah n3toh adress whda bin esp32 w arduino
// CE=4, CSN=5 
RF24 radio(4, 5); 
const byte address[6] = "00001";
///--------------------------------
#define JOY_Y 35 

void setup() {
  Serial.begin(115200);
  //--------------
  //hada ghiur test bh z3ma n3rf ask oled rahi mbronchiya wla la 
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("OLED Error!"));
  }
  //------------------------------
  if (!radio.begin()) {
    Serial.println(F("NRF Error!"));
    display.clearDisplay();
    display.setCursor(0,0);
    display.println("NRF Error!");
    display.display();
  }

  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_LOW);      
  
  // hna kifkif dkhla m3a bah ytconictaw fi rahba 
  radio.setDataRate(RF24_250KBPS);   
  radio.setChannel(115);            
  radio.stopListening(); 

  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setCursor(0,10);
  display.println("Remote Ready!");
  display.display();
  delay(1000);
}

void loop() {
  //------
  //hadi nta3  joystique
  int yVal = analogRead(JOY_Y); 
  char command = 'S';
  int speed = 0;

  if (yVal > 2800) {  
    command = 'F';
    speed = map(yVal, 2800, 4095, 0, 255); 
  } 
  else if (yVal < 1200) { 
    command = 'B';
    speed = map(yVal, 1200, 0, 0, 255);
  } 
  else { 
    command = 'S';
    speed = 0;
  }

  
  byte data[2] = {(byte)command, (byte)speed};
  bool success = radio.write(&data, sizeof(data));

  display.clearDisplay();
  display.setCursor(0,0);
  display.println("--- TELECOMMANDE ---");
  display.setCursor(0,20);
  display.print("CMD: "); display.println(command);
  display.setCursor(0,35);
  display.print("SPD: "); display.print(speed);
  
  display.setCursor(0,55);
  if (success) {
    display.println("Status: LINK OK");
  } else {
    display.setTextColor(BLACK, WHITE);
    display.println("Status: NO LINK!");
    display.setTextColor(WHITE, BLACK);
  }
  display.display();

  delay(50); 
}
