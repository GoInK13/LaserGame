/*

Arduino → Autres
  0 → TX du RC
  2 → Interruption du RC
  3 → Laser
  6 → Moteur
  7 → LCD - D6
  8 → LCD - D7
  9 → LCD - D5
  10 → LCD - D4
  11 → Buzzer
  12 → LCD - E
  13 → LCD - RS
  A0 → Gachette
  A1 → BP Recharge
  
  Version 21 : 
    Basé sur V20_NewScreen
    Ajoue des noms des joueurs dans le SetUP via la connexion USB tty
    Nécessite le programme "SoftPC" pour fonctionner
    Type de Réception
      _$dureePartie
      $nbJoueur
      $NuméroDuJoueurActuel
      $Num:$NomDuNum
      
  Fonctionnement du tableau des noms : 
    nomJoueur[numeroJoueur][numLettre]
      → Avec nomJoueur[numeroJoueur][0]=nb de fois touché par numeroJoueur
      
  Version 22 : 
    Remplacer le temps qui s'accumule par le temps restant avant la fin de la partie
*/

#include <LiquidCrystal.h>  //Bibliothèque du LCD

//Initialisation du LCD
LiquidCrystal lcd(13, 12, 10, 9, 7, 8);  //Pin (RS,E,D4,D5,D6,D7)
//Boite de messages
char* message[]={"Demarrage....", "Restant :", "Munitions : "};
char messageClear[]="                    ";

//---------------------------------VARIABLE SoftPC----------------------------------
char numeroJoueur=0;
char nomJoueur[20][9];   //10: nb total de joueurs, 9: nb de carac par joueurs + 0=nb de touchés
char dureePartie=0;    //Durée de la partie en minutes
char nbJoueurs=0;

//--------------------------DÉFINITIONS DES VARIABLES---------------------------------
unsigned int munitions, tpsRestant;
//Variable comptant le temps entre deux events
unsigned long tpsRestantStart, tpsRecharge=0, tpsFinPartie;
unsigned char charGachette, charSwTir;
boolean retroGachette;

//Création de caractères spéciaux
byte eAigu[8] = {130,132,142,145,159,144,142,128};
uint8_t smileyOui[8] = {B00000,B00000,B01010,B01010,B00000,B10001,B01110,B00000};
uint8_t smileyNon[8] = {B00000,B10001,B11011,B00000,B00000,B01110,B10001,B00000};  //{128,145,128,132,128,142,145,128};
uint8_t smileyNon2[8] = {B00000,B10001,B11011,B00000,B00000,B11111,B00000,B00000};
uint8_t smileyBof[8] = {B00000,B11011,B11011,B00000,B00000,B11111,B00000,B00000}; //{128,138,128,132,128,159,128,128};
uint8_t smileyTest[8] = {128,138,128,132,145,142,145,128};
uint8_t chevronOuvert[8] = {128,128,133,138,148,138,133,128};
uint8_t chevronFerme[8] = {128,128,148,138,133,138,148,128};
//Like1
uint8_t main1[8] = {B00000,B00000,B00000,B00000,B00001,B00010,B01100,B01000};
uint8_t main2[8] = {B00110,B01001,B01001,B10001,B00010,B01111,B00000,B00000};
uint8_t main3[8] = {B00000,B00000,B00000,B00000,B00000,B00000,B11000,B00100};
uint8_t main4[8] = {B01000,B01000,B01000,B01000,B01110,B00001,B00000,B00000};
uint8_t main5[8] = {B00000,B00000,B00000,B00000,B00000,B10000,B01111,B00000};
uint8_t main6[8] = {B11000,B00100,B11000,B00100,B11000,B01000,B10000,B00000};
//Like2
uint8_t main12[8] = {B00000,B00000,B00000,B00001,B00010,B01100,B01000,B01000};
uint8_t main22[8] = {B00110,B01001,B10001,B00010,B00111,B00000,B00000,B00000};
uint8_t main32[8] = {B00000,B00000,B00000,B00000,B00000,B11000,B00100,B11000};
uint8_t main42[8] = {B01000,B01000,B01000,B01110,B00001,B00000,B00000,B00000};
uint8_t main52[8] = {B00000,B00000,B00000,B00000,B10000,B01111,B00000,B00000};
uint8_t main62[8] = {B00100,B11000,B00100,B11000,B01000,B10000,B00000,B00000};
//Droite
uint8_t droite1[8] = {B00000,B00000,B00000,B00011,B11100,B10000,B10000,B10000};
uint8_t droite2[8] = {B00000,B00000,B00000,B11000,B00111,B00000,B00111,B01100};
uint8_t droite3[8] = {B00000,B00000,B00000,B00000,B11000,B00100,B11000,B00000};
uint8_t droite4[8] = {B10000,B10000,B10000,B10000,B11000,B00110,B00001,B00000};
uint8_t droite5[8] = {B01100,B00010,B01100,B00010,B01100,B00100,B11000,B00000};
//Droite2
uint8_t droite12[8] = {B00000,B00000,B00000,B00001,B01110,B01000,B01000,B00000};
uint8_t droite22[8] = {B00000,B00000,B00000,B11100,B00011,B00000,B00011,B00110};
uint8_t droite32[8] = {B00000,B00000,B00000,B00000,B11100,B00010,B11100,B00000};
uint8_t droite42[8] = {B00000,B01000,B01000,B01000,B01100,B00011,B00000,B00000};
uint8_t droite52[8] = {B00110,B00001,B00110,B00001,B00110,B00010,B11100,B00000};
//_InitLCD

