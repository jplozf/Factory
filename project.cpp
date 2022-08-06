#include "project.h"

QXmlStreamReader Project::xml;
QFile Project::FactoryFile;

//******************************************************************************
// getLanguages()
//******************************************************************************
QList<QString> Project::getLanguages(QString appDir) {
    QList<QString> languages;
    FactoryFile.setFileName(appDir + QDir::separator() + "factory.xml");
    if (!FactoryFile.open(QFile::ReadOnly | QFile::Text)) {
        qDebug() << "Error loading XML file.";
    }
    Project::xml.setDevice(&FactoryFile);
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
    FactoryFile.close();
    return languages;
}

//******************************************************************************
// getToolkits()
//******************************************************************************
QList<QString> Project::getToolkits(QString appDir, QString l) {
    QList<QString> toolkits;
    FactoryFile.setFileName(appDir + QDir::separator() + "factory.xml");
    if (!FactoryFile.open(QFile::ReadOnly | QFile::Text)) {
        qDebug() << "Error loading XML file.";
    }
    bool inLanguage(false);
    bool toolkitFound(false);
    Project::xml.setDevice(&FactoryFile);
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
    FactoryFile.close();
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
    FactoryFile.setFileName(appDir + QDir::separator() + "factory.xml");
    if (!FactoryFile.open(QFile::ReadOnly | QFile::Text)) {
        qDebug() << "Error loading XML file.";
    }
    bool inLanguage(false);
    bool inToolkit(false);
    Project::xml.setDevice(&FactoryFile);
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
    FactoryFile.close();
    return files;
}

//******************************************************************************
// getHelpFiles()
// Retrieve help files from factory.xml according to the language and toolkit set into the dialog box (new project)
// The default toolkit (if any) is "Raw"
// This method is also used later when retrieving help files when creating a project file for an already populated folder
//******************************************************************************
QList<QStringList> Project::getHelpFiles(QString appDir, QString l, QString tk) {
    QList<QStringList> helpFiles;
    QString tag;
    QString source;
    FactoryFile.setFileName(appDir + QDir::separator() + "factory.xml");
    if (!FactoryFile.open(QFile::ReadOnly | QFile::Text)) {
        qDebug() << "Error loading XML file.";
    }
    bool inLanguage(false);
    bool inToolkit(false);
    Project::xml.setDevice(&FactoryFile);
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
           if (name == "toolkit") {
               inToolkit = false;
           }
           if (name == "help" && inLanguage == true && inToolkit == true) {
               helpFiles.append({tag, source});
           }
        }
       xml.readNext();
    }
    if (xml.hasError()) {
        qDebug() << "Error loading XML : " << xml.errorString();
    }
    FactoryFile.close();
    return helpFiles;
}

//******************************************************************************
// Project()
//******************************************************************************
Project::Project(App *app, QString projectDir) {
    this->app = app;
    this->projectDir = projectDir;
    this->projectFile = fetchProjectFile();
    if (this->projectFile.isNull()) {
        createProjectFile();
    }
    QDir projectFolder(projectDir);
    projectName = projectFolder.dirName();
    projectLanguage = fetchProgrammingLanguage();
}

//******************************************************************************
// fetchProjectFile()
// Try to return the FRX project's file (if any) otherwise returns NULL
//******************************************************************************
QString Project::fetchProjectFile() {
    QString frx = NULL;
    QStringList projectFilter("*" + app->appConstants->getQString("PROJECT_FILE_EXTENSION"));
    QDir directory(projectDir);
    QStringList proFiles = directory.entryList(projectFilter);
    if (!proFiles.isEmpty()) {
        frx = projectDir  + "/" + proFiles.at(0);
    }
    return frx;
}

//******************************************************************************
// fetchProgrammingLanguage()
// Try to guess the language for an already populated folder
//******************************************************************************
QString Project::fetchProgrammingLanguage() {
    Languages *l = new Languages();
    return l->guessLanguage(projectDir);
}

