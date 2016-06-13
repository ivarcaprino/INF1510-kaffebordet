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
//tilstander til brytere
int stateBrasil = 0;
int lastBrasil = 0;
int brasilTrykk = 0;
int brasilPaa = 0;
int stateEtiopia = 0;
int lastEtiopia = 0;
int etiopiaTrykk = 0;
int etiopiaPaa = 0;
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
  brasil();
  etiopia();
  melk();
  stor();
  bestill();
}

void brasil() {
  // bruker debounce og tilstandsendring på knappen for aa se om den er
  // trykket flere ganger. slaar av/paa lys
  brasilTrykk = digitalRead(swEn);
  if(brasilTrykk != lastBrasil) {
    time = millis();
  }
  if(millis() - time > debounce) {
    if(brasilTrykk != stateBrasil) {
      stateBrasil = brasilTrykk;
      if(stateBrasil == HIGH){
        brasilPaa = !brasilPaa;
        //maa nullstille det andre landet
        etiopiaPaa = 0;
        digitalWrite(etiopiaLys, etiopiaPaa);
      }
    }
  }
  digitalWrite(brasilLys, brasilPaa);
  //digitalWrite(etiopiaLys, etiopiaPaa);
  lastBrasil = brasilTrykk;

  //kaller funksjon til shiftregisteret
  for(int i=0; i<=8; i++){
    shiftPin(i, brasilPaa);
  }
  shiftPin(9, LOW);
  shiftPin(10, LOW);
}

void etiopia() {
  // bruker debounce og tilstandsendring på knappen for aa se om den er
  // trykket flere ganger. slaar av/paa lys
  etiopiaTrykk = digitalRead(swTo);
  if(etiopiaTrykk != lastEtiopia) {
    time = millis();
  }
  if(millis() - time > debounce) {
    if(etiopiaTrykk != stateEtiopia) {
      stateEtiopia = etiopiaTrykk;
      if(stateEtiopia == HIGH){
        etiopiaPaa = !etiopiaPaa;
        //maa nullstille det andre landet
        brasilPaa = 0;
        digitalWrite(brasilLys, brasilPaa);
      }
    }
  }
  digitalWrite(etiopiaLys, etiopiaPaa);
  lastEtiopia = etiopiaTrykk;

  //kaller funksjon for shiftregister til displayet
  //i egen funksjon
  shiftPin(0, etiopiaPaa);
  shiftPin(1, etiopiaPaa);
  shiftPin(2, LOW);
  shiftPin(3, LOW);
  shiftPin(4, etiopiaPaa);
  shiftPin(5, etiopiaPaa);
  shiftPin(6, etiopiaPaa);
  shiftPin(8, etiopiaPaa);
  shiftPin(9, etiopiaPaa);
  shiftPin(10, etiopiaPaa);
}

void melk() {
  // bruker debounce og tilstandsendring på knappen for aa se om den er
  // trykket flere ganger. slaar av/paa lys
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
  // bruker debounce og tilstandsendring på knappen for aa se om den er
  // trykket flere ganger. slaar av/paa lys
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
    digitalWrite(storLys, LOW);
    melkPaa=0;
    storPaa=0;
    etiopiaPaa=0;
    brasilPaa=0;
    for(int i=0; i<12; i++){
      shiftPin(i, LOW);
    }

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
