#include "gitapi.h"

//******************************************************************************
// Class GitAPI()
//******************************************************************************
GitAPI::GitAPI(QString gitBin, Ui::MainWindow *ui) {
    this->gitBin = gitBin;
    this->ui = ui;
    this->commands.append(QStringList{"version", "--version"});
    this->commands.append(QStringList{"status", "status"});
    this->commands.append(QStringList{"clone", "clone", "%REMOTE_GIT%"});
    this->commands.append(QStringList{"init", "init"});
    this->commands.append(QStringList{"add <FILES or DIRECTORIES>", "add", "%FILES_DIRECTORIES%"});
    this->commands.append(QStringList{"add .", "add", "."});
    this->commands.append(QStringList{"commit", "commit", "-m", "%TEXT%"});

    foreach(QStringList cmd, this->commands) {
        ui->cbxGitCommands->addItem(cmd[0], QVariant(cmd));
    }

    connect(ui->btnGitOutputClear, SIGNAL(clicked()), this, SLOT(slotDoClearOutput()));
    connect(ui->cbxGitCommands, SIGNAL(currentIndexChanged(int)), this, SLOT(slotDoChangeCommandIndex(int)));
    connect(ui->btnGitXeq, SIGNAL(clicked()), this, SLOT(slotDoXeq()));

    slotDoChangeCommandIndex(0);
}

//******************************************************************************
// Class GitAPI()
//******************************************************************************
GitAPI::GitAPI(QString gitBin, Ui::MainWindow *ui, QString rootFolder) : GitAPI(gitBin, ui) {
    this->rootFolder = rootFolder;
}

//******************************************************************************
// setRootFolder()
//******************************************************************************
void GitAPI::setRootFolder(QString rootFolder) {
    this->rootFolder = rootFolder;
}

//******************************************************************************
// getRootFolder()
//******************************************************************************
QString GitAPI::getRootFolder() {
    return this->rootFolder;
}

//******************************************************************************
// xeq()
//******************************************************************************
GitAPI::xeqResult GitAPI::xeq(QStringList params) {
    xeqResult xr;
    QProcess p;
    if (this->gitBin != NULL && QFile::exists(this->gitBin)) {
        p.setWorkingDirectory(this->rootFolder);
        p.start(this->gitBin, params);
        p.waitForFinished();

        xr.rc = p.exitCode();
        xr.out = p.readAllStandardOutput();
        xr.err = p.readAllStandardError();
    } else {
        qDebug() << "Git binary not set";
        xr.rc = GitAPI::ERR_GIT_BINARY_NOT_SET;
        xr.out = "";
        xr.err = "GitAPI::Git binary not set";
    }
    ui->lblGitRC->setText("RC=" + QString::number(xr.rc));
    ui->lblGitRC->setToolTip(xr.err);
    return xr;
}

//******************************************************************************
// slotDoClearOutput()
//******************************************************************************
void GitAPI::slotDoClearOutput() {
    this->ui->txtGitOutput->setText("");
}

//******************************************************************************
// slotDoChangeCommandIndex()
//******************************************************************************
void GitAPI::slotDoChangeCommandIndex(int i) {
    qDebug() << ui->cbxGitCommands->itemData(i);
    QStringList d = ui->cbxGitCommands->itemData(i).toStringList();
    this->command.clear();
    ui->cbxGitParams->setEnabled(false);
    int j = 0;
    foreach(QString p, d) {
        if (j > 0) {
            this->command.append(p);
            if (p[0] == "%") {
                ui->cbxGitParams->setEnabled(true);
            }
        }
        j++;
        qDebug() << p;
    }
}

//******************************************************************************
// slotDoXeq()
//******************************************************************************
void GitAPI::slotDoXeq() {
    /*
    git = new GitAPI(app->appSettings->get("GIT_BINARY_PATH").toString(), ui, project->projectDir);
    GitAPI::xeqResult gxr = git->xeq(QStringList("status"));
    ui->txtGitOutput->setText(gxr.out);
    ui->toolBox->setCurrentIndex(TAB_GIT);
    qDebug() << gxr.out;
    qDebug() << gxr.err;
    qDebug() << gxr.rc;
    */
    // TODO : check in command if %KEYWORD% and replace it with cbxGitParams.text
    foreach(QString p, this->command) {
        if (p[0] == "%") {

        }
    }

    GitAPI::xeqResult gxr = xeq(this->command);
    ui->txtGitOutput->append("> git " + this->command.join(" "));
    ui->txtGitOutput->append(gxr.out);
}

