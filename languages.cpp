#include "languages.h"

//******************************************************************************
// guessLanguage()
//******************************************************************************
QString Languages::guessLanguage(QString dir, bool recurse) {
    // TODO : Manage recurse flag
    int nbFiles(0);
    QString prgl = "UNKNOWN";
    QStringList fileFilter("*");
    QDir directory(dir);
    QStringList proFiles = directory.entryList(fileFilter);
    if (!proFiles.isEmpty()) {
        foreach(QString fName, proFiles) {
            nbFiles++;
            QString ext = getExtension(fName);
            foreach(QStringList lstExtensions, languages) {
                for (int i = 1; i < lstExtensions.count(); i++) {
                    if (ext == lstExtensions[i]) {
                        if (languageHits.contains(lstExtensions[0])) {
                            languageHits[lstExtensions[0]]++;
                        } else {
                            languageHits.insert(lstExtensions[0], 1);
                        }
                        break;
                    }
                }
            }
        }
        QPair<QString, int> top;
        top.first = "UNKNOWN";
        top.second = 0;
        QMapIterator<QString, int> i(languageHits);
         while (i.hasNext()) {
             i.next();
             if (i.value() > top.second) {
                 top.first = i.key();
                 top.second = i.value();
             }
         }
         prgl = top.first;
    }
    return prgl;
}

//******************************************************************************
// getExtension()
//******************************************************************************
QString Languages::getExtension(QString file) {
    QFileInfo fi(file);
    return ("." + fi.suffix());
}