//******************************************************************************
// getElapsedTime()
// Return the sum of all sessions from FRX file in seconds
//******************************************************************************
qint64 Project::getElapsedTime() {
    qint64 sum = 0;
    QString modificationDate;
    QFile XMLFile(projectFile);
    QDomDocument document;
    if (!XMLFile.open(QIODevice::ReadWrite | QIODevice::Text)) {
        qDebug("Failed to open file for reading");
    } else {
        if(!document.setContent(&XMLFile)) {
            qDebug("Failed to parse the file into a DOM tree");
            XMLFile.close();
        } else {
            QDomElement root = document.documentElement();
            QDomElement node = root.firstChild().toElement();
            while(node.isNull() == false) {
                if(node.tagName() == "session") {
                    QString sStart = node.attribute("start");
                    QString sEnd = node.attribute("end");
                    QDateTime qdtStart = QDateTime::fromString(sStart, "yyyyMMdd-hhmmss");
                    QDateTime qdtEnd = QDateTime::fromString(sEnd, "yyyyMMdd-hhmmss");
                    qint64 diff = qdtStart.secsTo(qdtEnd);
                    sum += diff;
                }
                node = node.nextSibling().toElement();
            }
            XMLFile.close();
        }
    }
    return (sum);
}

//******************************************************************************
// createProjectFile()
// Create a FRX project file filled with values into the project folder
//******************************************************************************
void Project::createProjectFile() {
    QDir projectFolder(projectDir);
    QString newFileName = projectDir + QDir::separator() + projectFolder.dirName() + app->appConstants->getQString("PROJECT_FILE_EXTENSION");
    QString source = ":/templates/PROJECT.frx";

    QFile::copy(source, newFileName);
    QFile(newFileName).setPermissions(QFileDevice::ReadOwner | QFileDevice::WriteOwner | QFileDevice::ReadGroup | QFileDevice::WriteGroup | QFileDevice::ReadOther);
    QFile(newFileName).close();

    projectFile = newFileName;
    replaceVarsInFile(newFileName, projectName);
}

//******************************************************************************
// startSession()
//******************************************************************************
void Project::startSession() {
    this->sessionStart = QDateTime::currentDateTime().toString("yyyyMMdd-hhmmss");
}

//******************************************************************************
// endSession()
//******************************************************************************
void Project::endSession() {
    this->sessionEnd = QDateTime::currentDateTime().toString("yyyyMMdd-hhmmss");
    QFile XMLFile(projectFile);
    QDomDocument document;
    if (!XMLFile.open(QIODevice::ReadWrite | QIODevice::Text)) {
        qDebug("Failed to open file for reading");
    } else {
        if(!document.setContent(&XMLFile)) {
            qDebug("Failed to parse the file into a DOM tree");
            XMLFile.close();
        } else {
            QDomElement documentElement = document.documentElement();
            QDomElement a = document.createElement("session");
            a.setAttribute("start", this->sessionStart);
            a.setAttribute("end", this->sessionEnd);
            documentElement.insertBefore(a, QDomNode());
            XMLFile.close();
        }
    }
    // Truncate the old file, save the new document and close
    if (!XMLFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        qDebug("Failed to open file for writing");
    } else {
        QTextStream stream(&XMLFile);
        stream << document.toString();
        XMLFile.close();
    }
}

//******************************************************************************
// getHelpFiles()
// Retrieve help files from factory.xml according to the determined language's project
// Calls the static method
// getHelpFilesFromFRX => if null, getHelpFiles from factory.xml and set them into the FRX file
//******************************************************************************
QList<QStringList> Project::getHelpFiles() {
    QList<QStringList> r;
    QList<QStringList> h = getHelpFilesFromFRX();
    if (h.empty()) {
        // getHelpFiles from factory.xml
        h = getHelpFiles(app->appDir, projectLanguage);
        QList<QStringList> r;
        foreach(QStringList item, h) {
            item[1] = setVars(item[1], projectName);
            r.append({item[0], item[1]});
        }
        // save them into the FRX file
        setHelpFiles(r);
    } else {
        r = h;
    }
    return r;
}

