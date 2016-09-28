/*
  Version 51
  A0 → BPW34
  Digital2 → digital2 du Émetteur
  TX → RX du Émetteur

*/
//augmenter la fréquence d'échantillonnage
//$$ en virer 3 sur 4
const unsigned char PS_16 = (1 << ADPS2);
const unsigned char PS_32 = (1 << ADPS2) | (1 << ADPS0);
const unsigned char PS_64 = (1 << ADPS2) | (1 << ADPS1);
const unsigned char PS_128 = (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);

//valeur échantillonnée
int Tab_RC[2];
boolean valBool; //bit trouvé
int deriv, moy;
const char rapportMoyDeriv=6, moyMin=1; //paramètres de comparaison

//Numéro attaquant
char bitRCChar[3];
char attkInter =0;

//compteur
char comptCode,iStart,iBitRCChar;
boolean iTab_RC;

//temps d'échantillonnage (µs)
int Tech=85;
//temps pour l'échantillonnage
long tps1, tps2;
long tpsDebutStart, tpsDebutCode;
const int dureeStartMax = 8*Tech+300;
const int dureeCodeMax = 18*Tech+600+200;

char nbValId; //nombre de bit successif à avoir
const char nbValCode=3, nbValStart=4, nbValBit1=5;
char intermed; //compteur comptant le nombre de réaparission d'un bit
const char tpsMaxi=30;  //Nombre maximal d'échantillon identique successif (1-0-1:111111)

//$1
int ires;
char result[500][3];

//--------------------------------------------SET UP--------------------------------------------------------
void setup() 
{
  pinMode(2, OUTPUT);
  digitalWrite(2,LOW);
  //$$ à virer
  delay(15000);
  Serial.begin(115200);  //Config de l'usb a 115 200 bauds
 // Serial.flush();
 // Serial.println("Starting");
    
  // set up the ADC
  ADCSRA &= ~PS_128;  // remove bits set by Arduino library
  ADCSRA |= PS_16;
  
  Tab_RC[0]=0;
  Tab_RC[1]=0;  
  
  nbValId=5; //nombre de bit à 1 à trouver au début
  comptCode=0;
  iStart=0;
  intermed=0;
  iTab_RC=0;
  
  //attaquant
  iBitRCChar=0;  
  bitRCChar[0]=1;
  bitRCChar[1]=1;
  bitRCChar[2]=1;
  
  //$1
  ires=0;
  /*
  for (int ligne=0; ligne<20; ligne++)
  {
    for(int colonne=0; colonne<3; colonne++)
    {
      result[ligne][colonne]=0;
    }
  }
*/

}

//-----------------------------------------------LOOP-------------------------------------------------------
void loop(){
  
  tps1=micros();
  
  //iTab_RC ++
  iTab_RC = !iTab_RC;
  Tab_RC[iTab_RC]=analogRead(0);
  
  //$$
  moy=max(Tab_RC[1],Tab_RC[0]);
  //essayer en appliquant a+b/2 - min(a,b)
  
  moy/=rapportMoyDeriv;
  //deriv = Tab_RC[nouvelle valeur] - Tab_RC[ancienne valeur]
  deriv=Tab_RC[iTab_RC]-Tab_RC[!iTab_RC];
  
  Echantillonnage(deriv,moy);

  if(intermed>tpsMaxi)  //Reset si trop longtemps sans activité
  {
    intermed=0;
    valBool=0;
  }
  
  Identification();
   
  tps2=micros();
  
  while(tps2-tps1<Tech)  //Attente prochain cycles d'échantillonnage
  {
     tps2=micros();
  }
}


//---------------------------------------------------PROCEDURES---------------------------------------------
//  ECHANTILLONNAGE : si on observe une forte variation, on stocke le bit trouvé valBool à 0 ou 1.
// Sinon le bit trouvé est identique au précédent, on incrémente intermed.
inline void Echantillonnage(int derivee, int moyenne){
  if(derivee>moyenne && moyenne>moyMin){
    valBool=1;
    intermed=1;
    /* TODO:
    une source de problème est : si on échantillonne au milieu de la montée, 
    on détecte un 1, mais on continue à monter, donc on remet intermed à 1
    il faudrait peut-être ajouter une condition pour vérifier que intermed n'est pas déjà à 1 
    (pas sûr que ça ne marche pas moins bien)
    */
  }
  else if(derivee<-moyenne && moyenne>moyMin)
  {
    valBool=0;
    intermed=1;
  }
  else
  {
    intermed++;
  }  
}

