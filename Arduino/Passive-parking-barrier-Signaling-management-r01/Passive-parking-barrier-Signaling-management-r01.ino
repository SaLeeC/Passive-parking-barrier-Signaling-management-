//Versione che utilizza delle strisce RGB sulle sbarre

//===================================================================================================
//Sensori di SBLOCCO
//===================================================================================================
#define Sblocco1PIN 6
#define Sblocco2PIN 7
#define NumeroSblocchi 2
#define SbloccoAntibounce 200

uint8_t SbloccoPIN[NumeroSblocchi] = {Sblocco1PIN,
                                      Sblocco2PIN};
//Memorizza lo stato dei pulsanti di sblocco durante il loop
uint8_t SbloccoFlag = 0;
                                      
//===================================================================================================
// Buzzer
//===================================================================================================
#define Buzzer1PIN 12
#define BuzzerTCiclo 1000
#define BuzzerTono1 800
#define BuzzerTono2 1200

uint8_t BuzzerState=0;
uint32_t BuzzerTime;


//===================================================================================================
// Barre LED
//===================================================================================================
#define LEDBarraPIN12RossoPIN 2
#define LEDBarraPIN12VerdePIN 3
#define LEDBarraPIN34RossoPIN 4
#define LEDBarraPIN34VerdePIN 5

#define NumeroBarre 2
#define NumeroColori 2
const uint8_t LEDBarraPIN[NumeroBarre] [NumeroColori] = {LEDBarraPIN12RossoPIN,
                                                         LEDBarraPIN12VerdePIN,
                                                         LEDBarraPIN34RossoPIN,
                                                         LEDBarraPIN34VerdePIN};

#define LEDVisibleLevel 128

#define LEDFlash 300
uint8_t LEDColor[2]={0, 0};
uint8_t LEDCounter=0;
uint32_t LEDTime;

//===================================================================================================
// Sensori Sbarre
//===================================================================================================
#define SbarraSensor1APIN 0
#define SbarraSensor2APIN 1
#define SbarraSensor3APIN 2
#define SbarraSensor4APIN 3
#define NumSensori 4
uint16_t HallLettura[NumSensori];
uint8_t HallAPIN[NumSensori] = {SbarraSensor1APIN,
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

#define SerialSpeed 56700

//===================================================================================================
// Setup
//===================================================================================================
void setup() 
{
  Serial.begin(SerialSpeed);

  //Inizializza i sensori di sblocco
  for (uint8_t ii=0; ii<NumeroSblocchi; ii++)
  {
    pinMode(SbloccoPIN[ii],INPUT_PULLUP);
  }

  //Inizializza il controllo dei LED
  for (uint8_t ii=0; ii<NumeroBarre; ii++)
  {
    for (uint8_t iii=0; iii< NumeroColori; iii++)
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
  for (uint8_t ii=0; ii< NumSensori; ii++)
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
  for (uint8_t ii=0; ii< NumSensori; ii++)
  {
    //Se uno degli sblocchi è premuto ed è la prima volta che lo trova premuto
    if ((digitalRead(SbloccoPIN[ii] == LOW)) & (bitRead(SbloccoFlag,5+ii) == LOW))
    //Gestisce l'anti bounche
    {
      delay(SbloccoAntibounce);
      if (digitalRead(SbloccoPIN[ii] == LOW))
      {
        bitWrite(HallDigitalState,5+ii,!(bitRead(HallDigitalState,5+ii)));
        bitSet(SbloccoFlag,5+ii);
      }
    }
    else
    {
      //Resetta il bit che informa il loop che era già premuto
      bitClear(SbloccoFlag,5+ii);
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
  if (HallDigitalState!=0)
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
  if ((HallDigitalState&B00000011)>0)
  //Se uno dei sensori della varco 1 è in allarme setta il colore rosso
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
  }

}
