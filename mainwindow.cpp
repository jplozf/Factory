#include "mainwindow.h"
#include "GitTimeEstimator.h"

//******************************************************************************
// MainWindow()
//******************************************************************************
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) , ui(new Ui::MainWindow) {
    ui->setupUi(this);
    setWindowFlags(Qt::Window | Qt::MSWindowsFixedSizeDialogHint);
    app = new App();
    runningSession = false;

    setObjectName("borderlessMainWindow");
    setWindowFlags(Qt::FramelessWindowHint| Qt::WindowSystemMenuHint | Qt::WindowStaysOnTopHint | Qt::Dialog | Qt::Tool);
    setWindowFlags(windowFlags() | Qt::WindowMinimizeButtonHint);
    setAttribute(Qt::WA_QuitOnClose, true);
    // The following instruction leads to a "double free or corruption (out)" error on exit
    // setAttribute(Qt::WA_DeleteOnClose, true);
    setStyleSheet("#borderlessMainWindow{border:1px solid palette(highlight);}");

    connect(ui->btnOpenProject, SIGNAL(clicked()), this, SLOT(slotDoOpenProject()));
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

    model->setRootPath(app->appSettings->get("DEFAULT_REPOSITORY").toString());
    ui->tvwProject->setRootIndex(model->index(app->appSettings->get("DEFAULT_REPOSITORY").toString()));

    ui->tvwProject->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tvwProject, SIGNAL(clicked(QModelIndex)), this, SLOT(slotClickedProject(QModelIndex)));
    connect(ui->tvwProject, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(slotDoubleClickedProject(QModelIndex)));
    connect(ui->tvwProject, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slotMenuContextProject(QPoint)));

    ui->btnArchiveProject->setEnabled(false);
    ui->btnCloseProject->setEnabled(false);
    ui->btnPropertiesProject->setEnabled(false);

    connect(ui->btnExit, SIGNAL(clicked()), this, SLOT(slotDoExit()));
    connect(ui->action_Exit, SIGNAL(triggered()), this, SLOT(slotDoExit()));

    connect(ui->btnPin, SIGNAL(clicked()), this, SLOT(slotDoPin()));

    readSettings();
    app->appSettings->form(ui->scaSettings);
    connect(app->appSettings->btnTemplatesCutomizing, SIGNAL(clicked()), this, SLOT(slotDoTemplatesCutomizing()));
    connect(app->appSettings->btnTemplatesReset, SIGNAL(clicked()), this, SLOT(slotDoTemplatesReset()));
    app->mruProjects = this->mruProjects;

    ui->btnArchiveProject->setToolTip("Archive this project");
    ui->btnCloseProject->setToolTip("Close this project");
    ui->btnOpenProject->setToolTip("Manage projects");
    ui->btnPropertiesProject->setToolTip("Properties of this project");
    ui->btnBrowser->setToolTip("Open the browser");
    ui->btnBuild->setToolTip("Build");
    ui->btnEdit->setToolTip("Open the editor");
    ui->btnExit->setToolTip("Exit the Factory");
    ui->btnPin->setToolTip("Pin the Factory");
    ui->btnSettings->setToolTip("Settings");
    ui->btnTerm->setToolTip("Open the terminal");

    ui->btnDoBuild->setToolTip("Build the current project");
    ui->btnDoRun->setToolTip("Run the current project");
    ui->btnDoSaveBuild->setToolTip("Save the customized build command");
    ui->btnDoSaveRun->setToolTip("Save the customized run command");
    ui->btnStopRun->setToolTip("Stop or abort the current run");
    ui->chkRunTerminal->setToolTip("Run the current project in a terminal");

    IsVisible = true;
    ui->lblTitle->setText(QString("%1 v%2.%3-%4")
                             .arg(app->appConstants->getQString("APPLICATION_NAME"))
                             .arg(APP_VERSION)
                             .arg(GIT_COMMIT_COUNT)
                             .arg(GIT_HASH));
    clearProjectProperties();
}

//******************************************************************************
// mousePressEvent()
//******************************************************************************
void MainWindow::mousePressEvent(QMouseEvent* event) {
    if (!ui->centralwidget->underMouse() && !ui->lblTitle->underMouse())
        return;

    if(event->button() == Qt::LeftButton) {
        mMoving = true;
        mLastMousePosition = event->pos();
    }
}

