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
#define Sblocco1PIN 7
#define Sblocco2PIN 8
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
#define BuzzerLeftPIN 2
#define BuzzerTCiclo 1000
#define BuzzerTono1 800
#define BuzzerTono2 1200

int BuzzerState=0;
float BuzzerTime;


//===================================================================================================
// Barre LED
//===================================================================================================
#define LEDBarraPIN12RossoPIN 3
#define LEDBarraPIN12VerdePIN 4
#define LEDBarraPIN34RossoPIN 5
#define LEDBarraPIN34VerdePIN 6

#define NumeroBarre 2
#define NumeroColori 2
const int LEDBarraPIN[NumeroBarre] [NumeroColori] = {LEDBarraPIN12RossoPIN,
                                                     LEDBarraPIN12VerdePIN,
                                                     LEDBarraPIN34RossoPIN,
                                                     LEDBarraPIN34VerdePIN};

uint16_t LEDFlash[2] = {1400,800};
uint8_t LEDColor[2]={0, 0};
boolean LEDCounter=LOW;
float LEDTime;

//===================================================================================================
// Sensori Sbarre
//===================================================================================================
#define SbarraSensor1APIN 4
#define SbarraSensor2APIN 5
#define SbarraSensor3APIN 6
#define SbarraSensor4APIN 7
#define NumSensori 4
uint16_t HallLettura[NumSensori];
int HallAPIN[NumSensori] = {SbarraSensor1APIN,
                                SbarraSensor2APIN,
                                SbarraSensor3APIN,
                                SbarraSensor4APIN};
//Registra lo stato del sistema
//bit 8,7 richiesta di sblocco
//bit 6 Esegue Star Wars Teme
//bit 5 Blocca sirena
//bit 4,3 stato sensori porta 2
//bit 2,1 stato sensori porta 1
uint8_t HallDigitalState = 0;

#define SogliaPerHall 600

//===================================================================================================
// Seriale
//===================================================================================================
#define SerialSpeed 115200



//===================================================================================================
// Musica
//===================================================================================================
float ThisMoment;
uint8_t ThisNote;
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_CS8 4435
#define NOTE_D8  4699
#define NOTE_DS8 4978
#define REST      0


// change this to make the song slower or faster
int tempo = 108;

// there are tree values per note (pitch, duration and LED associated), so for each note there are four bytes
#define notes 88

// notes of the moledy followed by the duration.
// a 4 means a quarter note, 8 an eighteenth , 16 sixteenth, so on
// !!negative numbers are used to represent dotted notes,
// so -4 means a dotted quarter note, that is, a quarter plus an eighteenth!!
int melody[notes*2];
const int StarWarsOverture[notes*2] PROGMEM = {
  
  // Dart Vader theme (Imperial March) - Star wars 
  // Score available at https://musescore.com/user/202909/scores/1141521
  // The tenor saxophone part was used

//A -> 13
//B -> 12
//C -> 11
//D -> 10
//E -> 9
//F -> 7
//G -> 6
//REST -> 5
  
  
  NOTE_AS4,8, NOTE_AS4,8, NOTE_AS4,8,//1
  NOTE_F5,2,  NOTE_C6,2,
  NOTE_AS5,8, NOTE_A5,8,  NOTE_G5,8,  NOTE_F6,2, NOTE_C6,4,
  NOTE_AS5,8, NOTE_A5,8,  NOTE_G5,8,  NOTE_F6,2, NOTE_C6,4,
  NOTE_AS5,8, NOTE_A5,8,  NOTE_AS5,8, NOTE_G5,2, NOTE_C5,8, NOTE_C5,8, NOTE_C5,8,
  NOTE_F5,2,  NOTE_C6,2,
  NOTE_AS5,8, NOTE_A5,8,  NOTE_G5,8,  NOTE_F6,2, NOTE_C6,4,  
  
  NOTE_AS5,8, NOTE_A5,8, NOTE_G5,8,  NOTE_F6,2, NOTE_C6,4, //8  
  NOTE_AS5,8, NOTE_A5,8, NOTE_AS5,8, NOTE_G5,2, NOTE_C5,-8, NOTE_C5,16,
  NOTE_D5,-4, NOTE_D5,8, NOTE_AS5,8, NOTE_A5,8, NOTE_G5,8,  NOTE_F5,8, 
  NOTE_F5,8,  NOTE_G5,8, NOTE_A5,8,  NOTE_G5,4, NOTE_D5,8,  NOTE_E5, 4, NOTE_C5,-8, NOTE_C5,16,
  NOTE_D5,-4, NOTE_D5,8, NOTE_AS5,8, NOTE_A5,8, NOTE_G5,8,  NOTE_F5,8,
  
  NOTE_C6,-8, NOTE_G5,16, NOTE_G5,2,  REST,8,    NOTE_C5,8,//13
  NOTE_D5,-4, NOTE_D5,8,  NOTE_AS5,8, NOTE_A5,8, NOTE_G5,8,  NOTE_F5,8,
  NOTE_F5,8,  NOTE_G5,8,  NOTE_A5,8,  NOTE_G5,4, NOTE_D5,8,  NOTE_E5,4,  NOTE_C6,-8, NOTE_C6,16,
  NOTE_F6,4,  NOTE_DS6,8, NOTE_CS6,4, NOTE_C6,8, NOTE_AS5,4, NOTE_GS5,8, NOTE_G5,4,  NOTE_F5,8,
  NOTE_C6,1
  
};

