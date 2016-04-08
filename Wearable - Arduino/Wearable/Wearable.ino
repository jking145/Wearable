
/*  Pulse Sensor Amped 1.4    by Joel Murphy and Yury Gitman   http://www.pulsesensor.com
 
 ----------------------  Notes ----------------------  ---------------------- 
 This code:
 1) Blinks an LED to User's Live Heartbeat   PIN 13
 2) Fades an LED to User's Live HeartBeat
 3) Determines BPM
 4) Prints All of the Above to Serial
 
 Read Me:
 https://github.com/WorldFamousElectronics/PulseSensor_Amped_Arduino/blob/master/README.md   
 ------------------------------------------------------------------
 */
#include "Timer.h" 

//  Variables
int pulsePin = 0;                 // Pulse Sensor purple wire connected to analog pin 0
int blinkPin = 13;                // pin to blink led at each beat
int fadePin = 5;                  // pin to do fancy classy fading blink at each beat
int fadeRate = 0;                 // used to fade LED on with PWM on fadePin

// Volatile Variables, used in the interrupt service routine!
volatile int BPM;                   // int that holds raw Analog in 0. updated every 2mS
volatile int Signal;                // holds the incoming raw data
volatile int IBI = 600;             // int that holds the time interval between beats! Must be seeded! 
volatile boolean Pulse = false;     // "True" when User's live heartbeat is detected. "False" when not a "live beat". 
volatile boolean QS = false;        // becomes true when Arduoino finds a beat.

//*---------- CUSTOM VAR CALIBRATION---------*
//boolean startup = false;
int counter = 0;
unsigned long timer = 0;

//*---------- CUSTOM VAR BUTTON---------*
int buttonPin = 2;
//int ledPin = 4;
int buttonState = 0;

//*---------- CUSTOM VAR RGB LED---------*
int redPin = 9;
int greenPin = 8;
int bluePin = 7;

//*---------- CUSTOM VAR CHECKUP----------*
Timer t;
int checkupReadings[10];                   //array to store checkup readings
int readingsIndex = 0;

//*---------- CUSTOM----------*
int currentCalibration = -1;
int previousCalibration = -1;

//*---------- CUSTOM GLOBAL VAR----------*
int newAverage = 0;
boolean nextCheck = false;

// Regards Serial OutPut  -- Set This Up to your needs
static boolean serialVisual = false;   // Set to 'false' by Default.  Re-set to 'true' to see Arduino Serial Monitor ASCII Visual Pulse 


void setup(){
  pinMode(buttonPin, INPUT_PULLUP);         //pin for button, enables the internal pull-up resistor
  //  pinMode(blinkPin,OUTPUT);                 //pin that will blink to your heartbeat!
  //  pinMode(fadePin,OUTPUT);                  //pin that will fade to your heartbeat!
  pinMode(redPin, OUTPUT);                  // sets the pins as output...
  pinMode(greenPin, OUTPUT);   
  pinMode(bluePin, OUTPUT); 
  Serial.begin(115200);                     //we agree to talk fast!
  t.every(5000, takeReading);              //every 2 min do a checkup

  interruptSetup();

  // IF YOU ARE POWERING The Pulse Sensor AT VOLTAGE LESS THAN THE BOARD VOLTAGE, 
  // UN-COMMENT THE NEXT LINE AND APPLY THAT VOLTAGE TO THE A-REF PIN
  //   analogReference(EXTERNAL);  
}


//  Where the Magic Happens
void loop(){

  //serialOutput() ;       

  if (QS == true){     // A Heartbeat Was Found
    // BPM and IBI have been Determined
    // Quantified Self "QS" true when arduino finds a heartbeat
    fadeRate = 255;         // Makes the LED Fade Effect Happen
    // Set 'fadeRate' Variable to 255 to fade LED with pulse
    serialOutputWhenBeatHappens();   // A Beat Happened, Output that to serial.     
    QS = false;                      // reset the Quantified Self flag for next time    
  }

  // ledFadeToBeat();                      // Makes the LED Fade Effect Happen 
  // delay(20);                             //  take a break

  // read the state of the pushbutton value:
  button(); 

  //update the timer to run the check every 2 min
  t.update();

  if (nextCheck) {
    checkArrayAverage();
    nextCheck = !nextCheck;
  }
}

//code here

void button() {
  buttonState = digitalRead(buttonPin);

  if(buttonState == LOW) {                                         
    delay(125);

    calibrate(20000);
  } 
  else { 
  } 
}

void setColour(int red, int green, int blue) {
  red = 255 - red;
  green = 255 - green;
  blue = 255 - blue;
  digitalWrite(redPin, red);
  digitalWrite(greenPin, green);
  digitalWrite(bluePin, blue);
}

void blinkLed(int led, int repeat, int intensity, int time) {

  for(int i = 0; i < repeat; i++) {
    digitalWrite(led, intensity);
    delay(time);
    digitalWrite(led, LOW);
    delay(time);
  }

}

//calibrate returns boolean 
void calibrate(int period) {
  // FIRST RUN

  timer = millis();
  int average = 0;
  digitalWrite(redPin, HIGH);

  while(millis() - timer < period) {
    // stay in here
    // IBI varialble 
    if (QS == true) {
      Serial.print("Counter: "); 
      Serial.print(counter++);
      Serial.print(" BPM: "); 
      Serial.print(BPM);
      average += BPM;
      Serial.println("");
      QS = false;
    }
  }

  average = average / counter;
  Serial.print(" AVERAGE: "); 
  Serial.print(average);
  Serial.println("");
  currentCalibration = average; 
  counter = 0;

  blinkLed(redPin, 2, HIGH, 200);
  digitalWrite(redPin, LOW);

  /// other 
}

//void checkUp() {
//    t.every(12000, takeReading);  
//    blinkLed(bluePin, 2, HIGH, 200);
//    
//}

void takeReading() {
  static int aCounter = 0;

  //store BPM from checkup into array
  checkupReadings[aCounter++] = BPM;
  Serial.print(" CHECKUP: "); 
  Serial.println(BPM);
  if (aCounter > 9) {

    for (int j = 0; j <= 9; j++) {
      newAverage += checkupReadings[j];
      checkupReadings[j] = 0;
    }

    newAverage /= 10;
    aCounter = 0;
    nextCheck = true;
  }

  Serial.print(" NEW AVERAGE: "); 
  Serial.println(newAverage);
}


void checkArrayAverage() {

  int overBPM = 10;
  int underBPM = 15;

  if ((newAverage + overBPM) > currentCalibration || (newAverage - underBPM) < currentCalibration){
    previousCalibration = currentCalibration;
    calibrate(7000);

    if ((newAverage + overBPM) > currentCalibration || (newAverage - underBPM) < currentCalibration){
      previousCalibration = currentCalibration;
      motor(....);
      newAverage = 0;
    }

    Serial.print("RECALIBRATING");
    newAverage = 0;
  }
}


void motor(   ) {

  }

}

//}
//void ledFadeToBeat(){
//  fadeRate -= 15;                         //  set LED fade value
//  fadeRate = constrain(fadeRate,0,255);   //  keep LED fade value from going into negative numbers!
//  analogWrite(fadePin,fadeRate);          //  fade LED
//}













