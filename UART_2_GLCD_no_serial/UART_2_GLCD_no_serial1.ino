/*
 *  openGLCD Library - Serial Input displayed on glcd
 * 
 * This sketch displays text characters sent over the serial port 
 * (e.g. from the Serial Monitor) on an attached GLCD.
 * Lines will wrap and scroll the text when needed.
 * When a new line character is received new text is wrapped to the next line.
 *
 * If you use the Arduino IDE Serial Monitor and want lines to wrap
 * when you press <enter> or click on the [Send] button, you must
 * turn on the NewLine option down by the BaudRate setting.
 * 
 * 
 * 2013.06.15  bperrybap  - conversion to openGLCD
 * 2011.12.04  bperrybap  - original creation
 *	bperrybap@opensource.billsworld.billandterrie.com
 */

// include the library header
// no font headers have to be included
#include <openGLCD.h>
#define CLEAR_SRN_SMBL '-'
#define LINE_START_SYMBOL 'p'



bool clearScreen() {
        GLCD.ClearScreen();
        GLCD.println("Listening.!.");
        return true;
}



void setup() {
  // Initialize the GLCD 
 GLCD.Init();

 // Select the font for the default text area
  GLCD.SelectFont(System5x7);

  clearScreen();
  

  // could also use gText string output routine
  // GLCD.Puts("Listening...\n"); 


  Serial.begin(115200);
  Serial1.begin(115200);
}

bool printing = false;

void loop()
{
  char c;

  // when characters arrive over the serial port...
  if (Serial1.available())
  {
      // read character from serial library buffer
      c = Serial1.read();
      switch (c) {
        case CLEAR_SRN_SMBL: {
          printing || clearScreen();
          break;
        }
        case LINE_START_SYMBOL: {
            printing = true;
            return;
        }
        case '\n': {
            printing && GLCD.write(c);
            printing = false;
            break;
        }
      }
      printing && GLCD.write(c);
      // display character on glcd
      // GLCD.write(c); // use Print class for output
      // Serial.write(c);
   }
      // could also use gText character output routine
      // GLCD.PutChar(c);
}
