#ifndef PROJECT_H
#define PROJECT_H

#include <QSqlDatabase>
#include <QSqlDriver>
#include <QSqlError>
#include <QXmlStreamReader>
#include <QFile>
#include <QDir>

#include "qdebug.h"

class Project
{
public:
    static QList<QString> getLanguages(QString appDir);
    static QList<QString> getToolkits(QString appDir, QString l);
    static QList<QStringList> getFiles(QString appDir, QString l, QString tk);
    static QList<QStringList> getHelpFiles(QString appDir, QString l, QString tk);
    static QXmlStreamReader xml;
    static QFile XMLFile;

};

#endif // PROJECT_H
