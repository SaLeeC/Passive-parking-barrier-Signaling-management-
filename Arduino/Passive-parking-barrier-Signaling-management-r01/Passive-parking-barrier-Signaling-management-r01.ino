//Versione che utilizza delle strisce RGB sulle sbarre

//===================================================================================================
//Sensori di SBLOCCO
//===================================================================================================
#define Sblocco1PIN 6
#define Sblocco2PIN 7
#define NumeroSblocchi 2
#define SbloccoAntibounce 200

int SbloccoPIN[NumeroSblocchi] = {Sblocco1PIN,
                                      Sblocco2PIN};
//Memorizza lo stato dei pulsanti di sblocco durante il loop
uint8_t SbloccoFlag = 0;

//===================================================================================================
//LED di SBLOCCO
//===================================================================================================
#define SbloccoLED1PIN 10
#define SbloccoLED2PIN 11
int SbloccoLEDPIN[NumeroSblocchi] = {SbloccoLED1PIN,
                                         SbloccoLED2PIN};

//===================================================================================================
// Buzzer
//===================================================================================================
#define Buzzer1PIN 12
#define BuzzerTCiclo 1000
#define BuzzerTono1 800
#define BuzzerTono2 1200

int BuzzerState=0;
float BuzzerTime;


//===================================================================================================
// Barre LED
//===================================================================================================
#define LEDBarraPIN12RossoPIN 2
#define LEDBarraPIN12VerdePIN 3
#define LEDBarraPIN34RossoPIN 4
#define LEDBarraPIN34VerdePIN 5

#define NumeroBarre 2
#define NumeroColori 2
const int LEDBarraPIN[NumeroBarre] [NumeroColori] = {LEDBarraPIN12RossoPIN,
                                                     LEDBarraPIN12VerdePIN,
                                                     LEDBarraPIN34RossoPIN,
                                                     LEDBarraPIN34VerdePIN};

#define LEDFlash 300
uint8_t LEDColor[2]={0, 0};
boolean LEDCounter=LOW;
float LEDTime;

//===================================================================================================
// Sensori Sbarre
//===================================================================================================
#define SbarraSensor1APIN 0
#define SbarraSensor2APIN 1
#define SbarraSensor3APIN 2
#define SbarraSensor4APIN 3
#define NumSensori 4
uint16_t HallLettura[NumSensori];
int HallAPIN[NumSensori] = {SbarraSensor1APIN,
                                SbarraSensor2APIN,
                                SbarraSensor3APIN,
                                SbarraSensor4APIN};
//Registra lo stato del sistema
//bit 8,7 richiesta di sblocco
//bit 6,5 Non usati
//bit 4,3 stato sensori porta 2
//bit 2,1 stato sensori porta 1
uint8_t HallDigitalState = 0;

#define SogliaPerHall 600

#define SerialSpeed 115200

//===================================================================================================
// Setup
//===================================================================================================
void setup() 
{
  Serial.begin(SerialSpeed);

  //Inizializza i sensori e i LED di sblocco
  for (int ii=0; ii<NumeroSblocchi; ii++)
  {
    pinMode(SbloccoPIN[ii],INPUT_PULLUP);
    pinMode(SbloccoLEDPIN[ii],OUTPUT);
  }

  //Inizializza il controllo dei LED
  for (int ii=0; ii<NumeroBarre; ii++)
  {
    for (int iii=0; iii< NumeroColori; iii++)
    {
      pinMode(LEDBarraPIN[ii][iii],OUTPUT);
    }
  }

  pinMode(Buzzer1PIN, OUTPUT);

}

//===================================================================================================
// LOOP
//===================================================================================================
void loop() 
{
  LeggeSblocco();
  Serial.print("Stato generale ");
  Serial.print(HallDigitalState, BIN);
  Serial.print("   -  Sblocco Flag ");
  Serial.println(SbloccoFlag, BIN);
  LeggeSensori(SogliaPerHall);
  BuzzAllarm();
  LEDAllarm();
}