//******************************************************************************
// mouseMoveEvent()
//******************************************************************************
void MainWindow::mouseMoveEvent(QMouseEvent* event) {
    if (!ui->centralwidget->underMouse() && !ui->lblTitle->underMouse())
        return;

    if( event->buttons().testFlag(Qt::LeftButton) && mMoving) {
        move(pos() + (event->pos() - mLastMousePosition));
    }
}

//******************************************************************************
// mouseReleaseEvent()
//******************************************************************************
void MainWindow::mouseReleaseEvent(QMouseEvent* event) {
    if (!ui->centralwidget->underMouse() && !ui->lblTitle->underMouse())
        return;

    if(event->button() == Qt::LeftButton) {
        mMoving = false;
    }
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
    if (app->appSettings->get("CONFIRM_EXIT").toBool()) {
        QMessageBox::StandardButton rc;
        rc = QMessageBox::question(this, app->appConstants->getQString("APPLICATION_NAME"), QString("Close the factory ?\n"), QMessageBox::Yes|QMessageBox::No);
        if (rc == QMessageBox::Yes) {
            saveSettings();
            if (runningSession == true && project!=NULL) {
                closeProject();
            }
            event->accept();
        } else {
            event->ignore();
        }
    } else {
        event->accept();
    }
}

//******************************************************************************
// saveSettings()
//******************************************************************************
void MainWindow::saveSettings() {
    //**************************************************************************
    // Application state saving
    //**************************************************************************
    QSettings registry(app->appConstants->getQString("ORGANIZATION_NAME"), app->appConstants->getQString("APPLICATION_NAME"));
    registry.setValue("geometry", saveGeometry());
    registry.setValue("windowState", saveState());

    //**************************************************************************
    // MRU Projects saving
    //**************************************************************************
    int jProjects(0);
    int cProjects(0);
    int mProjetcs(app->appSettings->get("MRU_PROJECTS").toInt());
    int eProjects(mruProjects.length());
    qDebug() << eProjects;
    registry.beginWriteArray("Projects");
    for (const QString &project : mruProjects) {
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
    QSettings registry(app->appConstants->getQString("ORGANIZATION_NAME"), app->appConstants->getQString("APPLICATION_NAME"));

    const QByteArray geometry = registry.value("geometry", QByteArray()).toByteArray();
    if (geometry.isEmpty()) {
        const QRect availableGeometry = QGuiApplication::primaryScreen()->availableGeometry();
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

    //**************************************************************************
    // Restoring last opening project and open it
    //**************************************************************************
    const QString lastProject = registry.value("lastProject", QString()).toString();
    if (!lastProject.isEmpty()) {
        openProject(lastProject);
        this->helpFiles = project->getHelpFiles();
        setHelpFilesInToolbar(this->helpFiles);
    } else {
        closeProject();
    }
}

//******************************************************************************
// setHelpFilesInToolbar()
//******************************************************************************
void MainWindow::setHelpFilesInToolbar(const QList<QStringList> helpFiles) {
    // 1. Safe layout clearing (no double free)
    QLayoutItem *lItem;
    while ((lItem = ui->layToolbar->takeAt(0)) != nullptr) {
        if (QWidget *widget = lItem->widget()) {
            widget->deleteLater(); // Or: delete widget; (without deleting lItem)
        }
        delete lItem; // Safe to delete layout item after widget parent cleanup
    }

    // 2. Add new buttons
    for (const QStringList &item : helpFiles) {
        if (item.size() < 2) continue;

        qDebug() << "Adding help button:" << item[0];
        QPushButton *helpButton = new QPushButton(item[0]);
        helpButton->setMinimumSize(helpButton->sizeHint());

        QString filePath = item[1];
        QObject::connect(helpButton, &QPushButton::clicked, this, [filePath]() {
            QDesktopServices::openUrl(QUrl::fromLocalFile(filePath));
        });

        ui->layToolbar->addWidget(helpButton);
        helpButton->show();
    }

    // 3. Spacer to keep buttons aligned to the left
    ui->layToolbar->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Fixed));
    // Force layout recalculation
    ui->layToolbar->invalidate();
    ui->layToolbar->activate();

    if (QWidget *parent = ui->layToolbar->parentWidget()) {
        parent->updateGeometry();
        parent->adjustSize();
        parent->update();
    }
    qDebug() << "Toolbar item count:" << ui->layToolbar->count();
    if (ui->layToolbar->parentWidget()) {
        qDebug() << "Parent widget visible:" << ui->layToolbar->parentWidget()->isVisible();
        qDebug() << "Parent widget size:" << ui->layToolbar->parentWidget()->size();
    }
}

