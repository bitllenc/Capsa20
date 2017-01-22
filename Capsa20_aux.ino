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
#define COMBINACIO_SECRETA 7 //85 //0101_0101
#define MAX_CANCONS 5  //Nombre de cançons que tenim
#define MAX_NOTES 55   //Nombre màxim de notes que pot tenir una cançó

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

byte SWpremuts, SWpremuts_ant;
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
int NOTES[MAX_CANCONS][MAX_NOTES] = {
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
   Serial.println("Configurant sortides");
   for(idxGen=0;idxGen<7;idxGen++){
      pinMode(pLED[idxGen], OUTPUT);
   }
   pinMode(pLOCK, OUTPUT);
   pinMode(pALTAVEU, OUTPUT);
   
   //Es posen totes les sortides en estat de repòs
   Serial.println("Inincialitzant sortides");
   for(idxGen=0;idxGen<7;idxGen++){
     digitalWrite(pLED[idxGen], LOW);
   }
   pinMode(pLOCK, LOW);
   pinMode(pALTAVEU, LOW);  //Assegurem que la capsa està tancada!
 
   
  Estat = ST_1_INICI; //Ens posem al primer estat de la màquina
  idxGen = 0;
 
 Serial.flush();
}

void loop(){

  
  //Gestió de l'estat següent
  if (Estat == ST_1_INICI) {
    if (SWpremuts == COMBINACIO_SECRETA){
      EstatSeguent = ST_10_DESBLOQUEIG;
    } else {
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
      } else if (idxNota >= numNotes[idxCanco]) {  //S'ha arribat al màxim de notes
          EstatSeguent = ST_5_CANCO_CORRECTA;  //Es va al procediment de canvi de canço
      }
    } else {
       EstatSeguent = ST_6_EQUIVOCACIO;    //Es va a l'estat d'equivocacio
    }
      
  } else if (Estat == ST_42_SEQUENCIA_CORRECTA){
    EstatSeguent = ST_4_SEQUENCIA_REPRO;    //Es torna a reproduir la cançó nota a nota, amb una nota més
      
  } else if (Estat == ST_5_CANCO_CORRECTA) {
    if (idxCanco > MAX_CANCONS){          //Si s'han resolt totes les cançons
        EstatSeguent = ST_10_DESBLOQUEIG;    //S'obre la capsa
    }
        EstatSeguent = ST_2_INDICACIO_CANCO;
        
  } else if (Estat == ST_6_EQUIVOCACIO) {
    EstatSeguent = ST_4_SEQUENCIA_REPRO;    //Es torna a reproduir la cançó nota a nota
        
  } else if (Estat == ST_10_DESBLOQUEIG) {
    EstatSeguent = ST_1_INICI;    //Tornem a començar
  } else {
    EstatSeguent = ST_1_INICI;
  }
  

      
  //Accions a l'estat actual
  if (Estat == ST_1_INICI) {
    Serial.println(Estat);
        
    //Posem índexs a zero
    idxCanco = 0;
    idxNota = 0;
    idxMemoria = 0;
    errorMemoria = NO_ERROR;
    
    //Musiqueta de benvinguda
    //reprodueixSintonia(1);

    //Donem un temps...
    delay(500);    
    
    Serial.println("1 - post Delay");
    //Llegim els polsadors per veure si hi ha la combinació secreta
    //Fem un beep per avisar?
    SWpremuts = LlegeixPolsadors();
    Serial.print("SWpremuts: ");
    Serial.println(SWpremuts);
    Serial.println("1 - post LlegeixPolsadors()");
    
  } else if (Estat == ST_2_INDICACIO_CANCO) {
        Serial.println(Estat);
    //Nº de LEDs encesos
    //LEDSetAcum_Pulse(idxCanco+1, 1000)
    
  } else if (Estat == ST_3_REPRODUIR_CANCO) {
    Serial.println(Estat);
    //Reproduir cançó sencera
    reprodueixCanco(NOTES[idxCanco],TEMPS[idxCanco],numNotes[idxCanco]);
    delay(2000);

    idxNota = 0;
    idxMemoria = 0;
    errorMemoria = NO_ERROR;
    
  } else if (Estat == ST_4_SEQUENCIA_REPRO) {
        Serial.print(Estat);
        Serial.print("  --  ");
        Serial.println(idxNota);
        
    //Reproduir notes fins a la nota on s'hagi avançat
    reprodueixCanco(NOTES[idxCanco],TEMPS[idxCanco],idxNota);
      
  } else if (Estat == ST_41_SEQUENCIA_ESCOLTA) {    
        
    //Escolta les polsacions i comprova que siguin correctes
      //Actualització dels botons polsats
      SWpremuts = LlegeixPolsadors();
      //ActualitzaLEDs(SWpremuts);
      
      if (!(SWpremuts == 0 | SWpremuts == SWpremuts_ant)){  //Si no s'ha premut cap botó o és el mateix que l'anterior, no es fa res
//        Serial.println(Estat);
        Serial.print("SWpremuts: ");
        Serial.println(SWpremuts);
      
        if (SWpremuts == N.led(NOTES[idxCanco][idxMemoria])) {    //1. Ha premut correctament (un silenci = 0, s'ha de tenir en compte!)
          
          Serial.println("41 -> a)Botó correcte!");
          Serial.print("idxCanco: ");
          Serial.println(idxCanco);
          Serial.print("idxMemoria: ");
          Serial.println(idxMemoria);
          Serial.print("NOTA: ");
          Serial.println(NOTES[idxCanco][idxMemoria]);
          Serial.print("TEMPS:");
          Serial.println(TEMPS[idxCanco][idxMemoria]);
          Serial.print("Frequencia:");
          Serial.println(N.freq(NOTES[idxCanco][idxMemoria]));
          
          //Reprodueix la nota a l'escala de la melodia original
          tone(pALTAVEU, N.freq(NOTES[idxCanco][idxMemoria])); //Es reprodueix el to, T[temps[idxN]]);
          delay(200);// Es posa un temps fix - TEMPS[idxCanco][idxMemoria]);  //Es deixa passar el temps
          noTone(pALTAVEU);  //S'apaga el so
        
          idxMemoria++;  //S'incrementa el comptador de les notes memoritzades
          errorMemoria = NO_ERROR;
        
        } else {
          //Avisa de l'error
          errorMemoria = SI_ERROR;
          Serial.println("41 -> c)Botó equivocat");
          
          //Reprodueix un to d'error SiB1
          tone(pALTAVEU, N.freq(SiB1)); //Es reprodueix el to, T[temps[idxN]]);
          delay(500);  //Es deixa passar el temps
          noTone(pALTAVEU);  //S'apaga el so
        }
      } else {//DEBUG: borrar, només hi és pel PRINT!
         //Serial.println("41 -> b)Botó no premut");
      }
      
      //Es guarda el valor de l'últim botó polsat
      SWpremuts_ant = SWpremuts;
      
  }else if (Estat == ST_42_SEQUENCIA_CORRECTA){
        Serial.println(Estat);
    //reprodueixSintonia(5);
    
    idxNota++;    //Augmenta en una nota
    idxMemoria = 0;  //Reinicia la memòria
    errorMemoria = NO_ERROR;
    
  } else if (Estat == ST_5_CANCO_CORRECTA) {
        Serial.println(Estat);
    //Si completa la cançó. Musiqueta de correcte!
    //reprodueixSintonia(2);
    
    idxNota = 0;//Reinicia la nota
    idxMemoria = 0;  //Reinicia la memòria
    errorMemoria = NO_ERROR;
    idxCanco++;    //Es va a la següent cançó!
    
  } else if (Estat == ST_6_EQUIVOCACIO) {
        Serial.println(Estat);
    
    //Si s'equivoca. Musiqueta d'incorrecte!
    //reprodueixSintonia(3);
    idxNota = 0;  //Reinicia la canco
    idxMemoria = 0;  //Reinicia la memòria
    errorMemoria = NO_ERROR;
    
  } else if (Estat == ST_10_DESBLOQUEIG) {
    Serial.println(Estat);
    //En completar totes les cançons desbloqueig i musiqueta
    digitalWrite(pLOCK, HIGH);  //Es debloqueja
    //reprodueixSintonia(4);
    digitalWrite(pLOCK, LOW);  //Es torna a bloquejar
  } else {
        Serial.println(Estat);
    //Wow! Com has arribat fins aquí? No facis res!
  }
  
        //Actualitza l'estat de les entrades
      Estat = EstatSeguent;
      delay(10);
      Serial.flush();
      
}


void reprodueixCanco(int * canco, unsigned int * temps, int num_notes){
  unsigned int idxN;
  
  for (idxN=0; idxN<num_notes; idxN++){
    Serial.print("Nota: ");
    Serial.print(idxN);
    Serial.print("  -  Freq: ");
    Serial.print(N.freq(canco[idxN]));
    Serial.print("  -  Durada: ");
    Serial.println(temps[idxN]);
    
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

    
    /*
void reprodueixSintonia(byte idxSintonia){
   switch (idxSintonia){
     case 1:  
       Serial.println("PLAYING: Sintonia 1");
       tone(pALTAVEU, 440);
       delay(1000);
       noTone(pALTAVEU);
       break;
     case 2:
       Serial.println("PLAYING: Sintonia 2");
       tone(pALTAVEU, 880);
       delay(1000);
       noTone(pALTAVEU);
       break;
     case 3:
       Serial.println("PLAYING: Sintonia 3");
       tone(pALTAVEU, 300);
       delay(1000);
       noTone(pALTAVEU);
       break;
     default: 
       Serial.println("PLAYING: Sintonia ELSE");
       
   }
};
*/
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

