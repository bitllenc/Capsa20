#include <Capsa20.h>

#define ST_1_INICI              1
#define ST_2_INDICACIO_CANCO    2 
#define ST_3_REPRODUIR_CANCO    3
#define ST_4_SEQUENCIA_REPRO    4
#define ST_41_SEQUENCIA_ESCOLTA 41
#define ST_42_SEQUENCIA_CORRECTA 42
#define ST_5_CANCO_CORRECTA     5
#define ST_6_EQUIVOCACIO        6
#define ST_10_DESBLOQUEIG       7
#define ST_11_DESBLOQUEJAT      8

#define TEMPS_OBERTURA  5000 //ms
#define COMBINACIO_SECRETA 85 //0101_0101
#define MAX_CANCONS 6  //Nombre de cançons que tenim
#define MAX_NOTES 100   //Nombre màxim de notes que pot tenir una cançó

#define NO_ERROR 0
#define SI_ERROR 1

#define pLOCK  4
#define pALTAVEU 5
#define pLED_Do  6
#define pLED_Re  7
#define pLED_Mi  8
#define pLED_Fa  9
#define pLED_Sol 10
#define pLED_La  11
#define pLED_Si  12
#define pSW_Do  A0 
#define pSW_Re  A1
#define pSW_Mi  A2
#define pSW_Fa  A3
#define pSW_Sol A4
#define pSW_La  A5
#define pSW_Si  3
/*
typedef struct tNotes {
	unsigned int freq;
	byte led;	
}tNota;
*/

byte SWpremuts;
byte pSW[7] = {pSW_Do,   pSW_Re,  pSW_Mi,  pSW_Fa,  pSW_Sol,  pSW_La,  pSW_Si};
byte pLED[7] = {pLED_Do, pLED_Re, pLED_Mi, pLED_Fa, pLED_Sol, pLED_La, pLED_Si};

int idxGen = 0;        //Índex multiús
int idxCanco = 0;      //Nombre de cançons endevinades
int idxNota = 0;       //Nombre de notes màxim que ha assolit en una cançó
int idxMemoria = 0;    //Notes que ha avançat en el joc (idxMemoria <= idxNota)
char errorMemoria = NO_ERROR; //Indica si ha comès algun error en les notes premudes
int Estat, EstatSeguent;


Nota N;

int numNotes[MAX_CANCONS] = {18,52,39,32,9};
char NOTES[MAX_CANCONS][MAX_NOTES] = {
        {Re4, Sol4, Sol4, Sol4, Re4, La4, FaS4, Sol4, Re4, Sol4, Do5, Do5, Si4, La4, Sol4, FaS4, Sol4, La4},
        {Do4, Fa4, Sol4, La4, Sol4, Mi4, Do4, Fa4, Sol4, La4, Sol4, Do4, Fa4, Sol4, La4, Sol4, Mi4, Mi4, Fa4, Mi4, Do4, Do4, Si4, Si4, La4, Sol4, Si4, Sol4, La4, Fa4, Sol4, Si4, Si4, La4, Sol4, Si4, Sol4, Do4, Si4, Si4, La4, Sol4, Si4, Sol4, La4, Fa4, Sol4, Mi4, Fa4, Mi4, Do4, Do4},
        {La4, Re4, Fa4, Sol4, La4, Re4, Fa4, Sol4, La4, Re4, Fa4, Sol4, La4, Re4, Fa4, Sol4, La4, Re4, Fa4, Sol4, La4, Re4, Fa4, Sol4, Sol4, Do4, Fa4, Mi4, Sol4, Do4, La4, Re4, Mi4, Fa4, Sol4, Do4, Fa4, Mi4, Re4},
        {La4, La4, La4, Si4, La4, La4, Sol4, La4, La4, La4, La4, Si4, La4, La4, Si4, Si4, Si4, La4, Sol4, La4, Sol4, La4, Sol4, Si4, La4, La4, La4, Si4, Si4, La4, La4, Sol4},
        {La4, La4, La4, La4, La4, Si4, La4, La4, Fa4}
        };

                
