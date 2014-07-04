#ifndef ARDSERIAL_H
#define ARDSERIAL_H

#include <QtCore/QDebug>
#include <QtCore/QCoreApplication>
#include <abstractserial.h>
#include <iostream>
#include <serialdeviceenumerator.h>

class ArdSerial  : public QObject
{
    Q_OBJECT

    private:
        QString PortStr;
        int BaudRate;
        bool Opened;
        AbstractSerial *serial;
        void initEnumerator();
        void deinitEnumerator();
    public:
        ArdSerial();
        SerialDeviceEnumerator *enumerator;
        bool isOpen();
        ~ArdSerial();
    public slots:
        void SetPort(QString pn);
        void SetBaudRate(int br);
        void SetBaudRate(QString br);
        void sendData(QByteArray by);
        void sendDataString(QString by);
        bool OpenPort();
    private slots:
        void emitSerialMessages(QString s,QDateTime t);
        void readAllData();
    signals:
        void EnteredSerialMessage(QString s,QDateTime t);
        void EnteredData(QByteArray b);
        void OverHeadInfo(QString s);

};

#endif // ARDSERIAL_H