//******************************************************************************
// slotDoOpenProject()
//******************************************************************************
void MainWindow::slotDoOpenProject() {
    DlgOpenProject *dlg = new DlgOpenProject(app);
    dlg->setModal(true);
    if(dlg->exec() == QDialog::Accepted)
    {
        QString dirProject = dlg->getProjectPath();
        if (!dirProject.isEmpty()) {
            openProject(dlg->getProjectPath());
        }
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
void MainWindow::launchProgram(const QString pgm, const QStringList args) {
    qDebug() << pgm << args;
    QProcess::startDetached(pgm, args);
}

//******************************************************************************
// openProject()
//******************************************************************************
void MainWindow::openProject(QString projectPath) {
    if (runningSession == true && this->project != NULL) {
        closeProject();
    }
    this->projectPath = projectPath;
    if (!mruProjects.contains(projectPath))
    {
        mruProjects.append(projectPath);
    }

    QDir projectDir(projectPath);
    if (projectDir.exists()) {
        this->project = new Project(app, projectPath);
        qDebug() << "PROJECT LANGUAGE : " << this->project->projectLanguage;
        qDebug() << "PROJECT HELP : " << this->project->getHelpFiles();
        if (this->project->getHelpFiles().isEmpty()) {
            Languages *l = new Languages();
            QList<QStringList> h = Project::getHelpFiles(app->appDir, l->guessLanguage(projectPath), "raw");
            QList<QStringList> r;
            foreach(QStringList item, h) {
                item[1] = this->project->setVars(item[1],this->project->projectName);
                r.append({item[0], item[1]});
            }
            this->project->setHelpFiles(r);
        }
        qDebug() << "PROJECT HELP : " << this->project->getHelpFiles();
        setHelpFilesInToolbar(this->project->getHelpFiles());

        model->setRootPath(projectPath);
        ui->tvwProject->setRootIndex(model->index(projectPath));
        ui->toolBox->setCurrentIndex(TAB_EDIT);
        QFileInfo fi(projectPath);
        ui->toolBox->setTabText(TAB_PROJECT, QString("%1").arg(fi.fileName()));
        ui->toolBox->tabBar()->setTabTextColor(0, QColor(app->appSettings->get("COLOR_ENHANCED").toString()));
        ui->btnArchiveProject->setEnabled(true);
        ui->btnCloseProject->setEnabled(true);
        ui->btnPropertiesProject->setEnabled(true);
        ui->txtBuildCommand->setText(this->project->getBuildCommand(app->appDir));

        bool inTerminal = false;
        QString cmd = this->project->getRunCommand(app->appDir, inTerminal);
        ui->txtRunCommand->setText(cmd);
        ui->chkRunTerminal->setChecked(inTerminal);

        project->startSession();
        runningSession = true;

        displayProjectProperties();

        git = new GitAPI(app->appSettings->get("GIT_BINARY_PATH").toString(), ui, project->projectDir);
    } else {
        closeProject();
    }
}

//******************************************************************************
// closeProject()
//******************************************************************************
void MainWindow::closeProject() {
    projectPath = "";
    model->setRootPath(app->appSettings->get("DEFAULT_REPOSITORY").toString());
    ui->tvwProject->setRootIndex(model->index(app->appSettings->get("DEFAULT_REPOSITORY").toString()));
    ui->toolBox->setCurrentIndex(TAB_EDIT);
    ui->toolBox->setTabText(TAB_PROJECT, "Project");

    ui->btnArchiveProject->setEnabled(false);
    ui->btnCloseProject->setEnabled(false);
    ui->btnPropertiesProject->setEnabled(false);

    if (this->project != NULL) {
        this->project->endSession();
        this->project->close();
    }

    runningSession = false;
    // Clear the help files toolbar
    QLayoutItem *lItem;
    while((lItem=ui->layToolbar->takeAt(0))!=NULL)
    {
        delete lItem->widget();
        delete lItem;
    }
    // Clear the Project's properties panel
    // ui->scaProjectProperties->clear();
}

//******************************************************************************
// slotDoEdit()
//******************************************************************************
void MainWindow::slotDoEdit() {
    launchProgram(app->appSettings->get("DEFAULT_EDITOR").toString());
}

//******************************************************************************
// slotDoEditFile()
//******************************************************************************
void MainWindow::slotDoEditFile() {
    QAction *action = qobject_cast<QAction *>(sender());
    QString f = action->property("file").toString();
    launchProgram(app->appSettings->get("DEFAULT_EDITOR").toString(), {f});
}

//******************************************************************************
// slotDoDefaultFile()
//******************************************************************************
void MainWindow::slotDoDefaultFile() {
    QAction *action = qobject_cast<QAction *>(sender());
    QString f = action->property("file").toString();
#ifdef Q_OS_LINUX
    QString url = QString("file://%1").arg(f);
#else
    QString url = QString("file:///%1").arg(f);
#endif
    qDebug() << url;
    QDesktopServices::openUrl(QUrl(QString(url), QUrl::TolerantMode));
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
    launchProgram(app->appSettings->get("DEFAULT_BROWSER").toString(), {f});
}

//******************************************************************************
// slotDoTerm()
//******************************************************************************
void MainWindow::slotDoTerm() {
    launchProgram(app->appSettings->get("DEFAULT_TERMINAL").toString());
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
    launchProgram(app->appSettings->get("DEFAULT_BROWSER").toString());
}

//******************************************************************************
// slotDoSettings()
//******************************************************************************
void MainWindow::slotDoSettings() {
    app->appSettings->form(ui->scaSettings);
    ui->toolBox->setCurrentIndex(TAB_SETTINGS);
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
    bool isDir = model->isDir(idx);
    if (!isDir) {
        launchProgram(app->appSettings->get("DEFAULT_EDITOR").toString(), {model->filePath(idx)});
    }
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
    rc = QMessageBox::question(this, app->appConstants->getQString("APPLICATION_NAME"), QString("Delete this folder ?\n\n%1\n").arg(f), QMessageBox::Yes|QMessageBox::No);
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
    rc = QMessageBox::question(this, app->appConstants->getQString("APPLICATION_NAME"), QString("Delete this file ?\n\n%1\n").arg(f), QMessageBox::Yes|QMessageBox::No);
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
    launchProgram(app->appSettings->get("DEFAULT_EDITOR").toString(), {app->appDir + QDir::separator() + app->appConstants->getQString("TEMPLATES_FILE")});
}

//******************************************************************************
// slotDoTemplatesReset()
//******************************************************************************
void MainWindow::slotDoTemplatesReset() {
    QMessageBox::StandardButton rc;
    rc = QMessageBox::question(this, app->appConstants->getQString("APPLICATION_NAME"), QString("Resetting the default templates file ?\n"), QMessageBox::Yes|QMessageBox::No);
    if (rc == QMessageBox::Yes) {
        QString localFile = app->appDir + QDir::separator() + app->appConstants->getQString("TEMPLATES_FILE");
        QString backupFile   = localFile + ".orig";
        if (QFile::exists(backupFile)) {
            // a local backup file exists
            // remove the local file before
            if (QFile::exists(localFile)) {
                QFile::remove(localFile);
            }
            // duplicating the original templates file
            if (QFile::copy(backupFile, localFile)) {
                qDebug() << "templates file restored";
            } else {
                qDebug() << "templates file NOT restored";
            }
        } else {
            // there is no local backup file
            QString distantFile = app->appConstants->getQString("WEB_REPOSITORY") + app->appConstants->getQString("TEMPLATES_FILE");
            // remove the local file before
            if (QFile::exists(localFile)) {
                QFile::remove(localFile);
            }
            Downloader::downloadFile(distantFile, localFile);
            // Duplicate it
            if (QFile::copy(localFile, backupFile)) {
                qDebug() << "templates file duplicated";
            } else {
                qDebug() << "templates file NOT duplicated";
            }
        }
    }
}

//******************************************************************************
// slotDoPin()
//******************************************************************************
void MainWindow::slotDoPin()
{
    if (IsVisible == true) {
        saveSettings();

        // QScreen *myScreen = QGuiApplication::primaryScreen();
        // QRect screenGeometry = myScreen->availableGeometry();

        // QScreen *pScreen = QGuiApplication::screenAt(this->mapToGlobal({this->width()/2,0}));
        QScreen *pScreen = getActiveScreen(this);
        QRect availableScreenSize = pScreen->availableGeometry();

        // QDesktopWidget widget;
        // QRect screenGeometry = widget.screenGeometry();//.availableGeometry(widget.primaryScreen());
        int height = this->pos().y() + ui->btnPin->pos().y();
        /*
        bool topBar = appSettings->get("TITLE_BAR_POSITION_TOP").toBool();
        if (topBar == false) {
            height += this->height();
        }
        */

        qDebug("Screen Width = %d", availableScreenSize.width());
        qDebug("Screen Left  = %d", availableScreenSize.left());
        int width = availableScreenSize.width() + availableScreenSize.left() - ui->btnPin->width()
                    - 2;
        qDebug("POS          = %d", width);
        // shrink the window as the titlebar only
        this->layout()->setSizeConstraint(QLayout::SetFixedSize);
        // hide unnecessary controls in the titlebar
        IsVisible=false;
        this->ui->lblTitle->setVisible(false);
        this->ui->horizontalSpacer_3->changeSize(0, 0, QSizePolicy::Fixed, QSizePolicy::Fixed);
        this->ui->horizontalSpacer_3->invalidate();
        this->ui->btnExit->setVisible(false);
        this->ui->toolBox->setVisible(false);
        this->ui->btnBrowser->setVisible(false);
        this->ui->btnEdit->setVisible(false);
        this->ui->btnTerm->setVisible(false);
        this->ui->btnBuild->setVisible(false);
        this->ui->horizontalSpacer->changeSize(0, 0, QSizePolicy::Fixed, QSizePolicy::Fixed);
        this->ui->horizontalSpacer->invalidate();
        this->ui->btnSettings->setVisible(false);
        this->ui->statusbar->setVisible(false);
        for (int i = 0; i < ui->layToolbar->count(); ++i) {
            QLayoutItem *item = ui->layToolbar->itemAt(i);
            if (item && item->widget()) {
                item->widget()->setVisible(false);
            }
        }

        // move the remaining button against the edge of the screen
        move(width, height);

    } else {
        this->layout()->setSizeConstraint(QLayout::SetMinAndMaxSize);
        this->setVisible(true);
        IsVisible=true;

        this->ui->lblTitle->setVisible(true);
        this->ui->horizontalSpacer_3->changeSize(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
        this->ui->horizontalSpacer_3->invalidate();
        this->ui->btnExit->setVisible(true);
        this->ui->toolBox->setVisible(true);
        this->ui->btnBrowser->setVisible(true);
        this->ui->btnEdit->setVisible(true);
        this->ui->btnTerm->setVisible(true);
        this->ui->btnBuild->setVisible(true);
        this->ui->horizontalSpacer->changeSize(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
        this->ui->horizontalSpacer->invalidate();
        this->ui->btnSettings->setVisible(true);
        this->ui->statusbar->setVisible(true);
        for (int i = 0; i < ui->layToolbar->count(); ++i) {
            QLayoutItem *item = ui->layToolbar->itemAt(i);
            if (item && item->widget()) {
                item->widget()->setVisible(true);
            }
        }
        readSettings();
    }
}

//******************************************************************************
// getActiveScreen()
//******************************************************************************
QScreen *MainWindow::getActiveScreen(
    QWidget *pWidget) const
{
    QScreen *pActive = nullptr;
    while (pWidget) {
        auto w = pWidget->windowHandle();
        if (w != nullptr) {
            pActive = w->screen();
            break;
        } else
            pWidget = pWidget->parentWidget();
    }
    return pActive;
}

//******************************************************************************
// displayProjectProperties()
//******************************************************************************
void MainWindow::displayProjectProperties() {
    QMap<QString, QString> props = this->project->getProperties();

    qDebug() << "Displaying project properties for " << props["Name"];

    // Identify target container widget (handling QScrollArea vs QWidget)
    QWidget *targetWidget = ui->scaProjectProperties;
    if (auto *scrollArea = qobject_cast<QScrollArea*>(ui->scaProjectProperties)) {
        if (!scrollArea->widget()) {
            scrollArea->setWidget(new QWidget());
            scrollArea->setWidgetResizable(true);
        }
        targetWidget = scrollArea->widget();
    }

    // Get existing layout or create a new one (DO NOT call targetWidget->setLayout(vbox) afterwards)
    QVBoxLayout *vbox = qobject_cast<QVBoxLayout*>(targetWidget->layout());
    if (!vbox) {
        vbox = new QVBoxLayout(targetWidget); // Automatically sets layout on targetWidget
    } else {
        // Clear previous widgets and spacers safely
        QLayoutItem *item;
        while ((item = vbox->takeAt(0)) != nullptr) {
            if (QWidget *w = item->widget()) {
                w->deleteLater();
            } else {
                delete item;
            }
        }
    }

    // Populate layout
    auto addProperty = [vbox](const QString &label, const QString &value) {
        QLabel *lbl = new QLabel(label);
        lbl->setStyleSheet("font-weight: bold;");
        vbox->addWidget(lbl);

        QLineEdit *txt = new QLineEdit(value);
        txt->setReadOnly(true);
        vbox->addWidget(txt);
    };

    GitTimeEstimator estimator;
    qint64 totalSeconds = estimator.calculateElapsedTime(this->project->projectDir);
    if (totalSeconds<=0) {
        totalSeconds=props["Elapsed"].toInt();
    }

    addProperty("🢒 Project :", props["Name"]);
    addProperty("🢒 Created :", Utils::tsToString(props["Created"], app->appSettings->get("DATETIME_FORMAT").toString()));
    addProperty("🢒 Modified :", Utils::tsToString(props["Modified"], app->appSettings->get("DATETIME_FORMAT").toString()));
    addProperty("🢒 Language :", props["Language"]);
    addProperty("🢒 Estimated Elapsed Time :", Utils::secondsToString(totalSeconds));
    addProperty("🢒 Size :", Utils::formatSize(props["Size"].toInt()) + " (" + props["Size"] + " bytes)");

    vbox->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Fixed, QSizePolicy::Expanding));
}

//******************************************************************************
// clearProjectProperties()
//******************************************************************************
void MainWindow::clearProjectProperties() {
    QVBoxLayout *vbox = new QVBoxLayout(ui->scaProjectProperties);
    QLabel *lbl01 = new QLabel("🢒 Project :");
    lbl01->setStyleSheet("font-weight: bold;");
    vbox->addWidget(lbl01);
    QLineEdit *txt01 = new QLineEdit("*NONE");
    txt01->setReadOnly(true);
    vbox->addWidget(txt01);

    QLabel *lbl02 = new QLabel("🢒 Created :");
    lbl02->setStyleSheet("font-weight: bold;");
    vbox->addWidget(lbl02);
    QLineEdit *txt02 = new QLineEdit("*NONE");
    txt02->setReadOnly(true);
    vbox->addWidget(txt02);

    QLabel *lbl03 = new QLabel("🢒 Modified :");
    lbl03->setStyleSheet("font-weight: bold;");
    vbox->addWidget(lbl03);
    QLineEdit *txt03 = new QLineEdit("*NONE");
    txt03->setReadOnly(true);
    vbox->addWidget(txt03);

    QLabel *lbl04 = new QLabel("🢒 Language :");
    lbl04->setStyleSheet("font-weight: bold;");
    vbox->addWidget(lbl04);
    QLineEdit *txt04 = new QLineEdit("*NONE");
    txt04->setReadOnly(true);
    vbox->addWidget(txt04);

    QLabel *lbl05 = new QLabel("🢒 Elapsed :");
    lbl05->setStyleSheet("font-weight: bold;");
    vbox->addWidget(lbl05);
    QLineEdit *txt05 = new QLineEdit("*NONE");
    txt05->setReadOnly(true);
    vbox->addWidget(txt05);

    QLabel *lbl06 = new QLabel("🢒 Size :");
    lbl06->setStyleSheet("font-weight: bold;");
    vbox->addWidget(lbl06);
    QLineEdit *txt06 = new QLineEdit("*NONE");
    txt06->setReadOnly(true);
    vbox->addWidget(txt06);

    vbox->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Fixed));
    ui->scaProjectProperties->setLayout(vbox);
}

