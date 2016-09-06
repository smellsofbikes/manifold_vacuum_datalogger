#include <SD.h>

const int chipSelect = 10;  // cs to sdcard

int flag = 0; // set when ISR trips
int debug = 1; // turns serial output on/off

void setup()
{
  if(debug)
 {
  Serial.begin(9600);
  Serial.print("Initializing SD card...");
 }
  pinMode(chipSelect, OUTPUT);
  
  // see if the card is present and can be initialized:
  if(debug)
  {
    if (!SD.begin(chipSelect)) 
    {
      Serial.println("Card failed, or not present");
      // don't do anything more:
      return;
    }
    Serial.println("card initialized.");
  }
  ADMUX = 0x43;            // measuring on ADC3, use the internal 1.1 reference
  ADCSRA = 0b10101111;           // AD-converter on, interrupt enabled, prescaler = 128
  ADCSRB = 0x40;           // AD channels MUX on, free running mode
  ADCSRA |= (1<<ADSC);  // Start the conversion by setting bit 6 (=ADSC) in ADCSRA
  sei();                   // set interrupt flag
}

ISR(ADC_vect)
{
  flag = 1;
}

void loop()
{
  // make a string for assembling the data to log:
  String dataString = "";
  if (flag) // ISR has triggered
  {
    cli();
    long adc_result = ADCL;
    adc_result += (ADCH << 8);
    dataString += String(millis());
    dataString += ",";
    dataString += String(adc_result);
    File dataFile = SD.open("datalog.txt", FILE_WRITE);
    dataFile.println(dataString);
    dataFile.close();
    if (debug)
      Serial.println(dataString);
    flag = 0; // clear ISR flag
    sei();
  }  
  
}