boolean interOn=0;    //Actif si interruption sur pin 2

//--------------------------DÉFINITIONS DES SORTIES/ENTRÉES---------------------------------
//définition des sorties
const char pinLaser = 3;
const char pinSon = 11;
const char pinMoteur = 6;
//Définition des entrées
const char analogGachette = 0;
const char analogRecharge1 = 1;
const char analogRecharge2 = 2;
const char analogSwTir = 4;

//--------------------------DÉFINITIONS DES TEMPS D'EMISSION--------------------------------
// Init durée des composantes du signal à emmettre
const int tech = 100;
const int dureeStart1 = 5*tech;
const int dureeStart23 = 4*tech;
const int dureeCode = 3*tech;

//-----------------------------DÉFINITIONS DES DURÉES------------------------------------
//Pierrot ne relit jamais son code
const int muniMax = 1000;  //Nombre de signaux possible à emmettre sans recharger
const char tpsMort = 5; //secondes d'attente quand touché


boolean recharger=0; //Utiliser pour faire clignoter quand plus de munitions
int note = 5000; //Variable utilisé pour le son du laser


//-----------------------------------SET UP----------------------------------------------
void setup()
{
    //tableau en 0 = Dieu
    nomJoueur[0][0]=4;
    nomJoueur[0][1]='D';
    nomJoueur[0][2]='i';
    nomJoueur[0][3]='e';
    nomJoueur[0][4]='u';
    //sorties
    pinMode(pinLaser, OUTPUT);
    pinMode(pinSon, OUTPUT);
    pinMode(pinMoteur, OUTPUT);
  
    //initialisé éteint
    digitalWrite(pinLaser, LOW);
    
    //Création des caractères spéciaux
    lcd.createChar(0, eAigu);    //lcd.write(byte(0))=é
    lcd.createChar(1, smileyOui);
    lcd.createChar(2, smileyBof);
    lcd.createChar(3, smileyNon);
    lcd.createChar(4, smileyTest);
    lcd.createChar(5, chevronOuvert);
    lcd.createChar(6, chevronFerme);
    lcd.begin(16, 2);    //Écran 16co×2li
    
    //Port USB pour RX le numéro reçu par l'autre arduino
    Serial.begin(115200);  //115200bauds
    
    pinMode(2, INPUT);  //Interruption reçu par l'ard Recepteur
    attachInterrupt(0,ProgInter,RISING);  //0=Inter sur Pin2, 1=Pin2=3
    
    //---------------------------Affichage version----------------------------------
    lcd.setCursor(0,0);
    lcd.print("Version 22");
    lcd.setCursor(0,1);
    lcd.print("NomDansSetUP+TPSDec");
    delay(2500);
    //-------------------------------Check Batterie----------------------------------
    float tensionBat = analogRead(5)*10.0/1023;
    Serial.print("Tension de batterie : ");
    Serial.println(tensionBat);
    lcd.setCursor(0,0);
    if(tensionBat>7.4){
      lcd.print("Tension batterie");  //Vivant : 
      lcd.setCursor(0,1);
      lcd.print(tensionBat);
      lcd.print("V ");
      lcd.print("-> OK!!");
    }
    else{
      lcd.print("*****Danger*****");
      lcd.setCursor(0,1);
      lcd.print("Tension = ");
      lcd.print(tensionBat);
      lcd.print(" V");
    }
    while(Serial.available() > 0) Serial.read();  //Vide le serial
    delay(2000);
    //----------------------------END Check Batterie---------------------------------
    
  for(int c=0;c<18;c++)  //Affichage de la séquence
  {
    SonnerLaser();
    delay(10);
  }
  
  lcd.setCursor(0,0);
  lcd.print(messageClear);
  lcd.setCursor(0,1);
  lcd.print(messageClear);
  lcd.setCursor(4,0);
  lcd.print("Attente");  //<<
  lcd.setCursor(3,1);
  lcd.print("de SoftPC");
  
  
  //----------------------------RÉCUPÉRATION DES NOMS VIA SoftPC----------------------------
  char lastByte;  //Byte de réception USB
  while (Serial.available() <= 0);  //Attente de reception
    lastByte = Serial.read();  //Stock le prem octet recu de la serie
    if(lastByte=='_'){  //Début du signal provenant du soft
      //Réception durée de la partie
      lastByte = SerialRC();
      do{
        dureePartie = 10*dureePartie + lastByte - 48;  //int(48)=char(0)
        lastByte = SerialRC();
      }while(lastByte!='\n');
      Serial.print("Duree partie : ");  //Arguments : DEC, HEX, OCT, BIN, BYTE
      //Par defaut = chaine ASCII
      Serial.println(dureePartie,DEC);
      //Réception du nombre total de joueurs
      lastByte = SerialRC();
      do{
        nbJoueurs = 10*nbJoueurs + lastByte - 48;  //int(48)=char(0)
        lastByte = SerialRC();
      }while(lastByte!='\n');
      Serial.print("Nombre de joueurs :");
      Serial.println(nbJoueurs,DEC);
      //Réception numéro du joueur actuel
      lastByte = SerialRC();
      do{
        numeroJoueur = 10*numeroJoueur + lastByte - 48;  //int(48)=char(0)
        lastByte = SerialRC();
      }while(lastByte!='\n');
      Serial.print("Votre numero :");
      Serial.println(numeroJoueur,DEC);
      //Réception des numéros des joueurs suivie de leur noms
      char numeroActuel;
          
      for(int c=0; c<nbJoueurs;c++){
          lastByte = SerialRC();
          numeroActuel=0;
          do{
            numeroActuel = 10*numeroActuel + lastByte - 48;  //int(48)=char(0)
            lastByte = SerialRC();
          }while(lastByte!=':'); 
          
          //RC noms
          char iTabNom=1;
          nomJoueur[numeroActuel][0]=0;  //en 0 : on stock le nombre de lettre du nom
          lastByte = SerialRC();
          do{
            nomJoueur[numeroActuel][iTabNom] = lastByte;
            lastByte = SerialRC();
            iTabNom++;
            nomJoueur[numeroActuel][0]++;
          }while(lastByte!='\n');
          
      }
      //Ok 
      for(int c=0;c<=nbJoueurs;c++){
        Serial.print("Joueur ");
        Serial.print(c);
        Serial.print(" s'appelle ");
        for(char cpp=1;cpp<=nomJoueur[c][0];cpp++){
          Serial.print(nomJoueur[c][cpp]);
        }
      //  nomJoueurGlobal[c]=nomJoueur[c];
        Serial.println();
      }
    }

  //--------------------------FIN RÉCUPÉRATION DES NOMS VIA SoftPC--------------------------*/
  
  lcd.setCursor(0,0);
  lcd.print(messageClear);
  lcd.setCursor(0,1);
  lcd.print(messageClear);
  lcd.setCursor(0,0);
  lcd.print("Nom : ");
  lcd.print(nomJoueur[numeroJoueur]+1);
  lcd.setCursor(0,1);
  lcd.print("Tirez pour start");
  
  //Reset du nombre de lettre car plus utilisé, on y stock maintenant le nombre de touchés
  for(int c=0;c<=nbJoueurs;c++){
    nomJoueur[c][0]=0;
  }
  //Attente du tir du joueur pour lancer la partie
  while(analogRead(analogGachette)<512) delay(50);
  tpsRestantStart=millis();
  tpsFinPartie=tpsRestantStart+dureePartie*60000;  //DureePartie(minutes)→ milliSec
  Initialiser(dureePartie*60);
  Serial.println("Start...");  
}

