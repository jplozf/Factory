#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCloseEvent>
#include <QProcess>
#include <QSettings>
#include <QApplication>
#include <QDesktopWidget>
#include <QStandardItemModel>
#include <QDesktopServices>
#include <QFileDialog>
#include <QInputDialog>
#include <QDebug>
#include <QPainter>
#include <QDirModel>
#include <QFileSystemModel>
#include <QDir>
#include <QMessageBox>

#include "ui_mainwindow.h"
#include "settings.h"
#include "constants.h"
#include "utils.h"
#include "dialogs.h"
#include "downloader.h"

#ifdef Q_OS_LINUX
#include <QtX11Extras/QX11Info>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <QFileSystemModel>
#endif

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    QString projectPath;
    QVector<QString> mruProjects;
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QString appDir;
    QString docDir;
    QString launcherDir;
    QString currentLauncherDir;
    Settings *appSettings;
    Constants *appConstants;
    void closeEvent(QCloseEvent *);
    void saveSettings();
    void readSettings();
    QFileSystemModel *model;

private:
    Ui::MainWindow *ui;
    void launchProgram(QString pgm);
    void openProject(QString project);
    void closeProject();

private slots:
    void slotDoEdit();
    void slotDoEditFile();
    void slotDoBrowseFile();
    void slotDoDefaultFile();
    void slotDoLaunchFile();
    void slotDoLaunchTerminal();
    void slotDoExit();
    void slotDoTemplatesCutomizing();

    void slotDoNewItem();
    void slotDoOpenTerminal();
    void slotDoDeleteFolder();
    void slotDoPropertiesFolder();
    void slotDoDuplicateFile();
    void slotDoRenameFile();
    void slotDoDeleteFile();
    void slotDoPropertiesFile();
    void slotDoNewProject();
    void slotDoOpenProject();
    void slotDoOpenRecentProject(QString);
    void slotDoPropertiesProject();
    void slotDoArchiveProject();
    void slotDoCloseProject();

    void slotDoTerm();
    void slotDoBrowser();
    void slotDoSettings();
    void slotDoBuild();
    void slotClickedProject(QModelIndex idx);
    void slotDoubleClickedProject(QModelIndex idx);
    void slotMenuContextProject(QPoint p);

};
#endif // MAINWINDOW_H