//******************************************************************************
// on_btnDoBuild_clicked()
//******************************************************************************
void MainWindow::on_btnDoBuild_clicked() {
    QString commandStr = ui->txtBuildCommand->text().trimmed();

    if (commandStr.isEmpty()) {
        ui->txtBuildOutput->append("<b>[Error] No build command specified.</b>");
        return;
    }

    ui->txtBuildOutput->clear();
    ui->txtBuildOutput->append(QString("<b>> Executing: %1</b>\n").arg(commandStr));

    if (buildProcess) {
        buildProcess->kill();
        buildProcess->deleteLater();
        buildProcess = nullptr;
    }

    buildProcess = new QProcess(this);

    // Set the working directory to the project's folder location
    buildProcess->setWorkingDirectory(this->project->projectDir);
    buildProcess->setProcessChannelMode(QProcess::MergedChannels);

    connect(buildProcess, &QProcess::readyReadStandardOutput, this, &MainWindow::readBuildOutput);
    connect(buildProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &MainWindow::buildFinished);

    ui->btnDoBuild->setEnabled(false);

#if defined(Q_OS_WIN)
    buildProcess->start("cmd.exe", QStringList() << "/c" << commandStr);
#else
    buildProcess->start("/bin/sh", QStringList() << "-c" << commandStr);
#endif
}

