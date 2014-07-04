/*
 *  Copyright (c) 2011 Evgeny Proydakov <lord.tiran@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QStringList>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_adapter(0),
    buttonVector(QVector<bool>(MAX_JOYSTICK_BUTTONS)),
    ui(new Ui::MainWindow),
    m_joyId(-1)
{
    ui->setupUi(this);

    connect(ui->exitPushButton, SIGNAL(clicked()), this, SLOT(close()));
    connect(ui->connectPushButton, SIGNAL(clicked()), this, SLOT(connectToJoystick()));
    connect(ui->disconnectPushButton, SIGNAL(clicked()), this, SLOT(disconnectFromJoystick()));
    connect(ui->rescanPushButton, SIGNAL(clicked()), this, SLOT(rescanJoystickDevice()));

    buttonVector.fill(false);

    ui->joystickStateBox->setDisabled(true);
    ui->joystickInformationBox->setDisabled(true);
    ui->disconnectPushButton->setDisabled(true);

    m_adapter = new VJoystickAdapter(this);

    setAvalibleJoystick();
    setDefaultText();

    setFixedSize(590, 365);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setDefaultText()
{
    ui->joystickNameLabel->setText("Not connected");
    ui->joystickIdLabel->setText(tr("-1"));
    ui->joystickAxisLabel->setText(tr("-1"));
    ui->joystickHatsLabel->setText(tr("-1"));
    ui->joystickBallsLabel->setText(tr("-1"));
    ui->joystickButtonsLabel->setText(tr("-1"));

    ui->joystickXaxisLabel->setText(tr("0"));
    ui->joystickYaxisLabel->setText(tr("0"));
    ui->joystickXrotationLabel->setText(tr("0"));
    ui->joystickYrotationLabel->setText(tr("0"));
    ui->joystickZLTaxisLabel->setText(tr("0"));
    ui->joystickZRTaxisLabel->setText(tr("0"));
    ui->joystickButtonsTestLabel->setText(tr(""));
    ui->joystickPOV0Label->setText(tr("0"));
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
            connect(m_adapter, SIGNAL(sigButtonChanged(int, bool)), this, SLOT(buttonSetup(int,bool)));
            connect(m_adapter, SIGNAL(sigAxisChanged(int,int)), this, SLOT(axisSetup(int,int)));
            connect(m_adapter, SIGNAL(sigHatCanged(int,int)), this, SLOT(hatSetup(int,int)));
            connect(m_adapter, SIGNAL(sigBallChanged(int,int,int)), this, SLOT(ballSetup(int,int,int)));
        }

        QString joyName = m_adapter->getJoystickName();
        if(joyName == "")
            joyName = "Not connected";
        ui->joystickNameLabel->setText(joyName);
        ui->joystickIdLabel->setText(tr("%1").arg(m_adapter->getJoystickId()));
        ui->joystickAxisLabel->setText(tr("%1").arg(m_adapter->getJoystickNumAxes()));
        ui->joystickHatsLabel->setText(tr("%1").arg(m_adapter->getJoystickNumHats()));
        ui->joystickBallsLabel->setText(tr("%1").arg(m_adapter->getJoystickNumBalls()));
        ui->joystickButtonsLabel->setText(tr("%1").arg(m_adapter->getJoystickNumButtons()));

        ui->joystickStateBox->setEnabled(true);
        ui->joystickInformationBox->setEnabled(true);
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
        disconnect(m_adapter, SIGNAL(sigButtonChanged(int, bool)), this, SLOT(buttonSetup(int,bool)));
        disconnect(m_adapter, SIGNAL(sigAxisChanged(int,int)), this, SLOT(axisSetup(int,int)));
        disconnect(m_adapter, SIGNAL(sigHatCanged(int,int)), this, SLOT(hatSetup(int,int)));
        disconnect(m_adapter, SIGNAL(sigBallChanged(int,int,int)), this, SLOT(ballSetup(int,int,int)));
    }

    ui->joystickStateBox->setDisabled(true);
    ui->joystickInformationBox->setDisabled(true);

    ui->connectPushButton->setEnabled(true);
    ui->disconnectPushButton->setDisabled(true);
    ui->joystickComboBox->setEnabled(true);
    ui->rescanPushButton->setEnabled(true);

    m_joyId = -1;

    setDefaultText();
    setAvalibleJoystick();
}

void MainWindow::axisSetup(int id, int state)
{
    switch(id)
    {
    case 0:
        ui->joystickXaxisLabel->setText(tr("%1").arg(state));
        break;
    case 1:
        ui->joystickYaxisLabel->setText(tr("%1").arg(-1*state));
        break;
    case 2:
        ui->joystickZLTaxisLabel->setText(tr("%1").arg(state));
        break;
    case 3:
        ui->joystickXrotationLabel->setText(tr("%1").arg(state));
        break;
    case 4:
        ui->joystickYrotationLabel->setText(tr("%1").arg(-1*state));
        break;
    case 5:
        ui->joystickZRTaxisLabel->setText(tr("%1").arg(state));
        break;
    }
}

void MainWindow::hatSetup(int id, int state)
{
    Q_UNUSED(id);
    ui->joystickPOV0Label->setText(tr("%1").arg(state));
}

void MainWindow::buttonSetup(int id, bool state)
{
    buttonVector[id] = state;

    QString buttonTest = "";
    for(int i = 0; i < MAX_JOYSTICK_BUTTONS; ++i)
    {
        if(buttonVector[i] == true)
        {
            if(i < 10)
                buttonTest += tr("0%1  ").arg(i);
            else
                buttonTest += tr("%1").arg(i);
        }
    }
    ui->joystickButtonsTestLabel->setText(buttonTest);
}

void MainWindow::ballSetup(int id, int stateX, int stateY)
{
    Q_UNUSED(id);
    Q_UNUSED(stateX);
    Q_UNUSED(stateY);
}

#include "moc_mainwindow.cpp"
