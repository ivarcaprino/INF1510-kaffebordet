/*
konstanter
*/

//brytere
int swEn = 1; // brasil
int swTo = 2; // etiopia
int swTre = 3; // melk
int swFire = 4; // stor
int swFem = 5; // bestill

//lys
int brasilLys = 6;
int etiopiaLys = 7;
int melkLys = 8;
int storLys = 9;
int bestillLys = 10;

//shiftregister
int dataPin = 11;
int latchPin = 12;
int clockPin = 13;

//bytes
const int antRegistere = 2; //hvor mange shiftregistere
byte data[antRegistere]; // array av bytes som sendes til shiftregistere

//debounce
unsigned long time = 0;
unsigned long debounce = 10;

/*
variabler
*/
//booleans
boolean etTrue = false;
boolean brTrue = false;
boolean meTrue = false;
boolean stTrue = false;
boolean beTrue = false;

//tilstander til brytere
int stateMelk = 0;
int lastMelk = 0;
int melkTrykk = 0;
int melkPaa = 0;
int stateStor = 0;
int lastStor = 0;
int storTrykk = 0;
int storPaa = 0;
int stateBestill = 0;

void setup() {
  // initialisere byte som sendes
  data[0] = 0;
  data[1] = 0;
  // initialisere utputs
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(brasilLys, OUTPUT);
  pinMode(etiopiaLys, OUTPUT);
  pinMode(melkLys, OUTPUT);
  pinMode(storLys, OUTPUT);
  pinMode(bestillLys, OUTPUT);
  // initialisere inputs
  pinMode(swEn, INPUT);
  pinMode(swTo, INPUT);
  pinMode(swTre, INPUT);
  pinMode(swFire, INPUT);
  pinMode(swFem, INPUT);
}

void loop() {
  if(digitalRead(swEn) == HIGH) { // brasil
    brTrue = true;
    etTrue = false;
  }
  if(digitalRead(swTo) == HIGH) { // etiopia
    etTrue = true;
    brTrue = false;
  }
  if(digitalRead(swTre) == HIGH) { // melk
    meTrue = true;
  }
  if(digitalRead(swFire) == HIGH) { // stor
    stTrue = true;
  }
  if(digitalRead(swFem) == HIGH) { // bestill
    beTrue = true;
  }
  
  //pga virkemaate for shiftregisteret maa funksjonen konstant kalles
  //for aa holde alle leds lysende i displayet for karakteristikk for de forskjellige
  //landene
  //kaller derfor egne funksjoner for aa styre hva som horer til hvert land
  if(brTrue){
    brasil();
  }
  if(etTrue){
    etiopia();
  }
  
  //for aa gjore koden penere er funksjonene for tilvalg flyttet til egne funksjoner
  if(meTrue){
    melk();
  }
  if(stTrue){
    stor();
  }
  if(beTrue){
    bestill();
  }
}

void brasil() {
  //slaar av og paa lys i brytere	  
  digitalWrite(brasilLys, HIGH);
  digitalWrite(etiopiaLys, LOW);
  //kaller funksjon for shiftregister til displayet
  //i egen funksjon
  shiftPin(0, HIGH);
  shiftPin(1, HIGH);
  shiftPin(2, HIGH);
  shiftPin(3, HIGH);
  shiftPin(4, HIGH);
  shiftPin(5, HIGH);
  shiftPin(6, LOW);
  shiftPin(8, HIGH);
  shiftPin(9, LOW);
  shiftPin(10, LOW);
}

void etiopia() {
  //slaar av og paa lys i brytere	  
  digitalWrite(etiopiaLys, HIGH);
  digitalWrite(brasilLys, LOW);
  //kaller funksjon for shiftregister til displayet
  //i egen funksjon
  shiftPin(0, HIGH);
  shiftPin(1, HIGH);
  shiftPin(2, LOW);
  shiftPin(3, LOW);
  shiftPin(4, HIGH);
  shiftPin(5, HIGH);
  shiftPin(6, HIGH);
  shiftPin(8, HIGH);
  shiftPin(9, HIGH);
  shiftPin(10, HIGH);
}

void melk() {
  /*
	koden folger standard oppsett for
	a sla av/pa for hvert trykk med en unlatched bryter
	bruker debounce og laststate for a se etter endringer
  */
  melkTrykk = digitalRead(swTre);
  if(melkTrykk != lastMelk) {
    time = millis();
  }
  if(millis() - time > debounce) {
    if(melkTrykk != stateMelk) {
      stateMelk = melkTrykk;
      if(stateMelk == HIGH){
        melkPaa = !melkPaa;
      }
    }
  }
  digitalWrite(melkLys, melkPaa);
  lastMelk = melkTrykk;
}

void stor() {
  /*
	koden folger standard oppsett for
	a sla av/pa for hvert trykk med en unlatched bryter
	bruker debounce og laststate for a se etter endringer
  */
  storTrykk = digitalRead(swFire);
  if(storTrykk != lastStor) {
    time = millis();
  }
  if(millis() - time > debounce) {
    if(storTrykk != stateStor) {
      stateStor = storTrykk;
      if(stateStor == HIGH){
        storPaa = !storPaa;
      }
    }
  }
  digitalWrite(storLys, storPaa);
  lastStor = storTrykk;
}

void bestill() {
  stateBestill = digitalRead(swFem);
  if(stateBestill == HIGH) {
	//alle valg blir resatt
    digitalWrite(brasilLys, LOW);
    digitalWrite(etiopiaLys, LOW);
    digitalWrite(melkLys, LOW);
	melkPaa=0;
    digitalWrite(storLys, LOW);
	storPaa=0;
    etTrue = false;
    brTrue = false;
    meTrue = false;
    stTrue = false;
    beTrue = false;
    shiftPin(0, LOW);
    shiftPin(1, LOW);
    shiftPin(2, LOW);
    shiftPin(3, LOW);
    shiftPin(4, LOW);
    shiftPin(5, LOW);
    shiftPin(6, LOW);
    shiftPin(7, LOW);
    shiftPin(8, LOW);
    shiftPin(9, LOW);
    shiftPin(10, LOW);
    shiftPin(11, LOW);

	//blinker bestillingsknapp 3 ganger
    for(int i = 0; i < 3; i++) {
      digitalWrite(bestillLys, HIGH);
      delay(200);
      digitalWrite(bestillLys, LOW);
      delay(200);

    }
  }
}

// funksjon for shifting av led
void shiftPin(int pin, boolean state){
  // regner ut hvilket register
  int reg = pin / 8;
  // finn pin i det aktuelle registeret
  int actualPin = pin - (8 * reg);
  // begynner write
  digitalWrite(latchPin, LOW);

	/*  
	shiftregistere er daisychainet sammen
	og pusher bytes ut videre ut av seg selv
	saa lenge det kommer et nytt
	man maa derfor sende til alle registere
	i rekken hver gang man skal oppdatere 
	
	pga dette er det kun siste pin man har oppdatert som holder seg high
	*/ 
  
  for (int i = 0; i < antRegistere; i++) {
    // finn state for pin i register
    byte states = data[i];
    // oppdater state
	// bitWrite er innebygget funksjon i spraket
    if (i == reg) {
      bitWrite(states, actualPin, state);
    }
    // write
	// shiftOut er innebygget funksjon i spraket
    shiftOut(dataPin, clockPin, MSBFIRST, states);
  }
  // avslutt write
  digitalWrite(latchPin, HIGH);
}