//-----------------------------------------LOOP--------------------------------------
void loop()
{
  //-------------------------------Stock les analogs dans variable-------------------
  charGachette = analogRead(analogGachette) >> 2; // = analogGachette/4 car 1023/4<256 (char)
  charSwTir = analogRead(analogSwTir) >> 2;
  
  if(charGachette<128) retroGachette=0;
  
    //-----------------------------Tir continue-------------------------------------
    if(charSwTir<128){
      while(charGachette>128 && interOn==0 && millis()-tpsRestantStart<1000 && munitions>0){      //Ouais parce que j'ai remis les trucs que t'as enlevé!
        emission_generale(dureeCode);
        //Serial.println("Tir continue");
        digitalWrite(pinMoteur, HIGH);
        munitions--;
        if(munitions%2==0){
          SonnerLaser();
        }
        if(munitions%10==0){
          AfficherMunitions();
        }
        charGachette = analogRead(analogGachette) >> 2;
      }
    }
    //-------------------------------Tir simple--------------------------------------
    else if(charGachette>128 && retroGachette==0 && munitions>0){
      retroGachette=1;
      for(int cTirSimple=0; cTirSimple<100; cTirSimple++){
        emission_generale(dureeCode);
        if(cTirSimple%4==0 && cTirSimple<75) 
        SonnerLaser();//75=18(nbElemBuzzer)*4(%4)+delta(3)  nb lancement de SonnerLaser pour faire un son entier
        analogWrite(pinMoteur, 200); //204=80%
      }
      munitions=int((munitions-1)/10)*10;  //Décrémente de 1 les munitions sur l'affichage
      AfficherMunitions();
    }
    digitalWrite(pinMoteur, LOW);
    
    while(note!=5000 && munitions > 0 && charGachette < 128){  //Fin du son meme sans tirer
      SonnerLaser();
      delay(10);
    }
    
    //Son click si plus de minution
    if(charGachette>128 && munitions == 0){
      for(unsigned char c=0; c<3; c++){
        tone(pinSon, 50);
        delay(100);
        tone(pinSon, 3000,30);
        delay(30);
      }
    }
    //_Son click
    
    //actualiser temps vivant
    if(millis()-tpsRestantStart>1000) {
        tpsRestantStart=millis();
        AfficherTpsRestant();
        tpsRestant--;
    }
    
    //affichage rechargement
    if(munitions==0 && millis()-tpsRecharge>1000){
      tpsRecharge=millis();
      AfficherRecharger();  //Affiche "<<Rechargez!>>" ou "Munitions : 0 :-(" 
    }
    //Si chargeur descendu du pistolet
    if(analogRead(analogRecharge1)<512){
      Recharger();    
    }
    
    // Si ArduinoRC a detecté une touche (=Interruption et buffer RX plein)
    if(interOn==1){
        Reception();  //Lis le RX  
    }
}