//******************************************************************************
// readBuildOutput()
//******************************************************************************
void MainWindow::readBuildOutput() {
    if (buildProcess) {
        // Read available output and append it to the QTextEdit
        QByteArray output = buildProcess->readAllStandardOutput();
        ui->txtBuildOutput->append(QString::fromUtf8(output));
    }
}

//******************************************************************************
// buildFinished()
//******************************************************************************
void MainWindow::buildFinished(int exitCode, QProcess::ExitStatus exitStatus) {
    Q_UNUSED(exitStatus);

    if (exitCode == 0) {
        ui->txtBuildOutput->append("\n<b>=== BUILD SUCCESSFUL ===</b>");
    } else {
        ui->txtBuildOutput->append(QString("\n<b style='color:red;'>=== BUILD FAILED (Exit code: %1) ===</b>").arg(exitCode));
    }

    // Re-enable the build button
    ui->btnDoBuild->setEnabled(true);
}

//******************************************************************************
// on_btnDoRun_clicked()
//******************************************************************************
void MainWindow::on_btnDoRun_clicked() {
    QString commandStr = ui->txtRunCommand->text().trimmed();

    if (commandStr.isEmpty()) {
        ui->txtRunOutput->append("<b>[Error] No run command specified.</b>");
        return;
    }

    ui->txtRunOutput->clear();
    ui->txtRunOutput->append(QString("<b>> Executing: %1</b>\n").arg(commandStr));

    // Abort previous run instance if active
    if (runProcess) {
        runProcess->kill();
        runProcess->deleteLater();
        runProcess = nullptr;
    }

    runProcess = new QProcess(this);

    // Set working directory to the project folder
    runProcess->setWorkingDirectory(this->project->projectDir);

    // Merge standard output and error channels
    runProcess->setProcessChannelMode(QProcess::MergedChannels);

    // Connect signals for asynchronous output and completion
    connect(runProcess, &QProcess::readyReadStandardOutput, this, &MainWindow::readRunOutput);
    connect(runProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &MainWindow::runFinished);

    ui->btnDoRun->setEnabled(false);
    ui->btnStopRun->setEnabled(true);
    bool runInTerminal = ui->chkRunTerminal->isChecked();

#if defined(Q_OS_WIN)
    if (runInTerminal) {
        ui->txtRunOutput->append(QString("<b>> Executing in external console: %1</b>\n").arg(commandStr));
        // 'start' opens a separate Windows Command Prompt window
        runProcess->start("cmd.exe", QStringList() << "/c" << "start" << "cmd.exe" << "/k" << commandStr);
    } else {
        txtRunOutput->append(QString("<b>> Executing: %1</b>\n").arg(commandStr));
        runProcess->start("cmd.exe", QStringList() << "/c" << commandStr);
    }
#else
    if (runInTerminal) {
        ui->txtRunOutput->append(QString("<b>> Launching in external terminal: %1</b>\n").arg(commandStr));

        // Check popular Linux terminal emulators in order of common availability
        QString terminalApp;
        if (QStandardPaths::findExecutable("x-terminal-emulator").length() > 0) {
            terminalApp = "x-terminal-emulator";
        } else if (QStandardPaths::findExecutable("gnome-terminal").length() > 0) {
            terminalApp = "gnome-terminal";
        } else if (QStandardPaths::findExecutable("konsole").length() > 0) {
            terminalApp = "konsole";
        } else if (QStandardPaths::findExecutable("xfce4-terminal").length() > 0) {
            terminalApp = "xfce4-terminal";
        } else {
            terminalApp = "xterm";
        }

        if (terminalApp == "gnome-terminal") {
            runProcess->start("gnome-terminal", QStringList() << "--" << "bash" << "-c" << commandStr + "; exec bash");
        } else if (terminalApp == "konsole") {
            runProcess->start("konsole", QStringList() << "-e" << "bash" << "-c" << commandStr + "; exec bash");
        } else {
            // Generic x-terminal-emulator / xfce4-terminal / xterm syntax (-e)
            runProcess->start(terminalApp, QStringList() << "-e" << QString("bash -c '%1; exec bash'").arg(commandStr));
        }
    } else {
        ui->txtRunOutput->append(QString("<b>> Executing: %1</b>\n").arg(commandStr));
        runProcess->start("/bin/sh", QStringList() << "-c" << commandStr);
    }
#endif
}

