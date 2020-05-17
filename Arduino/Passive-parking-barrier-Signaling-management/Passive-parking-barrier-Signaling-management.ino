


#define Buzzer1 12
#define BuzzerTCiclo 1000
#define BuzzerTono1 800
#define BuzzerTono2 1200
uint8_t BuzzerState=0;
uint32_t BuzzerTime;


#include <NeoPixelBus.h>

#define PixelNumber 3
const uint16_t PixelCount = PixelNumber;
const uint8_t PixelPin1DX = 2;  // make sure to set this to the correct pin, ignored for Esp8266
const uint8_t PixelPin1SX = 3;  // make sure to set this to the correct pin, ignored for Esp8266
const uint8_t PixelPin2DX = 4;  // make sure to set this to the correct pin, ignored for Esp8266
const uint8_t PixelPin2SX = 5;  // make sure to set this to the correct pin, ignored for Esp8266

#define colorSaturation 128

// three element pixels, in different order and speeds
NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> Strip1DX(PixelCount, PixelPin1DX);
NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> Strip1SX(PixelCount, PixelPin2SX);
NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> Strip2DX(PixelCount, PixelPin2DX);
NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> Strip2SX(PixelCount, PixelPin2SX);
//NeoPixelBus<NeoRgbFeature, Neo400KbpsMethod> strip(PixelCount, PixelPin);

RgbColor red(colorSaturation, 0, 0);
RgbColor green(0, colorSaturation, 0);
RgbColor blue(0, 0, colorSaturation);
RgbColor white(colorSaturation);
RgbColor black(0);

HslColor hslRed(red);
HslColor hslGreen(green);
HslColor hslBlue(blue);
HslColor hslWhite(white);
HslColor hslBlack(black);

#define LEDFlash 300
uint8_t LEDColor[2]={0, 0};
uint8_t LEDCounter=0;
uint32_t LEDTime;


#define HallSensor1 0
#define HallSensor2 1
#define HallSensor3 2
#define HallSensor4 3
#define NumSensori 4
uint16_t HallLettura[NumSensori];
uint8_t HallPin[NumSensori] = {HallSensor1,
                               HallSensor2,
                               HallSensor3,
                               HallSensor4};
uint8_t HallDigitalState;
#define SogliaPerHall 600

#define SerialSpeed 56700


void setup() 
{
  Serial.begin(SerialSpeed);

  // this resets all the neopixels to an off state
  Strip1DX.Begin();
  Strip1SX.Begin();
  Strip2DX.Begin();
  Strip2SX.Begin();
  Strip1DX.Show();
  Strip1SX.Show();
  Strip2DX.Show();
  Strip2SX.Show();

  pinMode(Buzzer1, OUTPUT);

}

void loop() 
{
  LeggeSensori(SogliaPerHall);
  BuzzAllarm();
  LEDAllarm();
}

void LeggeSensori(uint16_t Soglia)
//Legge i 4 sensori ad effetto di Hall in analogico e restituisce
//il valore corrente e lo stato logico rispetto alla Soglia
{
  HallDigitalState = 0;//Azzera lo stato digitale degli allarmi
  //Legge i sensori in analogico e ricava lo stato digitale
  for (uint8_t ii=0; ii< NumSensori; ii++)
  {
    HallLettura[ii] = analogRead(HallPin[ii]);
    //Setta il risultato sotto forma digitale
    if(HallLettura[ii]> Soglia)
    {
      bitSet(HallDigitalState,ii);
    }
    else
    {
      bitClear(HallDigitalState, ii);
    }
  }
}

void BuzzAllarm()
{
  //Se un sensore è in allarme e il buzzer NON sta già suonando
  //Lo fa suonare
  if (HallDigitalState!=0)
  {
    switch (BuzzerState)
    {
      case 0://Allarme scattato Primo ciclo di allarme
        Serial.println("Imposto il primo tono");
        BuzzerState=1;//Indica che sta per suonare
        BuzzerTime = millis();//Prende il tempo quando inizia a suonare
        tone(Buzzer1,BuzzerTono1,BuzzerTCiclo);
        break;
      case 1://Allarme scattata Ciclo 1 in corso 
        if (millis()>=(BuzzerTime+BuzzerTCiclo))
        {
          Serial.println("Imposto tono 2");
          BuzzerState=2;//Indica che sta per suonare
          BuzzerTime = millis();//Prende il tempo quando inizia a suonare
          tone(Buzzer1,BuzzerTono2,BuzzerTCiclo);
        }
        break;
      case 2://Allarme scattata Ciclo 2 in corso 
        if (millis()>=(BuzzerTime+BuzzerTCiclo))
        {
          Serial.println("Imposto tono 1");
          BuzzerState=1;//Indica che sta per suonare
          BuzzerTime = millis();//Prende il tempo quando inizia a suonare
          tone(Buzzer1,BuzzerTono1,BuzzerTCiclo);
        }
        break;
      default:
        break;
    }
  }
  else
  {
    BuzzerState=0;
    LEDColor[0]= LEDColor[1] = 0;
  }
  
}

void LEDAllarm()
{
  //Aggiorna il colore dei LED
  if ((HallDigitalState&B00000011)>0)
  {
    LEDColor[0]=1;
  }
  if ((HallDigitalState&B0001100)>0)
  {
    LEDColor[1]=1;
  }
  
  if (millis()>(LEDTime+LEDFlash))
  //Quando il tempo è finito aggiorna i LED
  {
    LEDCounter++;
    if (LEDCounter<PixelNumber)
    //Se siamo all'interno del numero di LED per barra aggiorna i
    //LED accendendoli in sequenza
    {
      switch (LEDColor[0])
      //Cancello 1
      {
        case 0:
          Strip1DX.SetPixelColor(LEDCounter, red);
          Strip1SX.SetPixelColor(LEDCounter, red);
          break;
        case 1:
          Strip1DX.SetPixelColor(LEDCounter, blue);
          Strip1SX.SetPixelColor(LEDCounter, blue);
          break;
        case 2:
          Strip1DX.SetPixelColor(LEDCounter, green);
          Strip1SX.SetPixelColor(LEDCounter, green);
          break;
        default:
          break;        
      }        
      switch (LEDColor[1])
      //Cancello 2
      {
        case 0:
          Strip2DX.SetPixelColor(LEDCounter, red);
          Strip2SX.SetPixelColor(LEDCounter, red);
          break;
        case 1:
          Strip2DX.SetPixelColor(LEDCounter, blue);
          Strip2SX.SetPixelColor(LEDCounter, blue);
          break;
        case 2:
          Strip2DX.SetPixelColor(LEDCounter, green);
          Strip2SX.SetPixelColor(LEDCounter, green);
          break;
        default:
          break;        
      }        
    }
    if ((LEDCounter>=PixelNumber) & (LEDCounter<(PixelNumber*2)))
    //Spegne i LED in sequenza
    {      
      Strip1DX.SetPixelColor(LEDCounter-PixelNumber, black);
      Strip1SX.SetPixelColor(LEDCounter-PixelNumber, black);
      Strip2DX.SetPixelColor(LEDCounter-PixelNumber, black);
      Strip2SX.SetPixelColor(LEDCounter-PixelNumber, black);
    }
  }
  Strip1DX.Show();
  Strip1SX.Show();
  Strip2DX.Show();
  Strip2SX.Show();

}
