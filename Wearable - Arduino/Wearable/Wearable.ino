
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
Timer timer;
int checkupReadings[10];                   //array to store checkup readings

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
  interruptSetup();                         //sets up to read Pulse Sensor signal every 2mS 

  // IF YOU ARE POWERING The Pulse Sensor AT VOLTAGE LESS THAN THE BOARD VOLTAGE, 
  // UN-COMMENT THE NEXT LINE AND APPLY THAT VOLTAGE TO THE A-REF PIN
  //   analogReference(EXTERNAL);   
}


//  Where the Magic Happens
void loop(){

  serialOutput() ;       

  if (QS == true){     // A Heartbeat Was Found
    // BPM and IBI have been Determined
    // Quantified Self "QS" true when arduino finds a heartbeat
    fadeRate = 255;         // Makes the LED Fade Effect Happen
    // Set 'fadeRate' Variable to 255 to fade LED with pulse
    serialOutputWhenBeatHappens();   // A Beat Happened, Output that to serial.     
    QS = false;                      // reset the Quantified Self flag for next time    
  }

  ledFadeToBeat();                      // Makes the LED Fade Effect Happen 
  delay(20);                             //  take a break

  // read the state of the pushbutton value:
  button();
}

//code here

void button() {
  buttonState = digitalRead(buttonPin);

  if(buttonState == LOW) {                                           //SHOULD THIS BE A WHILE LOOP?
    delay(125);
    //blink led 2x so user can move finger

    calibrate();
//    blinkLed(redPin, 2, HIGH, 1000);
  } 
  else {
    //    digitalWrite(ledPin, LOW);    
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
void calibrate() {
    // FIRST RUN
    timer = millis();
    int average = 0;
    digitalWrite(redPin, HIGH);

    while(millis() - timer < 20000) {
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
    counter = 0;
    
     blinkLed(redPin, 2, HIGH, 200);
     digitalWrite(redPin, LOW);
  /// other 
}

void checkUp() {
    timer.every(120000, takeReading);  
}

void takeReading() {
    Serial.print(" CHECKUP: "); 
    Serial.print(BPM);
    Serial.println("");


}



void ledFadeToBeat(){
  fadeRate -= 15;                         //  set LED fade value
  fadeRate = constrain(fadeRate,0,255);   //  keep LED fade value from going into negative numbers!
  analogWrite(fadePin,fadeRate);          //  fade LED
}






