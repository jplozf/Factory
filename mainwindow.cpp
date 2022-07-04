#include "mainwindow.h"

//******************************************************************************
// MainWindow()
//******************************************************************************
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) , ui(new Ui::MainWindow) {
    ui->setupUi(this);
    setWindowFlags(Qt::Window | Qt::MSWindowsFixedSizeDialogHint);

    appSettings = new Settings();
    appConstants = new Constants();

    //****************************************************************************
    // Create application folder if not exists, and...
    // Dump the resources content into this new folder
    //****************************************************************************
    appDir = QDir(QDir::homePath()).filePath(appConstants->getQString("APP_FOLDER"));
    if (!QDir(appDir).exists()) {
        QDir().mkdir(appDir);
        Utils:: copyDirectoryNested(":/", appDir + QDir::separator());
    }

    connect(ui->btnNewProject, SIGNAL(clicked()), this, SLOT(slotDoNewProject()));
    connect(ui->btnOpenProject, SIGNAL(clicked()), this, SLOT(slotDoOpenProject()));
    connect(ui->cbxMRUProjects, SIGNAL(currentIndexChanged(QString)), this, SLOT(slotDoOpenRecentProject(QString)));
    connect(ui->btnPropertiesProject, SIGNAL(clicked()), this, SLOT(slotDoPropertiesProject()));
    connect(ui->btnArchiveProject, SIGNAL(clicked()), this, SLOT(slotDoArchiveProject()));
    connect(ui->btnCloseProject, SIGNAL(clicked()), this, SLOT(slotDoCloseProject()));

    connect(ui->btnEdit, SIGNAL(clicked()), this, SLOT(slotDoEdit()));
    connect(ui->btnBrowser, SIGNAL(clicked()), this, SLOT(slotDoBrowser()));
    connect(ui->btnBuild, SIGNAL(clicked()), this, SLOT(slotDoBuild()));
    connect(ui->btnSettings, SIGNAL(clicked()), this, SLOT(slotDoSettings()));
    connect(ui->btnTerm, SIGNAL(clicked()), this, SLOT(slotDoTerm()));      

    model = new QFileSystemModel(this);
    model->setReadOnly(true);
    ui->tvwProject->setModel(model);
    ui->tvwProject->setAnimated(false);
    ui->tvwProject->setIndentation(10);
    ui->tvwProject->setSortingEnabled(true);
    ui->tvwProject->sortByColumn(0, Qt::AscendingOrder);
    model->setFilter(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);

    model->setRootPath(appSettings->get("DEFAULT_REPOSITORY").toString());
    ui->tvwProject->setRootIndex(model->index(appSettings->get("DEFAULT_REPOSITORY").toString()));

    ui->tvwProject->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tvwProject, SIGNAL(clicked(QModelIndex)), this, SLOT(slotClickedProject(QModelIndex)));
    connect(ui->tvwProject, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(slotDoubleClickedProject(QModelIndex)));
    connect(ui->tvwProject, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slotMenuContextProject(QPoint)));

    ui->btnArchiveProject->setEnabled(false);
    ui->btnCloseProject->setEnabled(false);
    ui->btnPropertiesProject->setEnabled(false);

    connect(ui->action_Exit, SIGNAL(triggered()), this, SLOT(slotDoExit()));
    // connect(ui->action_Exit, SIGNAL(triggered()), this, SLOT(this->close()));

    readSettings();
    appSettings->form(ui->boxSettings);
    connect(appSettings->btnTemplatesCutomizing, SIGNAL(clicked()), this, SLOT(slotDoTemplatesCutomizing()));
}

//******************************************************************************
// ~MainWindow()
//******************************************************************************
MainWindow::~MainWindow() {
    delete ui;
}

//******************************************************************************
// slotDoExit()
//******************************************************************************
void MainWindow::slotDoExit() {
    this->close();
}

