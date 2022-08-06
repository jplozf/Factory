#include "dialogs.h"
#include "qfiledialog.h"

//******************************************************************************
// DlgProperties()
//******************************************************************************
DlgProperties::DlgProperties(QMap<QString, QString> props, QString title, QDialog *parent) : QDialog{parent}
{
    this->setWindowTitle(title);
    QFormLayout *layout = new QFormLayout();
    this->setLayout(layout);
    QMap<QString, QString>::iterator iter;
    for (iter = props.begin(); iter != props.end(); ++iter) {
        QLabel *lblKey = new QLabel(iter.key());
        QLineEdit *txtValue = new QLineEdit(iter.value());
        txtValue->setReadOnly(true);
        const QFont fixedFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
        txtValue->setFont(fixedFont);
        layout->addRow(lblKey, txtValue);
    }
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    layout->addWidget(buttonBox);
}

//******************************************************************************
// DlgOpenProject()
//******************************************************************************
DlgOpenProject::DlgOpenProject(App *app, QDialog *parent) : QDialog{parent}
{
    this->app = app;
    this->repository = app->appSettings->get("DEFAULT_REPOSITORY").toString();
    this->setWindowTitle("Project");
    tbwProject = new QTabWidget();

    // NEW PROJECT TAB =========================================================
    QWidget *tabNewProject = new QWidget();
    QFormLayout *layNewProject = new QFormLayout();
    // NAME
    QLabel *lblName = new QLabel("Name");
    QHBoxLayout *layoutName = new QHBoxLayout();
    txtProjectName = new QLineEdit();
    QPushButton *btnName = new QPushButton("...");
    connect(btnName, SIGNAL(clicked()), this, SLOT(slotDoRandomName()));
    connect(txtProjectName, SIGNAL(textChanged(QString)), this, SLOT(slotNameChanged(QString)));
    layoutName->addWidget(txtProjectName);
    layoutName->addWidget(btnName);
    layNewProject->addRow(lblName, layoutName);
    // LANGUAGE
    QLabel *lblLanguage = new QLabel("Language");
    cbxLanguage = new QComboBox();
    QList<QString> languages = Project::getLanguages(app->appDir);
    foreach(QString item, languages) {
        cbxLanguage->addItem(item, item);
    }
    connect(cbxLanguage, SIGNAL(activated(int)), this, SLOT(slotSelectLanguage(int)));
    layNewProject->addRow(lblLanguage, cbxLanguage);
    // TOOLKIT
    QLabel *lblToolkit = new QLabel("Toolkit");
    cbxToolkit = new QComboBox();
    connect(cbxToolkit, SIGNAL(activated(int)), this, SLOT(slotSelectToolkit(int)));
    layNewProject->addRow(lblToolkit, cbxToolkit);
    // LICENSE
    QLabel *lblLicenses = new QLabel("License");
    cbxLicenses = new QComboBox();
    QDirIterator it(app->appDir + QDir::separator() + "licenses/", QDir::AllEntries | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        QFileInfo fi(it.next());
        cbxLicenses->addItem(fi.fileName(),fi.absoluteFilePath());
    }
    connect(cbxLicenses, SIGNAL(activated(int)), this, SLOT(slotSelectLicense(int)));
    layNewProject->addRow(lblLicenses, cbxLicenses);
    // SUMMARY
    QLabel *lblSummary = new QLabel("Summary");
    lstSummary = new QListWidget();
    layNewProject->addRow(lblSummary, lstSummary);
    // BUTTON BOX
    QDialogButtonBox *buttonBox1 = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttonBox1, &QDialogButtonBox::accepted, this, &DlgOpenProject::slotAccept);
    connect(buttonBox1, &QDialogButtonBox::rejected, this, &QDialog::reject);
    layNewProject->addWidget(buttonBox1);
    // DISPOSE IN TAB
    tabNewProject->setLayout(layNewProject);
    tbwProject->addTab(tabNewProject, "New Project");

    // OPEN PROJECT TAB ========================================================
    QWidget *tabOpenProject = new QWidget();
    QVBoxLayout *layOpenProject = new QVBoxLayout();
    // LABEL + BROWSE BUTTON
    QLabel *lblInstructions = new QLabel("Please browse to the folder containing your project");
    layOpenProject->addWidget(lblInstructions);
    lblOpenProject = new QLabel("Open Project Folder...");
    // lblOpenProject->setFrameStyle(QFrame::Box);
    QPushButton *btnOpenProject = new QPushButton("...");
    QHBoxLayout *layRowProject = new QHBoxLayout();
    layRowProject->addWidget(lblOpenProject, 1, Qt::AlignLeft);
    layRowProject->addWidget(btnOpenProject);
    layOpenProject->addLayout(layRowProject);
    layOpenProject->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Fixed, QSizePolicy::Expanding));
    // BUTTON BOX
    QDialogButtonBox *buttonBox2 = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttonBox2, &QDialogButtonBox::accepted, this, &DlgOpenProject::slotAccept);
    connect(buttonBox2, &QDialogButtonBox::rejected, this, &QDialog::reject);
    QFileDialog *qfd = new QFileDialog();   // The lambda in the following line will need it
    connect(btnOpenProject, &QPushButton::clicked, this, [qfd, this](){
        this->lblOpenProject->setText(qfd->getExistingDirectory(this, tr("Open Project Folder"),
        QDir::homePath(),
        qfd->ShowDirsOnly | qfd->DontResolveSymlinks));
    });
    layOpenProject->addWidget(buttonBox2);
    // DISPOSE IN TAB
    tabOpenProject->setLayout(layOpenProject);
    tbwProject->addTab(tabOpenProject, "Open Project");

    // RECENT PROJECT TAB ======================================================
    QWidget *tabRecentProject = new QWidget();
    QFormLayout *layRecentProject = new QFormLayout();
    QVectorIterator<QString> iProjects(app->mruProjects);
    QListWidget *lstProjects = new QListWidget();
    while (iProjects.hasNext()) {
        QString project = iProjects.next();
        lstProjects->addItem(project);
    }
    layRecentProject->addRow(lstProjects);
    // LABEL
    lblRecentProject = new QLabel();
    layRecentProject->addRow(lblRecentProject);
    // BUTTON BOX
    QDialogButtonBox *buttonBox3 = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttonBox3, &QDialogButtonBox::accepted, this, &DlgOpenProject::slotAccept);
    connect(buttonBox3, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(lstProjects, &QListWidget::currentTextChanged, this, [this](QString txt){this->lblRecentProject->setText(txt);});
    layRecentProject->addWidget(buttonBox3);
    // DISPOSE IN TAB
    tabRecentProject->setLayout(layRecentProject);
    tbwProject->addTab(tabRecentProject, "Recents Projects");

    // DISPOSE IN DIALOG
    QGridLayout *layMain = new QGridLayout();
    this->setLayout(layMain);
    layMain->addWidget(tbwProject);
}

