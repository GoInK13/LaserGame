#include "fenetre.h"

//Variables globales
int oldNbJoueurs = nbJoueurs;
bool newWidget = 0;

MaFenetre::MaFenetre() : QWidget()
{
	setWindowTitle(tr("Laser Game"));

	setMinimumSize(300,53);
//Bouton Quitter
    QObject::connect(this, SIGNAL(quitter()), qApp, SLOT(quit()));    //Quitte le programme apres la question QMessage
    boutQuit = new QPushButton("Quitter", this);
    boutQuit->setGeometry(1,1,110,25);
    QObject::connect(boutQuit, SIGNAL(clicked()), this, SLOT(ouvrirQuit()));    //Lance ouvrirQuit

//Nombre de joueur
    boutNbJoueurs = new QPushButton("Nombre de joueurs",this);
    boutNbJoueurs->setGeometry(1,60,110,25);
    QObject::connect(boutNbJoueurs,SIGNAL(clicked()),this,SLOT(ouvrirNbJoueurs()));

//Layout de page de départ contenant que deux boutons
	layoutConfig = new QHBoxLayout;
	layoutConfig->addWidget(boutNbJoueurs);
	layoutConfig->addWidget(boutQuit);
	this->setLayout(layoutConfig); 

}

void MaFenetre::ouvrirQuit()
{
    int reponse = QMessageBox::question(this, "Quitter la fenetre?", "<strong>Voulez vous vraiment quitter?</strong>", QMessageBox::No | QMessageBox::Yes);
    if (reponse == QMessageBox::Yes)
      {
        emit quitter();
      }
}

void MaFenetre::ouvrirNbJoueurs()
{
    bool ok;    //Vérifie la validation
	oldNbJoueurs = nbJoueurs;
    nbJoueurs = QInputDialog::getInteger(this, "Nombre de joueurs", "Entrez le nombre de joueurs présent dans la partie :",nbJoueurs,0,120,1, &ok);

//-----------------------------------------------------------

//
//
//
/*Inutile
delete signalMapper;
delete qTabBout;
delete tabNom;
delete tabLabelJoueurs;
*/

delete layoutConfig;
if(newWidget==1){
	delete layoutTotal;
//	delete layoutJoueursTotal;
	delete layoutJoueursEq1;
	delete layoutJoueursEq2;
	delete lblTemps;
	delete lineTps;
	delete boutVerifNom;
	for(int c=1; c<=oldNbJoueurs; c++){
		delete qTabBout[c];
		delete tabNom[c];
		delete tabLabelJoueurs[c];
	}
	delete gpEquipe1;
	delete gpEquipe2;
}
newWidget=1;
delete boutQuit;
delete boutNbJoueurs;

//----------------------------------INITIALISATION---------------------------
//Taille de la fenêtre
	setMinimumSize(600,82+30*int((nbJoueurs+1)/2));
	resize(650,82+39*int((nbJoueurs+1)/2));
//OK : 	setMinimumSize(600,82+39*int((nbJoueurs+1)/2));

//Bouton Quitter
    QObject::connect(this, SIGNAL(quitter()), qApp, SLOT(quit()));    //Quitte le programme apres la question QMessage
    boutQuit = new QPushButton("Quitter", this);
    boutQuit->setGeometry(1,1,110,25);
    QObject::connect(boutQuit, SIGNAL(clicked()), this, SLOT(ouvrirQuit()));    //Lance ouvrirQuit

//Nombre de joueur
    boutNbJoueurs = new QPushButton("Nombre de joueurs",this);
    boutNbJoueurs->setGeometry(1,60,110,25);
    QObject::connect(boutNbJoueurs,SIGNAL(clicked()),this,SLOT(ouvrirNbJoueurs()));

//Durée de la partie
	lblTemps = new QLabel("Durée partie (minutes) :",this);
	lineTps = new QLineEdit;
//Bouton vérification du nom des joueurs
	boutVerifNom = new QPushButton("Charger?",this);
	QObject::connect(boutVerifNom, SIGNAL(clicked()), this, SLOT(checkNom())); 
//Layout de la première ligne
	layoutConfig = new QHBoxLayout;
	layoutConfig->addWidget(boutQuit);
	layoutConfig->addWidget(boutNbJoueurs);
	layoutConfig->addWidget(lblTemps);
	layoutConfig->addWidget(lineTps);
	layoutConfig->addWidget(boutVerifNom);

//Add charger Joueur X + LineEdit + boutton
	for (int c=1; c<=nbJoueurs; c++){
		QString str, str2;
		str.setNum(c);
		str2="Charger Joueur "+str;
		qTabBout[c] = new QPushButton(str2,this);
		qTabBout[c]->setEnabled(false);
		tabNom[c] = new QLineEdit;
		str2="Joueur "+str;
		tabLabelJoueurs[c] = new QLabel(str2,this);
	}
	signalMapper = new QSignalMapper(this);
//Connexion des boutons avec la map de la fonction boutTab()
	for (int c=1; c<=nbJoueurs; c++){
		QObject::connect(qTabBout[c], SIGNAL(clicked()), signalMapper, SLOT(map())); 
    	signalMapper->setMapping(qTabBout[c], c);  
	}

//GroupBox équipe1
	gpEquipe1 = new QGroupBox(tr("Equipe 1"));
	layoutJoueursEq1 = new QGridLayout;
	for (int c=1; c<=(nbJoueurs+1)/2; c++){
		layoutJoueursEq1->addWidget(tabLabelJoueurs[c], c, 0);
		layoutJoueursEq1->addWidget(tabNom[c], c, 1);
		layoutJoueursEq1->addWidget(qTabBout[c], c, 2);
	}
	gpEquipe1->setLayout(layoutJoueursEq1);

	gpEquipe2 = new QGroupBox(tr("Equipe 2"));
	layoutJoueursEq2 = new QGridLayout;
	for (int c=(nbJoueurs+1)/2+1; c<=nbJoueurs; c++){
		layoutJoueursEq2->addWidget(tabLabelJoueurs[c], c, 0);
		layoutJoueursEq2->addWidget(tabNom[c], c, 1);
		layoutJoueursEq2->addWidget(qTabBout[c], c, 2);
	}
	gpEquipe2->setLayout(layoutJoueursEq2);


	layoutJoueursTotal = new QHBoxLayout;
	layoutJoueursTotal->addWidget(gpEquipe1);
	layoutJoueursTotal->addWidget(gpEquipe2);
	
/* MODIF gridLayout
    layoutJoueurs = new QGridLayout;
	for (int c=0; c<nbJoueurs; c++){
			if(c>=nbJoueurs*0.5){
			layoutJoueurs->addWidget(tabLabelJoueurs[c], c-nbJoueurs*0.5, 3);
			layoutJoueurs->addWidget(tabNom[c], c-nbJoueurs*0.5, 4);
			layoutJoueurs->addWidget(qTabBout[c], c-nbJoueurs*0.5, 5);
		}
		else{
			layoutJoueurs->addWidget(tabLabelJoueurs[c], c, 0);
			layoutJoueurs->addWidget(tabNom[c], c, 1);
			layoutJoueurs->addWidget(qTabBout[c], c, 2);
		}
	}
*/

	 QObject::connect(signalMapper, SIGNAL(mapped(int)), this, SLOT(boutTab(int))); //le int correspond au numéro du joueur de 0 à x

	layoutTotal = new QVBoxLayout;
	layoutTotal->addLayout(layoutConfig);
	layoutTotal->addLayout(layoutJoueursTotal);
//	layoutTotal->addLayout(layoutJoueurs);

    this->setLayout(layoutTotal);       //Place les Layouts sur toute les fenetres MaFenetre
//-------------------------------------------------



	std::cout << "END";

}

