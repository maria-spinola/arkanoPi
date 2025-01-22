#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Max72xxPanel.h>

const byte interruptPin = 2;

const int pinCS = 10;
const int pinDIN = 11;
const int pinCLK = 13;
const int devices = 2;
int cont = 0;

//LedControl  = LedControl(11,13,10,devices);
Max72xxPanel matrix = Max72xxPanel(10, 2, 1);

unsigned long delaytime = 500;
const int wait = 100; // In milliseconds

String tape = "BIENVENIDO A ARKANO PI";  //text
String tape2 = "";
const int spacer = 1;
const int width = 5 + spacer; // The font width is 5 pixels

void setupMatrix() {
   matrix.setPosition(0, 0, 0); // The first display is at <0, 0>
   matrix.setPosition(1, 1, 0); // The second display is at <1, 0>
   matrix.setRotation(0, 0);    // Display is position upside down
   matrix.setRotation(1, 2);    // Display is position upside down
   apagarPantalla();
}

void apagarPantalla(){
    for (int i = 0; i < width * tape2.length() + matrix.width() - 1 - spacer; i++) {

      matrix.fillScreen(LOW);

      int letter = i / width;
      int x = (matrix.width() - 1) - i % width;
      int y = (matrix.height() - 8) / 2; // center the text vertically

      while (x + width - spacer >= 0 && letter >= 0) {
         if (letter < tape2.length()) {
            matrix.drawChar(x, y, tape[letter], LOW, LOW, 1);
         }

         letter--;
         x -= width;
      }
      matrix.write(); // Send bitmap to display

      delay(wait);
   } 
}

void scroll() {
   for (int i = 0; i < width * tape.length() + matrix.width() - 1 - spacer; i++) {

      matrix.fillScreen(LOW);

      int letter = i / width;
      int x = (matrix.width() - 1) - i % width;
      int y = (matrix.height() - 8) / 2; // center the text vertically

      while (x + width - spacer >= 0 && letter >= 0) {
         if (letter < tape.length()) {
            matrix.drawChar(x, y, tape[letter], HIGH, LOW, 1);
         }

         letter--;
         x -= width;
      }
      matrix.write(); // Send bitmap to display

      delay(wait);
   }
}

void interrupt2() {
  cont = cont +1;
}

void setup() {
  setupMatrix();
  Serial.begin(9600);
  pinMode(interruptPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin), interrupt2, LOW);
}

void loop() {
 if(cont == 1){
  scroll();
  cont++;
 }
}


   