//******************************************************************************
// slotNameChanged()
//******************************************************************************
void DlgOpenProject::slotNameChanged(QString txt) {
    buildSummary();
}

//******************************************************************************
// slotSelectLanguage()
//******************************************************************************
void DlgOpenProject::slotSelectLanguage(int item) {
    QList<QString> toolkits = Project::getToolkits(app->appDir, cbxLanguage->itemText(item));
    cbxToolkit->clear();
    foreach(QString item, toolkits) {
        cbxToolkit->addItem(item, item);
    }
    buildSummary();
}

//******************************************************************************
// slotSelectToolkit()
//******************************************************************************
void DlgOpenProject::slotSelectToolkit(int item) {
    buildSummary();
}

//******************************************************************************
// slotSelectLicense()
//******************************************************************************
void DlgOpenProject::slotSelectLicense(int item) {
    buildSummary();
}

//******************************************************************************
// slotAccept()
//******************************************************************************
void DlgOpenProject::slotAccept() {
    switch(tbwProject->currentIndex()) {
        case 0:         // TAB NEW PROJECT
        createFiles();
        projectPath = Utils::pathAppend(repository, Utils::pathAppend(cbxLanguage->currentText(), txtProjectName->text()));
        break;

        case 1:         // TAB OPEN PROJECT
        projectPath = lblOpenProject->text();
        break;

        case 2:         // TAB RECENT PROJECTS
        projectPath = lblRecentProject->text();
        break;
    }
    QDialog::accept();
}