const int PinkPanterTeme[notes*2] PROGMEM= {

  // Pink Panther theme
  // Score available at https://musescore.com/benedictsong/the-pink-panther
  // Theme by Masato Nakamura, arranged by Teddy Mason

  REST,2, REST,4, REST,8, NOTE_DS4,8, 
  NOTE_E4,-4, REST,8, NOTE_FS4,8, NOTE_G4,-4, REST,8, NOTE_DS4,8,
  NOTE_E4,-8, NOTE_FS4,8,  NOTE_G4,-8, NOTE_C5,8, NOTE_B4,-8, NOTE_E4,8, NOTE_G4,-8, NOTE_B4,8,   
  NOTE_AS4,2, NOTE_A4,-16, NOTE_G4,-16, NOTE_E4,-16, NOTE_D4,-16, 
  NOTE_E4,2, REST,4, REST,8, NOTE_DS4,4,

  NOTE_E4,-4, REST,8, NOTE_FS4,8, NOTE_G4,-4, REST,8, NOTE_DS4,8,
  NOTE_E4,-8, NOTE_FS4,8,  NOTE_G4,-8, NOTE_C5,8, NOTE_B4,-8, NOTE_G4,8, NOTE_B4,-8, NOTE_E5,8,
  NOTE_DS5,1,   
  NOTE_D5,2, REST,4, REST,8, NOTE_DS4,8, 
  NOTE_E4,-4, REST,8, NOTE_FS4,8, NOTE_G4,-4, REST,8, NOTE_DS4,8,
  NOTE_E4,-8, NOTE_FS4,8,  NOTE_G4,-8, NOTE_C5,8, NOTE_B4,-8, NOTE_E4,8, NOTE_G4,-8, NOTE_B4,8,   
  
  NOTE_AS4,2, NOTE_A4,-16, NOTE_G4,-16, NOTE_E4,-16, NOTE_D4,-16, 
  NOTE_E4,-4, REST,4,
  REST,4, NOTE_E5,-8, NOTE_D5,8, NOTE_B4,-8, NOTE_A4,8, NOTE_G4,-8, NOTE_E4,-8,
  NOTE_AS4,16, NOTE_A4,-8, NOTE_AS4,16, NOTE_A4,-8, NOTE_AS4,16, NOTE_A4,-8, NOTE_AS4,16, NOTE_A4,-8,   
  NOTE_G4,-16, NOTE_E4,-16, NOTE_D4,-16, NOTE_E4,16, NOTE_E4,16, NOTE_E4,2,
 
};


