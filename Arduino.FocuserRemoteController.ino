// Copyright (C) 2020 Aleksey Kalyuzhny. Released under the terms of the
// GNU General Public License version 3. See <http://www.gnu.org/licenses/>

#include <Stepper.h>

class MyFocuser {
public:
  void begin()
  {
    pos = 0;
    stepper.setSpeed( 5 );
  }

  void setSpeed( int newValue ) { stepper.setSpeed( newValue ); }

  void stepForward( int steps ) { stepper.step( steps ); pos += steps; }
  void stepBackward( int steps ) { stepper.step( -steps ); pos -= steps; }
  
private:
  int pos = 0;
  const int stepsPerMotorRotation = 2048;
  Stepper stepper = Stepper( stepsPerMotorRotation, 2, 4, 3, 5 );
} myFocuser;

const String RESET = "RESET";
const String ON = "ON";
const String OFF = "OFF";
const String SPEED = "SPEED ";
const String FWD = "FWD ";
const String BWD = "BWD ";
const String OK = "OK ";

String fetchLineFromSerial()
{
  static String lineBuffer;
  
  while( Serial.available() ) {
    if( lineBuffer.length() == 0 ) {
      lineBuffer.reserve( 100 );
    }
    char ch = (char)Serial.read();
    if( ch == '\n' ) {
      String line = lineBuffer;
      lineBuffer = "";
      return line;
    }
    lineBuffer += ch;
  }
  return String();
}

void setup() {
  // Initialize serial port communication at 9600 baud
  Serial.begin( 9600 );
  // Using built-in LED for debugging and motor power on/off
  pinMode( LED_BUILTIN, OUTPUT );
  // Initialize the focuser
  myFocuser.begin();
  // Notify the controlling application that the focuser has been reset
  Serial.println( RESET );
}

void loop()
{
  String line = fetchLineFromSerial();
  if( line.length() > 0 ) {
    if( line.equals( ON ) ) {
      digitalWrite( LED_BUILTIN, HIGH );
      Serial.println( OK + line );
    } else if( line.equals( OFF ) ) {
      digitalWrite( LED_BUILTIN, LOW );
      Serial.println( OK + line );
    } else if( line.startsWith( SPEED ) ) {
      myFocuser.setSpeed( 10 );
      Serial.println( OK + line );
    } else if( line.startsWith( FWD ) ) {
      digitalWrite( LED_BUILTIN, HIGH );
      myFocuser.stepForward( 100 );
      digitalWrite( LED_BUILTIN, LOW );
      Serial.println( OK + line );
    } else if( line.startsWith( BWD ) ) {
      digitalWrite( LED_BUILTIN, HIGH );
      myFocuser.stepBackward( 100 );
      digitalWrite( LED_BUILTIN, LOW );
      Serial.println( OK + line );
    } else {
      Serial.println( "UNRECOGNIZED COMMAND " + line );
    }
    line = "";
  }
}