//-------------------------------------Émission du code---------------------------------------
//FUNCTION emission_generale
void emission_generale(unsigned int temporisation)
{
    
    //motif de départ 1 0 1
    AllumeMicros(dureeStart1, true);
    AllumeMicros(dureeStart23, false);
    AllumeMicros(dureeStart23, true);
    
    //message sur 6 bits
    for(int compteur = 0; compteur < 6; compteur++)
    {
        AllumeMicros(temporisation, (numeroJoueur >> compteur)%2);
    }
    
    //$$ChangementV13
    digitalWrite(pinLaser, LOW);
}

//FUNCTION ALLUME_MICROS : avec delayMicroseconds
void AllumeMicros ( unsigned int temporisation, boolean emissionTrue)
{ 
  digitalWrite(pinLaser, emissionTrue);
  delayMicroseconds(temporisation);   
} 

//-----------------------------------Réception du numéro reçu----------------------------------
// Lis le buffer de RX USB, pour stocker un char dans valRecu + affiche nom du joueur
void Reception(){
  char valRecu;
      Serial.println("Recu");
  if(Serial.available() > 0){  //Si buffer non vide
    valRecu=Serial.read();  //Lecture
    Serial.println(valRecu,DEC);
    if(valRecu-33<=nbJoueurs){
      nomJoueur[valRecu-33][0]++;
      Serial.println("Killed");
      //------------Afficher "Tué par XXX :-(" ↓ "Réarmement Xsec"
      lcd.setCursor(0,0);
      lcd.print(messageClear);
      lcd.setCursor(0,0);
      lcd.print("Tu");
      lcd.write(byte(0));  //é
      lcd.print(" par");
      lcd.print(nomJoueur[valRecu-33]);
      lcd.print(" ");
      lcd.write(byte(3));  //smiley pas content
      lcd.setCursor(7,0);  //Éfface le nombre de mort du joueur
      lcd.print(" ");
      lcd.setCursor(0,1);
      lcd.print("R");
      lcd.write(byte(0));  //é
      lcd.print("armement ");
      
      lcd.print(tpsMort,DEC);
      lcd.print("sec    ");
      
      //-------------------Son touché--------------------
      int sonMort[]=  {262, 32, 277, 32, 294, 16, 0, 4, 0, 16, 247, 8, 349, 8, 0, 8, 349, 8, 349, 6, 
      330, 6, 294, 6, 262, 8, 165, 8, 0, 8, 165, 8, 131, 8};
      int tpsAttMort=0;
      tpsRecharge=millis();
      for(int c=0;c<34;c+=2){
        tone(pinSon, sonMort[c], 1000/sonMort[c + 1]);
        delay((1000/sonMort[c + 1]) * 1.20);
        if(millis()-tpsRecharge>1000){  //Affichage de l'attente du réarmement
          tpsAttMort++;
          tpsRecharge=millis();
          lcd.setCursor(11,1); 
          lcd.print(tpsMort-tpsAttMort,DEC);
          lcd.print("sec    ");
        }
      }
      noTone(pinSon);
      //+++++++++++++++++++Fin Son touché++++++++++++++++++
      delay(694);  //694=1000(us)-304 → tps écoulé depuis la dernière MAJ du LCD pendant que le son était joué
      for(int c=tpsMort-tpsAttMort-1; c>0; c--){
          lcd.setCursor(11,1); 
          lcd.print(c,DEC);
          lcd.print("sec    ");
          delay(1000);
      }
      
      Initialiser((tpsFinPartie-millis())*0.001);
    }
  }
  interOn=0;  //Réactivation de la variable d'interruption
  attachInterrupt(0,ProgInter,RISING);
}

