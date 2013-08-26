#define ON true
#define OFF false

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
}

void loop() {
  // put your main code here, to run repeatedly: 
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
     }
  } else {
       // leave the bit low for errorTimeOut
     if ((unsigned long)(millis() - endOfError) >= errorTimeOut) {
        // it's done
       errorState = ON;
       endOfError = endOfError + errorTimeOut;
       displayError = true;
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

void processSerial() {
  int incoming = Serial.read();

  if (incoming == '!')
    enableError = !enableError;

  if (incoming == '+')
    delayTime += 10;

  if (incoming == '-')
    delayTime -= 10;

  if (delayTime <= 0) delayTime = 1;

  if (delayTime >= 1000) delayTime = 1000; 

  showNewTime = true;
  delay(1); //add a little more time to give another character a chance to arrive.
}

