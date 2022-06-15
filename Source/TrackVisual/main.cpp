#include "widget.h"

#include <QApplication>

#include <QDebug>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    Widget w;

    QFile file("trackvisual-inputs.txt");
    file.open(QIODevice::WriteOnly);
    QTextStream ss(&file);
    for(int i = 0; i < argc; ++i) ss << argv[i] << endl;

    qDebug() << argc;
    if(argc == 3) {
        if(!strlen(argv[1]) || !strlen(argv[2])) return 1;
        qDebug() << QString(argv[0]) << endl << QString(argv[1]) << endl << QString(argv[2]) << endl;
        w.readTracks(argv[1], argv[2]);
    }

    w.show();
    return a.exec();
}
