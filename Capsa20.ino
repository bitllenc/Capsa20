#include <Capsa20.h>

#define RAMON_HUB		170284
#define ALBA			160184
#define CODI			12345
#define COSA			54321


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

#define TEMPS_OBERTURA  10000 //ms
#define COMBINACIO_SECRETA  19 //0001_0011
#define MAX_CANCONS 4  //Nombre de cançons que tenim
#define MAX_NOTES 55   //Nombre màxim de notes que pot tenir una cançó
#define MAX_SINTONIES 4  //Nombre de cançons que tenim
#define MAX_NOTES_SINTONIA 30   //Nombre màxim de notes que pot tenir una cançó

#define N_SINTONIA_INICI 0
#define N_SINTONIA_OK 1 
#define N_SINTONIA_NO 2 
#define N_SINTONIA_FI 3


#define NO_ERROR 0
#define SI_ERROR 1

#define LOCK_OBERT LOW
#define LOCK_TANCAT HIGH

#define pLOCK  2
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

byte SWpremuts, SWpremuts_ant;
byte pSW[7] = {pSW_Do,   pSW_Re,  pSW_Mi,  pSW_Fa,  pSW_Sol,  pSW_La,  pSW_Si};
byte pLED[7] = {pLED_Do, pLED_Re, pLED_Mi, pLED_Fa, pLED_Sol, pLED_La, pLED_Si};

int idxGen = 0;        //Índex multiús
int idxCanco = 0;      //Nombre de cançons endevinades
int idxNota = 0;       //Nombre de notes màxim que ha assolit en una cançó
int idxMemoria = 0;    //Notes que ha avançat en el joc (idxMemoria <= idxNota)
char errorMemoria = NO_ERROR; //Indica si ha comès algun error en les notes premudes
int Estat, EstatSeguent;
boolean doItOnce = LOW;

Nota N;

/*

int numNotes[MAX_CANCONS] = {4,4};//,4};//,4,4};
int NOTES[MAX_CANCONS][MAX_NOTES] = {
        {Do4, Re4, Mi4, Fa4},
        {Sol4, La4,Si4, Do5},
        {Re5, Mi5, Fa5, Sol5},
        {La5, Si5, Do6, Si5},
        {La5, Sol5, Fa5, Mi5}
        };                
unsigned int TEMPS[MAX_CANCONS][MAX_NOTES] = {
        {NE, NE, NE, NE},
        {NE, NE, NE, NE},
        {NE, NE, NE, NE},
        {NE, NE, NE, NE},
        {NE, NE, NE, NE}
        }; 
*/

int numNotes[MAX_CANCONS] = {18,51,39,32};//,32,9};
int NOTES[MAX_CANCONS][MAX_NOTES] = {
        {Re4, Sol4, Sol4, Sol4, Re4, La4, FaS4, Sol4, Re4, Sol4, Do5, Do5, Si4, La4, Sol4, FaS4, Sol4, La4},
        {Do4, Fa4, Sol4, La4, Sol4, Mi4, Do4, Fa4, Sol4, La4, Sol4, Do4, Fa4, Sol4, La4, Sol4, Mi4, Mi4, Fa4, Mi4, Do4, Do4, Do5, Si4, La4, Sol4, Si4, Sol4, La4, Fa4, Sol4, Do5, Si4, La4, Sol4, Si4, Sol4, Do5, Si4, La4, Sol4, Si4, Sol4, La4, Fa4, Sol4, Mi4, Fa4, Mi4, Do4, Do4},
        {La4, Re4, Fa4, Sol4, La4, Re4, Fa4, Sol4, La4, Re4, Fa4, Sol4, La4, Re4, Fa4, Sol4, La4, Re4, Fa4, Sol4, La4, Re4, Fa4, Sol4, Sol4, Do4, Fa4, Mi4, Sol4, Do4, La4, Re4, Mi4, Fa4, Sol4, Do4, Fa4, Mi4, Re4},
        {La4, La4, La4, Si4, La4, La4, Sol4, La4, La4, La4, La4, Si4, La4, La4, Si4, Si4, Si4, La4, Sol4, La4, Sol4, La4, Sol4, Si4, La4, La4, La4, Si4, Si4, La4, La4, Sol4}
      };/*,
        {La4, La4, La4, La4, La4, Si4, La4, La4, Fa4}
        };
*/
                
