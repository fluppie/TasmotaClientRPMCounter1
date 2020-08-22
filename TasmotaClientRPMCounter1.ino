/*
  BlinkRelay.ino - Example for TasmotaSlave receiving the FUNC_EVERY_SECOND
                   callback and respond by toggling the LED as configured.
                   This example also extends sending commands back to the
                   Tasmota device by which could be any command normally
                   executed from the console but in this particular example
                   we just use a simple publish command.

  Copyright (C) 2019  Andre Thomas

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <Arduino.h>
#include <TasmotaSlave.h>

TasmotaSlave slave(&Serial);

const int hallSensorPin = 2;                      // connect the hall effect sensor on pin 2
int rpm = 0;
const unsigned long sampleTime = 20000;

bool ledstate = false;

/*******************************************************************\
 * Function which will be called when Tasmota sends a
 * FUNC_EVERY_SECOND command
\*******************************************************************/

void user_FUNC_EVERY_SECOND(void)
{
  char buffer[40]; //allow space for the string, int and terminating '/0'p  int rpm = 123;
  rpm = getRPM();
  if (ledstate) {
    ledstate = false;
    digitalWrite(LED_BUILTIN, LOW);
  } else {
    ledstate = true;
    digitalWrite(LED_BUILTIN, HIGH);
  }

  char rpmChar[8];
  strcpy (buffer, "Counter1 ");
  itoa(rpm, rpmChar, 10);
  strcat(buffer, rpmChar);

  slave.ExecuteCommand((char*)buffer);
}

/*******************************************************************\
 * Normal setup() function for Arduino to configure the serial port
 * speed (which should match what was configured in Tasmota) and
 * attach any callback functions associated with specific requests
 * or commands that are sent by Tasmota.
\*******************************************************************/

void setup() {
  // Configure serial port
  Serial.begin(57600);
  // We're going to use the builtin LED so lets configure the pin as OUTPUT
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(hallSensorPin,INPUT_PULLUP); //hall sensor with internal pull-up enabled
  // Attach the callback function which will be called when Tasmota requests it
  slave.attach_FUNC_EVERY_SECOND(user_FUNC_EVERY_SECOND);
}

void loop() {
  // Call the slave loop function every so often to process incoming requests
  slave.loop();
}

int getRPM()
{
  int count = 0;
  boolean countFlag = HIGH;
  unsigned long currentTime = 0;
  unsigned long startTime = millis();
  while (currentTime <= sampleTime)
  {
    if (digitalRead(hallSensorPin) == LOW)
    {
      countFlag = LOW;
    }
    if (digitalRead(hallSensorPin) == HIGH && countFlag == LOW)
    {
      count++;
      countFlag=HIGH;
    }
    currentTime = millis() - startTime;
  }
  int countRpm = int(60000/float(sampleTime))*count;
  return countRpm;
}