//******************************************************************************
// createFiles()
//******************************************************************************
void DlgOpenProject::createFiles() {
    foreach(QStringList item, filesToCreate) {
        QString newFileName = item[0];
        QString source = item[1];

        QFileInfo fi(newFileName);
        QString path = fi.absolutePath();
        QDir dir(path);
        if (!dir.exists())
            dir.mkpath(".");

        QFile::copy(source, newFileName);
        QFile(newFileName).setPermissions(QFileDevice::ReadOwner | QFileDevice::WriteOwner | QFileDevice::ReadGroup | QFileDevice::WriteGroup | QFileDevice::ReadOther);
        QFile(newFileName).close();
        replaceVarsInFile(newFileName);
    }
}

//******************************************************************************
// buildSummary()
//******************************************************************************
void DlgOpenProject::buildSummary() {
    filesToCreate.clear();
    files = Project::getFiles(app->appDir, cbxLanguage->currentText(), cbxToolkit->currentText());
    lstSummary->clear();
    foreach(QStringList item, files) {
        lstSummary->addItem(appendFileToList(item[0], item[1]));
    }
    lstSummary->addItem(appendFileToList(cbxLicenses->currentText(), cbxLicenses->currentData().toString()));
    lstSummary->addItem(appendFileToList("%PROJECT%" + app->appConstants->getQString("PROJECT_FILE_EXTENSION"), ":/templates/PROJECT.frx"));
}

//******************************************************************************
// appendFileToList()
//******************************************************************************
QString DlgOpenProject::appendFileToList(QString filename, QString source) {
    filename = renameFileWithVars(filename);
    QString newFileName = Utils::pathAppend(repository, Utils::pathAppend(cbxLanguage->currentText(), Utils::pathAppend(txtProjectName->text(), filename)));
    filesToCreate.append({newFileName, source});
    return(newFileName);
}

//******************************************************************************
// getProjectPath()
//******************************************************************************
QString DlgOpenProject::getProjectPath() {
    // QString projectPath = Utils::pathAppend(repository, Utils::pathAppend(cbxLanguage->currentText(), txtProjectName->text()));
    return projectPath;
}

//******************************************************************************
// getHelpFiles()
//******************************************************************************
QList<QStringList> DlgOpenProject::getHelpFiles() {
    QList<QStringList> h = Project::getHelpFiles(app->appDir, cbxLanguage->currentText(), cbxToolkit->currentText());
    QList<QStringList> r;
    foreach(QStringList item, h) {
        item[1] = setVars(item[1]);
        r.append({item[0], item[1]});
    }
    return r;
}

//******************************************************************************
// renameFileWithVars()
//******************************************************************************
QString DlgOpenProject::renameFileWithVars(QString filename) {
    filename = setVars(filename);
    return(filename);
}

//******************************************************************************
// replaceVarsInFile()
//******************************************************************************
void DlgOpenProject::replaceVarsInFile(QString filename) {
    QByteArray fileData;
    QFile file(filename);
    file.open(QIODevice::ReadWrite);
    fileData = file.readAll();
    QString text(fileData);

    text = setVars(text);

    file.seek(0);
    file.write(text.toUtf8());
    file.close();
}

//******************************************************************************
// setVars()
//******************************************************************************
QString DlgOpenProject::setVars(QString text) {
    text.replace("%PROJECT%", txtProjectName->text());
    text.replace("%LANGUAGE%", cbxLanguage->currentText());
    text.replace("%TOOLKIT%", cbxToolkit->currentText());
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

//******************************************************************************
// slotDoRandomName()
//******************************************************************************
void DlgOpenProject::slotDoRandomName() {
    txtProjectName->setText(Project::randomName());
}
