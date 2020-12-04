// Copyright (C) 2020 Aleksey Kalyuzhny. Released under the terms of the
// GNU General Public License version 3. See <http://www.gnu.org/licenses/>

class LowPowerStepper {
  public:
    void begin( int motor_pin_1, int motor_pin_2, int motor_pin_3, int motor_pin_4 )
    {
      this->motor_pin_1 = motor_pin_1;
      this->motor_pin_2 = motor_pin_2;
      this->motor_pin_3 = motor_pin_3;
      this->motor_pin_4 = motor_pin_4;
      
      pinMode( motor_pin_1, OUTPUT );
      pinMode( motor_pin_2, OUTPUT );
      pinMode( motor_pin_3, OUTPUT );
      pinMode( motor_pin_4, OUTPUT );
    }

    void step( int stepsToGo, int timePerStep = 6000 )
    {
      if( stepsToGo == 0 ) {
        return;
      }
      
      int stepIncrement = stepsToGo > 0 ? 1 : -1;
      unsigned long nextStepTime = 0;
      
      while( true ) {
        unsigned long now = micros();
        if( nextStepTime == 0 || now >= nextStepTime ) {
          nextStepTime = now + timePerStep;
          if( stepsToGo == 0 ) {
            break;
          }
          pos += stepIncrement;
          doStep( pos % 4 );
          stepsToGo -= stepIncrement;
        }
      }

      powerOff();
    }

    void setZeroPos()
    {
      // When the power is switched on we do not know where motor's rotor actually is
      // and on the first step there will be a missmatch between the controlling signal and the rotor position.
      // The position can be calibrated by moving to a known phisical position and setting that position as zero position
      
      zeroPos = pos;
    }

    int getPos() const
    {
      if( pos >= zeroPos ) {
        return (int)( pos - zeroPos );
      } else {
        return -(int)( zeroPos - pos );
      }
    }

  private:
    unsigned int pos = 0;
    unsigned int zeroPos = 0;
    
    int motor_pin_1;
    int motor_pin_2;
    int motor_pin_3;
    int motor_pin_4;

    void doStep( int phase )
    {
      // Moving one phase full step to minimize power consumption.
      // At any given moment only one phase is energized
      
      powerOff();
      switch( phase ) {
        case 0:
          digitalWrite( motor_pin_1, HIGH ); break;
        case 1:
          digitalWrite( motor_pin_2, HIGH ); break;
        case 2:
          digitalWrite( motor_pin_3, HIGH ); break;
        case 3:
          digitalWrite( motor_pin_4, HIGH ); break;
      }
    }

    void powerOff()
    {
      digitalWrite( motor_pin_1, LOW );
      digitalWrite( motor_pin_2, LOW );
      digitalWrite( motor_pin_3, LOW );
      digitalWrite( motor_pin_4, LOW );
    }
};

LowPowerStepper focuserMotor;
const int timePerStep = 6000; // microseconds

const String RESET = "RESET";
const String FWD = "FWD ";
const String BWD = "BWD ";
const String POS = "POS ";

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

String getCommandArgs( String command )
{
  int pos = command.indexOf( ' ' );
  if( pos != -1 ) {
    return command.substring( pos  + 1 );
  }
  return "";
}

void setup() {
  // Initialize serial port communication at 9600 baud
  Serial.begin( 9600 );
  // Using built-in LED for debugging and motor power on/off
  pinMode( LED_BUILTIN, OUTPUT );
  focuserMotor.begin( 2, 3, 4, 5 );
  // Notify the controlling application that the focuser has been reset
  Serial.println( RESET );
}

void loop()
{
  String line = fetchLineFromSerial();
  if( line.length() > 0 ) {
    if( line.startsWith( FWD ) ) {
      int steps = getCommandArgs( line ).toInt();
      digitalWrite( LED_BUILTIN, HIGH );
      focuserMotor.step( steps, timePerStep );
      digitalWrite( LED_BUILTIN, LOW );
      Serial.println( POS + String( focuserMotor.getPos() ) );
    } else if( line.startsWith( BWD ) ) {
       int steps = getCommandArgs( line ).toInt();
      digitalWrite( LED_BUILTIN, HIGH );
      focuserMotor.step( -steps, timePerStep );
      digitalWrite( LED_BUILTIN, LOW );
      Serial.println( POS + String( focuserMotor.getPos() ) );
    } else {
      Serial.println( "UNRECOGNIZED COMMAND " + line );
    }
    line = "";
  }
}