//Fonction activée en cas de front montant sur la pin2
void ProgInter(){
  detachInterrupt(0);
  interOn=1;
}

//----------------------------------------ÉCRAN LCD--------------------------------------------

// Initialise l'affichage en mettant le temps vivant reçu et le nb de muni=100
void Initialiser(unsigned int tpsVivantInit){
  lcd.setCursor(0,0);
  lcd.print(messageClear);
  lcd.setCursor(0,1);
  lcd.print(messageClear);
  lcd.setCursor(0,0);
  lcd.print(message[1]);  //Restant : 
  tpsRestant=tpsVivantInit;
  AfficherTpsRestant();
  lcd.setCursor(0,1);
  lcd.print(message[2]);  //Munitions : 
  munitions=muniMax;
  lcd.print("100");
}

// Affiche le temps encore en vie : à ligne 0, col 9 : XmXX :-|
void AfficherTpsRestant(){
  if(tpsRestant<=0) FinPartie();
  lcd.setCursor(10,0);
  lcd.print(int(tpsRestant/60),DEC);
  lcd.print("m");
  if(tpsRestant%60<10) lcd.print("0");
  lcd.print(tpsRestant%60,DEC);
  lcd.print(" ");
  lcd.setCursor(15,0);
  if(tpsRestant<120){
    lcd.write(byte(3));  //Pas content
  }
  else if (tpsRestant<300){  //Bof
    lcd.write(byte(2));
  }
  else{  //Content
    lcd.write(byte(1));
  }
}