//******************************************************************************
// on_btnStopRun_clicked()
//******************************************************************************
void MainWindow::on_btnStopRun_clicked() {
    if (runProcess && runProcess->state() != QProcess::NotRunning) {
        ui->txtRunOutput->append("\n<b style='color:orange;'>=== ABORTING PROCESS... ===</b>");

        // Try soft termination first (SIGTERM on Linux / WM_CLOSE on Windows)
        runProcess->terminate();

        // If process doesn't close within 1 second, force kill it (SIGKILL)
        if (!runProcess->waitForFinished(1000)) {
            runProcess->kill();
        }
    }
}

//******************************************************************************
// readRunOutput()
//******************************************************************************
void MainWindow::readRunOutput() {
    if (runProcess) {
        QByteArray output = runProcess->readAllStandardOutput();
        ui->txtRunOutput->append(QString::fromUtf8(output));
    }
}

//******************************************************************************
// runFinished()
//******************************************************************************
void MainWindow::runFinished(int exitCode, QProcess::ExitStatus exitStatus) {
    Q_UNUSED(exitStatus);

    if (exitCode == 0) {
        ui->txtRunOutput->append("\n<b>=== PROCESS EXITED NORMAL (Code: 0) ===</b>");
    } else {
        ui->txtRunOutput->append(QString("\n<b style='color:red;'>=== PROCESS EXITED WITH ERROR (Code: %1) ===</b>").arg(exitCode));
    }

    ui->btnDoRun->setEnabled(true);
    ui->btnStopRun->setEnabled(false);
}

