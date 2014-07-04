#include "mainwindow.h"
#include <QTimer>
#include "ui_mainwindow.h"
int output_servo=0;
int old_servo=0;
int output_dv=0;
int old_dv=0;
int m_joyId;


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->SendButton->setShortcut(Qt::Key_Return);
    arduino = new ArdSerial();
    procEnumerate(arduino->enumerator->devicesAvailable());
    arduino->SetPort(ui->PortBox->currentText());
    arduino->SetBaudRate(ui->BaudRateEdit->text());
    connect(arduino->enumerator, SIGNAL(hasChanged(QStringList)), this, SLOT(procEnumerate(QStringList)));
    connect(arduino, SIGNAL(EnteredSerialMessage(QString,QDateTime)), this, SLOT(procSerialMessages(QString,QDateTime)));
    connect(arduino, SIGNAL(OverHeadInfo(QString)), this, SLOT(procOverHeadMessages(QString)));
    connect(ui->OpenPornButton, SIGNAL(clicked()), arduino, SLOT(OpenPort()));
    connect(ui->PortBox, SIGNAL(currentIndexChanged(QString)), arduino, SLOT(SetPort(QString)));
    connect(ui->BaudRateEdit, SIGNAL(textChanged(QString)), arduino, SLOT(SetBaudRate(QString)));
    connect(arduino, SIGNAL(EnteredData(QByteArray)), this, SLOT(procEnteredData(QByteArray)));
    connect(ui->SendButton, SIGNAL(clicked()), this, SLOT(Send()));

    m_adapter = new VJoystickAdapter(this);
    setAvalibleJoystick();

    connect(ui->connectPushButton, SIGNAL(clicked()), this, SLOT(connectToJoystick()));
    connect(ui->disconnectPushButton, SIGNAL(clicked()), this, SLOT(disconnectFromJoystick()));
    connect(ui->rescanPushButton, SIGNAL(clicked()), this, SLOT(rescanJoystickDevice()));

    /*if(m_adapter->open(0))
    {
        connect(m_adapter, SIGNAL(sigAxisChanged(int,int)), this, SLOT(rotateServoByGP(int,int)));
        connect(m_adapter, SIGNAL(sigButtonChanged(int, bool)), this, SLOT(buttonGP(int,bool)));
        procOverHeadMessages(QString("GP connected"));
    }*/

    timer = new QTimer(this);
    connect(ui->setUpRate, SIGNAL(clicked()), this, SLOT(setUpRate()));
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->start(ui->upRate->value());
}

MainWindow::~MainWindow()
{
    delete arduino;
    delete ui;
}

void MainWindow::procEnumerate(QStringList ps)
{
    ui->PortBox->clear();
    ui->PortBox->addItems(ps);
}

void MainWindow::procSerialMessages(QString msg, QDateTime dt)
{
    QString s = dt.time().toString() + " > " + msg;
    ui->SerialMsg->append(s);
}

void MainWindow::procOverHeadMessages(QString s)
{
    ui->SerialMsg->append(s);
}

void MainWindow::procEnteredData(QByteArray b)
{
    QString s = b.data();
    QTextCursor cursor = ui->ReadEdit->textCursor();
    cursor.movePosition(QTextCursor::MoveOperation::End);
    ui->ReadEdit->setTextCursor(cursor);
    ui->ReadEdit->insertPlainText(s);
}

void MainWindow::Send()
{
    QByteArray byte;
    byte.append(ui->SendValue->text().toAscii());
    arduino->sendData(byte);
    //arduino->sendDataString(ui->SendValue->text());
}

void MainWindow::rotateServo(int corner)
{
    QByteArray byte;
    if(corner ==0) corner = 181;
    QString s;  s = QString("%1") .arg(corner);
    byte.append(s.toAscii());
    byte.append('/');
    arduino->sendData(byte);
}