unsigned int TEMPS[MAX_CANCONS][MAX_NOTES] = {
      {NE,  COp,  SC,   BL,   NE,  COp, SC,   BL,   NE,  COp,  SC,  NE,  COp, SC,   NE,   COp,  SC,   BL},
      {CO, CO, CO, CO, NE, BL, CO, CO, CO, CO, BL, CO, CO, CO, CO, NE, BL, CO, CO, CO, CO, BL, CO, CO, CO, CO, NE, CO, NE, CO, NE, CO, CO, CO, CO, NE, NE, CO, CO, CO, CO, NE, CO, NE, CO, NE, CO, CO, CO, CO, BL},
      {CO, CO, SC, SC, CO, CO, SC, SC, CO, CO, SC, SC, CO, CO, SC, SC, BL, BL, SC, SC, NE, NE, SC, SC, BL, BL, SC, SC, NE, BL, BL, BL, CO, CO, NE, BL, CO, CO, BL},
      {NE, CO, NE, NE, CO, NE, NE, NE, CO, NE, SC, CO, CO, NE, CO, CO, SC, NE, SC, CO, SC, CO, SC, NE, CO, CO, CO, CO, CO, CO, CO, BL}
    };/*,
      {CO, CO, CO, CO, CO, RO, CO, NE, BL}
      };
      
      */
int sint_numNotes[MAX_SINTONIES] = {8,6,5,28};
int sint_NOTES[MAX_SINTONIES][MAX_NOTES_SINTONIA] = {
        {Sol5, FaS5, Do5, Mi5, Re5, Do5, La4, Sol4}, //Marxa nupcial Inici
        {Sol4, Do5, Mi5, Sol5, Mi5, Sol5}, //NFL Cançó encertada
        {Do5, Si4, SiB4, La4, SolS4},  //Error!
        {Mi4, Fa4, Sol4, Sol4, Fa4, Mi4, Re4, Do4, Do4, Re4, Mi4, Mi4, Re4, Re4, Mi4, Fa4, Sol4, Sol4, Fa4, Mi4, Re4, Do4, Do4, Re4, Mi4, Re4, Do4, Do4}, //Himne Alegria, FI!
        };
unsigned int sint_TEMPS[MAX_SINTONIES][MAX_NOTES_SINTONIA] = {
      {NE, COp, SC, CO, CO, CO, CO, BL}, 
      {SC, SC, SC, CO, SC, NE},
      {CO, CO, CO, CO, BL},
      {NE, CO, CO, CO, CO, CO, CO, CO, CO, CO, CO, COp, SC, NE, NE, CO, CO, CO, CO, CO, CO, CO, CO, CO, CO, COp, NE, BL},
      };


void setup(){
   N.InitTaula();
   //***Serial.begin(9600);
 
   //Inicialitzem els modes dels PINS com a sortida
   //***Serial.println("Configurant sortides");
   for(idxGen=0;idxGen<7;idxGen++){
      pinMode(pLED[idxGen], OUTPUT);
   }
   pinMode(pLOCK, OUTPUT);
   pinMode(pALTAVEU, OUTPUT);
   
   //Es posen totes les sortides en estat de repòs
   //***Serial.println("Inincialitzant sortides");
   for(idxGen=0;idxGen<7;idxGen++){
     digitalWrite(pLED[idxGen], LOW);
   }
   digitalWrite(pLOCK, LOCK_TANCAT);  //Assegurem que la capsa està tancada! (lògica inversa)
   digitalWrite(pALTAVEU, LOW);  
 
   
  Estat = ST_1_INICI; //Ens posem al primer estat de la màquina
  idxGen = 0;
 
 //***Serial.flush();
}

