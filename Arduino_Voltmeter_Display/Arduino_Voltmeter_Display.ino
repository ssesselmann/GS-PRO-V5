#include <Arduino.h>
#include <TM1637TinyDisplay.h>
#include <Oversampling.h>
#define GPIO2_PREFER_SPEED 1  //who knows what this is ?


#define CLK 4  // module connection pins (Digital Pins)
#define DIO 5  // module connection pins (Digital Pins)

const int pin2 = 2;  // interrupt counter only works on pin #2
const int t = 10;    // define scaler integral (seconds)
const int button = 6;

unsigned long secondCounter = 0;  // define variable
unsigned long counts = 0;         // define variable
int x = 0;
int sensorValue = 0;        // define variable
int y = 0;
int countArray[t];  // define array size to be t
int arraySum = 0;
int arraySize = 0;
int cpm = 0;    // define counts per minute
int cps = 0;    // define counts per minute
int state = 0;  // start state at 0

boolean buttonState;  // define variable


TM1637TinyDisplay display(CLK, DIO);  // define pins for display data

Oversampling adc(12, 14, 12);  //Adc Bytes 10 or 12, Oversampling Bytes 11-18, 2 Is avaraging count.

void pulse() {
  counts++;
  delay(1);
}  // executed on interrupt adds 1 to counts on pulse

void setup() {
  Serial.begin(9200);                 // initialise baud rate
  display.clear();                     // clear display
  display.setBrightness(BRIGHT_HIGH);  // set display brightness
  pinMode(A0, INPUT);                  // define A0 as INPUT for volt reading
  pinMode(button, INPUT);              // initialize input 6 for mode switch
  pinMode(pin2, INPUT);

  attachInterrupt(digitalPinToInterrupt(pin2), pulse, RISING);  // executes pulse() when pin 2 rising

  display.showString("GS");  // boot text
  delay(1000);
  display.showString("Pro");
  delay(1000);
  display.showString("HELO");
  delay(1000);
  state = 0;
}

void loop() {
  int buttonState = digitalRead(button);  // get state of button pin

  if (buttonState == HIGH) {  // add one to state when button pin goes low
    state++;
    while (digitalRead(button) == HIGH) {
    } 
  } 

  if (state > 2) { state = 0; }

  if (state == 0) { loop0(); }  // Display voltage
  if (state == 1) { loop1(); }  // Display cpm
  if (state == 2) { loop2(); }  // Display cps
}

void loop0() {  // read calculate and display average voltage
  sensorValue = adc.read(0);
  x = (sensorValue) * 1.221;
 
  
  display.showNumber(x);
  delay(500);
}

void loop1() {

  if (millis() > ((secondCounter + 1) * 1000)) {  // counts seconds from start
    countArray[secondCounter % t] = counts;       // writes last counts into array
    counts = 0;                                   // resets counts
    secondCounter++;                              // increases secondCounter by 1.
    arraySum = 0;                                 // reset sum
    arraySize = min(secondCounter, t);            // only relevant at startup

    for (int i = 0; i < arraySize; i++) {
      arraySum += countArray[i];  // sum counts in array.
    }
    cpm = arraySum * (60 / t);  // Counts per minute
    cps = arraySum / t;         // Counts per second
    display.showNumber(cpm);    // Counts print display
  }
}

void loop2() {

  if (millis() > ((secondCounter + 1) * 1000)) {  // counts seconds from start
    countArray[secondCounter % t] = counts;       // writes last counts into array
    counts = 0;                                   // resets counts
    secondCounter++;                              // increases secondCounter by 1.
    arraySum = 0;                                 // reset sum
    arraySize = min(secondCounter, t);            // only relevant at startup

    for (int i = 0; i < arraySize; i++) {
      arraySum += countArray[i];  // sum counts in array.
    }
    cpm = arraySum * (60 / t);  // Counts per minute
    cps = arraySum / t;         // Counts per second
    display.showNumber(cps);    // Counts print display
  }
}