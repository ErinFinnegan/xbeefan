
/*

NOTE:  IN ORDER TO AVOID A NO SYNC ERROR YOU HAVE TO UNPLUG PIN 0 WHILE YOU UPLOAD THE SKETCH

XBee Analog Duplex sender
 Context: Arduino
 This sketch configures an XBee radio via the serial port,
 sends the value of an analog sensor out, and listens for input
 from the radio, using it to set the value of a PWM output.
 Thanks to Robert Faludi for the critique and improvements.
 */
const int sensorPin = A0; // input sensor
const int analogLed = 3; // LED that changes brightness with incoming value
const int threshold = 10; // threshold for sensor's change
int lastSensorReading = 0; // previous state of the sensor

const int PowerSwitchTail = 8;  //my addition

String inputString = "";
void setup() {
  // configure serial communications:
  Serial.begin(9600);
  // configure output pin:
  pinMode (analogLed, OUTPUT);

  pinMode(PowerSwitchTail, OUTPUT);  //my addition
  digitalWrite(PowerSwitchTail, LOW); //set the initial state

  // set XBee's destination address:
  setDestination();
  // blink the TX LED indicating that the main program's about to start:
  blink(analogLed, 3);
}


void loop() {
  // listen for incoming serial data:
  if (Serial.available() > 0) {
    handleSerial();
  }
  // listen to the potentiometer:
  int sensorValue = readSensor();
  // if there's something to send, send it:
  if (sensorValue > 0) {
    Serial.println(sensorValue, DEC);
  }
}



void setDestination() {
  // put the radio in command mode:
  Serial.print("+++");
  // wait for the radio to respond with "OK\r"
  char thisByte = 0;
  while (thisByte != '\r') {
    if (Serial.available() > 0) {
      thisByte = Serial.read();
    }
  }
  // set the destination address, using 16-bit addressing.
  // if you're using two radios, one radio's destination
  // should be the other radio's MY address, and vice versa:
  Serial.print("ATDH0, DL5555\r");
  // set my address using 16-bit addressing:
  Serial.print("ATMY15\r");
  // set the PAN ID. If you're working in a place where many people
  // are using XBees, you should set your own PAN ID distinct
  // from other projects.
  Serial.print("ATIDFEED\r");
  // put the radio in data mode:
  Serial.print("ATCN\r");
}


void blink(int thisPin, int howManyTimes) {
  // Blink the LED:
  for (int blinks = 0; blinks < howManyTimes; blinks++) {
    digitalWrite(thisPin, HIGH);
    // Serial.print("blink");  //check to see if this ever runs
    delay(200);
    digitalWrite(thisPin, LOW);
    delay(200);
  }
}


void handleSerial() {
  char inByte = Serial.read();
  Serial.print(inByte);
  if (inByte == 'X' || inByte == 'x') {
    digitalWrite(PowerSwitchTail, HIGH);
  } 
  //else {
  //  digitalWrite(PowerSwitchTail, LOW);
  //}
}

// save only ASCII numeric characters (ASCII 0 - 9):
//    if (isDigit(inByte)) {
//      inputString = inputString + inByte;
//    }
//    // if you get an ASCII newline:
//    if (inByte == '\n') {
//      // convert the string to a number:
//      // int brightness = inputString.toInt();  //I don't want to convert to integers anymore
//      // set the analog output LED:
//      analogWrite(analogLed, brightness);
//      // clear the input string for the
//      // next value:
//      inputString = "";
//      Serial.print(brightness);
//      // Serial.print("brightness = " + brightness);
//    }
//}


int readSensor() {
  int result = analogRead(sensorPin);
  // look for a change from the last reading
  // that's greater than the threshold:
  if (abs(result - lastSensorReading) > threshold) {
    result = result / 4;
    lastSensorReading = result;
  }
  else {
    // if the change isn't significant, return 0:
    result = 0;
  }
  return result;
}