//******************************************************************************
// on_btnDoSaveBuild_clicked()
//******************************************************************************
void MainWindow::on_btnDoSaveBuild_clicked() {
    if (!project || project->projectFile.isEmpty()) {
        ui->txtBuildOutput->append("<b style='color:red;'>[Error] No active project loaded.</b>");
        return;
    }

    QFile file(project->projectFile);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return;

    QDomDocument doc;
    if (!doc.setContent(&file)) {
        file.close();
        return;
    }
    file.close();

    QDomElement root = doc.documentElement();
    QDomElement buildNode = root.firstChildElement("build");
    if (buildNode.isNull()) {
        buildNode = doc.createElement("build");
        root.appendChild(buildNode);
    }

    // Set the text using a QDomText child
    setElementText(doc, buildNode, ui->txtBuildCommand->text().trimmed());

    if (file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        QTextStream stream(&file);
        doc.save(stream, 4);
        file.close();
        ui->txtBuildOutput->append("<b>[Info] Build command saved successfully.</b>");
    }
}

//******************************************************************************
// on_btnDoSaveRun_clicked()
//******************************************************************************
void MainWindow::on_btnDoSaveRun_clicked() {
    if (!project || project->projectFile.isEmpty()) {
        ui->txtRunOutput->append("<b style='color:red;'>[Error] No active project loaded.</b>");
        return;
    }

    QFile file(project->projectFile);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return;

    QDomDocument doc;
    if (!doc.setContent(&file)) {
        file.close();
        return;
    }
    file.close();

    QDomElement root = doc.documentElement();
    QDomElement runNode = root.firstChildElement("run");
    if (runNode.isNull()) {
        runNode = doc.createElement("run");
        root.appendChild(runNode);
    }

    // Set text node and attribute
    setElementText(doc, runNode, ui->txtRunCommand->text().trimmed());
    runNode.setAttribute("inTerminal", ui->chkRunTerminal->isChecked() ? "true" : "false");

    if (file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        QTextStream stream(&file);
        doc.save(stream, 4);
        file.close();
        ui->txtRunOutput->append("<b>[Info] Run command and terminal option saved successfully.</b>");
    }
}

//******************************************************************************
// setElementText()
// Helper lambda or inline function to easily update element text in QDomDocument
//******************************************************************************
void setElementText(QDomDocument &doc, QDomElement &element, const QString &text) {
    // Remove existing child text nodes if any
    while (!element.firstChild().isNull()) {
        element.removeChild(element.firstChild());
    }
    // Append the new text node
    element.appendChild(doc.createTextNode(text));
}