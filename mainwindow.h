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
#include <QMenu>

#include "ui_mainwindow.h"
#include "settings.h"
#include "constants.h"
#include "utils.h"
#include "dialogs.h"
#include "downloader.h"
#include "project.h"
#include "app.h"
#include "gitapi.h"

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
    QString launcherDir;
    QString currentLauncherDir;
    void closeEvent(QCloseEvent *);
    void saveSettings();
    void readSettings();
    QFileSystemModel *model;
    App *app;
    QList<QStringList> helpFiles;
    GitAPI *git;

protected:
    void mouseMoveEvent(QMouseEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);

private:
    Project *project;
    Ui::MainWindow *ui;
    void launchProgram(const QString pgm, const QStringList args={""});
    void openProject(QString project);
    void closeProject();
    void setHelpFilesInToolbar(const QList<QStringList> helpFiles);
    QPoint mLastMousePosition;
    bool mMoving;
    bool runningSession;
    const int TAB_PROJECT = 0;
    const int TAB_EDIT = 1;
    const int TAB_BUILD = 2;
    const int TAB_RUN = 3;
    const int TAB_GIT = 4;
    const int TAB_SETTINGS = 5;

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
    void slotDoOpenProject();
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