//// sizeof gives the number of bytes, each int value is composed of two bytes (16 bits)
//// there are two values per note (pitch and duration), so for each note there are four bytes
//int notes = sizeof(melody) / sizeof(melody[0]) / 3;

// this calculates the duration of a whole note in ms
int wholenote = (60000 * 4) / tempo;

int divider = 0, noteDuration = 0;


//===================================================================================================
// Setup
//===================================================================================================
void setup() 
{
  Serial.begin(SerialSpeed);
  Serial.println("Start");
  Serial.println(notes);
  //Inizializza il lettore di card RFID
  SPI.begin();                // Init SPI bus
  mfrc522.PCD_Init();         // Init MFRC522 card

/*
  Versione con la gestione degli sblocchi a pulsanti
  //Inizializza i sensori e i LED di sblocco
  for (int ii=0; ii<NumeroSblocchi; ii++)
  {
    pinMode(SbloccoPIN[ii],INPUT_PULLUP);
    pinMode(SbloccoLEDPIN[ii],OUTPUT);
  }
*/

  //Inizializza il controllo dei LED
  for (int ii=0; ii<NumeroBarre; ii++)
  {
    for (int iii=0; iii< NumeroColori; iii++)
    {
      pinMode(LEDBarraPIN[ii][iii],OUTPUT);
      digitalWrite(LEDBarraPIN[ii][iii],HIGH);
      delay(2000);
      digitalWrite(LEDBarraPIN[ii][iii],LOW);
      delay(500);
    }
  }

  pinMode(BuzzerLeftPIN, OUTPUT);
  PlayPink();
  ThisNote=0;
  ThisMoment=0;
  while(ThisNote<=48)
  {
    Serial.println(ThisNote);
    PlayMusic();
  }
  PlayStar();
}

//===================================================================================================
// LOOP
//===================================================================================================
void loop() 
{
  ReadRFID();
  GestisceCardSblocco();
  if (bitRead(HallDigitalState,5)==1)
  {
    PlayMusic();
  }
  else
  {
    LeggeSensori(SogliaPerHall);
    BuzzAllarm();
  }
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
  //Serial.print("Analogici ");
  for (int ii=0; ii< NumSensori; ii++)
  {
    HallLettura[ii] = analogRead(HallAPIN[ii]);
    //Serial.print(HallLettura[ii]);
    //Serial.print(" - ");
    //Setta il risultato sotto forma digitale
    if(HallLettura[ii]> Soglia)
    {
      bitClear(HallDigitalState,ii);
    }
    else
    {
      bitSet(HallDigitalState, ii);
    }
  }
//  //Serial.println();
}

//===================================================================================================
// Legge le card RFID
//===================================================================================================
void ReadRFID()
{
//  //Serial.println("Inizio controllo CARD");
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
//        //Serial.println("Nessuna card rilevata");
      }
    }
    return;
  }

  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial())
  {
    //Serial.println("--------------------------- Card Serial");
    RFIDCompare=99;
    return;
  }
  if(RFIDCompare==99)
  {
    //Serial.println("Ciclo di reset");
    mfrc522.PCD_Init();         // Init MFRC522 card
    RFID_UID(mfrc522.uid.uidByte, mfrc522.uid.size);
  }
  //Serial.print("Fine controllo CARD. Flag ");
  //Serial.print(RFIDFlag);
  //Serial.print(" Compare ");
  //Serial.println(RFIDCompare);
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
//    //Serial.print("Contatore ");
//    //Serial.println(i);
    ReadKey[i]="";
    ReadKey[i]=buffer[i];