void loop(){

  
  //Gestió de l'estat següent
  if (Estat == ST_1_INICI) {

    if (SWpremuts == COMBINACIO_SECRETA){
      EstatSeguent = ST_10_DESBLOQUEIG;
    } else if (doItOnce){
      EstatSeguent = ST_2_INDICACIO_CANCO;
    }
    
  } else if (Estat == ST_2_INDICACIO_CANCO) {
      EstatSeguent = ST_3_REPRODUIR_CANCO;
      
  } else if (Estat == ST_3_REPRODUIR_CANCO) {
      EstatSeguent = ST_4_SEQUENCIA_REPRO;  //Un cop s'ha reproduit la cançó es passa a nota a nota
      
  } else if (Estat == ST_4_SEQUENCIA_REPRO) {
      EstatSeguent = ST_41_SEQUENCIA_ESCOLTA;   //Després de reproduir idxNotes notes es passa a escoltar 
      
  } else if (Estat == ST_41_SEQUENCIA_ESCOLTA) {
    
    if (errorMemoria == NO_ERROR) {  //No s'ha equivocat
      if (idxMemoria == idxNota){ //Ha arribat fins a la nota que se li demana
         EstatSeguent = ST_42_SEQUENCIA_CORRECTA;  //Es felicita que ha acabat aquesta seqüència
       }
     } else {
       EstatSeguent = ST_6_EQUIVOCACIO;    //Es va a l'estat d'equivocacio
    }
      
  } else if (Estat == ST_42_SEQUENCIA_CORRECTA){
    EstatSeguent = ST_4_SEQUENCIA_REPRO;    //Es torna a reproduir la cançó nota a nota, amb una nota més
    
    if (idxNota >= numNotes[idxCanco]) {  //S'ha arribat al màxim de notes
      EstatSeguent = ST_5_CANCO_CORRECTA;  //Es va al procediment de canvi de canço
     }
      
  } else if (Estat == ST_5_CANCO_CORRECTA) {
    EstatSeguent = ST_2_INDICACIO_CANCO;
    
    if (idxCanco >= (MAX_CANCONS-1)){          //Si s'han resolt totes les cançons
        EstatSeguent = ST_10_DESBLOQUEIG;    //S'obre la capsa
    }
        
        
  } else if (Estat == ST_6_EQUIVOCACIO) {
    EstatSeguent = ST_4_SEQUENCIA_REPRO;    //Es torna a reproduir la cançó nota a nota
        
  } else if (Estat == ST_10_DESBLOQUEIG) {
    EstatSeguent = ST_1_INICI;    //Tornem a començar
  } else {
    EstatSeguent = ST_1_INICI;
  }
  

      
  //Accions a l'estat actual
  if (Estat == ST_1_INICI) {
    //***Serial.println(Estat);
    digitalWrite(pLOCK, LOCK_TANCAT);  //Assegurem que la capsa està tancada! (lògica inversa)
        
    //Posem índexs a zero
    idxCanco = 0;
    idxNota = 0;
    idxMemoria = 0;
    errorMemoria = NO_ERROR;
    
    //Musiqueta de benvinguda
    if (!doItOnce){
      reprodueixCanco(sint_NOTES[N_SINTONIA_INICI],sint_TEMPS[N_SINTONIA_INICI],sint_numNotes[N_SINTONIA_INICI]);
    }
    //Donem un temps...
    delay(500);    
    
    //Llegim els polsadors per veure si hi ha la combinació secreta
    //Fem un beep per avisar?
    SWpremuts = LlegeixPolsadorsMulti();
    //***Serial.print("CONTRASENYA!!! -   SWpremuts: ");
    //***Serial.println(SWpremuts);
    
    //Notifiquem que s'ha fet el primer cicle... si no, no s'entra al mode contrasenya
    doItOnce = HIGH;
    
  } else if (Estat == ST_2_INDICACIO_CANCO) {
        //***Serial.println(Estat);
        digitalWrite(pLOCK, LOCK_TANCAT);  //Assegurem que la capsa està tancada! (lògica inversa)
    //Nº de LEDs encesos
    //LEDSetAcum_Pulse(idxCanco+1, 1000)
    
  } else if (Estat == ST_3_REPRODUIR_CANCO) {
    //***Serial.println(Estat);
        digitalWrite(pLOCK, LOCK_TANCAT);  //Assegurem que la capsa està tancada! (lògica inversa)
    //Reproduir cançó sencera
    reprodueixCanco(NOTES[idxCanco],TEMPS[idxCanco],numNotes[idxCanco]);
    delay(1000);

    idxNota = 0;
    idxMemoria = 0;
    errorMemoria = NO_ERROR;
    
  } else if (Estat == ST_4_SEQUENCIA_REPRO) {
        //***Serial.print(Estat);
        //***Serial.print("  --  ");
        //***Serial.println(idxNota);
            digitalWrite(pLOCK, LOCK_TANCAT);  //Assegurem que la capsa està tancada! (lògica inversa)
    //Reproduir notes fins a la nota on s'hagi avançat
    reprodueixCanco(NOTES[idxCanco],TEMPS[idxCanco],idxNota);
      
  } else if (Estat == ST_41_SEQUENCIA_ESCOLTA) {    
            digitalWrite(pLOCK, LOCK_TANCAT);  //Assegurem que la capsa està tancada! (lògica inversa)
    //Escolta les polsacions i comprova que siguin correctes
      //Actualització dels botons polsats
      SWpremuts = LlegeixPolsadors();
      //ActualitzaLEDs(SWpremuts);
      
      if (!(SWpremuts == 0 | SWpremuts == SWpremuts_ant)){  //Si no s'ha premut cap botó o és el mateix que l'anterior, no es fa res
//        Serial.println(Estat);
        //***Serial.print("SWpremuts: ");
        //***Serial.println(SWpremuts);
      
        if (SWpremuts == N.led(NOTES[idxCanco][idxMemoria])) {    //1. Ha premut correctament (un silenci = 0, s'ha de tenir en compte!)
          
          //Reprodueix la nota a l'escala de la melodia original
          tone(pALTAVEU, N.freq(NOTES[idxCanco][idxMemoria])); //Es reprodueix el to, T[temps[idxN]]);
          delay(200);// Es posa un temps fix - TEMPS[idxCanco][idxMemoria]);  //Es deixa passar el temps
          noTone(pALTAVEU);  //S'apaga el so
        
          idxMemoria++;  //S'incrementa el comptador de les notes memoritzades
          errorMemoria = NO_ERROR;
          
          /*Serial.println("41 -> a)Botó correcte!");
          Serial.print("idxCanco: ");
          Serial.println(idxCanco);
          Serial.print("idxNota: ");
          Serial.println(idxNota);
          Serial.print("idxMemoria: ");
          Serial.println(idxMemoria);
          Serial.print("NOTA: ");
          Serial.println(NOTES[idxCanco][idxMemoria]);
          Serial.print("TEMPS:");
          Serial.println(TEMPS[idxCanco][idxMemoria]);
          Serial.print("Frequencia:");
          Serial.println(N.freq(NOTES[idxCanco][idxMemoria]));
        */
        } else {
          //Avisa de l'error
          errorMemoria = SI_ERROR;
          //***Serial.println("41 -> c)Botó equivocat");
         
        }
      } else {//DEBUG: borrar, només hi és pel PRINT!
         //Serial.println("41 -> b)Botó no premut");
      }
      
      //Es guarda el valor de l'últim botó polsat
      SWpremuts_ant = SWpremuts;
      
  }else if (Estat == ST_42_SEQUENCIA_CORRECTA){
        //***Serial.println(Estat);
        digitalWrite(pLOCK, LOCK_TANCAT);  //Assegurem que la capsa està tancada! (lògica inversa)
    delay(200); //Espera un segon abans no continua amb la següent seqüència
    
    idxNota++;    //Augmenta en una nota
    idxMemoria = 0;  //Reinicia la memòria
    errorMemoria = NO_ERROR;
    
  } else if (Estat == ST_5_CANCO_CORRECTA) {
        //***Serial.println(Estat);
            digitalWrite(pLOCK, LOCK_TANCAT);  //Assegurem que la capsa està tancada! (lògica inversa)
    //Si completa la cançó. Musiqueta de correcte!
    reprodueixCanco(sint_NOTES[N_SINTONIA_OK],sint_TEMPS[N_SINTONIA_OK],sint_numNotes[N_SINTONIA_OK]);    //reprodueixSintonia(5);
    reprodueixCanco(sint_NOTES[N_SINTONIA_OK],sint_TEMPS[N_SINTONIA_OK],sint_numNotes[N_SINTONIA_OK]);    //reprodueixSintonia(5);
    delay(1000);
    
    idxNota = 0;//Reinicia la nota
    idxMemoria = 0;  //Reinicia la memòria
    errorMemoria = NO_ERROR;
    idxCanco++;    //Es va a la següent cançó!
    
  } else if (Estat == ST_6_EQUIVOCACIO) {
        //***Serial.println(Estat);
        digitalWrite(pLOCK, LOCK_TANCAT);  //Assegurem que la capsa està tancada! (lògica inversa)
    //Si s'equivoca. Musiqueta d'incorrecte!
    //Reprodueix un to d'error SiB1
          tone(pALTAVEU, N.freq(SiB1)); //Es reprodueix el to, T[temps[idxN]]);
          delay(500);  //Es deixa passar el temps
          noTone(pALTAVEU);  //S'apaga el so
          reprodueixCanco(sint_NOTES[N_SINTONIA_NO],sint_TEMPS[N_SINTONIA_NO],sint_numNotes[N_SINTONIA_NO]);
          
          
    idxNota = 0;  //Reinicia la canco
    idxMemoria = 0;  //Reinicia la memòria
    errorMemoria = NO_ERROR;
    
  } else if (Estat == ST_10_DESBLOQUEIG) {
    //***Serial.println(Estat);
    //***Serial.println("HO HAS ACONSEGUIT!!!!");
    
    //En completar totes les cançons desbloqueig i musiqueta
    digitalWrite(pLOCK, LOCK_OBERT);  //Es debloqueja (lògica inversa)
    delay(1000);
    //Cançoneta de final de la tortura i obertura de la porta
    reprodueixCanco(sint_NOTES[N_SINTONIA_FI],sint_TEMPS[N_SINTONIA_FI],sint_numNotes[N_SINTONIA_FI]);
    
    LEDSetAll(HIGH); //Encen tots els llums
    delay(500);  //Temps per obrir
    LEDSetAll(LOW); //Apaga tots els llums
    delay(500);  //Temps per obrir
    LEDSetAll(HIGH); //Encen tots els llums
    delay(500);  //Temps per obrir
    LEDSetAll(LOW); //Apaga tots els llums
    delay(500);  //Temps per obrir    
     LEDSetAll(HIGH); //Encen tots els llums
    delay(500);  //Temps per obrir
    LEDSetAll(LOW); //Apaga tots els llums
    delay(500);  //Temps per obrir   
     LEDSetAll(HIGH); //Encen tots els llums
    delay(500);  //Temps per obrir
    LEDSetAll(LOW); //Apaga tots els llums
    delay(500);  //Temps per obrir   
     LEDSetAll(HIGH); //Encen tots els llums
    delay(500);  //Temps per obrir
    LEDSetAll(LOW); //Apaga tots els llums
    delay(500);  //Temps per obrir
    
    //***Serial.println("TANCANT LA CAIXA!!!");
    digitalWrite(pLOCK, LOCK_TANCAT);  //Es torna a bloquejar (lògica inversa)
    
    doItOnce = LOW;

  } else {
        //***Serial.println(Estat);
    //Wow! Com has arribat fins aquí? No facis res!
  }
  
        //Actualitza l'estat de les entrades
      Estat = EstatSeguent;
      delay(10);
      //***Serial.flush();
      
}