//******************************************************************************
// closeEvent()
//******************************************************************************
void MainWindow::closeEvent(QCloseEvent *event) {
    QMessageBox::StandardButton rc;
    rc = QMessageBox::question(this, appConstants->getQString("APPLICATION_NAME"), QString("Close the factory ?\n"), QMessageBox::Yes|QMessageBox::No);
    if (rc == QMessageBox::Yes) {
        saveSettings();
        event->accept();
    } else {
        event->ignore();
    }
}

//******************************************************************************
// saveSettings()
//******************************************************************************
void MainWindow::saveSettings() {
    //**************************************************************************
    // Application state saving
    //**************************************************************************
    QSettings registry(appConstants->getQString("ORGANIZATION_NAME"), appConstants->getQString("APPLICATION_NAME"));
    registry.setValue("geometry", saveGeometry());
    registry.setValue("windowState", saveState());

    //**************************************************************************
    // MRU Projects saving
    //**************************************************************************
    QVectorIterator<QString> iProjects(mruProjects);
    int jProjects(0);
    int cProjects(0);
    int mProjetcs(appSettings->get("MRU_PROJECTS").toInt());
    int eProjects(mruProjects.length());
    qDebug() << eProjects;
    registry.beginWriteArray("Projects");
    while (iProjects.hasNext()) {
        QString project = iProjects.next();
        if ((eProjects - cProjects) <= mProjetcs) {
            registry.setArrayIndex(jProjects++);
            registry.setValue("Project", project);
        }
        cProjects++;
    }
    registry.endArray();

    registry.setValue("lastProject", projectPath);

    //**************************************************************************
    // Settings saving
    //**************************************************************************
    Settings mySettings;
    mySettings.write();
}

//******************************************************************************
// readSettings()
//******************************************************************************
void MainWindow::readSettings() {
    QSettings registry(appConstants->getQString("ORGANIZATION_NAME"), appConstants->getQString("APPLICATION_NAME"));

    const QByteArray geometry = registry.value("geometry", QByteArray()).toByteArray();
    if (geometry.isEmpty()) {
        const QRect availableGeometry = QApplication::desktop()->availableGeometry();
        resize(availableGeometry.width() / 3, availableGeometry.height() / 2);
        move((availableGeometry.width() - width()) / 2, (availableGeometry.height() - height()) / 2);
    } else {
        restoreGeometry(geometry);
    }

    const QByteArray windowState = registry.value("windowState", QByteArray()).toByteArray();
    if (!windowState.isEmpty()) {
        restoreState(registry.value("windowState").toByteArray());
    }

    //**************************************************************************
    // MRU Projects restoring
    //**************************************************************************
    int sizeProjects = registry.beginReadArray("Projects");
    for (int i = 0; i < sizeProjects; ++i) {
        registry.setArrayIndex(i);
        mruProjects.append(registry.value("Project").toString());
    }
    registry.endArray();
    QVectorIterator<QString> iProjects(mruProjects);
    while (iProjects.hasNext()) {
        QString project = iProjects.next();
        ui->cbxMRUProjects->addItem(project);
    }

    //**************************************************************************
    // Restoring last opening project and open it
    //**************************************************************************
    const QString lastProject = registry.value("lastProject", QString()).toString();
    if (!lastProject.isEmpty()) {
        openProject(lastProject);
    } else {
        closeProject();
    }

    //**************************************************************************
    // Download the documentation files
    //**************************************************************************
    docDir = appDir + QDir::separator() + "dox" + QDir::separator();
    if (!QDir(docDir).exists()) {
        QDir().mkdir(docDir);
    }
    QList<QStringList> f = Downloader::getFilesFromIndex(appConstants->getQString("WEB_REPOSITORY") + "index.txt");
    foreach(QStringList item, f) {
        Downloader::downloadFile(appConstants->getQString("WEB_REPOSITORY") + item[0], docDir + item[0]);
    }
}

//******************************************************************************
// slotDoNewProject()
//******************************************************************************
void MainWindow::slotDoNewProject() {
    DlgNewProject *dlg = new DlgNewProject(appSettings, appConstants);
    dlg->setModal(true);
    if(dlg->exec() == QDialog::Accepted)
    {
        openProject(dlg->getProjectPath());
    }
}

