#ifndef DEF_MAFENETRE
#define DEF_MAFENETRE

//Pour l'interface
#include <QTranslator>
#include <QLocale>
#include <QLibraryInfo>
#include <QSignalMapper>
#include <iostream>

#include <QTextCodec>

#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <QMessageBox>
#include <QInputDialog>
#include <QStringList>
#include <QColorDialog>
#include <QFileDialog>
#include <QGridLayout>
#include <QLineEdit>
#include <QFormLayout>
#include <QLabel>
#include <QLCDNumber>
#include <QCheckBox>
#include <QGroupBox>

#include <QTextEdit>   // Affiche le contenu du fichier
#include <QFile>       // Pour utiliser le fichier
#include <QString>     // Stocke le contenu du fichier
#include <QTextStream> // Flux sortant du fichier
#include <stdio.h>

extern int nbJoueurs;

class MaFenetre : public QWidget
{
    Q_OBJECT

    public:
    MaFenetre();

    public slots:
    void ouvrirNbJoueurs();
	void ouvrirQuit();
	void boutTab(int num);
	void checkNom();

    signals:
    void quitter();


    private:
    //bouton de chiffres
    QPushButton *boutQuit, *boutNbJoueurs, *boutVerifNom;
	
	QPushButton *qTabBout[32];

    QVBoxLayout *layoutTotal;
	QHBoxLayout *layoutConfig, *layoutJoueursTotal;
    QGridLayout *layoutJoueursEq1, *layoutJoueursEq2;

	QLabel *tabLabelJoueurs[32];
//Remplace lblEquipe
	QGroupBox *gpEquipe1, *gpEquipe2;

	QLabel *lblTemps;
	QLineEdit *lineTps;
	QLineEdit *tabNom[32];
	

	QSignalMapper *signalMapper;

};

#endif
