# Passive-parking-barrier-Signaling-management
 Signaling management for parking barrier hand-operated

This device sits halfway between the game and the functional.

It is controlled by contactless cards (RFID) which allow you to activate and deactivate the various functions.

Functions

- controls 1 or 2 gates with 1 or 2 bars for each gate (the bars are monitored by the system but are NOT controlled by the system)

- controls two RGB LED strips (one for each gate. In the case of gates that have two bars, the two strips of a gate are connected in parallel)

- produces signaling sounds (siren and simple musical sequences)

- reads RFID cards to allow control of the various functions.

Functionality

- gate bar control

Using Hall effect sensors (magnetic flow sensors), it checks whether the bars are in the closed gate position or not.
If one or more bars are not in the rest position (closed), it updates the system.

- control the RGB LED strips

The strips are used as a traffic light and as an indicator.

As traffic lights they can be:

> red flashing lights to indicate the bar closed or that access is NOT allowed in any case

> Green flashing lights to indicate that (once the bar is open) access to the workshop is allowed (with the Green traffic light the siren does NOT activate if the bars are raised)

As an indicator the LED strips:

> flash slowly (about 1.5 "on and 1.5" off) to indicate that the bus alarm system is ACTIVE (if the bars are not in the closed position, the siren is activated)

> flash quickly (about 0.8 "on and 0.8" off) to indicate that the bus alarm system is OFF (the siren will NOT intervene in any case)

- siren

It is a two-tone horn. It activates to signal that a bar has been raised with the red traffic light

- reading RFID cards

The system is controlled by RFID card. The minimum number of cards needed is 4 for two gates (3 for a single gate). The following function is associated with each card:

> Card 1 - Checking the status of Gate 1 (Red light or Green light)

> Card 2 - Checking the status of Gate 2 (Semafor Red or Green)

> Card 3 - Siren control (activate or deactivate the siren)

> Card 4 - Activate / deactivate Star Wars mode (in Star Wars mode the two gates are Green and the siren sounds the Star Wars Overtur). When the Overture is over or when the execution is interrupted, the traffic lights of the two gates remain Green

Components:

- 1 old (or cheap) computer speakers. They must be of sufficient size to contain all the electronics. They must be amplified and powered at 230V.

- 1 Arduino Nano or equivalent

- 1 RFID reader (MFRC-522)

- RFID card (4 or more as needed)

- 4 Mosfett optoisolated switch board

- 1 12V 10A external power supply (possibly double insulation)

- 1 standard 9-pin D-Sub female connector (9-pin serial connector)

- 1 male 9-pin D-Sub Standard flying connector (9-pin serial connector)

- 1 female D-Sub DB15 connector (VGA connector)

- 1 male D-Sub DB15 connector (VGA connector)

- 1 female SCART connector for panel

- 1 Scart cable with at least 3 coax and 1 unshielded wire (length as needed)

- RGB LED strip (length as needed)