void MaFenetre::boutTab(int num){

//Savoir si un Arduino est connecté	
	system("./VerifUSB.sh");	//fichier bash permettant de savoir si USB co
	QFile fichier("./USB.temp");
	fichier.open(QIODevice::ReadOnly);
	QString nomUSB = fichier.readAll();
	nomUSB = nomUSB.mid(0, nomUSB.length() - 1);

		std::cout << std::endl << "nomUSB : " << nomUSB.toStdString().c_str();
	
	system("rm ./USB.temp");
//Si pas d'arduino USBTemp=noUSB, si non /dev/ttyUSBx
	if(nomUSB[0]=='/'){
		QString strNum, strNbJoueurs, strAffichage, strCmdNumNom, strNumActif, strTps;
		//Set ligne USB à 115200 bit/s
		strCmdNumNom = "stty -F ";
		strCmdNumNom = strCmdNumNom + nomUSB;
		strCmdNumNom = strCmdNumNom + " cs8 115200 ignbrk -brkint -icrnl -imaxbel -opost -onlcr -isig -icanon -iexten -echo -echoe -echok -echoctl -echoke noflsh -ixon -crtscts";
		std::cout << std::endl << "Command Ligne : ";
		std::cout << strCmdNumNom.toStdString().c_str() << std::endl;
		system(strCmdNumNom.toStdString().c_str());
		strNum.setNum(num);
		strAffichage="Joueur "+strNum+" sur ";
		strNbJoueurs.setNum(nbJoueurs);
		strAffichage=strAffichage+strNbJoueurs+" nommé \"";
		strAffichage=strAffichage+tabNom[num]->text();
		strAffichage=strAffichage+"\" chargé.";
			//Émission de la durée de la partie en minutes
		strTps = "echo \"_"+lineTps->text()+"\" > "+nomUSB;
		system(strTps.toStdString().c_str());
		std::cout << strTps.toStdString().c_str() << std::endl;

			//Émission du nombre total de joueurs
		strNbJoueurs = "echo \""+strNbJoueurs+"\" > "+nomUSB;
		system(strNbJoueurs.toStdString().c_str());

			//Émission de : "_$NuméroJoueurActif"
		strNumActif = "echo \""+strNum+"\" > "+nomUSB;
		system(strNumActif.toStdString().c_str());

			//Émission de : "$numéro:$NomJoueur"
		for(int c=1; c<=nbJoueurs; c++){
			strNum.setNum(c);
			strCmdNumNom = "echo \""+strNum+":"+tabNom[c]->text()+"\" > "+nomUSB;
			system(strCmdNumNom.toStdString().c_str());
		}

		QMessageBox::information(this, "Téléversement", strAffichage);
	}
	else{
		QMessageBox::information(this, "Erreur", "Aucun pistolet détecté");
	}
}

void MaFenetre::checkNom(){
	bool validationNom=1;
	for (int c=1; c<=nbJoueurs; c++){
		if(tabNom[c]->text()=="") validationNom=0;
	}
	if(lineTps->text()=="") validationNom=0;
	if(validationNom==1){
		for (int c=1; c<=nbJoueurs; c++)	{
			qTabBout[c]->setEnabled(true);
		}
	}
}

