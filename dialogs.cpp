#include "dialogs.h"

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
// DlgNewProject()
//******************************************************************************
DlgNewProject::DlgNewProject(Settings *appSettings, Constants *appConstants, QDialog *parent) : QDialog{parent}
{
    this->appSettings = appSettings;
    this->appConstants = appConstants;
    this->appDir = appDir = QDir(QDir::homePath()).filePath(appConstants->getQString("APP_FOLDER"));
    this->repository = appSettings->get("DEFAULT_REPOSITORY").toString();
    this->setWindowTitle("New Project");
    QFormLayout *layout = new QFormLayout();
    this->setLayout(layout);

    // NAME
    QLabel *lblName = new QLabel("Name");
    QHBoxLayout *layoutName = new QHBoxLayout();
    txtProjectName = new QLineEdit();
    QPushButton *btnName = new QPushButton("...");
    connect(btnName, SIGNAL(clicked()), this, SLOT(slotDoRandomName()));
    connect(txtProjectName, SIGNAL(textChanged(QString)), this, SLOT(slotNameChanged(QString)));
    layoutName->addWidget(txtProjectName);
    layoutName->addWidget(btnName);
    layout->addRow(lblName, layoutName);

    // LANGUAGE
    QLabel *lblLanguage = new QLabel("Language");
    cbxLanguage = new QComboBox();
    QList<QString> languages = Project::getLanguages(appDir);
    foreach(QString item, languages) {
        cbxLanguage->addItem(item, item);
    }
    connect(cbxLanguage, SIGNAL(activated(int)), this, SLOT(slotSelectLanguage(int)));
    layout->addRow(lblLanguage, cbxLanguage);

    // TOOLKIT
    QLabel *lblToolkit = new QLabel("Toolkit");
    cbxToolkit = new QComboBox();
    connect(cbxToolkit, SIGNAL(activated(int)), this, SLOT(slotSelectToolkit(int)));
    layout->addRow(lblToolkit, cbxToolkit);

    // LICENSE
    QLabel *lblLicenses = new QLabel("License");
    cbxLicenses = new QComboBox();
    QDirIterator it(this->appDir + QDir::separator() + "licenses/", QDir::AllEntries | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        QFileInfo fi(it.next());
        cbxLicenses->addItem(fi.fileName(),fi.absoluteFilePath());
    }
    connect(cbxLicenses, SIGNAL(activated(int)), this, SLOT(slotSelectLicense(int)));
    layout->addRow(lblLicenses, cbxLicenses);

    // SUMMARY
    QLabel *lblSummary = new QLabel("Summary");
    lstSummary = new QListWidget();
    layout->addRow(lblSummary, lstSummary);

    // BUTTON BOX
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &DlgNewProject::slotAccept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    layout->addWidget(buttonBox);
}

//******************************************************************************
// slotNameChanged()
//******************************************************************************
void DlgNewProject::slotNameChanged(QString txt) {
    buildSummary();
}

//******************************************************************************
// slotSelectLanguage()
//******************************************************************************
void DlgNewProject::slotSelectLanguage(int item) {
    QList<QString> toolkits = Project::getToolkits(appDir, cbxLanguage->itemText(item));
    cbxToolkit->clear();
    foreach(QString item, toolkits) {
        cbxToolkit->addItem(item, item);
    }
    buildSummary();
}

//******************************************************************************
// slotSelectToolkit()
//******************************************************************************
void DlgNewProject::slotSelectToolkit(int item) {
    buildSummary();
}

//******************************************************************************
// slotSelectLicense()
//******************************************************************************
void DlgNewProject::slotSelectLicense(int item) {
    buildSummary();
}

//******************************************************************************
// slotAccept()
//******************************************************************************
void DlgNewProject::slotAccept() {
    createFiles();
    QDialog::accept();
}

//******************************************************************************
// createFiles()
//******************************************************************************
void DlgNewProject::createFiles() {
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
void DlgNewProject::buildSummary() {
    filesToCreate.clear();
    files = Project::getFiles(appDir, cbxLanguage->currentText(), cbxToolkit->currentText());
    lstSummary->clear();
    foreach(QStringList item, files) {
        lstSummary->addItem(appendFileToList(item[0], item[1]));
    }
    lstSummary->addItem(appendFileToList(cbxLicenses->currentText(), cbxLicenses->currentData().toString()));
}

//******************************************************************************
// appendFileToList()
//******************************************************************************
QString DlgNewProject::appendFileToList(QString filename, QString source) {
    filename = renameFileWithVars(filename);
    QString newFileName = Utils::pathAppend(repository, Utils::pathAppend(cbxLanguage->currentText(), Utils::pathAppend(txtProjectName->text(), filename)));
    filesToCreate.append({newFileName, source});
    return(newFileName);
}

//******************************************************************************
// getProjectPath()
//******************************************************************************
QString DlgNewProject::getProjectPath() {
    QString projectPath = Utils::pathAppend(repository, Utils::pathAppend(cbxLanguage->currentText(), txtProjectName->text()));
    return projectPath;
}

//******************************************************************************
// renameFileWithVars()
//******************************************************************************
QString DlgNewProject::renameFileWithVars(QString filename) {
    filename = setVars(filename);
    return(filename);
}

//******************************************************************************
// replaceVarsInFile()
//******************************************************************************
void DlgNewProject::replaceVarsInFile(QString filename) {
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
QString DlgNewProject::setVars(QString text) {
    text.replace("%PROJECT%", txtProjectName->text());
    text.replace("%LANGUAGE%", cbxLanguage->currentText());
    text.replace("%TOOLKIT%", cbxToolkit->currentText());
    text.replace("%YEAR%", QDateTime::currentDateTime().toString("yyyy"));
    text.replace("%MONTH%", QDateTime::currentDateTime().toString("MM"));
    text.replace("%DAY%", QDateTime::currentDateTime().toString("dd"));
    text.replace("%TIME%", QDateTime::currentDateTime().toString("hhmmss"));
    text.replace("%DATE%", QDateTime::currentDateTime().toString("yyyyMMdd"));
    text.replace("%DATETIME%", QDateTime::currentDateTime().toString("yyyyMMdd-hhmmss"));
    text.replace("%AUTHOR%", appSettings->get("PROJECT_USER_NAME").toString());
    text.replace("%COMPANY%", appSettings->get("PROJECT_USER_COMPANY").toString());
    text.replace("%MAIL%", appSettings->get("PROJECT_USER_MAIL").toString());
    text.replace("%WEB%", appSettings->get("PROJECT_USER_WEB").toString());

    return (text);
}

//******************************************************************************
// slotDoRandomName()
//******************************************************************************
void DlgNewProject::slotDoRandomName() {
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

    QString strAdjective = scratchAdjectives.at(QRandomGenerator::global()->generate() % scratchAdjectives.size());
    QString strAnimal = scratchAnimals.at(QRandomGenerator::global()->generate() % scratchAnimals.size());
    txtProjectName->setText(Utils::capitalize(strAdjective) + Utils::capitalize(strAnimal));
}