void MainWindow::rotateServoByGP(int id, int val)
{
    if (id==4)
    {
        //QByteArray byte;
        int corner=90+val/1724.631578947368;
        //32768
        //int corner=90+val/1600;
        if(corner ==0) corner = 181;
//        QString s;  s = QString("%1") .arg(corner);
//        byte.append(s.toAscii());
//        byte.append('/');
//        arduino->sendData(byte);

        output_servo=corner;

    }
    if ((id==1))
    {
        val=val*(-1);
        //QByteArray byte;
        int corner;

        //линейная зависимость
        //corner=499+val/128;

        //квадратичная зависимость
        if(val<0)
            corner=500-((val/128)*(val/128))/255;
        else
            corner=500+((val/128)*(val/128))/255;



        if((corner>=245)&&(corner<756))
        {

//            QString s;  s = QString("%1") .arg(corner);
//            byte.append(s.toAscii());
//            byte.append('/');
//            arduino->sendData(byte);

            output_dv=corner;

        }
    }
}

void MainWindow::buttonGP(int id, bool val)
{
    if ((id==0)&&(val))
    {
        QByteArray byte;
        QString s;  s = QString("1");
        byte.append(s.toAscii());
        byte.append('/');
        arduino->sendData(byte);
    }
    else
    if ((id==1))
    {
        QByteArray byte;
        QString s;  s = QString("1");
        byte.append(s.toAscii());
        byte.append('/');
        arduino->sendData(byte);
    }
}

void MainWindow::update()
{
    if(output_servo!=old_servo)
    {
        QByteArray byte;
        old_servo=output_servo;
        QString s;  s = QString("%1") .arg(output_servo);
        byte.append(s.toAscii());
        byte.append('/');
        arduino->sendData(byte);
    }
    if(output_dv!=old_dv)
    {
        QByteArray byte;
        old_dv=output_dv;
        QString s;  s = QString("%1") .arg(output_dv);
        byte.append(s.toAscii());
        byte.append('/');
        arduino->sendData(byte);
    }
}

void MainWindow::setAvalibleJoystick()
{
    ui->joystickComboBox->clear();
    ui->joystickComboBox->addItems(VJoystickAdapter::getAvaliableJoystickName());
}

void MainWindow::rescanJoystickDevice()
{
    setAvalibleJoystick();
}

void MainWindow::connectToJoystick()
{
    int joyComboBox = ui->joystickComboBox->currentIndex();

    if(joyComboBox != -1)
    {
        m_joyId = joyComboBox;

        if(m_adapter->open(m_joyId))
        {
            procOverHeadMessages(QString("Gamepad connected"));
            connect(m_adapter, SIGNAL(sigAxisChanged(int,int)), this, SLOT(rotateServoByGP(int,int)));
            connect(m_adapter, SIGNAL(sigButtonChanged(int, bool)), this, SLOT(buttonGP(int,bool)));
            //connect(m_adapter, SIGNAL(sigButtonChanged(int, bool)), this, SLOT(buttonSetup(int,bool)));
            //connect(m_adapter, SIGNAL(sigAxisChanged(int,int)), this, SLOT(axisSetup(int,int)));
            //connect(m_adapter, SIGNAL(sigHatCanged(int,int)), this, SLOT(hatSetup(int,int)));
            //connect(m_adapter, SIGNAL(sigBallChanged(int,int,int)), this, SLOT(ballSetup(int,int,int)));
        }

        QString joyName = m_adapter->getJoystickName();
        if(joyName == "")
            joyName = "Not connected";
        ui->connectPushButton->setDisabled(true);
        ui->disconnectPushButton->setEnabled(true);
        ui->joystickComboBox->setDisabled(true);
        ui->rescanPushButton->setDisabled(true);
    }
}

void MainWindow::disconnectFromJoystick()
{
    if(m_adapter->isConnected())
    {
        m_adapter->close();
        disconnect(m_adapter, SIGNAL(sigAxisChanged(int,int)), this, SLOT(rotateServoByGP(int,int)));
        disconnect(m_adapter, SIGNAL(sigButtonChanged(int, bool)), this, SLOT(buttonGP(int,bool)));
    }

    ui->connectPushButton->setEnabled(true);
    ui->disconnectPushButton->setDisabled(true);
    ui->joystickComboBox->setEnabled(true);
    ui->rescanPushButton->setEnabled(true);

    m_joyId = -1;

    setAvalibleJoystick();
}

void MainWindow::setUpRate()
{
    timer->setInterval(ui->upRate->value());
    procOverHeadMessages(QString("Send rate upgraded"));
}


