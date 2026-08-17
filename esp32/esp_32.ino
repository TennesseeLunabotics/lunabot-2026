#include <Arduino.h>

// Pin definitions
int digitalPins[4] = {16, 17, 14, 23};
int pwmPins[4] = {26, 27, 13, 25};
int analogPins[3] = {35, 32, 33};
bool isShutdown = true;

String input = "";

void shutdownAllOutputs() {
  for (int i = 0; i < 4; i++) {
    digitalWrite(digitalPins[i], LOW);
    analogWrite(pwmPins[i], 0);
  }
}

void setup() {
  Serial.begin(9600);

  // Setup digital pins
  for (int i = 0; i < 4; i++) {
    pinMode(digitalPins[i], OUTPUT);
    digitalWrite(digitalPins[i], LOW);
  }
  
  // Setup PWM pins
  for (int i = 0; i < 4; i++) {
    pinMode(pwmPins[i], OUTPUT);
    analogWrite(pwmPins[i], 0);
  }
}

void processCommand(String cmd) {

  if (isShutdown && !cmd.startsWith("RESET")) {
    return;
  }

  if (cmd.startsWith("D")) {
    int pin, val;
    sscanf(cmd.c_str(), "D,%d,%d", &pin, &val);
    digitalWrite(pin, val);
  }

  else if (cmd.startsWith("P")) {
    int pin, val;
    sscanf(cmd.c_str(), "P,%d,%d", &pin, &val);
    analogWrite(pin, val);
  }

  else if (cmd.startsWith("S")) {
    shutdownAllOutputs();
    isShutdown = true;
    Serial.println("SHUTDOWN");
  }

  else if (cmd.startsWith("RESET")) {
    isShutdown = false;
    Serial.println("OK");
  }

  else if (cmd.startsWith("R")) {
    int val0 = analogRead(analogPins[0]);
    int val1 = analogRead(analogPins[1]);
    int val2 = analogRead(analogPins[2]);

    Serial.print("A,");
    Serial.print(val0);
    Serial.print(",");
    Serial.print(val1);
    Serial.print(",");
    Serial.println(val2);
  }
}

void loop() {
  while (Serial.available()) {
    char c = Serial.read();

    if (c == '\n') {
      processCommand(input);
      input = "";
    } else {
      input += c;
    }
  }
}


