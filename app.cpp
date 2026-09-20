#include "app.h"

//******************************************************************************
// Class App()
//******************************************************************************
App::App() {
    appSettings = new Settings();
    appConstants = new Constants();

    //**************************************************************************
    // Create application folder if not exists, and...
    // Dump the resources content into this new folder.
    //**************************************************************************
    appDir = QDir(QDir::homePath()).filePath(appConstants->getQString("APP_FOLDER"));
    if (!QDir(appDir).exists()) {
        QDir().mkdir(appDir);
        Utils:: copyDirectoryNested(":/", appDir + QDir::separator());
    }

    //**************************************************************************
    // Download the documentation files
    //**************************************************************************
    docDir = appDir + QDir::separator() + appConstants->getQString("DOC_SUBFOLDER");
    if (!QDir(docDir).exists()) {
        QDir().mkdir(docDir);
    }
    QList<QStringList> f = Downloader::getFilesFromIndex(appConstants->getQString("WEB_REPOSITORY") + "docs/" + appConstants->getQString("WEB_INDEX"));
    foreach (const QStringList &item, f) {
        QString fileName = item[0];
        QString remoteHash = item[1];
        QString targetPath = docDir + QDir::separator() + fileName;
        QFileInfo fileInfo(targetPath);
        QString localHash = Utils::fileHash(targetPath, QCryptographicHash::Md5);
        // Skip if file exists and hash matches local file hash
        if (fileInfo.exists() && localHash == remoteHash) {
            continue;
        }
        Downloader::downloadFile(appConstants->getQString("WEB_REPOSITORY") + "docs/" + fileName, targetPath);
    }

    //**************************************************************************
    // Download the template file if it not exists
    //**************************************************************************
    QString templatesFile = appDir + QDir::separator() + appConstants->getQString("TEMPLATES_FILE");
    if (!QFile::exists(templatesFile)) {
        // Download it
        QString distantFile = appConstants->getQString("WEB_REPOSITORY") + appConstants->getQString("TEMPLATES_FILE");
        QString localFile = appDir + QDir::separator() + appConstants->getQString("TEMPLATES_FILE");
        Downloader::downloadFile(distantFile, localFile);

        // Duplicate it
        QString backupFile   = localFile + ".orig";
        if (QFile::copy(localFile, backupFile)) {
            qDebug() << "templates file duplicated";
        } else {
            qDebug() << "templates file NOT duplicated";
        }
    }
}