void reprodueixCanco(int * canco, unsigned int * temps, int num_notes){
  unsigned int idxN;
  
  for (idxN=0; idxN<num_notes; idxN++){
    /*Serial.print("Nota: ");
    Serial.print(idxN);
    Serial.print("  -  Freq: ");
    Serial.print(N.freq(canco[idxN]));
    Serial.print("  -  Durada: ");
    Serial.println(temps[idxN]);
    */
    tone(pALTAVEU, N.freq(canco[idxN])); //Es reprodueix el to, T[temps[idxN]]);
    digitalWrite(pLED[N.led(canco[idxN])-1], HIGH);  //S'il.lumina el LED
    delay(temps[idxN]);  //Es deixa passar el temps
    noTone(pALTAVEU);  //S'apaga el so
    digitalWrite(pLED[N.led(canco[idxN])-1], LOW);  //S'apaga el LED
    delay(10);  //Temps de separacio entre notes
  }
}



byte LlegeixPolsadors(void){
  int idx;
  byte  polsadors = 0;
  
  for (idx=0;idx<=6;idx++){
    digitalWrite(pLED[idx], LOW); //**Apaga els LEDs no premuts
           
    if (!digitalRead(pSW[idx])){
      digitalWrite(pLED[idx], HIGH); //**Encén els LEDs premut
      //bitSet(polsadors,idx); 
      //** Retorna el número més baix del polsador premut
     return idx+1;
    }
  }
  return polsadors;
}


byte LlegeixPolsadorsMulti(void){
  int idx;
  byte  polsadors = 0;
  
  for (idx=0;idx<=6;idx++){
    if (!digitalRead(pSW[idx])){
      bitSet(polsadors,idx); 
    }
  }
  return polsadors;
}
    
    
void LEDSetON(byte numLED){
  digitalWrite(numLED, HIGH);
}

void LEDSetOFF(byte numLED){
  digitalWrite(numLED, LOW);
}

void LEDSetAll(boolean valor){
  int idx;
  for (idx=0;idx<=6;idx++){
    digitalWrite(pLED[idx],valor);
  }
}
    
/*
LEDSetIdx_Pulse(nLED, 1000)
LEDSetAcum_Pulse(nLEDs, 1000)
LEDSetIdx_Ct(nLED)
LEDSetAcum_Ct(nLED)
LEDSetIdx_Blink(nLED, tHigh, tLow, tBlink)
LEDSetAcum_Blink(nLED, tHigh, tLow, tBlink)

LEDClearIdx(nLED)
LEDClearAcum(nLED)
*/