//Affiche le nombre de munitions restantes : en ligne 1, colonne 12
void AfficherMunitions(){
  lcd.setCursor(12,1);
  lcd.print(munitions/10);
  lcd.print(" ");
  if(munitions>700){
    lcd.write(byte(1));
  }
  else if (munitions>400){
    lcd.write(byte(2));
  }
  else if (munitions>399){
    lcd.write(byte(3));
  }
}

//Fait clignoter le message "<<Rechargez!>>" puis "Munitions : 0 :-(" sur la deuxième ligne
void AfficherRecharger(){
  lcd.setCursor(0,1);
  if(recharger==0){
    lcd.write(byte(5));  //<<
    lcd.print("Rechargez!");
    lcd.write(byte(6));  //>>
    lcd.print("   ");
    lcd.createChar(3, smileyNon2);
    lcd.write(byte(3));
  }
  else{
    lcd.print(message[2]);  //Munitions : 
    lcd.print("0  ");
    lcd.createChar(3, smileyNon);
    lcd.write(byte(3));
  }
  recharger=!recharger;
  
}

//Fait charger une barre de progression
void Recharger(){
  boolean pouce=1;
  unsigned long tpsPouce=millis();
  lcd.createChar(1, main1);
  lcd.createChar(2, main2);
  lcd.createChar(3, main3);
  lcd.createChar(4, main4);
  lcd.createChar(5, main5);
  lcd.createChar(6, main6);
  while(analogRead(analogRecharge1)<512){  //Attente front descendant
    lcd.setCursor(0,0);
    lcd.write(byte(1));
    lcd.write(byte(2));
    lcd.write(byte(3));
    lcd.print(" Chargeur ");
    lcd.write(byte(1));
    lcd.write(byte(2));
    lcd.write(byte(3));
    lcd.setCursor(0,1);
    lcd.write(byte(4));
    lcd.write(byte(5));
    lcd.write(byte(6));
    lcd.print(" d");
    lcd.write(byte(0));  //é
    lcd.print("falqu");
    lcd.write(byte(0));  //é
    lcd.print(" ");
    lcd.write(byte(4));
    lcd.write(byte(5));
    lcd.write(byte(6));
    if(millis()-tpsPouce>500) {  //Changement de l'affichage des pouces
        if(pouce){
          lcd.createChar(1, main12);
          lcd.createChar(2, main22);
          lcd.createChar(3, main32);
          lcd.createChar(4, main42);
          lcd.createChar(5, main52);
          lcd.createChar(6, main62);
        }
        else{
          lcd.createChar(1, main1);
          lcd.createChar(2, main2);
          lcd.createChar(3, main3);
          lcd.createChar(4, main4);
          lcd.createChar(5, main5);
          lcd.createChar(6, main6);
        }
        tpsPouce=millis();
        pouce=!pouce;
    }
  }
  lcd.setCursor(0,0);
  lcd.print(messageClear);
  lcd.setCursor(0,1);
  lcd.print(messageClear);
  lcd.setCursor(5,0);
  lcd.print("Tirer");
  lcd.setCursor(4,1);
  lcd.print("le canon");

  lcd.createChar(1, droite1);
  lcd.createChar(2, droite2);
  lcd.createChar(3, droite3);
  lcd.createChar(4, droite4);
  lcd.createChar(5, droite5);
  while(analogRead(analogRecharge2)<512){
    lcd.setCursor(0,0);
    lcd.write(byte(1));
    lcd.write(byte(2));
    lcd.write(byte(3));
    lcd.setCursor(13,0);
    lcd.write(byte(1));
    lcd.write(byte(2));
    lcd.write(byte(3));
    lcd.setCursor(0,1);
    lcd.write(byte(4));
    lcd.write(byte(5));
    lcd.setCursor(13,1);
    lcd.write(byte(4));
    lcd.write(byte(5));
    if(millis()-tpsPouce>500) {  //Changement de l'affichage de la main droite
        if(pouce){
          lcd.createChar(1, droite12);
          lcd.createChar(2, droite22);
          lcd.createChar(3, droite32);
          lcd.createChar(4, droite42);
          lcd.createChar(5, droite52);
        }
        else{
          lcd.createChar(1, droite1);
          lcd.createChar(2, droite2);
          lcd.createChar(3, droite3);
          lcd.createChar(4, droite4);
          lcd.createChar(5, droite5);
        }
        tpsPouce=millis();
        pouce=!pouce;
    }
  }
  tone(pinSon, 3500, 50);
  delay(100);
  tone(pinSon, 3500, 50);
  lcd.createChar(1, smileyOui);
  lcd.createChar(2, smileyBof);
  lcd.createChar(3, smileyNon);
  lcd.createChar(4, smileyTest);
  lcd.createChar(5, chevronOuvert);
  lcd.createChar(6, chevronFerme);
  munitions=muniMax;
  note=5000;
  Initialiser((tpsFinPartie-millis())*0.001);
}