unsigned int TEMPS[MAX_CANCONS][MAX_NOTES] = {
      {NE,  COp,  SC,   BL,   NE,  COp, SC,   BL,   NE,  COp,  SC,  NE,  COp, SC,   NE,   COp,  SC,   BL},
      {CO, CO, CO, CO, NE, BL, CO, CO, CO, CO, BL, CO, CO, CO, CO, NE, BL, CO, CO, CO, CO, BL, CO, CO, CO, CO, NE, CO, NE, CO, NE, CO, CO, CO, CO, NE, NE, CO, CO, CO, CO, CO, NE, CO, NE, CO, NE, CO, CO, CO, CO, BL},
      {CO, CO, SC, SC, CO, CO, SC, SC, CO, CO, SC, SC, CO, CO, SC, SC, BL, BL, SC, SC, NE, NE, SC, SC, BL, BL, SC, SC, NE, BL, BL, BL, CO, CO, NE, BL, CO, CO, BL},
      {NE, CO, NE, NE, CO, NE, NE, NE, CO, NE, SC, CO, CO, NE, CO, CO, SC, NE, SC, CO, SC, CO, SC, NE, CO, CO, CO, CO, CO, CO, CO, BL},
      {CO, CO, CO, CO, CO, RO, CO, NE, BL}
      };
      
