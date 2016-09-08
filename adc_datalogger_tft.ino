
#include <Adafruit_GFX.h>    // Core graphics library
#include "SWTFT.h" // Hardware-specific library
#include <SD.h>

#define	BLACK   0x0000
#define	BLUE    0x001F
#define	RED     0xF800
#define	GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

SWTFT tft;
// If using the shield, all control and data lines are fixed, and
// a simpler declaration can optionally be used:
// SWTFT tft;
const int chipSelect = 10;  // cs to sdcard
int flag = 0; // set when ISR trips
int debug=0;
long adcval;

void setup(void) {
  ADMUX = 0x43;            // measuring on ADC3, use the internal 1.1 reference
  ADCSRA = 0b10101111;           // AD-converter on, interrupt enabled, prescaler = 128
  ADCSRB = 0x40;           // AD channels MUX on, free running mode
  ADCSRA |= (1<<ADSC);  // Start the conversion by setting bit 6 (=ADSC) in ADCSRA
  sei();                   // set interrupt flag
  
  if (debug)
  {
  Serial.begin(9600);
  Serial.println(F("TFT LCD test"));
  }
  pinMode(chipSelect, OUTPUT);
  
  tft.reset();
   
  uint16_t identifier = tft.readID();

 if (debug)
  {  
    Serial.print(F("LCD driver chip: "));
    Serial.println(identifier, HEX);
  }
   tft.begin(identifier);
   if (!SD.begin(chipSelect)) 
    {
      tft.setCursor(0, 0);
      tft.setTextColor(WHITE);  tft.setTextSize(3);
      tft.println("Card failed");
      // don't do anything more:
      return;
    }
  tft.setCursor(0, 0);
  tft.setTextColor(WHITE);  tft.setTextSize(3);
  tft.println("Card initialized");
  delay(1000);
  testFillScreen();
}

ISR(ADC_vect)
{
  flag = 1;
  cli();
  adcval = ADCL;
  adcval += (ADCH << 8);
  sei();
}

void loop(void) 
{
  testText();
  String dataString = "";
  long timer_mills = 0;
  while(1)
  {
    dataString = "";
    if (flag) // ISR has triggered
    {
      dataString += String(millis());
      dataString += ",";
      dataString += String(adcval);
      File dataFile = SD.open("datalog.txt", FILE_WRITE);
      dataFile.println(dataString);
      dataFile.close();
      if (debug)
        Serial.println(dataString);
      flag = 0; // clear ISR flag
    }
  tft.fillScreen(BLACK);
  tft.setCursor(0, 0);
  tft.setTextColor(WHITE);  tft.setTextSize(3);
  tft.println("adc val: ");
  tft.println(adcval); 
  delay(1000);
  }
}

unsigned long testFillScreen() {
  unsigned long start = micros();
  tft.fillScreen(BLACK);
  tft.fillScreen(RED);
  tft.fillScreen(GREEN);
  tft.fillScreen(BLUE);
  tft.fillScreen(BLACK);
  return micros() - start;
}

unsigned long testText() {
  tft.fillScreen(BLACK);
  unsigned long start = micros();
  tft.setCursor(0, 0);
  tft.setTextColor(WHITE);  tft.setTextSize(3);
  tft.println("Recording");
  tft.setTextColor(WHITE); tft.setTextSize(3);
  tft.println(start);
  return micros() - start;
}