//******************************************************************************
// getHelpFilesFromFRX()
// Retrieve help files from the FRX project file
//******************************************************************************
QList<QStringList> Project::getHelpFilesFromFRX() {
    QList<QStringList> helpFiles;
    QString tag;
    QString source;
    QFile FRXFile;
    qDebug() << "The Project File : " << projectFile;
    FRXFile.setFileName(projectFile);
    if (!FRXFile.open(QFile::ReadOnly | QFile::Text)) {
        qDebug() << "Error loading XML file.";
    }
    Project::xml.setDevice(&FRXFile);
    while(!xml.atEnd()) {
       if (xml.isStartElement()) {
           QString name = xml.name().toString();
           if (name == "help") {
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
           if (name == "help") {
               helpFiles.append({tag, source});
           }
        }
       xml.readNext();
    }
    if (xml.hasError()) {
        qDebug() << "Error loading XML : " << xml.errorString();
    }
    FRXFile.close();
    return helpFiles;
}

//******************************************************************************
// getCreationDate()
// Get Creation Date from FRX
//******************************************************************************
QString Project::getCreationDate() {
    QString creationDate;
    QFile XMLFile(projectFile);
    QDomDocument document;
    if (!XMLFile.open(QIODevice::ReadWrite | QIODevice::Text)) {
        qDebug("Failed to open file for reading");
    } else {
        if(!document.setContent(&XMLFile)) {
            qDebug("Failed to parse the file into a DOM tree");
            XMLFile.close();
        } else {
            QDomElement root = document.documentElement();
            QDomElement node = root.firstChild().toElement();
            while(node.isNull() == false) {
                if(node.tagName() == "created") {
                    creationDate = node.text();
                    break;
                } else {
                    node = node.nextSibling().toElement();
                }
            }
            XMLFile.close();
        }
    }
    return creationDate;
}

//******************************************************************************
// getModificationDate()
// Get last Modification Date from FRX
//******************************************************************************
QString Project::getModificationDate() {
    QString modificationDate;
    QFile XMLFile(projectFile);
    QDomDocument document;
    if (!XMLFile.open(QIODevice::ReadWrite | QIODevice::Text)) {
        qDebug("Failed to open file for reading");
    } else {
        if(!document.setContent(&XMLFile)) {
            qDebug("Failed to parse the file into a DOM tree");
            XMLFile.close();
        } else {
            QDomElement root = document.documentElement();
            QDomElement node = root.firstChild().toElement();
            while(node.isNull() == false) {
                if(node.tagName() == "modified") {
                    modificationDate = node.text();
                    break;
                } else {
                    node = node.nextSibling().toElement();
                }
            }
            XMLFile.close();
        }
    }
    return modificationDate;
}

//******************************************************************************
// setHelpFiles()
// Write into the FRX file the help files section
//******************************************************************************
void Project::setHelpFiles(QList<QStringList> helpFiles) {
    QFile XMLFile(projectFile);
    QDomDocument document;
    if (!XMLFile.open(QIODevice::ReadWrite | QIODevice::Text)) {
        qDebug("Failed to open file for reading");
    } else {
        if(!document.setContent(&XMLFile)) {
            qDebug("Failed to parse the file into a DOM tree");
            XMLFile.close();
        } else {
            QDomElement documentElement = document.documentElement();
            QDomNodeList elements = documentElement.elementsByTagName("help");
            if (elements.isEmpty()) {
                foreach (QStringList item, helpFiles) {
                    QDomElement a = document.createElement("help");
                    a.setAttribute("tag", item[0]);
                    a.setAttribute("source", item[1]);
                    documentElement.insertBefore(a, QDomNode());
                }
            }
            XMLFile.close();
        }
    }
    // Truncate the old file, save the new document and close
    if (!XMLFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        qDebug("Failed to open file for writing");
    } else {
        QTextStream stream(&XMLFile);
        stream << document.toString();
        XMLFile.close();
    }
}

//******************************************************************************
// close()
//******************************************************************************
void Project::close() {
    QString modificationDate = QDateTime::currentDateTime().toString("yyyyMMdd-hhmmss");
    QFile XMLFile(projectFile);
    QDomDocument document;
    if (!XMLFile.open(QIODevice::ReadWrite | QIODevice::Text)) {
        qDebug("Failed to open file for reading");
    } else {
        if(!document.setContent(&XMLFile)) {
            qDebug("Failed to parse the file into a DOM tree");
            XMLFile.close();
        } else {
            // Get the required element
            QDomElement root = document.documentElement();
            QDomElement nodeTag = root.firstChildElement("modified");
            // Create a new node with a QDomText child
            QDomElement newNodeTag = document.createElement(QString("modified"));
            QDomText newNodeText = document.createTextNode(modificationDate);
            newNodeTag.appendChild(newNodeText);
            // Replace existing node with new node
            root.replaceChild(newNodeTag, nodeTag);
            // Write changes to same file
            XMLFile.resize(0);
            QTextStream stream;
            stream.setDevice(&XMLFile);
            document.save(stream, 4);   // 4 spaces for indentation
            XMLFile.close();
        }
    }
}

//******************************************************************************
// getProperties()
//******************************************************************************
QMap<QString, QString> Project::getProperties() {
    QMap<QString, QString> props;

    props["Name"] = this->projectName;
    props["Language"] = this->projectLanguage;
    props["Created"] = this->getCreationDate();
    props["Modified"] = this->getModificationDate();
    props["Elapsed"] = QString::number(this->getElapsedTime());
    if (!projectDir.isEmpty()) {
        qint64 size = Utils::dirSize(projectDir);
        qDebug() << size;
        props["Size"] = QString::number(size);
    } else {
        props["Size"] = "0";
    }


    return props;
}

//******************************************************************************
// randomName()
//******************************************************************************
QString Project::randomName() {
    QList<QString> scratchAdjectives;
    scratchAdjectives.append("tiny");
    scratchAdjectives.append("big");
    scratchAdjectives.append("shiny");
    scratchAdjectives.append("fast");
    scratchAdjectives.append("furious");
    scratchAdjectives.append("slow");
    scratchAdjectives.append("gentle");
    scratchAdjectives.append("slim");
    scratchAdjectives.append("tall");
    scratchAdjectives.append("thin");
    scratchAdjectives.append("short");
    scratchAdjectives.append("sharp");
    scratchAdjectives.append("happy");
    scratchAdjectives.append("bored");
    scratchAdjectives.append("lucky");
    scratchAdjectives.append("strong");
    scratchAdjectives.append("weak");
    scratchAdjectives.append("mad");
    scratchAdjectives.append("tired");
    scratchAdjectives.append("clever");
    scratchAdjectives.append("smart");
    scratchAdjectives.append("light");
    scratchAdjectives.append("heavy");
    scratchAdjectives.append("foolish");
    scratchAdjectives.append("black");
    scratchAdjectives.append("white");
    scratchAdjectives.append("blue");
    scratchAdjectives.append("green");
    scratchAdjectives.append("red");
    scratchAdjectives.append("yellow");
    scratchAdjectives.append("brown");
    scratchAdjectives.append("pink");
    scratchAdjectives.append("purple");
    scratchAdjectives.append("orange");
    scratchAdjectives.append("silver");
    scratchAdjectives.append("golden");
    scratchAdjectives.append("noisy");
    scratchAdjectives.append("freaky");
    scratchAdjectives.append("little");
    scratchAdjectives.append("hairy");
    scratchAdjectives.append("bold");
    scratchAdjectives.append("brave");
    scratchAdjectives.append("fat");
    scratchAdjectives.append("rude");
    scratchAdjectives.append("pretty");
    scratchAdjectives.append("old");
    scratchAdjectives.append("young");
    scratchAdjectives.append("good");
    scratchAdjectives.append("bad");
    scratchAdjectives.append("best");
    scratchAdjectives.append("new");
    scratchAdjectives.append("first");
    scratchAdjectives.append("last");
    scratchAdjectives.append("nice");
    scratchAdjectives.append("full");
    scratchAdjectives.append("empty");
    scratchAdjectives.append("hot");
    scratchAdjectives.append("cold");
    scratchAdjectives.append("high");
    scratchAdjectives.append("low");
    scratchAdjectives.append("fresh");
    scratchAdjectives.append("soft");
    scratchAdjectives.append("blurry");
    scratchAdjectives.append("sad");
    scratchAdjectives.append("healthy");
    scratchAdjectives.append("thrilled");
    scratchAdjectives.append("sick");

    QList<QString> scratchAnimals;
    scratchAnimals.append("dog");
    scratchAnimals.append("cat");
    scratchAnimals.append("bee");
    scratchAnimals.append("wasp");
    scratchAnimals.append("hornet");
    scratchAnimals.append("fish");
    scratchAnimals.append("cow");
    scratchAnimals.append("puppy");
    scratchAnimals.append("bird");
    scratchAnimals.append("parrot");
    scratchAnimals.append("canary");
    scratchAnimals.append("turtle");
    scratchAnimals.append("rabbit");
    scratchAnimals.append("mouse");
    scratchAnimals.append("snake");
    scratchAnimals.append("lizard");
    scratchAnimals.append("pony");
    scratchAnimals.append("horse");
    scratchAnimals.append("pig");
    scratchAnimals.append("hen");
    scratchAnimals.append("rooster");
    scratchAnimals.append("goose");
    scratchAnimals.append("turkey");
    scratchAnimals.append("duck");
    scratchAnimals.append("sheep");
    scratchAnimals.append("ram");
    scratchAnimals.append("lamb");
    scratchAnimals.append("goat");
    scratchAnimals.append("donkey");
    scratchAnimals.append("monkey");
    scratchAnimals.append("jellyfish");
    scratchAnimals.append("octopus");
    scratchAnimals.append("butterfly");
    scratchAnimals.append("bull");
    scratchAnimals.append("ox");
    scratchAnimals.append("calf");
    scratchAnimals.append("hare");
    scratchAnimals.append("beetle");
    scratchAnimals.append("snail");
    scratchAnimals.append("slug");
    scratchAnimals.append("ant");
    scratchAnimals.append("spider");
    scratchAnimals.append("worm");
    scratchAnimals.append("dove");
    scratchAnimals.append("crow");
    scratchAnimals.append("swan");
    scratchAnimals.append("fox");
    scratchAnimals.append("owl");
    scratchAnimals.append("beaver");
    scratchAnimals.append("bat");
    scratchAnimals.append("weasel");
    scratchAnimals.append("deer");
    scratchAnimals.append("doe");
    scratchAnimals.append("stag");
    scratchAnimals.append("tuna");
    scratchAnimals.append("salmon");
    scratchAnimals.append("lobster");
    scratchAnimals.append("shrimp");
    scratchAnimals.append("oyster");
    scratchAnimals.append("mussel");
    scratchAnimals.append("starfish");
    scratchAnimals.append("whale");
    scratchAnimals.append("shark");
    scratchAnimals.append("dolphin");
    scratchAnimals.append("walrus");
    scratchAnimals.append("seal");
    scratchAnimals.append("otter");
    scratchAnimals.append("panda");
    scratchAnimals.append("tiger");
    scratchAnimals.append("lion");
    scratchAnimals.append("bear");
    scratchAnimals.append("frog");
    scratchAnimals.append("toad");
    scratchAnimals.append("stork");
    scratchAnimals.append("ostrich");
    scratchAnimals.append("swallow");
    scratchAnimals.append("eagle");
    scratchAnimals.append("sparrow");
    scratchAnimals.append("tortoise");
    scratchAnimals.append("ferret");
    scratchAnimals.append("magpie");
    scratchAnimals.append("seagull");
    scratchAnimals.append("squirrel");
    scratchAnimals.append("hedgehog");
    scratchAnimals.append("cheetah");
    scratchAnimals.append("camel");
    scratchAnimals.append("hyena");
    scratchAnimals.append("giraffe");
    scratchAnimals.append("elephant");
    scratchAnimals.append("gorilla");
    scratchAnimals.append("baboon");
    scratchAnimals.append("badger");

    QString strAdjective = scratchAdjectives.at(QRandomGenerator::global()->generate() % scratchAdjectives.size());
    QString strAnimal = scratchAnimals.at(QRandomGenerator::global()->generate() % scratchAnimals.size());
    return Utils::capitalize(strAdjective) + Utils::capitalize(strAnimal);
}

//******************************************************************************
// replaceVarsInFile()
//******************************************************************************
void Project::replaceVarsInFile(QString filename, QString projectName) {
    QByteArray fileData;
    QFile XMLFile(filename);
    XMLFile.open(QIODevice::ReadWrite);
    fileData = XMLFile.readAll();
    QString text(fileData);

    text = setVars(text, projectName);

    XMLFile.seek(0);
    XMLFile.write(text.toUtf8());
    XMLFile.close();
}

//******************************************************************************
// setVars()
//******************************************************************************
QString Project::setVars(QString text, QString projectName) {
    text.replace("%PROJECT%", projectName);
    text.replace("%LANGUAGE%", projectLanguage);
    // text.replace("%TOOLKIT%", cbxToolkit->currentText());
    text.replace("%YEAR%", QDateTime::currentDateTime().toString("yyyy"));
    text.replace("%MONTH%", QDateTime::currentDateTime().toString("MM"));
    text.replace("%DAY%", QDateTime::currentDateTime().toString("dd"));
    text.replace("%TIME%", QDateTime::currentDateTime().toString("hhmmss"));
    text.replace("%DATE%", QDateTime::currentDateTime().toString("yyyyMMdd"));
    text.replace("%DATETIME%", QDateTime::currentDateTime().toString("yyyyMMdd-hhmmss"));
    text.replace("%AUTHOR%", app->appSettings->get("PROJECT_USER_NAME").toString());
    text.replace("%COMPANY%", app->appSettings->get("PROJECT_USER_COMPANY").toString());
    text.replace("%MAIL%", app->appSettings->get("PROJECT_USER_MAIL").toString());
    text.replace("%WEB%", app->appSettings->get("PROJECT_USER_WEB").toString());
    text.replace("%DOC_FOLDER%", app->docDir);

    return (text);
}

