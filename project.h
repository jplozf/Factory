#ifndef PROJECT_H
#define PROJECT_H

#include <QSqlDatabase>
#include <QSqlDriver>
#include <QSqlError>
#include <QXmlStreamReader>
#include <QDomDocument>
#include <QFile>
#include <QDir>
#include <QDateTime>
#include <QString>
#include <QStringList>
#include <QRandomGenerator>
#include <QDebug>

#include "app.h"
#include "languages.h"

class Project
{
public:
    static QList<QString> getLanguages(QString appDir);
    static QList<QString> getToolkits(QString appDir, QString l);
    static QList<QStringList> getFiles(QString appDir, QString l, QString tk);
    static QList<QStringList> getHelpFiles(QString appDir, QString l, QString tk = "Raw");
    static QString randomName();
    static QXmlStreamReader xml;
    static QFile FactoryFile;

    Project(App *app, QString projectDir);
    QString projectDir;
    QString sessionStart;
    QString sessionEnd;
    QString projectName;
    QString projectFile;
    QString projectLanguage;
    void startSession();
    void endSession();
    void close();
    QList<QStringList> getHelpFiles();
    QList<QStringList> getHelpFilesFromFRX();
    void setHelpFiles(QList<QStringList> helpFiles);
    QString fetchProjectFile();
    void createProjectFile();
    void replaceVarsInFile(QString filename, QString projectName);
    QString setVars(QString text, QString projectName);
    QString fetchProgrammingLanguage();
    QMap<QString, QString> getProperties();
    QString getCreationDate();
    QString getModificationDate();
    qint64 getElapsedTime();

private:
    App *app;
};

#endif // PROJECT_H
