// Copyright (C) 2020 Aleksey Kalyuzhny. Released under the terms of the
// GNU General Public License version 3. See <http://www.gnu.org/licenses/>

class Focuser {
public:
  void begin()
  {
    pos = 0;
    speed = 5;
  }

  int getSpeed() const { return speed; }
  void setSpeed( int newValue ) { speed = newValue; }
  
private:
  int pos = 0;
  int speed = 5;
};

Focuser focuser;

const String ACK = "ACK";
const String SPEED = "SPEED ";
const String RESET = "RESET";

void setup() {
  // Initialize serial port communication at 9600 baud
  Serial.begin( 9600 );

  focuser.begin();

  pinMode( LED_BUILTIN, OUTPUT );

  Serial.println( RESET );
}

String checkCommand(){
  static String commandBuffer;
  
  while( Serial.available() ) {
    if( commandBuffer.length() == 0 ) {
      commandBuffer.reserve( 100 );
    }
    char ch = (char)Serial.read();
    if( ch == '\n' ) {
      String command = commandBuffer;
      commandBuffer = "";
      return command;
    }
    commandBuffer += ch;
  }
  return String();
}

void loop() {
  String command = checkCommand();
  if( command.length() > 0 ) {
    if( command.equals( "ON" ) ) {
      digitalWrite(LED_BUILTIN, HIGH);
      Serial.println( ACK );
    } else if( command.equals( "OFF" ) ) {
      digitalWrite(LED_BUILTIN, LOW);
      Serial.println( ACK );
    } else if( command.startsWith( SPEED ) ) {
      digitalWrite(LED_BUILTIN, LOW);
      Serial.println( ACK + ' ' + command + String( focuser.getSpeed() ) );
    } else {
      Serial.println( "UNKNOWN " + command );
    }
    command = "";
  }
}
