#include "project.h"

QXmlStreamReader Project::xml;
QFile Project::XMLFile;

//******************************************************************************
// getLanguages()
//******************************************************************************
QList<QString> Project::getLanguages(QString appDir) {
    QList<QString> languages;
    XMLFile.setFileName(appDir + QDir::separator() + "factory.xml");
    if (!XMLFile.open(QFile::ReadOnly | QFile::Text)) {
        qDebug() << "Error loading XML file.";
    }
    Project::xml.setDevice(&XMLFile);
    while(!Project::xml.atEnd()) {
       if (Project::xml.isStartElement()) {
           QString name = Project::xml.name().toString();
           if (name == "language") {
               for (int i = 0; i < Project::xml.attributes().size(); i++) {
                   QString attName = Project::xml.attributes().at(i).name().toString();
                   if (attName == "tag") {
                       QString language = Project::xml.attributes().at(i).value().toString();
                       languages.append(language);
                   }
               }
           }
       }
       Project::xml.readNext();
    }
    if (Project::xml.hasError()) {
        qDebug() << "Error loading XML : " << Project::xml.errorString();
    }
    XMLFile.close();
    return languages;
}

//******************************************************************************
// getToolkits()
//******************************************************************************
QList<QString> Project::getToolkits(QString appDir, QString l) {
    QList<QString> toolkits;
    XMLFile.setFileName(appDir + QDir::separator() + "factory.xml");
    if (!XMLFile.open(QFile::ReadOnly | QFile::Text)) {
        qDebug() << "Error loading XML file.";
    }
    bool inLanguage(false);
    bool toolkitFound(false);
    Project::xml.setDevice(&XMLFile);
    while(!xml.atEnd()) {
       if (xml.isStartElement()) {
           QString name = xml.name().toString();
           if (name == "language") {
               for (int i = 0; i < xml.attributes().size(); i++) {
                   QString attName = xml.attributes().at(i).name().toString();
                   if (attName == "tag") {
                       QString language = xml.attributes().at(i).value().toString();
                       if (l == language) {
                           inLanguage = true;
                       }                       
                   }
               }
            }
            if (name == "toolkit" && inLanguage == true) {
                for (int i = 0; i < xml.attributes().size(); i++) {
                    QString attName = xml.attributes().at(i).name().toString();
                    if (attName == "tag") {
                        QString toolkit = xml.attributes().at(i).value().toString();
                        toolkits.append(toolkit);
                        toolkitFound = true;
                    }
                }
            }
       }
       if (xml.isEndElement()) {
           QString name = xml.name().toString();
           if (name == "toolkits") {
               inLanguage = false;
           }
       }
       xml.readNext();
    }
    if (xml.hasError()) {
        qDebug() << "Error loading XML : " << xml.errorString();
    }
    XMLFile.close();
    if (toolkitFound == false) {
        toolkits.append("*NONE");
    }
    return toolkits;
}

//******************************************************************************
// getFiles()
//******************************************************************************
QList<QStringList> Project::getFiles(QString appDir, QString l, QString tk) {
    QList<QStringList> files;
    QString filename;
    QString source;
    XMLFile.setFileName(appDir + QDir::separator() + "factory.xml");
    if (!XMLFile.open(QFile::ReadOnly | QFile::Text)) {
        qDebug() << "Error loading XML file.";
    }
    bool inLanguage(false);
    bool inToolkit(false);
    Project::xml.setDevice(&XMLFile);
    while(!xml.atEnd()) {
       if (xml.isStartElement()) {
           QString name = xml.name().toString();
           if (name == "language") {
               for (int i = 0; i < xml.attributes().size(); i++) {
                   QString attName = xml.attributes().at(i).name().toString();
                   if (attName == "tag") {
                       QString language = xml.attributes().at(i).value().toString();
                       if (l == language) {
                           inLanguage = true;
                       }
                   }
               }
            }
            if (name == "toolkit" && inLanguage == true) {
                for (int i = 0; i < xml.attributes().size(); i++) {
                    QString attName = xml.attributes().at(i).name().toString();
                    if (attName == "tag") {
                        QString toolkit = xml.attributes().at(i).value().toString();
                        if (tk == toolkit) {
                            inToolkit = true;
                        }
                    }
                }
            }
            if (name == "file" && inLanguage == true && inToolkit == true) {
                for (int i = 0; i < xml.attributes().size(); i++) {
                    QString attName = xml.attributes().at(i).name().toString();
                    if (attName == "name") {
                        filename = xml.attributes().at(i).value().toString();
                    }
                    if (attName == "source") {
                        source = xml.attributes().at(i).value().toString();
                    }
                }
            }
       }
       if (xml.isEndElement()) {
           QString name = xml.name().toString();
           if (name == "toolkits") {
               inLanguage = false;
               inToolkit = false;
           }
           if (name == "file" && inLanguage == true && inToolkit == true) {
               files.append({filename, source});
           }
        }
       xml.readNext();
    }
    if (xml.hasError()) {
        qDebug() << "Error loading XML : " << xml.errorString();
    }
    XMLFile.close();
    return files;
}

//******************************************************************************
// getHelpFiles()
//******************************************************************************
QList<QStringList> Project::getHelpFiles(QString appDir, QString l, QString tk) {
    QList<QStringList> helpFiles;
    QString tag;
    QString source;
    XMLFile.setFileName(appDir + QDir::separator() + "factory.xml");
    if (!XMLFile.open(QFile::ReadOnly | QFile::Text)) {
        qDebug() << "Error loading XML file.";
    }
    bool inLanguage(false);
    bool inToolkit(false);
    Project::xml.setDevice(&XMLFile);
    while(!xml.atEnd()) {
       if (xml.isStartElement()) {
           QString name = xml.name().toString();
           if (name == "language") {
               for (int i = 0; i < xml.attributes().size(); i++) {
                   QString attName = xml.attributes().at(i).name().toString();
                   if (attName == "tag") {
                       QString language = xml.attributes().at(i).value().toString();
                       if (l == language) {
                           inLanguage = true;
                       }
                   }
               }
            }
            if (name == "toolkit" && inLanguage == true) {
                for (int i = 0; i < xml.attributes().size(); i++) {
                    QString attName = xml.attributes().at(i).name().toString();
                    if (attName == "tag") {
                        QString toolkit = xml.attributes().at(i).value().toString();
                        if (tk == toolkit) {
                            inToolkit = true;
                        }
                    }
                }
            }
            if (name == "help" && inLanguage == true && inToolkit == true) {
                for (int i = 0; i < xml.attributes().size(); i++) {
                    QString attName = xml.attributes().at(i).name().toString();
                    if (attName == "tag") {
                        tag = xml.attributes().at(i).value().toString();
                    }
                    if (attName == "source") {
                        source = xml.attributes().at(i).value().toString();
                    }
                }
            }
       }
       if (xml.isEndElement()) {
           QString name = xml.name().toString();
           if (name == "toolkits") {
               inLanguage = false;
               inToolkit = false;
           }
           if (name == "file" && inLanguage == true && inToolkit == true) {
               helpFiles.append({tag, source});
           }
        }
       xml.readNext();
    }
    if (xml.hasError()) {
        qDebug() << "Error loading XML : " << xml.errorString();
    }
    XMLFile.close();
    return helpFiles;
}
