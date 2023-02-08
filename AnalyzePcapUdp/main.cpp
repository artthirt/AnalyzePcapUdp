#include <QApplication>
#include <QTranslator>

#include "mainwindow.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

    QTranslator translator_ru;
    translator_ru.load(":/translate_ru.qm");

    auto model = loadJsonFromFile(MainWindow::settingsFile());
    auto lang = model["language"].toString();
    if(lang == "ru"){
        a.installTranslator(&translator_ru);
    }

    MainWindow w;

	w.show();

	return a.exec();
}
