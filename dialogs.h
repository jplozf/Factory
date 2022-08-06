#ifndef DIALOGS_H
#define DIALOGS_H

#include <QWidget>
#include <QDialog>
#include <QFormLayout>
#include <QMap>
#include <QLabel>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QFontDatabase>
#include <QSizePolicy>
#include <QComboBox>
#include <QPushButton>
#include <QListWidget>
#include <QDirIterator>
#include <QDateTime>
#include "project.h"
#include "utils.h"
#include "settings.h"
#include "constants.h"
#include "app.h"

//******************************************************************************
// Class DlgProperties()
//******************************************************************************
class DlgProperties : public QDialog
{
    Q_OBJECT
public:
    explicit DlgProperties(QMap<QString, QString> props, QString title = "Properties", QDialog *parent = nullptr);

signals:

};

//******************************************************************************
// Class DlgOpenProject()
//******************************************************************************
class DlgOpenProject : public QDialog
{
    Q_OBJECT
public:
    explicit DlgOpenProject(App *app, QDialog *parent = nullptr);
    QString getProjectPath();
    QList<QStringList> getHelpFiles();
    QLabel *lblOpenProject;
    QLabel *lblRecentProject;

private:
    QTabWidget *tbwProject;
    QLineEdit *txtProjectName;
    QComboBox *cbxLanguage;
    QComboBox *cbxToolkit;
    QComboBox *cbxLicenses;
    QListWidget *lstSummary;
    QString appendFileToList(QString filename, QString source);
    QString repository;
    QString projectPath;
    QList<QStringList> filesToCreate;
    void buildSummary();
    QList<QStringList> files;
    QString renameFileWithVars(QString filename);
    void replaceVarsInFile(QString filename);
    void createFiles();
    QString setVars(QString text);
    App *app;

private slots:
    void slotDoRandomName();
    void slotNameChanged(QString txt);
    void slotSelectLanguage(int item);
    void slotSelectToolkit(int item);
    void slotSelectLicense(int item);
    void slotAccept();

signals:

};

#endif // DIALOGS_H
