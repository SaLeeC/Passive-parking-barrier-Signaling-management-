//Versione che utilizza delle strisce RGB sulle sbarre

//===================================================================================================
//Card RFID
//===================================================================================================
 /*
 * Typical pin layout used:
 * -----------------------------------------------------------------------------------------
 *             MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
 *             Reader/PCD   Uno/101       Mega      Nano v3    Leonardo/Micro   Pro Micro
 * Signal      Pin          Pin           Pin       Pin        Pin              Pin
 * -----------------------------------------------------------------------------------------
 * RST/Reset   RST          9             5         D9         RESET/ICSP-5     RST
 * SPI SS      SDA(SS)      10            53        D10        10               10
 * SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16
 * SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14
 * SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15
 *
 */

#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN         9           // Configurable, see typical pin layout above
#define SS_PIN          10          // Configurable, see typical pin layout above
#define RFIDResetManuale 8

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.

// Number of known default keys (hard-coded)
// NOTE: Synchronize the NR_KNOWN_KEYS define with the defaultKeys[] array
#define NR_KNOWN_KEYS   8
// Known keys, see: https://code.google.com/p/mfcuk/wiki/MifareClassicDefaultKeys
byte knownKeys[NR_KNOWN_KEYS][4] =  {{0x97, 0x96, 0x0C, 0x3B},//1
                                     {0x1E, 0xC8, 0x0C, 0x3B},//2
                                     {0x3C, 0xBA, 0xE7, 0xB5},//3
                                     {0x14, 0x08, 0x06, 0x9D},//4
                                     {0xE4, 0x7A, 0x9D, 0xA7},//5
                                     {0xD4, 0xED, 0x92, 0xA7},//6
                                     {0x3D, 0x63, 0x0B, 0x3B},//7
                                     {0xA4, 0x40, 0x1B, 0xB8} //8
                                     };

byte ReadKey[MFRC522::MF_KEY_SIZE];
byte RFIDFlag=99;
byte RFIDCompare=99;
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
uint8_t SbloccoCompare = 99;

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
  Serial.println("Start");
  //Inizializza il lettore di card RFID
  SPI.begin();                // Init SPI bus
  mfrc522.PCD_Init();         // Init MFRC522 card
  pinMode(RFIDResetManuale, OUTPUT);
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
  ReadRFID();
//  LeggePulsantiSblocco();
  GestisceCardSblocco();
//  Serial.print("Stato generale ");
//  Serial.print(HallDigitalState, BIN);
//  Serial.print("   -  Sblocco Flag ");
//  Serial.println(SbloccoFlag, BIN);
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
// Legge le card RFID
//===================================================================================================
void ReadRFID()
{
//  Serial.println("Inizio controllo CARD");
  // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
  if ( ! mfrc522.PICC_IsNewCardPresent())
  {
    if(RFIDFlag>=99)
    {
      RFIDFlag++;
      if (RFIDFlag==130)
      {
        SbloccoCompare=RFIDCompare;
        RFIDCompare=99;
        SbloccoFlag=0;
    Serial.println("Nessuna card rilevata");
      }
    }
    return;
  }

  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial())
  {
    Serial.println("--------------------------- Card Serial");
    RFIDCompare=99;
    return;
  }
  if(RFIDCompare==99)
  {
    Serial.println("Ciclo di reset");
//    digitalWrite(RFIDResetManuale,HIGH);
//    delay(10);
//    digitalWrite(RFIDResetManuale, !digitalRead(RFIDResetManuale));
//    delay(10);
    mfrc522.PCD_Init();         // Init MFRC522 card
    RFID_UID(mfrc522.uid.uidByte, mfrc522.uid.size);
  }
  Serial.print("Fine controllo CARD. Flag ");
  Serial.print(RFIDFlag);
  Serial.print(" Compare ");
  Serial.println(RFIDCompare);
}

//===================================================================================================
// Legge il buffer del lettore di card e controlla se conosce la card trovata
//===================================================================================================
void RFID_UID(byte *buffer, byte bufferSize)
{
  RFIDCompare = 99;
  for (byte i = 0; i < bufferSize; i++) 
  //Scandisce tutti i caratteri del codice
  {
//    Serial.print("Contatore ");
//    Serial.println(i);
    ReadKey[i]="";
    ReadKey[i]=buffer[i];
//    Serial.print(" Value [0] ");
//    Serial.print(ReadKey[i],HEX);
//    Serial.print(" Scan ");
    if ((i==0) & (RFIDCompare == 99))
    //Scandisce il primo carattere di ogni codice noto per vedere se corrisponde a quello letto
    {
      for (uint8_t iii=0; iii< NR_KNOWN_KEYS; iii++)
      {
//        Serial.print(iii);
//        Serial.print(" - ");
        if(ReadKey[i]==knownKeys[iii][i])
        {
//          Serial.print(" Value [1] ");
//          Serial.println(knownKeys[iii][i],HEX);
          RFIDCompare=iii;
          iii=NR_KNOWN_KEYS;
        }
      }
    }
    if ((i>0) & (RFIDCompare !=99))
    //Scandisce i caratteri successivi della chiave corrispondente al controllo del primo carattere
    {
        if(ReadKey[i]!=knownKeys[RFIDCompare][i])
        //Se uno dei caratteri successivi al primo non corrisponde blocca il controllo
        {
//          Serial.print(" Value [n] ");
//          Serial.println(knownKeys[RFIDCompare][i],HEX);
          RFIDCompare=99;
        }      
    }
  }
  Serial.print(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Fine controllo ");
  Serial.println(RFIDCompare);
  RFIDFlag=99;
}

//===================================================================================================
// Gestisce i comandi dati tramite le card di sblocco
//===================================================================================================
void GestisceCardSblocco()
{
  if (SbloccoFlag==0)
  //Se è la prima volta che ci passa
  //gestisce il caso
  {
    Serial.print("Sblocco Flag ");
    Serial.println(SbloccoFlag);
    Serial.print(" - Sblocco Compare ");
    Serial.println(RFIDCompare);
    SbloccoFlag++;
    switch (SbloccoCompare)
    {
      case 99://??
        break;
      case 0://Verde cancello 1
        bitWrite(HallDigitalState,6,!(bitRead(HallDigitalState,6)));
//        RFIDFlag=0;
        //RFIDCompare=99;
        Serial.println("========================================>Sbloccato cancello 1");
        break;
      case 1://Verde Cancello 2
        bitWrite(HallDigitalState,7,!(bitRead(HallDigitalState,7)));
//        RFIDFlag=1;
        //RFIDCompare=99;
        Serial.println("========================================>Sbloccato cancello 2");
        break;
      case 2:
        break;
      case 3:
        break;
      case 4:
        break;
      case 5:
        break;
      case 6:
        break;
      case 7:
        break;
      case 8:
        break;
      default:
        break;
    }
  }
}

//===================================================================================================
// Legge i pulsanti di sblocco
//===================================================================================================
void LeggePulsantiSblocco()
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
