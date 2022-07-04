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
#include <QRandomGenerator>
#include <QDirIterator>
#include <QDateTime>
#include "project.h"
#include "utils.h"
#include "settings.h"
#include "constants.h"

class DlgProperties : public QDialog
{
    Q_OBJECT
public:
    explicit DlgProperties(QMap<QString, QString> props, QString title = "Properties", QDialog *parent = nullptr);

signals:

};

class DlgNewProject : public QDialog
{
    Q_OBJECT
public:
    explicit DlgNewProject(Settings *appSettings, Constants *appConstants, QDialog *parent = nullptr);
    QString getProjectPath();

private:
    QString appDir;
    Settings *appSettings;
    Constants *appConstants;
    QLineEdit *txtProjectName;
    QComboBox *cbxLanguage;
    QComboBox *cbxToolkit;
    QComboBox *cbxLicenses;
    QListWidget *lstSummary;
    QString appendFileToList(QString filename, QString source);
    QString repository;
    QList<QStringList> filesToCreate;
    void buildSummary();
    QList<QStringList> files;
    QString renameFileWithVars(QString filename);
    void replaceVarsInFile(QString filename);
    void createFiles();
    QString setVars(QString text);

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
