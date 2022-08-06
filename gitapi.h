#ifndef GITAPI_H
#define GITAPI_H

#include "ui_mainwindow.h"
#include <QString>
#include <QProcess>
#include <QDebug>
#include <QObject>
#include <QFile>

class GitAPI : public QObject
{
    Q_OBJECT

public:
    const int ERR_GIT_BINARY_NOT_SET = -200;

    GitAPI(QString gitBin, Ui::MainWindow *ui);
    GitAPI(QString gitBin, Ui::MainWindow *ui, QString rootFolder);
    void setRootFolder(QString rootFolder);
    QString getRootFolder();

    struct xeqResult {
        int rc;
        QString out;
        QString err;
    };
    xeqResult xeq(QStringList params);
    QList<QStringList> commands;

private:
    QString gitBin;
    QString rootFolder;
    Ui::MainWindow *ui;
    QStringList command;
    void buildCommand();

private slots:
    void slotDoClearOutput();
    void slotDoChangeCommandIndex(int);
    void slotDoXeq();
};

#endif // GITAPI_H
