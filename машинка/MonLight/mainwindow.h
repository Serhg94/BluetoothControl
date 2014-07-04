#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "ardserial.h"
#include "v_joystick_adapter.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ArdSerial *arduino;
    VJoystickAdapter *m_adapter;
    QTimer *timer;
    ~MainWindow();

public slots:
    void procEnumerate(QStringList ps);
    void procSerialMessages(QString msg, QDateTime dt);
    void procOverHeadMessages(QString s);
    void procEnteredData(QByteArray b);
    void Send();
    void rotateServo(int corner);
    void rotateServoByGP(int id, int val);
    void buttonGP(int id, bool val);
    void update();
    void disconnectFromJoystick();
    void connectToJoystick();
    void rescanJoystickDevice();
    void setAvalibleJoystick();
    void setUpRate();
private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
