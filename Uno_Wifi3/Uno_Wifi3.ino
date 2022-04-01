#include <avr/io.h>
#include <avr/interrupt.h>

#define DEBUG true
const int RELAY_PIN = 3;
volatile int finished = 0;
bool on;

void setup() {
   // http://www.8bit-era.cz/arduino-timer-interrupts-calculator.html
   // TIMER 1 for interrupt frequency 1 Hz:
   cli(); // stop interrupts
   TCCR1A = 0; // set entire TCCR1A register to 0
   TCCR1B = 0; // same for TCCR1B
   TCNT1  = 0; // initialize counter value to 0
   // set compare match register for 1 Hz increments
   OCR1A = 62499; // = 16000000 / (256 * 1) - 1 (must be <65536)
   // turn on CTC mode
   TCCR1B |= (1 << WGM12);
   // Set CS12, CS11 and CS10 bits for 256 prescaler
   TCCR1B |= (1 << CS12) | (0 << CS11) | (0 << CS10);
   // enable timer compare interrupt
   TIMSK1 |= (1 << OCIE1A);
   sei(); // allow interrupts

   // Begin Serial
   Serial.begin(115200);

   // Set up relay and resolve the boolean quandary for the 'on' variable
   pinMode(RELAY_PIN, OUTPUT);
   digitalWrite(RELAY_PIN, HIGH);
   on = false;

   // Set the ESP8266's properties
   sendData("AT+RST\r\n", 2000, false); // reset module
   sendData("AT+CWMODE=2\r\n", 1000, false); // configure as access point
   sendData("AT+CIFSR\r\n", 1000, DEBUG); // get ip address
   sendData("AT+CWSAP?\r\n", 2000, DEBUG); // get SSID info (network name)
   sendData("AT+CIPMUX=1\r\n", 1000, false); // configure for multiple connections
   sendData("AT+CIPSERVER=1,80\r\n", 1000, false); // turn on server on port 80
}

void loop() {
 if (Serial.available()) {
   if (Serial.find("+IPD,")) {
     delay(500);
     int connectionId = Serial.read() - 48; // read()
     //function returns
     // ASCII decimal value and 0 (the first decimal number) starts at 48
     String webpage = "<h1>Turn relay ON/OFF:</h1><a href=\"/l0\"><button>ON</button></a>";
     webpage += "<a href=\"/l1\"><button>OFF</button></a>";
     String cipSend = "AT+CIPSEND=";
     cipSend += connectionId;
     cipSend += ",";
    
     cipSend += webpage.length();
     cipSend += "\r\n";
     sendData(cipSend, 100, DEBUG);
     sendData(webpage, 150, DEBUG);
    
     String closeCommand = "AT+CIPCLOSE=";
     closeCommand += connectionId; // append connection id
     closeCommand += "\r\n";
     sendData(closeCommand, 300, DEBUG);
   }
 }
}

String sendData(String command, const int timeout, boolean debug)
{
   String response = "";
   Serial.print(command); // send command to the esp8266
   long int time = millis();
   while ((time + timeout) > millis()) {
     while (Serial.available()) {
       char c = Serial.read(); // read next char
       response += c;
     }
   }
   if (finished >= 10) { // once the 'finished' variable reaches the specified time limit, the relay will turn off and await for another 'ON' impulse to reactivate
     // defaults to turning the relay off
     digitalWrite(RELAY_PIN, HIGH);
     on = true;
     
     if (response.indexOf("/l0") != -1) {
       digitalWrite(RELAY_PIN, LOW);
       on = true;
       finished = 0;
     }
   }
   else {
     if (response.indexOf("/l0") != -1) {
       digitalWrite(RELAY_PIN, LOW);
       on = true;
     }
     
     if (response.indexOf("/l1") != -1) {
       digitalWrite(RELAY_PIN, HIGH);
       on = false;
       finished = 0;
     }
   }
 return response;
}

// Handle the interrupt signal with the 1Hz frequency
ISR(TIMER1_COMPA_vect)
{
  if(on){
    finished = finished + 1;
  }
}
