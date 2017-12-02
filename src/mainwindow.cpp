#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "server.h"
#include "db.h"

#include <QFileInfo>
#include <QDir>
#include <QDateTime>

typedef QSerialPort P;

static DB connection;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->checkCollection();

    this->serial = new QSerialPort("/dev/ttyACM0");
    connect(serial, &QSerialPort::readyRead, [this]()
    {
        static QByteArray byteArray;
        byteArray += serial->readAll();
        if(!QString(byteArray).contains("\n"))
            return;
        QString data = QString( byteArray ).remove("\r").remove("\n");
        byteArray.clear();

        this->parseInfo(data);
    });

    QSerialPortInfo info(*serial);
    // Check info of the port
    qDebug() << "Name        : " << info.portName();
    qDebug() << "Manufacturer: " << info.manufacturer(); //if showing manufacturer, means Qstring &name is good
    qDebug() << "Busy: " << info.isBusy();

    if(serial->open(QIODevice::ReadWrite))
    {
        serial->setBaudRate(P::Baud9600);
        serial->setDataBits(P::Data8);
        serial->setParity(P::NoParity);
        serial->setStopBits(P::OneStop);
        serial->setFlowControl(P::NoFlowControl);
        qDebug()<<"OK";
    }

    server = new EchoServer(1234, true);
    connect(server, &EchoServer::message, [this](const QString &message)
    {
        QStringList res;
        for(auto info : message.split(","))
        {
            this->ui->remoteConsole->append(info);

            if(info.startsWith("D#"))
                res<<info.replace("D#","");
            else if(info.startsWith("L#"))
                res<<info.replace("L#","");
            else if(info.startsWith("I#"))
                res<<info.replace("I#","");
        }
        this->updateInfo(res);
    });
    connect(server, &EchoServer::connected, [this](const QString &host)
    {
        this->ui->stateIcon->setPixmap(QPixmap(":/img/img/on.svg"));
        this->ui->remoteConsole->append("CONNECTED >> "+host);
    });

    connect(server, &EchoServer::disconnected, [this](const QString &origin)
    {
        this->ui->stateIcon->setPixmap(QPixmap(":/img/img/off.svg"));
        this->ui->remoteConsole->append("DISCONNECTED >> "+origin);
    });

    QObject::connect(server, &EchoServer::closed, [this]()
    {
        this->close();
        qApp->quit();
    });

}

MainWindow::~MainWindow()
{

    delete ui;
}

void MainWindow::parseInfo(QString info)
{
    LOG log;

    if(info.startsWith("D#"))
    {
        auto distance = info.replace("D#","");
        this->ui->disLCD->display(distance);

        doAction([&]()
        {
            if(distance.toInt()<20)
                this->showFullScreen();
            else this->showNormal();
        });

        this->server->sendMessageTo(0,"D#"+info);
        log.insert(VAR::DISTANCE,distance);
        this->insertLog(log);
        return;

    }else if(info.startsWith("L#"))
    {
        auto luminosity = info.replace("L#","");
        this->ui->lumLCD->display(luminosity);
        this->server->sendMessageTo(0,"L#"+info);
        log.insert(VAR::LUMINOSITY,luminosity);
        this->insertLog(log);
        return;
    }else if(info.contains("ON#"))
    {
        doAction([this]()
        {
            this->setVisible(false);
        });
    }else if(info.contains("OFF#"))
    {
        doAction([this]()
        {
            this->setVisible(true);
        });
    }

    this->ui->console->append("<span style='color:pink'>"+this->serial->portName()+"</span> >> "+info);
}

void MainWindow::updateInfo(const QStringList &info)
{
    qDebug()<<info.join(",");
    if(this->serial->isWritable())
        this->serial->write(info.join(",").toLocal8Bit());
    else qDebug() <<"could't turn on";

}

template<typename F>
void MainWindow::doAction(F cb)
{
    cb();
}

void MainWindow::on_updateBtn_clicked()
{
    auto distance = this->ui->distance->value();
    auto luminosity = this->ui->luminosity->value();
    auto interval = this->ui->interval->value();

    this->updateInfo({QString::number(distance),QString::number(luminosity),QString::number(interval)});
}

void MainWindow::checkCollection()
{
    QDir collectionDBPath_dir(this->CollectionDBPath);
    if (!collectionDBPath_dir.exists())
        collectionDBPath_dir.mkpath(".");

    QFileInfo path(this->CollectionDBPath+this->CollectionDBName);

    if (path.exists())
    {
        connection.setUpCollection(this->CollectionDBPath + this->CollectionDBName);
        return;
    }

    connection.setUpCollection(this->CollectionDBPath + this->CollectionDBName);
    connection.prepareCollectionDB();
}

void MainWindow::insertLog(const LOG &log)
{
    for(auto key : log.keys())
    {
        QVariantMap logMap
        {
            {"key_id", VARMAP[key]},
            {"value", log[key]},
            {"date",QDateTime::currentDateTime()},
            {"span","1500"}
        };

        connection.insert("LOGS",logMap);
    }
}
