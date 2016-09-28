#include "fenetre.h"

int nbJoueurs=0;


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);


QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));


//Charge feuille.css pour avoir les lignes et centré
    app.setStyleSheet(
//	"QWidget { background-color: rgb(230, 255, 230); }"
	"QGroupBox  {"
	"	border: 1px solid gray;"
	"	border-radius: 5px;"
	"	margin-top: 1ex; /* leave space at the top for the title */"
//	"	background-color: rgb(182, 255, 230);"
	"}"
	"QGroupBox::title  {"
	"	subcontrol-origin: margin;"
	"	subcontrol-position: top center;"
	"	top: -5px;"
	"	padding: 0 3px;"
	"}"
);



    QString locale = QLocale::system().name().section('_', 0, 0);
    QTranslator translator;
    translator.load(QString("qt_") + locale, QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    app.installTranslator(&translator);

	std::cout << "Version 2.2" << std::endl;
	std::cout << "Amélioration interface" << std::endl;
	std::cout << "Nombre de joueurs : ";

    MaFenetre fenetre;

    fenetre.show();

    return app.exec();
}