//IDENTIFICATION
inline void Identification(){
    if(intermed%nbValId==0){
      
        switch(nbValId){          
          case nbValBit1: //1er bit
            if(valBool==1){
                iStart=1;
                nbValId=nbValStart;
                
                tpsDebutStart=micros(); //on enregistre le début du Start
            }
            break;
            
          case nbValStart: //bit 2 et 3
            IdentBit2et3();
            break;
          
          case nbValCode: //bit de Code
            IdentBitCode();
            break;
        }
    }
}

//  IDENTIFICATION BIT 2 ET 3
inline void IdentBit2et3(){
  
    if(iStart==1 && valBool==0){ //2nd bit d'identification = 0
      iStart=2;
    }
    else if(iStart==2 && valBool==1){ //Troisième bit d'identification = 1
      iStart=0;
      nbValId=nbValCode;
      intermed=0;
      comptCode=0;
      
      //$$1
      //test duree Start
      tpsDebutCode=micros();
      //Serial.print("S:");
      //Serial.println(tpsDebutCode - tpsDebutStart);
      if(tpsDebutCode-tpsDebutStart > dureeStartMax){
        //repartir tout de suite à zéro, au lieu de chercher à décoder le code
        iStart=0;
        nbValId=nbValBit1;
        intermed=0;
      }
    }
    else{    //Si problème = 101 pas successif : Raz des données
      // RESET start[]
      iStart=0;
      nbValId=nbValBit1;
      intermed=0;
    }
}

// IDENT BIT CODE
inline void IdentBitCode(){
   
    //on enregistre les bits, au fur et à mesure qu'on les reçoit
    attkInter = attkInter + (valBool<<comptCode);
    
    comptCode=(comptCode+1)%6;
    
    //Le CODE est RECU
    if(comptCode==0){
      
        //$$1
        //test duree Code
        tpsDebutStart=micros();
        //Serial.print("C:");
        //Serial.println(tpsDebutStart-tpsDebutCode);
        
        //Compare durée pour éliminer les codes trop long (en temps)
        if(tpsDebutStart-tpsDebutCode < dureeCodeMax){
            bitRCChar[iBitRCChar] = attkInter;
            ComparaisonDonnees();
        }
        
        //on réinitialise
        nbValId=nbValStart;
        intermed=0;
        attkInter=0;
    }
}

// COMPARAISON DONNEES
inline void ComparaisonDonnees(){
    iBitRCChar = (iBitRCChar+1)%3;    
       
    if(iBitRCChar==0){ //on a fait un tour
        
        //test si 2 codes sont identiques
        char ligneOk=-1;
        for(char c=0;c<3;c++){
            if (bitRCChar[c] == bitRCChar[(c+1)%3]){
                ligneOk=c;
            }
        }
        /*
        //TEST AFFICHAGE AU FUR ET A MESURE
        Serial.println("-");
        Serial.println(bitRCChar[0], DEC);
        Serial.println(bitRCChar[1], DEC);
        Serial.println(bitRCChar[2], DEC);
        //*/
        
        /*
        //TEST AFFICHAGE PAR BLOC
        int nbli = 500;
        //$1 test
        ires = ires +1;
        if(ires< nbli){
          //on enregistre
            for(int ind =0; ind<3; ind++){
              result[ires][ind]= bitRCChar[ind];
            }
        }
        else{
           // Serial.println("debut");
            for(int art =0; art<nbli; art++){
                  for(int ind =0; ind<3; ind++){
                   // Serial.println(result[art][ind], DEC);
                  }
            }
          //  Serial.println("fin");
            ires=0;
        }
        //fin block
        */
        
        if(ligneOk !=-1){
          if(bitRCChar[ligneOk]<32){
            TransBonCode(bitRCChar[ligneOk]);
          }
        }
    }
}

//---------------------------------------------------FONCTIONS----------------------------------------------

void TransBonCode(char valDec){
  Serial.write(valDec+33);  //Envoie un octet donc un char
  //Mise en interruption de l'Arduino Tireur par front montant
  digitalWrite(2, HIGH);
  delayMicroseconds(300);
  digitalWrite(2, LOW);
  delayMicroseconds(300);  
  //Fin mise en Interruption
  delay(5000);  //Attente de 21s
}


