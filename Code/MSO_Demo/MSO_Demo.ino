#define ON true
#define OFF false

const byte sw_mode = A0;
const byte sw_fast = A1;
const byte sw_slow = A2;
const byte mode_led = A3;

boolean previous_mode = HIGH;
boolean previous_fast = HIGH;
boolean previous_slow = HIGH;
boolean mode_led_state = false;

byte counter=0;
int delayTime = 50;
boolean enableError = false;
boolean errorRunning = false;
boolean errorState = OFF;
boolean displayError = false;

unsigned long incrementCounter = 0;
unsigned long errorTimeOut = 100;
unsigned long endOfError = 0;
boolean showNewTime = false;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  for(int i=0; i<8; i++)
    pinMode(i+2, OUTPUT);
  pinMode(13, OUTPUT);
  
  pinMode(sw_mode, INPUT_PULLUP); //MODE
  pinMode(sw_fast, INPUT_PULLUP); // FAST
  pinMode(sw_slow, INPUT_PULLUP); // SLOW
  pinMode(mode_led, OUTPUT);  // LED
  
  randomSeed(analogRead(A4));
}

void loop() {
  // put your main code here, to run repeatedly: 
  digitalWrite(mode_led, mode_led_state);
  processButtons();
  
  while (Serial.available())
    processSerial();


  if (showNewTime) printTime();

  if ((unsigned long)(millis() - incrementCounter) >= delayTime) {
    counter++;
//    incrementCounter = millis() + delayTime;
    incrementCounter = incrementCounter + delayTime; 
  }
  checkError();
  if (displayError) bitWrite(counter,1,1);

  for(int i=0; i<8; i++) {
    digitalWrite(i+2, bitRead(counter,i));
  }
//  delay(delayTime);
}

void checkError() {
  // if the global enable is off, don't do anything.
  if (enableError == false) {
    displayError = false;
    return;
  }
  
  digitalWrite(13, errorState);
  
  if (errorState == ON) {
     // leave the bit high for errorTimeOut
     if ((unsigned long)(millis() - endOfError) >= errorTimeOut) {
        // it's done
       errorState = OFF;
       endOfError = endOfError + errorTimeOut;
       displayError = false;
       analogWrite(11, 0);
     }
  } else {
       // leave the bit low for errorTimeOut
     if ((unsigned long)(millis() - endOfError) >= errorTimeOut) {
        // it's done
       errorState = ON;
       endOfError = endOfError + errorTimeOut;
       displayError = true;
       analogWrite(11, random(64,192));
     }
  }
}
void printTime() {
  Serial.print(delayTime);
  Serial.print(" - ");
  if (enableError) 
    Serial.print("Error On");
  else
    Serial.print("Error Off");
  Serial.println();
  showNewTime = false; 
}

/*
const byte sw_mode = A0;
const byte sw_fast = A1;
const byte sw_slow = A2;
const byte mode_led = A3;

boolean previous_mode = HIGH;
boolean previous_fast = HIGH;
boolean previous_slow = HIGH;
boolean mode_led_state = FALSE;
*/

void processButtons() {
  boolean current_mode = digitalRead(sw_mode);
  boolean current_fast = digitalRead(sw_fast);
  boolean current_slow = digitalRead(sw_slow);
  
  if ((previous_mode == HIGH) && (current_mode == LOW)) {
    enableError = !enableError;
    mode_led_state = enableError;
    showNewTime = true; 
  }
  
  if ((previous_fast == HIGH) && (current_fast == LOW)) {
    delayTime -= 10;
    showNewTime = true;
  }
  
  if ((previous_slow == HIGH) && (current_slow == LOW)) {
    delayTime += 10;
    showNewTime = true;
  }
  
  checkDelayBounds();
  
  
  previous_mode = current_mode;
  previous_fast = current_fast;
  previous_slow = current_slow;
}

void checkDelayBounds() {
    if (delayTime <= 0) delayTime = 1;
    if (delayTime >= 1000) delayTime = 1000; 
}

void processSerial() {
  int incoming = Serial.read();

  if (incoming == '!')
    enableError = !enableError;

  if (incoming == '+')
    delayTime += 10;

  if (incoming == '-')
    delayTime -= 10;

  checkDelayBounds();

  showNewTime = true;
  delay(1); //add a little more time to give another character a chance to arrive.
}