//******************************************************************************
// slotDoOpenProject()
//******************************************************************************
void MainWindow::slotDoOpenProject() {
    QString dirProject = QFileDialog::getExistingDirectory(this, tr("Open Project Folder"),
                                                    QDir::homePath(),
                                                    QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (!dirProject.isEmpty()) {
        openProject(dirProject);
    }
}

//******************************************************************************
// slotDoOpenRecentProject()
//******************************************************************************
void MainWindow::slotDoOpenRecentProject(QString project) {
    if (!project.isEmpty()) {
        openProject(project);
    }
}

//******************************************************************************
// slotDoPropertiesProject()
//******************************************************************************
void MainWindow::slotDoPropertiesProject() {
    if (!projectPath.isEmpty()) {
        qint64 size = Utils::dirSize(projectPath);
        qDebug() << size;
        QMessageBox msgBox;
        msgBox.setText(QString("%1\n%2 bytes\n%3").arg(projectPath,QString::number(size),Utils::formatSize(size)));
        msgBox.exec();
    }
}

//******************************************************************************
// slotDoArchiveProject()
//******************************************************************************
void MainWindow::slotDoArchiveProject() {

}

//******************************************************************************
// slotDoCloseProject()
//******************************************************************************
void MainWindow::slotDoCloseProject() {
    closeProject();
}

//******************************************************************************
// launchProgram()
//******************************************************************************
void MainWindow::launchProgram(QString pgm) {
    QProcess::startDetached(pgm);
}

//******************************************************************************
// openProject()
//******************************************************************************
void MainWindow::openProject(QString project) {
    projectPath = project;        
    if (!mruProjects.contains(projectPath))
    {
        mruProjects.append(projectPath);
        ui->cbxMRUProjects->addItem(projectPath);
    }

    QDir projectDir(projectPath);
    if (projectDir.exists()) {
        model->setRootPath(projectPath);
        ui->tvwProject->setRootIndex(model->index(projectPath));
        ui->toolBox->setCurrentIndex(1);
        QFileInfo fi(projectPath);
        ui->toolBox->setItemText(0, QString("Project [%1]").arg(fi.fileName()));
        ui->btnArchiveProject->setEnabled(true);
        ui->btnCloseProject->setEnabled(true);
        ui->btnPropertiesProject->setEnabled(true);
    } else {
        closeProject();
    }
}

//******************************************************************************
// closeProject()
//******************************************************************************
void MainWindow::closeProject() {
    projectPath = "";
    model->setRootPath(appSettings->get("DEFAULT_REPOSITORY").toString());
    ui->tvwProject->setRootIndex(model->index(appSettings->get("DEFAULT_REPOSITORY").toString()));
    ui->toolBox->setCurrentIndex(1);
    ui->toolBox->setItemText(0, "Project");

    ui->btnArchiveProject->setEnabled(false);
    ui->btnCloseProject->setEnabled(false);
    ui->btnPropertiesProject->setEnabled(false);
}

//******************************************************************************
// slotDoEdit()
//******************************************************************************
void MainWindow::slotDoEdit() {
    launchProgram(appSettings->get("DEFAULT_EDITOR").toString());
}

//******************************************************************************
// slotDoEditFile()
//******************************************************************************
void MainWindow::slotDoEditFile() {
    QAction *action = qobject_cast<QAction *>(sender());
    QString f = action->property("file").toString();
    launchProgram(appSettings->get("DEFAULT_EDITOR").toString() + " \"" + f + "\"");
}

//******************************************************************************
// slotDoDefaultFile()
//******************************************************************************
void MainWindow::slotDoDefaultFile() {
    QAction *action = qobject_cast<QAction *>(sender());
    QString f = action->property("file").toString();
    QString url = QString("file://%1").arg(f);
    qDebug() << url;
    QDesktopServices::openUrl(QUrl(QString(url)));
}

//******************************************************************************
// slotDoLaunchFile()
//******************************************************************************
void MainWindow::slotDoLaunchFile() {
    QAction *action = qobject_cast<QAction *>(sender());
    QString f = action->property("file").toString();
}

//******************************************************************************
// slotDoLaunchTerminal()
//******************************************************************************
void MainWindow::slotDoLaunchTerminal() {
    QAction *action = qobject_cast<QAction *>(sender());
    QString f = action->property("file").toString();
    // launchProgram(appSettings->get("DEFAULT_TERMINAL").toString() + " cd " + f);
}

//******************************************************************************
// slotDoBrowseFile()
//******************************************************************************
void MainWindow::slotDoBrowseFile() {
    QAction *action = qobject_cast<QAction *>(sender());
    QString f = action->property("file").toString();
    launchProgram(appSettings->get("DEFAULT_BROWSER").toString() + " \"" + f + "\"");
}

//******************************************************************************
// slotDoTerm()
//******************************************************************************
void MainWindow::slotDoTerm() {
    launchProgram(appSettings->get("DEFAULT_TERMINAL").toString());
}

//******************************************************************************
// slotDoBuild()
//******************************************************************************
void MainWindow::slotDoBuild() {
    // launchProgram("tabby");
}

//******************************************************************************
// slotDoBrowser()
//******************************************************************************
void MainWindow::slotDoBrowser() {
    launchProgram(appSettings->get("DEFAULT_BROWSER").toString());
}

//******************************************************************************
// slotDoSettings()
//******************************************************************************
void MainWindow::slotDoSettings() {
    appSettings->form(ui->boxSettings);
    ui->toolBox->setCurrentIndex(4);
}

//******************************************************************************
// slotClickedProject()
//******************************************************************************
void MainWindow::slotClickedProject(QModelIndex idx) {
    //
    qDebug() << model->filePath(idx);
}

//******************************************************************************
// slotDoubleClickedProject()
//******************************************************************************
void MainWindow::slotDoubleClickedProject(QModelIndex idx) {
    qDebug() << model->filePath(idx);
    launchProgram(appSettings->get("DEFAULT_EDITOR").toString() + " \"" + model->filePath(idx) + "\"");
}

//******************************************************************************
// slotMenuContextProject()
//******************************************************************************
void MainWindow::slotMenuContextProject(QPoint p) {
    auto idx = ui->tvwProject->indexAt(p);
    // qDebug() << model->filePath(idx);
    bool isDir = model->isDir(idx);

    auto menu = QMenu();
    if (isDir) {
        //*****************************************************************************************
        // DIRECTORY CONTEXT MENU
        //*****************************************************************************************
        // * New Item...
        // * Open Terminal Here
        // * Delete...
        // * Properties...
        //*****************************************************************************************
        auto newAction = QAction("New...");
        newAction.setProperty("file", model->filePath(idx));
        connect(&newAction, SIGNAL(triggered()), this, SLOT(slotDoNewItem()));
        menu.addAction(&newAction);

        auto openTerminal = QAction(QIcon(":/icons/icons/Computer.png"), "Open Terminal here");
        openTerminal.setProperty("file", model->filePath(idx));
        connect(&openTerminal, SIGNAL(triggered()), this, SLOT(slotDoOpenTerminal()));
        menu.addAction(&openTerminal);

        auto deleteFolderAction = QAction("Delete...");
        deleteFolderAction.setProperty("file", model->filePath(idx));
        connect(&deleteFolderAction, SIGNAL(triggered()), this, SLOT(slotDoDeleteFolder()));
        menu.addAction(&deleteFolderAction);

        auto propertiesFolderAction = QAction("Properties...");
        propertiesFolderAction.setProperty("file", model->filePath(idx));
        connect(&propertiesFolderAction, SIGNAL(triggered()), this, SLOT(slotDoPropertiesFolder()));
        menu.addAction(&propertiesFolderAction);

        menu.exec(ui->tvwProject->viewport()->mapToGlobal(p));
    } else {
        //*****************************************************************************************
        // FILE CONTEXT MENU
        //*****************************************************************************************
        // * Open with Editor
        // * Open in Browser
        // * Open Terminal Here
        // * Open with default application
        // * Launch
        // * Launch in Terminal
        // * Duplicate...
        // * Rename...
        // * Delete...
        // * Properties...
        //*****************************************************************************************
        auto openEditor = QAction(QIcon(":/icons/icons/document.png"), "Open with Editor");
        openEditor.setProperty("file", model->filePath(idx));
        connect(&openEditor, SIGNAL(triggered()), this, SLOT(slotDoEditFile()));
        menu.addAction(&openEditor);

        auto openBrowser = QAction(QIcon(":/icons/icons/Globe.png"), "Open in Browser");
        openBrowser.setProperty("file", model->filePath(idx));
        connect(&openBrowser, SIGNAL(triggered()), this, SLOT(slotDoBrowseFile()));
        menu.addAction(&openBrowser);

        auto openTerminal = QAction(QIcon(":/icons/icons/Computer.png"), "Open Terminal here");
        openTerminal.setProperty("file", model->filePath(idx));
        connect(&openTerminal, SIGNAL(triggered()), this, SLOT(slotDoOpenTerminal()));
        menu.addAction(&openTerminal);

        auto openDefault = QAction("Open with default application");
        openDefault.setProperty("file", model->filePath(idx));
        connect(&openDefault, SIGNAL(triggered()), this, SLOT(slotDoDefaultFile()));
        menu.addAction(&openDefault);

        auto launchFile = QAction("Launch");
        launchFile.setProperty("file", model->filePath(idx));
        connect(&launchFile, SIGNAL(triggered()), this, SLOT(slotDoLaunchFile()));
        menu.addAction(&launchFile);

        auto launchTerminal = QAction("Launch in Terminal");
        launchTerminal.setProperty("file", model->filePath(idx));
        connect(&launchTerminal, SIGNAL(triggered()), this, SLOT(slotDoLaunchTerminal()));
        menu.addAction(&launchTerminal);

        auto duplicateFileAction = QAction("Duplicate...");
        duplicateFileAction.setProperty("file", model->filePath(idx));
        connect(&duplicateFileAction, SIGNAL(triggered()), this, SLOT(slotDoDuplicateFile()));
        menu.addAction(&duplicateFileAction);

        auto renameFileAction = QAction("Rename...");
        renameFileAction.setProperty("file", model->filePath(idx));
        connect(&renameFileAction, SIGNAL(triggered()), this, SLOT(slotDoRenameFile()));
        menu.addAction(&renameFileAction);

        auto deleteFileAction = QAction("Delete...");
        deleteFileAction.setProperty("file", model->filePath(idx));
        connect(&deleteFileAction, SIGNAL(triggered()), this, SLOT(slotDoDeleteFile()));
        menu.addAction(&deleteFileAction);

        auto propertiesFileAction = QAction("Properties...");
        propertiesFileAction.setProperty("file", model->filePath(idx));
        connect(&propertiesFileAction, SIGNAL(triggered()), this, SLOT(slotDoPropertiesFile()));
        menu.addAction(&propertiesFileAction);

        menu.exec(ui->tvwProject->viewport()->mapToGlobal(p));
    }
}

//******************************************************************************
// slotDoNewItem()
//******************************************************************************
void MainWindow::slotDoNewItem() {
    QAction *action = qobject_cast<QAction *>(sender());
    QString f = action->property("file").toString();

}

//******************************************************************************
// slotDoOpenTerminal()
//******************************************************************************
void MainWindow::slotDoOpenTerminal() {
    QAction *action = qobject_cast<QAction *>(sender());
    QString f = action->property("file").toString();
    QFileInfo fi(f);
    QString fn = fi.absolutePath();
    qDebug() << fn;

    QProcess mProcess;
    mProcess.startDetached("/usr/bin/bash", QStringList() << "-c" << "cd " + fn);
    mProcess.waitForFinished();
    /*
    // mProcess.setReadChannelMode(QProcess::SeparateChannels);
    // mProcess.setProcessChannelMode(QProcess::MergedChannels);
    // mProcess.readAllStandardOutput();
    mProcess.startDetached(appSettings->get("DEFAULT_TERMINAL").toString());
    if(mProcess.waitForStarted(-1)) {
        // QString cmd = "cd " + fn + "\n";
        // mProcess.write(cmd.toLatin1());
        mProcess.write("cd /home/jpl/snap\n");
        // mProcess.closeWriteChannel();
     }
     */
}

//******************************************************************************
// slotDoDeleteFolder()
//******************************************************************************
void MainWindow::slotDoDeleteFolder() {
    QAction *action = qobject_cast<QAction *>(sender());
    QString f = action->property("file").toString();
    QMessageBox::StandardButton rc;
    rc = QMessageBox::question(this, appConstants->getQString("APPLICATION_NAME"), QString("Delete this folder ?\n\n%1\n").arg(f), QMessageBox::Yes|QMessageBox::No);
    if (rc == QMessageBox::Yes) {
        QDir dir(f);
        dir.removeRecursively();
    }
}

//******************************************************************************
// slotDoPropertiesFolder()
//******************************************************************************
void MainWindow::slotDoPropertiesFolder() {
    QAction *action = qobject_cast<QAction *>(sender());
    QString f = action->property("file").toString();

    QMap<QString, QString> props = Utils::fileProperties(f);
    DlgProperties *dlg = new DlgProperties(props, "Folder Properties");
    dlg->show();
}

//******************************************************************************
// slotDoDuplicateFile()
//******************************************************************************
void MainWindow::slotDoDuplicateFile() {
    QAction *action = qobject_cast<QAction *>(sender());
    QString f = action->property("file").toString();
    bool ok;
    QFileInfo fi(QFile(f).fileName());
    QString filename(fi.fileName());
    QString basename(fi.absolutePath());
    QString newName = QInputDialog::getText(this, tr("Duplicate file"), tr("New file name:"), QLineEdit::Normal, filename, &ok);
    if (ok && !newName.isEmpty()) {
        if (!QFile::exists(basename + QDir::separator() + newName)) {
            QFile::copy(f, basename + QDir::separator() + newName);
        }
    }
}

//******************************************************************************
// slotDoRenameFile()
//******************************************************************************
void MainWindow::slotDoRenameFile() {
    QAction *action = qobject_cast<QAction *>(sender());
    QString f = action->property("file").toString();
    bool ok;
    QString text = QInputDialog::getText(this, tr("Rename file"), tr("New file name:"), QLineEdit::Normal, f, &ok);
    if (ok && !text.isEmpty()) {
        QFile file(f);
        file.rename(text);
    }
}

//******************************************************************************
// slotDoDeleteFile()
//******************************************************************************
void MainWindow::slotDoDeleteFile() {
    QAction *action = qobject_cast<QAction *>(sender());
    QString f = action->property("file").toString();
    QMessageBox::StandardButton rc;
    rc = QMessageBox::question(this, appConstants->getQString("APPLICATION_NAME"), QString("Delete this file ?\n\n%1\n").arg(f), QMessageBox::Yes|QMessageBox::No);
    if (rc == QMessageBox::Yes) {
        QFile file(f);
        file.remove();
    }
}

//******************************************************************************
// slotDoPropertiesFile()
//******************************************************************************
void MainWindow::slotDoPropertiesFile() {
    QAction *action = qobject_cast<QAction *>(sender());
    QString f = action->property("file").toString();

    QMap<QString, QString> props = Utils::fileProperties(f);
    DlgProperties *dlg = new DlgProperties(props, "File Properties");
    dlg->show();
}

//******************************************************************************
// slotDoTemplatesCutomizing()
//******************************************************************************
void MainWindow::slotDoTemplatesCutomizing() {
    /*
    Constants *appConstants = new Constants();
    QDir appDir = QDir(QDir::homePath()).filePath(appConstants->getQString("APP_FOLDER"));
    Utils:: copyDirectoryNested(":/templates", appDir.path() + QDir::separator() + "templates" );
    */
    launchProgram(appSettings->get("DEFAULT_EDITOR").toString() + " " + appDir + QDir::separator() + "factory.xml");
}
