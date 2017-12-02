#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QString>
#include <QDebug>
#include <QStandardPaths>

class EchoServer;

enum VAR
{
    DISTANCE = 0,
    LUMINOSITY = 1,
    INTERVAL = 2
};

typedef QMap<VAR,QString> LOG;

static inline LOG VARMAP
{
    {VAR::DISTANCE, "DISTANCE"},
    {VAR::LUMINOSITY, "LUMINOSITY"},
    {VAR::INTERVAL, "LUMINOSITY"}
};

namespace Ui { class MainWindow; }

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void parseInfo(QString info);
    void updateInfo(const QStringList &info);
    template<typename F>
    void doAction(F cb);

private slots:

    void on_updateBtn_clicked();

private:
    Ui::MainWindow *ui;
    QSerialPort *serial;
    EchoServer *server;
    const QString CollectionDBPath = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation)+"/arduino/";
    const QString CollectionDBName = "logs.db";

    void checkCollection();
    void insertLog(const LOG &log);
};

#endif // MAINWINDOW_H