/*

int numNotes[1] = 18;
char NOTES[1]         = {Re4, Sol4, Sol4, Sol4, Re4, La4, FaS4, Sol4, Re4, Sol4, Do5, Do5, Si4, La4, Sol4, FaS4, Sol4, La4};
unsigned int TEMPS[1] = {NE,  COp,  SC,   BL,   NE,  COp, SC,   BL,   NE,  COp,  SC,  NE,  COp, SC,   NE,   COp,  SC,   BL};

int numNotes[2] = 30;//346;
char NOTES[2] =         {Mi4, Mi4, Mi4, Re4, Mi4, Fa4, Mi4, Sil, Mi4, Mi4, Mi4, Re4, Mi4, Fa4, Mi4, Re4, Do4, Sil, Re4, Re4, Si4, Re4, Do4, Do4, Re4, Mi4, Si4, Mi4, Mi4, Mi4}; //, Re4, Sil, Mi4, Mi4, Mi4, Re4, Fa4, Sil, Mi4, Mi4, Re4, Mi4, Do4, Sil, Mi4, Mi4, Fa4, Mi4, Sil, Mi4, Mi4, Mi4, Re4, Sil, Mi4, Re4, Mi4, Mi4, Re4, Mi4, Mi4, Sil, Fa4, Fa4, Fa4, Sol4, Fa4, Fa4, Fa4, Sil, Fa4, Fa4, Fa4, Sol4, Fa4, Fa4, Fa4, Sil, La4, La4, La4, Sol4, Sil, Sol4, Sol4, Fa4, Sol4, Sol4, Sol4, Sil, Fa4, Fa4, Fa4, Mi4, Mi4, Mi4, Mi4, Re4, Sil, Mi4, Mi4, Mi4, Re4, Mi4, Fa4, Mi4, Sil, Mi4, Mi4, Mi4, Re4, Mi4, Fa4, Mi4, Re4, Do4, Sil, Re4, Re4, Do4, Do4, Re4, Re4, Mi4, Sil, Mi4, Mi4, Mi4, Re4, Mi4, Mi4, Mi4, Re4, Fa4, Sil, Mi4, Mi4, Re4, Mi4, Do4, Sil, Mi4, Mi4, Mi4, Fa4, Mi4, Mi4, Sil, Mi4, Mi4, Mi4, Mi4, Re4, Sil, Mi4, Mi4, Mi4, Mi4, Mi4, Mi4, Mi4, Sil, Fa4, Fa4, Fa4, Sol4, Fa4, Fa4, Fa4, Sil, Fa4, Fa4, Fa4, Sol4, Fa4, Fa4, Fa4, Sil, La4, La4, La4, Sol4, Sil, Sol4, Sol4, Fa4, Sol4, Sol4, Sol4, Sil, Fa4, Fa4, Fa4, Mi4, Mi4, Mi4, Mi4, Re4, Sil, Fa4, Fa4, Fa4, Sol4, Fa4, Fa4, Fa4, Sil, Fa4, Fa4, Fa4, Sol4, Fa4, Fa4, Fa4, Sil, La4, La4, La4, Sol4, Sil, Sol4, Sol4, Fa4, Sol4, Sol4, Sol4, Sil, Fa4, Fa4, Fa4, Mi4, Mi4, Mi4, Mi4, Re4, Sil, Fa4, Fa4, Fa4, Fa4, Sol4, Fa4, Fa4, Mi4, Mi4, Re4, Sil, Fa4, Fa4, Fa4, Sol4, Fa4, Mi4, Sil, Sol4, La4, La4, La4, La4, Sol4, Sil, Sol4, La4, Sol4, La4, Sol4, La4, La4, Sol4, Fa4, Mi4, Si4, Sol4, Sol4, Sol4, Fa4, Mi4, Re4, Si4, Fa4, Fa4, Fa4, Fa4, Sol4, Fa4, Fa4, Mi4, Mi4, Re4, Sil, Fa4, Fa4, Fa4, Fa4, Sol4, Fa4, Fa4, Sil, Sol4, La4, La4, La4, La4, Sol4, Sil, Sol4, La4, Sol4, La4, Sol4, La4, La4, Sol4, Fa4, Mi4, Sil, Sol4, Sol4, Sol4, Fa4, Mi4, Re4, Sil, La4, La4, La4, Sol4, Sil, Fa4, Sol4, Fa4, Sol4, Fa4, Sol4, La4, Sol4,  Si4, La4, La4, La4, Si4, Si4,  La4, La4, Sol4,  Si4};
unsigned int TEMPS[2] = {CO,  CO,  BL,  BL,  BL,  BL,  RO,  NE,  CO,  CO,  CO,  CO,  CO,  CO,  CO,  CO,  BL,  NE,  CO,  CO,  CO,  CO,  CO,  CO,  CO,  CO,  NE,  CO,  CO,  CO}; //,  CO, CO, CO, CO, CO, CO, CO, NE, NE, NE, CO, CO, BL, NE, NE, NE, CO, BL, NE, CO, CO, CO, CO, NE, CO, CO, NE, CO, CO, CO, BL, NE, NE, NE, NE, NE, NE, NE, BL, NE, NE, NE, NE, NE, NE, NE, NE, NE, NE, NE, NE, NE, NE, NE, NE, NE, NE, NE, NE, NE,  NE, NE, NE, NE, NE, NE, NE, BL, NE, CO, CO, BL, BL, BL, BL, RO, NE,  CO, CO, CO, CO, CO, CO, CO, CO, BL, NE, CO, CO, CO, CO, CO, CO, CO, NE, CO, CO, CO, CO, CO, CO, CO, CO, CO, NE, NE, NE, CO, CO, BL, NE, CO, NE, SC, NE, SC, BL, NE, CO, CO, CO, CO, CO, NE, CO, CO, NE, CO, CO, CO, BL, NE, NE, NE, NE, NE, NE, NE, BL, NE, NE, NE, NE, NE, NE, NE, NE, NE, NE, NE, NE, NE, NE, NE, NE, NE, NE, NE, NE, NE,  NE, NE, NE, NE, NE, NE, NE, BL, NE, NE, NE, NE, NE, NE, NE, BL, NE, NE, NE, NE, NE, NE, NE, NE, NE, NE, NE, NE, NE, NE, NE, NE, NE, NE, NE, NE, NE,  NE, NE, NE, NE, NE, NE, NE, BL, NE, NE, NE, NE, NE, NE, NE, NE ,NE, NE, NE, NE, NE, NE, NE ,NE, NE, NE, NE, CO, CO, CO, CO, CO, CO, NE, CO, CO, CO, CO, CO, CO, CO, CO, CO, CO, NE CO, CO, CO, NE, NE, NE, NE, NE, NE, NE, BL, NE, NE, NE, NE, NE, BL, NE, NE, NE, NE, NE, NE, NE, BL, NE CO, CO, CO, CO, CO, NE, NE, CO, CO, CO, CO ,CO, CO, CO, CO, CO, CO, NE, NE, NE, NE, BL, BL, BL, NE, SC, SC, SC, SC, SC, SC, SC, SC, SC, SC, SC, SC, SC, NE, NE, NE, NE, NE, NE, NE, NE, BL, NE};

int numNotes[3] = 37; //178;
char NOTES[3] =         {Fa4, Fa4, Fa4, Fa4, Mi4, Fa4, Sil, Fa4, Fa4, Fa4, Fa4, Mi4, Fa4, Sil, Fa4, Fa4, Mi4, Sol4, La4, Sil, Fa4, Fa4, Mi4, Sil, Fa4, Fa4, Sol4, Fa4, Sil, Fa4, Fa4, Fa4, Fa4, Fa4, Sol4, Mi4, Fa4}; //, Sil, Fa4, Fa4, Fa4, Sil, Sol4, Fa4, Sol4, Sil, Fa4, Fa4, Fa4, Fa4, Fa4, Fa4, Fa4, Mi4, Sil, Fa4, Fa4, Fa4, Fa4, Mi4, Fa4, Sil, Fa4, Sol4, Fa4, Sol4, Fa4, Sol4, Fa4, Sil, Fa4, Fa4, Fa4, Mi4, Sil, Fa4, Fa4, Fa4, Fa4, Fa4, Sol4, Fa4, Sil, Fa4, Fa4, Fa4, Fa4, Sil, Mi4, Sol4, Fa4, Sil, Fa4, Fa4, Fa4, Sol4, Fa4, Sol4,Fa4, Fa4, Fa4, Mi4, Sil, Sol4, Fa4, Sol4, Fa4, Sol4, Sil, Fa4, Fa4, Fa4, Fa4, Sol4, Fa4, Sol4, Fa4, Sil, Fa4, Fa4, Fa4, Sol4, Fa4, Sol4, Sil, Fa4, Fa4, Fa4, Fa4, Fa4, Fa4, Mi4, Sil, Fa4, Fa4, Fa4, Fa4, Mi4, Fa4, Sil, Fa4, Sol4, Fa4, Sol4, Fa4, Sol4, Fa4, Sil, Fa4, Fa4, Fa4, Mi4, Sil, Fa4, Fa4, Fa4, Fa4, Mi4, Fa4, Sil, Fa4, Fa4, Fa4, Sol4, Fa4, Mi4, Sil, Fa4, Fa4, Fa4, Mi4, Mi4, Sil, Fa4, Sol4, Fa4, Sol4, Fa4, Sol4};
unsigned int TEMPS[3] = {NE,  NE,  NE,  NE,  SC,  BL,  BL,  NE,  NE,  SC,  SC,  NE,  BL,  BL,  CO,  CO,  SC,  CO,   NE,  BL,  SC,  SC,  NE,  NE,  NE,  NE,  CO,   BL,  BL,  NE,  NE,  NE,  CO,  NE,  NE,   NE,  BL}; //, BL, SC, SC, SC, NE, NE, NE, NE, BL, NE, NE, NE,  NE, NE, CO, NE, NE, BL, NE, NE, NE, NE, NE, RO, BL, CO, CO, CO, CO, CO, CO, CO, BL, BL, BL, BL, BL, BL, NE, NE, NE, CO, CO, NE, NE, BL, CO, CO, CO, CO, CO, CO, CO, CO, BL, CO, CO, CO, CO, CO, CO, CO, CO, CO, CO, CO, NE, CO, NE, CO, NE, BL, SC, SC, SC, SC, NE, NE, NE NE, BL, NE, NE, NE, NE, CO, NE, BL, NE, NE, NE, BL, NE, NE, BL, BL, NE, NE, NE, NE, NE, RO, BL, CO, CO, CO, CO, CO, CO, CO, BL, BL, BL, BL, BL, BL, NE, NE, NE, NE, NE, RO, BL, CO, CO, SC, CO, CO, BL, BL, BL, BL, NE, NE, BL, CO, NE, CO, NE, CO, NE};

*/
void setup(){
   N.InitTaula();
   Serial.begin(9600);
 
   //Inicialitzem els modes dels PINS com a sortida
   for(idxGen=0;idxGen<7;idxGen++){
      pinMode(pLED[idxGen], OUTPUT);
   }
   pinMode(pLOCK, OUTPUT);
   pinMode(pALTAVEU, OUTPUT);
   
   
   for(idxGen=0;idxGen<7;idxGen++){
     digitalWrite(pLED[idxGen], LOW);
   }
   pinMode(pLOCK, LOW);
   pinMode(pALTAVEU, LOW);  //Assegurem que la capsa està tancada!
 
  Estat = ST_1_INICI; //Ens posem al primer estat de la màquina
  idxGen = 0;
 
}

void loop(){
SWpremuts = LlegeixPolsadors();


if (idxGen==7){
  Serial.println("Zero!");
  idxGen = 0;
}

digitalWrite(pLED[idxGen], HIGH);


/*
for (idxGen=0;idxGen<7;idxGen++){
  if (bitRead(SWpremuts,idxGen)){
    digitalWrite(pLED[idxGen], HIGH);
  } else {
    digitalWrite(pLED[idxGen], LOW);
  }
}
*/
    Serial.print(SWpremuts);
    Serial.print(" - LED: ");
    Serial.println(idxGen);
    delay(3000);

   for(idxMemoria=0;idxMemoria<7;idxMemoria++){
     digitalWrite(pLED[idxMemoria], LOW);
   }
   
   idxGen++;
}


byte LlegeixPolsadors(void){
  int idx;
  byte  polsadors = 0;
  
  for (idx=0;idx<=6;idx++){
    if (!digitalRead(pSW[idx])){
     bitSet(polsadors,idx); 
    }
  }
  return polsadors;
}
