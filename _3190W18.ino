#include <LiquidCrystal.h>
#include <Wire.h>

// Netzteil mit LTC 3081
// Spannungseinstellung mit PCF8574 AP und Widerstandskette die von
// Optomosfets H11F1 ueberbrueckt wird
// Adresse PCF 8574 AP   38H
// Mesung der Ausgangsspannung  A0
// Messung Temperatur LTC 3081  A3 
// Messung Ausgangsstrom LTC 3081 A2  
// Digitalausgang D2 an LCD Register Select
// Digitalausgang D3 an LCD Enable
// Digitalausgang D4 ... D7 an LCD D4 ... D7
// Taster 1 =   D8
// Taster 2 =   D9
// Taster 3 =   D10
// A4 des Arduino = SDA
// A5 des Arduino = SCL  
// Ua Spannungsteiler 909/75 Ohm
// Ia Teiler /5000 * 50  
// ***************************Variablendefinition**************** 
int i2cadress = 0x38;                               // I2C Adresse PCF8574
int wert;                                           // Ausgabewert fuer PCF8574
int uawert;                                         // Spannungswert
int iawert;                                         // Stromwert
int tempwert;                                       // Temperaturwert    
int reading;
int lastreading= 0x00;       //Aenderungswunsch
unsigned long erstkontakt;
unsigned long entprellzeit = 100;
int tasterstatus;
int taste;
//********************* Definition der LCD Anzeigenanschluesse **************
LiquidCrystal lcd(2,3,4,5,6,7); 
 
void setup()
{
  Serial.begin(9600);
  analogReference(EXTERNAL);
  DDRB = B11111000;                         //Port B Bit 0,1,2 als Eingang
  PORTB = 0x07;                             // Port B pull up auf High
  lcd.begin(16,4);
  lcd.print ("  Pruefung EGS  ");
  lcd.setCursor(0,2);
  lcd.print (" Winter 2018/19  ");
  Wire.begin();
  delay(3000);  
 // taste = 0x07;                       //   Aenderungswunsch
}


void spannungmessen()
{
lcd.clear();
lcd.print("Spannung:");
uawert = analogRead(0);
Serial.println("Spannung");
uawert = uawert * 13 ;
Serial.println(uawert);
//uawert = uawert/1000;
lcd.print(uawert);
lcd.print("mV"); 
}

void testlauf()
{
  for (wert = 0 ; wert < 185 ; wert++)      // 185 entspricht ca 10,70 V damit Ende bei Ue = 12V Netzteil
 {
  Wire.begin();
  Wire.beginTransmission(i2cadress);
  Wire.write(wert);
  Wire.endTransmission();
  lcd.setCursor(0,1);
  lcd.print("Wert  :");
  lcd.setCursor(9,1);
  lcd.print (wert);
  delay(200); 
 }   
}
void temperaturmessen()
{
 lcd.clear();
lcd.print("Temperatur: ");
tempwert = analogRead(3);
tempwert = tempwert /50 ;                    // Korrekturfaktor einfügen
lcd.print(tempwert);
lcd.print("'C");  
}

void strommessen()
{
 lcd.clear();
lcd.print("Strom :");
iawert = analogRead(2);
iawert = iawert/2;                          //Korrekturfaktor einfügen
lcd.print(iawert);
lcd.print ("mA");
 
}

void tastenauswertung()
{
  Serial.print("gedrueckte Taste : ");
      switch (taste)
     {
        case 7:
            //Serial.println("keine taste gedrueckt");
        break; 
       case 6:
           lcd.clear();
           lastreading = 0x07;
           lcd.print("Wert erhoehen");
                if (wert < 184)
                 {
                   wert = wert + 1;
                   Wire.begin();
                   Wire.beginTransmission(i2cadress);
                   Wire.write(wert);
                   Wire.endTransmission();
                   lcd.setCursor(0,1);
                   lcd.print("Wert  :");
                   lcd.setCursor(9,1);
                   lcd.print (wert);  
                   delay(300);
                 }
                 spannungmessen();
                 break; 
       case 5:
                strommessen();
                lastreading = 0x07;
                break;
       case 4:  spannungmessen();
                lastreading = 0x07;
                break;         
       case 3:
        lcd.clear();
        lastreading = 0x07;
           lcd.print("Wert verringern");
                if (wert > 1)
                 {
                   wert = wert - 1;
                   Wire.begin();
                   Wire.beginTransmission(i2cadress);
                   Wire.write(wert);
                   Wire.endTransmission();
                   lcd.setCursor(0,1);
                   lcd.print("Wert  :");
                   lcd.setCursor(9,1);
                   lcd.print (wert); 
                   delay(300);  
                 }
                 spannungmessen();
                 break; 
      case 2 :
                 temperaturmessen();
                 lastreading = 0x07;
                 break;                  
      case 1:   spannungmessen();
                lastreading = 0x07;
                 break;          
      case 0:
                 lcd.clear();
                 lcd.print("TESTLAUF");
                 testlauf();
                 delay(3000);
                 lastreading = 0x07;
                 spannungmessen();
                 break;
}
}


void loop()
{
   reading = (PINB & 0x07);                        //Lesen Bit 1,2,3 PortB (digital 8,9,10)
          if (reading  != lastreading)
          {
          erstkontakt = millis();
          lastreading = reading;
          }
   if ((millis() - erstkontakt) > entprellzeit)
       {
        if (reading != tasterstatus)
          {
         tasterstatus = lastreading;
         erstkontakt = millis();
           }
         else
            {
            taste = (PINB & 0x07);
            } 
        }
tastenauswertung();
taste = 0x07;
}




