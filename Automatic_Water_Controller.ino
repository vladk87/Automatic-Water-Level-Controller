/*
  LiquidCrystal Library - Hello World

 Demonstrates the use a 16x2 LCD display.  The LiquidCrystal
 library works with all LCD displays that are compatible with the
 Hitachi HD44780 driver. There are many of them out there, and you
 can usually tell them by the 16-pin interface.

 This sketch prints "Hello World!" to the LCD
 and shows the time.

  The circuit:
 * LCD RS pin to digital pin 12
 * LCD Enable pin to digital pin 11
 * LCD D4 pin to digital pin 5
 * LCD D5 pin to digital pin 4
 * LCD D6 pin to digital pin 3
 * LCD D7 pin to digital pin 2
 * LCD R/W pin to ground
 * LCD VSS pin to ground
 * LCD VCC pin to 5V
 * 10K resistor:
 * ends to +5V and ground
 * wiper to LCD VO pin (pin 3)

*/

// include the library code:
#include <LiquidCrystal.h>

#include <Wire.h>



// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

#define trig 7
#define echo 6
#define pump 8
#define buttonPin 9

//Enter your tank max value(CM)
int MaxLevel = 28;



bool pumpStatus = false; 
bool buttonState = false;
bool lastButtonState = false;
bool systemOn = false;
bool restartNeeded = false;

void setup() {
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);

  // Debug console
  Serial.begin(115200);
  Serial.begin(9200);
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
  pinMode(pump, OUTPUT);
  pinMode(buttonPin, INPUT);
  lcd.setCursor(0, 0);
  lcd.print("System");
  lcd.setCursor(4, 1);
  lcd.print("Loading..");
  delay(4000);
  lcd.clear();
}

void ultrasonic() {
  if (!systemOn) { // If system is off, clear display and return
    lcd.clear();
    return;
  }
  
  lcd.display();
  digitalWrite(trig, LOW);
  delayMicroseconds(4);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  long t = pulseIn(echo, HIGH);
  int distance = t / 29 / 2;

// Adjusted levels based on specified percentages
float FullLevel = 28 - (MaxLevel * 0.45);        // 45% of tank capacity
float MediumLevel = 28 -  (MaxLevel * 0.25);      // 25% of tank capacity
float LowLevel = 28 - (MaxLevel * 0.18);         // 18% of tank capacity
float VeryLowLevel = 28 - (MaxLevel * 0.12);     // 12% of tank capacity

  Serial.println(distance);

  lcd.setCursor(0, 0);
  lcd.print("WLevel:");

  if (VeryLowLevel <= distance) {
    lcd.setCursor(8, 0);
    lcd.print("Very Low");
    delay(2000);
    digitalWrite(pump, 1);
    pumpStatus = true;
  } else if (VeryLowLevel > distance && LowLevel <= distance) {
    lcd.setCursor(8, 0);
    lcd.print("Low");
    lcd.print("      ");
  } else if (LowLevel > distance && MediumLevel <= distance) {
    lcd.setCursor(8, 0);
    lcd.print("Medium");
  } else if ( MediumLevel > distance && FullLevel <= distance && distance < 20) {
    lcd.setCursor(8, 0);
    lcd.print("Full");
    lcd.print("      ");
     digitalWrite(pump, 0);
    pumpStatus = false;
  }else if (FullLevel < distance) {
    lcd.setCursor(8, 0);
    lcd.print("Full");
    lcd.print("      ");
  }

  lcd.setCursor(0, 1);
  if (pumpStatus) {
    lcd.print("Pump: ON ");
  } else {
    lcd.print("Pump: OFF");
  }
}

void loop() {
  buttonState = digitalRead(buttonPin);

  if (buttonState && !lastButtonState) {
    if (!systemOn) {
      restartNeeded = true; // Set restart flag if the system is being turned on
    }
    systemOn = !systemOn; // Toggle system state
    if (!systemOn) { // If turning off the system, turn off the pump and clear the display
      digitalWrite(pump, LOW);
      pumpStatus = false;
      lcd.clear();
    }
  }

  lastButtonState = buttonState;

  if (restartNeeded) {
    restartNeeded = false; // Reset the restart flag
    setup(); // Perform a full setup
  }
  ultrasonic();
}