//--------------------------------------Son du laser-----------------------------------
void SonnerLaser(){
  tone(pinSon, note, 20);
  if(note>1500){
    note-=200;
  }
  else{
    note = 5000;
  }
}

//-------------------------------------RÉCEPTION VIA USB----------------------------------
char SerialRC(){
  while(Serial.available() <= 0);
  return Serial.read();
}

//-----------------------------------FIN DE LA PARTIE-------------------------------------
void FinPartie(){
  detachInterrupt(0);
  lcd.setCursor(0,0);
  lcd.print(messageClear);
  lcd.setCursor(0,1);
  lcd.print(messageClear);
  lcd.setCursor(0,0);
  lcd.print("Partie Termin");
  lcd.write(byte(0));
  lcd.print("e");
  lcd.setCursor(3,1);
  lcd.print("Rentrez! ");
  lcd.write(byte(1));
  
  //-------------------Son Fin de Partie = Vader--------------------
      int sonFin[]=  {
  392, 4, 392, 4, 392, 4, 311, 6, 466, 16, 392, 4, 311, 6, 466, 16, 392, 2, 587, 4, 587, 4,  //11 son, 22 valeurs
  587, 4, 622, 6, 466, 16, 370, 4, 311, 6, 466, 16, 392, 2, 784, 4, 392, 6, 392, 16, 784, 4,
  740, 6, 698, 16, 659, 16, 622, 16, 659, 8, 0, 8, 415, 8, 554, 4, 526, 6, 494, 16, 466, 16,
  440, 16, 466, 8, 0, 8, 311, 8, 370, 4, 311, 6, 370, 16, 466, 4, 392, 6, 466, 16, 587, 2,
  784, 4, 392, 6, 392, 16, 784, 4, 740, 6, 698, 16, 659, 16, 622, 16, 659, 8, 0, 8, 415, 8,
  554, 4, 523, 6, 494, 16, 466, 16, 440, 16, 466, 8, 0, 8, 311, 8, 370, 4, 311, 6, 466, 16,
  392, 4, 311, 6, 466, 16, 392, 2
  };
  for(int c=0;c<140;c+=2){
    tone(pinSon, sonFin[c], 1450/sonFin[c + 1]);
    delay((1450/sonFin[c + 1]) * 1.60);
  }
  noTone(pinSon);
  //Rajoute les tirs qu'on s'est pris
  lcd.setCursor(0,0);
  lcd.print(messageClear);
  lcd.setCursor(0,0);
  lcd.print("Statistique :");
  while(1){
    for(int c=1; c<=nbJoueurs; c++){
      lcd.setCursor(0,1);
      lcd.print(messageClear);
      lcd.setCursor(0,1);
      lcd.print(nomJoueur[c]+1);  //Affiche le nom du joueur uniquement
      lcd.print(" : ");
      lcd.print(nomJoueur[c][0],DEC);
      delay(1000);
    }
  }
  
}