//    //Serial.print(" Value [0] ");
//    //Serial.print(ReadKey[i],HEX);
//    //Serial.print(" Scan ");
    if ((i==0) & (RFIDCompare == 99))
    //Scandisce il primo carattere di ogni codice noto per vedere se corrisponde a quello letto
    {
      for (uint8_t iii=0; iii< NR_KNOWN_KEYS; iii++)
      {
//        //Serial.print(iii);
//        //Serial.print(" - ");
        if(ReadKey[i]==knownKeys[iii][i])
        {
//          //Serial.print(" Value [1] ");
//          //Serial.println(knownKeys[iii][i],HEX);
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
//          //Serial.print(" Value [n] ");
//          //Serial.println(knownKeys[RFIDCompare][i],HEX);
          RFIDCompare=99;
        }      
    }
  }
  //Serial.print(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Fine controllo ");
  //Serial.println(RFIDCompare);
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
    SbloccoFlag++;
    switch (SbloccoCompare)
    {
      case 99://??
        break;
      case 0://CARD 1 - Verde cancello 1
        ThisMoment=0;
        bitWrite(HallDigitalState,6,!(bitRead(HallDigitalState,6)));
        //Azzera il contatore dei LED così da effettuare subito il controllo
        LEDTime=0;
        //Serial.print(HallDigitalState,BIN);
        //Serial.println("========================================>Sbloccato cancello 1");
        break;
      case 1://CARD 2 - Verde Cancello 2
        ThisMoment=0;
        bitWrite(HallDigitalState,7,!(bitRead(HallDigitalState,7)));
        //Azzera il contatore dei LED così da effettuare subito il controllo
        LEDTime=0;
        //Serial.print(HallDigitalState,BIN);
        //Serial.println("========================================>Sbloccato cancello 2");
        break;
      case 2:
        break;
      case 3:
        break;
      case 4:
        break;
      case 5:
        break;
      case 6://CARD 7 - Silenzia la sirena
        bitWrite(HallDigitalState,4,!bitRead(HallDigitalState,4));
        break;
      case 7://CARD 8 - Tutto VERDE ed esegue Star Wars
        bitSet(HallDigitalState,7);
        bitSet(HallDigitalState,6);
        bitWrite(HallDigitalState,5,!bitRead(HallDigitalState,5));
        LEDAllarm();
        ThisNote=0;
        ThisMoment=0;
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
        //Serial.print(ii);
        //Serial.println(" SbloccoFlag 167, SET");
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
          //Serial.print(ii);
          //Serial.println(" SbloccoFlag 181, CLEAR");
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
//-Il bit 4 è settato spegne l'allarme e passa oltre
//
//-un sensore è in allarme
//-per il gate dove è scatatto l'allarme non è stato settato lo stato VERDE
//-il buzzer NON sta già suonando
//lo fa suonare
{
  if (HallDigitalState&B00010000==0)
  {
    if(((HallDigitalState&B00000011)!=0) & ((HallDigitalState&B01000000)==0) |
      ((HallDigitalState&B00001100)!=0) & ((HallDigitalState&B10000000)==0))
    {
      switch (BuzzerState)
      {
        case 0://Allarme scattato Primo ciclo di allarme
          //Serial.println("Imposto il primo tono");
          BuzzerState=1;//Indica che sta per suonare
          BuzzerTime = millis();//Prende il tempo quando inizia a suonare
          tone(BuzzerLeftPIN,BuzzerTono1,BuzzerTCiclo);
          break;
        case 1://Allarme scattato Ciclo 1 in corso 
          if (millis()>=(BuzzerTime+BuzzerTCiclo))
          {
            //Serial.println("Imposto tono 2");
            BuzzerState=2;//Indica che sta per suonare
            BuzzerTime = millis();//Prende il tempo quando inizia a suonare
            tone(BuzzerLeftPIN,BuzzerTono2,BuzzerTCiclo);
          }
          break;
        case 2://Allarme scattat0 Ciclo 2 in corso 
          if (millis()>=(BuzzerTime+BuzzerTCiclo))
          {
            //Serial.println("Imposto tono 1");
            BuzzerState=1;//Indica che sta per suonare
            BuzzerTime = millis();//Prende il tempo quando inizia a suonare
            tone(BuzzerLeftPIN,BuzzerTono1,BuzzerTCiclo);
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
  else
  {
    noTone(BuzzerLeftPIN);
  }
}

//===================================================================================================
// Gestisce i segnalatori a LED
//===================================================================================================
void LEDAllarm()
//Lampeggia e aggiorna il colore dei LED
//
//Lampeggia rosso quando le sbarre sono abbassate.
//Lampeggia rosso ancora per TAlzata quando una sbarra è alzata
//Passato TAlzata smette di lampeggiare
//Lampeggia Verde quando è stato dato il comando di "avanti"
{
  //La frequenza di lampeggio è doppia. Più lenta quando la sirena è attivabile, veloce quando
  //la sirena è stat disabilitata
  if (millis()>(LEDTime+LEDFlash[bitRead(HallDigitalState, 4)]))
  //Quando il tempo è finito aggiorna i LED
  {
    //Aggiorna il riferimento del tempo
    LEDTime=millis();

    //Serial.print("STATUS bit ");
    //Serial.print(HallDigitalState);
    //Serial.print(" & ");
    //Serial.print(HallDigitalState&B10000000);
    //Serial.print(" Sono dentro a ");

    //Cancello 1
    if (bitRead(HallDigitalState,6) == 1)
    //Verde sul cancello 1
    {
      //Serial.print("CANCELLO 1 - inverti VERDE");
      //Inverte lo stato della barra verde
      digitalWrite(LEDBarraPIN[0][1], !digitalRead(LEDBarraPIN[0][1]));
      //Spegne il rosso
      digitalWrite(LEDBarraPIN[0][0], LOW);
    }
    else
    //Inverte lo stato della barra rossa
    {
      //Serial.print("CANCELLO 1 - inverti ROSSO");
      digitalWrite(LEDBarraPIN[0][0], !digitalRead(LEDBarraPIN[0][0]));
      //Spegne il verde
      digitalWrite(LEDBarraPIN[0][1], LOW);
    }

    //Cancello 2
    if (bitRead(HallDigitalState,7) == 1)
    //Verde sul cancello 2
    {
      //Inverte lo stato della barra verde
      //Serial.println(" - CANCELLO 2 - inverti VERDE");
      digitalWrite(LEDBarraPIN[1][1], !digitalRead(LEDBarraPIN[1][1]));
      //Spegne il rosso
      digitalWrite(LEDBarraPIN[1][0], LOW);
    }
    else
    //Inverte lo stato della barra rossa
    {
      //Serial.println(" - CANCELLO 2 - inverti ROSSO");
      digitalWrite(LEDBarraPIN[1][0], !digitalRead(LEDBarraPIN[1][0]));
      //Spegne il verde
      digitalWrite(LEDBarraPIN[1][1], LOW);
    }
  }
}


void PlayMusic() 
{ 
  if (ThisMoment==0)
  //Se non sta suonando una nota inizia a suonare la prossima
  {
    // calculates the duration of each note
    divider = melody[ThisNote + 1];
    if (divider > 0) {
      // regular note, just proceed
      noteDuration = (wholenote) / divider;
    } else if (divider < 0) {
      // dotted notes are represented with negative durations!!
      noteDuration = (wholenote) / abs(divider);
      noteDuration *= 1.5; // increases the duration in half for dotted notes
    }

    //Azzera il tempo
    ThisMoment=millis();

    // we only play the note for 90% of the duration, leaving 10% as a pause
    tone(BuzzerLeftPIN, melody[ThisNote], noteDuration*0.95);
  }
  else 
  {
    if(millis() >= ThisMoment+noteDuration)
    {
      // stop the waveform generation before the next note.
      //noTone(BuzzerLeftPIN);
      ThisMoment=0;
      if (ThisNote<=(notes*3))
      {
        ThisNote+=2;
      }
      else
      {
        ThisNote=0;
      }
    }
  }
}

void PlayPink()
{
  tempo = 120;
  for (int ii=0;ii< notes*2; ii++)
  {
    melody[ii] = pgm_read_word_near(PinkPanterTeme + ii);
  }
}

void PlayStar()
{
  tempo = 108;
  for (int ii=0;ii< notes*2; ii++)
  {
    melody[ii] = pgm_read_word_near(StarWarsOverture + ii);
  }
}