//===================================================================================================
// Legge i sensori delle sbarre
//===================================================================================================
void LeggeSensori(uint16_t Soglia)
//Legge i 4 sensori ad effetto di Hall in analogico e restituisce
//il valore corrente nell'array HallLettura[] e lo stato logico
//rispetto alla Soglia e lo salva in HallDigitalState
{
  HallDigitalState = HallDigitalState & B11110000;//Azzera lo stato digitale degli allarmi
  //Legge i sensori in analogico e ricava lo stato digitale
  for (int ii=0; ii< NumSensori; ii++)
  {
    HallLettura[ii] = analogRead(HallAPIN[ii]);
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

//===================================================================================================
// Legge i pulsanti di sblocco
//===================================================================================================
void LeggeSblocco()
//Legge Sblocco permette di mettere in stato "VERDE" i gate
//Nello stato VERDE il buzzer non suona
{
  //Legge i sensori di sblocco
  for (int ii=0; ii< NumeroSblocchi; ii++)
  {
    //Se uno degli sblocchi è premuto ed è la prima volta che lo trova premuto
    if ((digitalRead(SbloccoPIN[ii]) == LOW) & (bitRead(SbloccoFlag,6+ii) == LOW))
    //Gestisce l'anti bounche
    {
      delay(SbloccoAntibounce);
      if (digitalRead(SbloccoPIN[ii]) == LOW)
      {
        bitWrite(HallDigitalState,6+ii,!(bitRead(HallDigitalState,6+ii)));
        bitSet(SbloccoFlag,6+ii);
        Serial.print(ii);
        Serial.println(" SbloccoFlag 167, SET");
        digitalWrite(SbloccoLEDPIN[ii],bitRead(HallDigitalState,6+ii));
      }
    }
    else
    {
      if ((digitalRead(SbloccoPIN[ii])==HIGH) & (bitRead(SbloccoFlag,6+ii) == HIGH))
      //Se il pulsante di sblocco è stato rilasciato
      //Gestisce l'anti bounche
      {
        delay(SbloccoAntibounce);
        if (digitalRead(SbloccoPIN[ii]) == HIGH)
        //Resetta il controllo del loop 
        {
          bitClear(SbloccoFlag,6+ii);
          Serial.print(ii);
          Serial.println(" SbloccoFlag 181, CLEAR");
        }
      }
    }
  }
}

//===================================================================================================
// Gestisce il buzzer
//===================================================================================================
void BuzzAllarm()
//Se:
//-un sensore è in allarme
//-per il gate dove è scatatto l'allarme non è stato settato lo stato VERDE
//-il buzzer NON sta già suonando
//lo fa suonare
{
  if (((HallDigitalState&B00000011)!=0) & ((HallDigitalState&B01000000)==0) |
      ((HallDigitalState&B00001100)!=0) & ((HallDigitalState&B10000000)==0))
  {
    switch (BuzzerState)
    {
      case 0://Allarme scattato Primo ciclo di allarme
        Serial.println("Imposto il primo tono");
        BuzzerState=1;//Indica che sta per suonare
        BuzzerTime = millis();//Prende il tempo quando inizia a suonare
        tone(Buzzer1PIN,BuzzerTono1,BuzzerTCiclo);
        break;
      case 1://Allarme scattato Ciclo 1 in corso 
        if (millis()>=(BuzzerTime+BuzzerTCiclo))
        {
          Serial.println("Imposto tono 2");
          BuzzerState=2;//Indica che sta per suonare
          BuzzerTime = millis();//Prende il tempo quando inizia a suonare
          tone(Buzzer1PIN,BuzzerTono2,BuzzerTCiclo);
        }
        break;
      case 2://Allarme scattat0 Ciclo 2 in corso 
        if (millis()>=(BuzzerTime+BuzzerTCiclo))
        {
          Serial.println("Imposto tono 1");
          BuzzerState=1;//Indica che sta per suonare
          BuzzerTime = millis();//Prende il tempo quando inizia a suonare
          tone(Buzzer1PIN,BuzzerTono1,BuzzerTCiclo);
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
//Lampeggia e aggiorna il colore dei LED
//
//Lampeggia rosso quando le sbarre sono abbassate.
//Lampeggia rosso ancora per TAlzata quando una sbarra è alzata
//Passato TAlzata smette di lampeggiare
//Lampeggia Verde quando è stato dato il comando di "avanti"
{
  if ((HallDigitalState & B0100000) == HIGH)
  //Verde sul cancello 1
  {
    LEDColor[0] = 1;
  }
  if ((HallDigitalState & B1000000) == HIGH)
  //Verde sul cancello 2
  {
    LEDColor[1] = 1;
  }
  if ((HallDigitalState & B0100000) == LOW)
  //Rosso sul cancello 1
  {
    LEDColor[0] = 0;
  }
  if ((HallDigitalState & B1000000) == LOW)
  //Rosso sul cancello 2
  {
    LEDColor[1] = 0;
  }
  if (millis()>(LEDTime+LEDFlash))
  //Quando il tempo è finito aggiorna i LED
  {
    //Spegne tutte le barre
    digitalWrite(LEDBarraPIN[0][0], HIGH);
    digitalWrite(LEDBarraPIN[0][1], HIGH);
    digitalWrite(LEDBarraPIN[1][0], HIGH);
    digitalWrite(LEDBarraPIN[1][1], HIGH);
    //Aggiorna il contatore di stato dei LED
    LEDCounter = !LEDCounter;
    if (LEDCounter == HIGH)
    //Se è il ciclo di LED accesi le setta
    {
      digitalWrite(LEDBarraPIN[0][LEDColor[0]], LOW);
      digitalWrite(LEDBarraPIN[1][LEDColor[1]], LOW);
    }
  }
}
