# Gestione passiva-parcheggio-barriera-Segnalazione
 Gestione della segnalazione per barriera di parcheggio azionata manualmente

Questo dispositivo si trova a metà strada tra il gioco e il funzionale.

È controllato da schede contactless (RFID) che consentono di attivare e disattivare le varie funzioni.

Funzioni

- controlla 1 o 2 cancelli con 1 o 2 barre per ciascun cancello (le barre sono monitorate dal sistema ma NON sono controllate dal sistema)

- controlla due strisce LED RGB (una per ogni cancello. Nel caso di cancelli che hanno due barre, le due strisce di un cancello sono collegate in parallelo)

- produce suoni di segnalazione (sirena e semplici sequenze musicali)

- legge le carte RFID per consentire il controllo delle varie funzioni.

Funzionalità

- controllo della barra del cancello

Utilizzando sensori ad effetto Hall (sensori di flusso magnetico), controlla se le barre sono in posizione di cancello chiuso o meno.
Se una o più barre non sono nella posizione di riposo (chiusa), aggiorna il sistema.

- controlla le strisce LED RGB

Le strisce sono utilizzate come semaforo e come indicatore.

Come semafori possono essere:

> luci rosse lampeggianti per indicare la barra chiusa o che l'accesso NON è consentito in nessun caso

> Luci verdi lampeggianti per indicare che (una volta aperta la barra) è consentito l'accesso all'officina (con il semaforo verde la sirena NON si attiva se le barre sono sollevate)

Come indicatore le strisce LED:

> lampeggia lentamente (circa 1,5 "acceso e 1,5" spento) per indicare che il sistema di allarme del bus è ATTIVO (se le barre non sono in posizione chiusa, la sirena è attivata)

> lampeggia rapidamente (circa 0,8 "acceso e 0,8" spento) per indicare che il sistema di allarme del bus è spento (la sirena NON interverrà comunque)

- sirena

È un dispositivo a due toni. Si attiva per segnalare che una barra è stata sollevata con il semaforo rosso

- lettura di carte RFID

Il sistema è controllato dalla scheda RFID. Il numero minimo di carte necessario è 4 per due porte (3 per una sola porta).
La seguente funzione è associata ad ogni carta:

> Scheda 1 - Verifica dello stato del Gate 1 (Semaforo rossa o luce verde)

> Scheda 2 - Verifica dello stato del Gate 2 (Semaforo rosso o verde)

> Scheda 3 - Controllo sirena (attiva o disattiva la sirena)

> Carta 4 - Attiva / disattiva la modalità Star Wars (nella modalità Star Wars le due porte sono verdi e la sirena suona il Star Wars Overtur). Quando l'Overture termina o quando l'esecuzione viene interrotta, i semafori delle due porte rimangono verdi

Struttura hardware

Il dispositivo è composto da:

- un'unità di controllo (scheda Arduino)

- quattro driver di alimentazione per l'accensione dei LED (MOSFET optoisolati)

- quattro sensori ad effetto Hall (tipo con uscita analogica ma possono funzionare anche quelli con uscita digitale)

- una scheda per la lettura di dispositivi RFID

- un amplificatore di potenza audio (quello contenuto negli cassette per PC) collegato agli altoparlanti.

- Un alimentatore per la parte logica (collega Arduino e le altre schede logiche all'alimentatore dell'amplificatore)

- un alimentatore da 12V (per accendere i LED)

- Striscia RGB led

I LED e la parte di potenza che li controlla sono alimentati separatamente tramite l'alimentatore a 12V. La terra logica NON è collegata alla terra di alimentazione. Questa scelta aumenta la sicurezza del dispositivo contro il pericolo di folgorazione.

Struttura del software

Il codice è stato sviluppato in moduli indipendenti e interconnessi. L'elemento principale che fornisce le indicazioni operative dei singoli moduli è la variabile di stato (HallDigitalState), i 4 bit meno significativi registrano lo stato dei sensori ad effetto Hall, il quinto bit attiva o disattiva la sirena, il sesto attiva o disattiva la modalità Guerre Stellari Overture e la settima e l'ottava riportano lo stato dei semafori.

I singoli moduli vengono richiamati in modo che tutte le funzioni vengano eseguite nella sequenza corretta.

Nel loop sono stati definiti due percorsi, uno semplificato dedicato alla fase di esecuzione di Star Wars Overture e l'altro completo. questa scelta è stata fatta per ottimizzare l'esecuzione del pezzo.

Ciclo di inizializzazione.

Durante il ciclo di inizializzazione, le luci verde e rossa di ciascun gate vengono accese in successione e la parte audio viene testata eseguendo l'avvio del tema Pantera rosa. Questo brano viene eseguito solo durante il ciclo di inizializzazione.

componenti:

- 1 altoparlanti per computer vecchi (o economici). Devono essere di dimensioni sufficienti per contenere tutta l'elettronica. Devono essere amplificati e alimentati a 230V.

- 1 Arduino Nano o equivalente

- 1 lettore RFID (MFRC-522)

- Scheda RFID (4 o più secondo necessità)

- 4 quadri elettrici optoisolati Mosfett

- 1 alimentatore esterno 12V 10A (possibilmente doppio isolamento)

- 1 connettore D-Sub femmina standard a 9 pin (connettore seriale a 9 pin)

- 1 connettore volante D-Sub standard a 9 pin maschio (connettore seriale a 9 pin)

- 1 connettore D-Sub DB15 femmina (connettore VGA)

- 1 connettore D-Sub DB15 maschio (connettore VGA)

- 1 connettore SCART femmina per pannello

- 1 cavo Scart con almeno 3 cavi coassiali e 1 filo non schermato (lunghezza secondo necessità)

- Striscia LED RGB (lunghezza secondo necessità)
