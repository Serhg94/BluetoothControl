#include "ardserial.h"
#include <string.h>


ArdSerial::ArdSerial()
{
    Opened=false;
    initEnumerator();
}

ArdSerial::~ArdSerial()
{
    deinitEnumerator();
    delete serial;
}


bool ArdSerial::OpenPort()
{
    //if(!Opened)
    {
        serial = new AbstractSerial(this);
        serial->setDeviceName(PortStr);
        emit OverHeadInfo(enumerator->description());
        if(serial->open(AbstractSerial::ReadWrite))
        {
            //serial->setTotalReadConstantTimeout(1000);
            //serial->setBaudRate(BaudRate);


            if (!serial->setBaudRate(BaudRate)) {
                qDebug() << "Set baud rate " <<  AbstractSerial::BaudRate115200 << " error.";

            };

            if (!serial->setDataBits(AbstractSerial::DataBits8)) {
                qDebug() << "Set data bits " <<  AbstractSerial::DataBits8 << " error.";

            }

            if (!serial->setParity(AbstractSerial::ParityNone)) {
                qDebug() << "Set parity " <<  AbstractSerial::ParityNone << " error.";

            }

            if (!serial->setStopBits(AbstractSerial::StopBits1)) {
                qDebug() << "Set stop bits " <<  AbstractSerial::StopBits1 << " error.";

            }

            if (!serial->setFlowControl(AbstractSerial::FlowControlOff)) {
                qDebug() << "Set flow " <<  AbstractSerial::FlowControlOff << " error.";

            }

            connect(this->serial, SIGNAL(signalStatus(QString,QDateTime)), this, SLOT(emitSerialMessages(QString,QDateTime)));
            connect(this->serial, SIGNAL(readyRead()), this, SLOT(readAllData()));
            emit OverHeadInfo("= Default parameters =");
            emit OverHeadInfo("Device name            : " + serial->deviceName());
            emit OverHeadInfo("Baud rate              : " + serial->baudRate());
            emit OverHeadInfo("Data bits              : " + serial->dataBits());
            emit OverHeadInfo("Parity                 : " + serial->parity());
            emit OverHeadInfo("Stop bits              : " + serial->stopBits());
            emit OverHeadInfo("Flow                   : " + serial->flowControl());
            QString s;  s = QString("%1") .arg(serial->totalReadConstantTimeout());
            emit OverHeadInfo("Total read timeout constant, msec : " + s);
            s = QString("%1") .arg(serial->charIntervalTimeout());
            emit OverHeadInfo("Char interval timeout, usec       : " + s);
            emit OverHeadInfo("PORT OPENED successfully");
            Opened=true;
            return true;
        }
        else
        {
            emit OverHeadInfo("PORT OPEN fail");
            return false;
        }
    }
    //else
    {
      //  emit OverHeadInfo("PORT already opened!");
        //return false;
    }
}

void ArdSerial::initEnumerator()
{
    this->enumerator = SerialDeviceEnumerator::instance();
    this->enumerator->setEnabled(true);
}

void ArdSerial::deinitEnumerator()
{
    if (this->enumerator && this->enumerator->isEnabled())
        this->enumerator->setEnabled(false);
}

void ArdSerial::SetPort(QString pn)
{
    PortStr = pn;
}

void ArdSerial::SetBaudRate(int br)
{
    BaudRate = br;
}

void ArdSerial::SetBaudRate(QString br)
{
    bool ok;
    BaudRate = br.toInt(&ok, 10);
}

void ArdSerial::emitSerialMessages(QString s,QDateTime t)
{
    emit EnteredSerialMessage(s, t);
}

void ArdSerial::readAllData()
{
    QByteArray byte = this->serial->readAll();
    qDebug() << byte.data();
    emit EnteredData(byte);
}

void ArdSerial::sendData(QByteArray by)
{
    if(Opened)
        serial->write(by);
    else emit OverHeadInfo("PORT isn't Open!!");
}

void ArdSerial::sendDataString(QString by)
{
     QByteArray text = by.toLocal8Bit();
     char *data = new char[text.size()];
     strcpy(data, text.data());
     if(Opened)
         serial->write(data, by.length());
     else emit OverHeadInfo("PORT isn't Open!!");
}


bool ArdSerial::isOpen()
{
    return Opened;
}

