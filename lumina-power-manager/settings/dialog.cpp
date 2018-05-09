/*
# Copyright (c) 2018, Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
*/

#include "dialog.h"
#include <QVBoxLayout>
#include <QIcon>
#include <QDebug>
#include <QLabel>
#include <QPixmap>
#include <QTabWidget>
#include "common.h"
#include <QDBusConnection>
#include <QDBusInterface>

Dialog::Dialog(QWidget *parent)
    : QDialog(parent)
    , lidActionBattery(0)
    , lidActionAC(0)
    , criticalActionBattery(0)
    , lowBattery(0)
    , criticalBattery(0)
{
    setAttribute(Qt::WA_QuitOnClose, true);
    setWindowTitle(tr("Lumina Power Settings"));

    QHBoxLayout *layout = new QHBoxLayout(this);
    QTabWidget *containerWidget = new QTabWidget(this);

    QLabel *powerLabel = new QLabel(this);
    QIcon powerIcon = QIcon::fromTheme("battery");
    powerLabel->setPixmap(powerIcon.pixmap(QSize(64, 64)));
    powerLabel->setMinimumSize(QSize(64, 64));
    powerLabel->setMaximumSize(QSize(64, 64));

    //layout->setSizeConstraint(QLayout::SetFixedSize);
    layout->addWidget(powerLabel);
    layout->addWidget(containerWidget);

    QWidget *batteryContainer = new QWidget(this);
    QVBoxLayout *batteryContainerLayout = new QVBoxLayout(batteryContainer);
    batteryContainerLayout->setMargin(0);
    batteryContainerLayout->setSpacing(0);

    QWidget *lidActionBatteryContainer = new QWidget(this);
    QHBoxLayout *lidActionBatteryContainerLayout = new QHBoxLayout(lidActionBatteryContainer);
    lidActionBattery = new QComboBox(this);
    QLabel *lidActionBatteryLabel = new QLabel(this);

    lidActionBatteryLabel->setText(tr("Lid Action"));
    lidActionBatteryContainerLayout->addWidget(lidActionBatteryLabel);
    lidActionBatteryContainerLayout->addWidget(lidActionBattery);
    batteryContainerLayout->addWidget(lidActionBatteryContainer);

    QWidget *criticalActionBatteryContainer = new QWidget(this);
    QHBoxLayout *criticalActionBatteryContainerLayout = new QHBoxLayout(criticalActionBatteryContainer);
    criticalActionBattery = new QComboBox(this);
    QLabel *criticalActionBatteryLabel = new QLabel(this);

    criticalActionBatteryLabel->setText(tr("Critical Action"));
    criticalActionBatteryContainerLayout->addWidget(criticalActionBatteryLabel);
    criticalActionBatteryContainerLayout->addWidget(criticalActionBattery);
    batteryContainerLayout->addWidget(criticalActionBatteryContainer);

    QWidget *lowBatteryContainer = new QWidget(this);
    QHBoxLayout *lowBatteryContainerLayout = new QHBoxLayout(lowBatteryContainer);
    lowBattery = new QSpinBox(this);
    lowBattery->setMinimum(0);
    lowBattery->setMaximum(99);
    QLabel *lowBatteryLabel = new QLabel(this);

    lowBatteryLabel->setText(tr("Low battery (%)"));
    lowBatteryContainerLayout->addWidget(lowBatteryLabel);
    lowBatteryContainerLayout->addWidget(lowBattery);
    batteryContainerLayout->addWidget(lowBatteryContainer);

    QWidget *criticalBatteryContainer = new QWidget(this);
    QHBoxLayout *criticalBatteryContainerLayout = new QHBoxLayout(criticalBatteryContainer);
    criticalBattery = new QSpinBox(this);
    criticalBattery->setMinimum(0);
    criticalBattery->setMaximum(99);
    QLabel *criticalBatteryLabel = new QLabel(this);

    criticalBatteryLabel->setText(tr("Critical battery (%)"));
    criticalBatteryContainerLayout->addWidget(criticalBatteryLabel);
    criticalBatteryContainerLayout->addWidget(criticalBattery);
    batteryContainerLayout->addWidget(criticalBatteryContainer);

    batteryContainerLayout->addStretch();
    containerWidget->addTab(batteryContainer, tr("On Battery"));

    QWidget *acContainer = new QWidget(this);
    QVBoxLayout *acContainerLayout = new QVBoxLayout(acContainer);
    acContainerLayout->setMargin(0);
    acContainerLayout->setSpacing(0);

    QWidget *lidActionACContainer = new QWidget(this);
    QHBoxLayout *lidActionACContainerLayout = new QHBoxLayout(lidActionACContainer);
    lidActionAC = new QComboBox(this);
    QLabel *lidActionACLabel = new QLabel(this);

    lidActionACLabel->setText(tr("Lid Action"));
    lidActionACContainerLayout->addWidget(lidActionACLabel);
    lidActionACContainerLayout->addWidget(lidActionAC);
    acContainerLayout->addWidget(lidActionACContainer);

    acContainerLayout->addStretch();
    containerWidget->addTab(acContainer, tr("On AC"));

    populate();
    loadSettings();

    connect(lidActionBattery, SIGNAL(currentIndexChanged(int)), this, SLOT(handleLidActionBattery(int)));
    connect(lidActionAC, SIGNAL(currentIndexChanged(int)), this, SLOT(handleLidActionAC(int)));
    connect(criticalActionBattery, SIGNAL(currentIndexChanged(int)), this, SLOT(handleCriticalAction(int)));
    connect(lowBattery, SIGNAL(valueChanged(int)), this, SLOT(handleLowBattery(int)));
    connect(criticalBattery, SIGNAL(valueChanged(int)), this, SLOT(handleCriticalBattery(int)));
}

void Dialog::populate()
{
    lidActionBattery->clear();
    lidActionBattery->addItem(tr("None"), lidNone);
    lidActionBattery->addItem(tr("Lock Screen"), lidLock);
    lidActionBattery->addItem(tr("Sleep"), lidSleep);
    lidActionBattery->addItem(tr("Hibernate"), lidHibernate);

    lidActionAC->clear();
    lidActionAC->addItem(tr("None"), lidNone);
    lidActionAC->addItem(tr("Lock Screen"), lidLock);
    lidActionAC->addItem(tr("Sleep"), lidSleep);
    lidActionAC->addItem(tr("Hibernate"), lidHibernate);

    criticalActionBattery->clear();
    criticalActionBattery->addItem(tr("None"), criticalNone);
    criticalActionBattery->addItem(tr("Hibernate"), criticalHibernate);
    criticalActionBattery->addItem(tr("Shutdown"), criticalShutdown);
}

void Dialog::loadSettings()
{
    int defaultLowBattery = LOW_BATTERY;
    if (Common::validPowerSettings("lowBattery")) {
        defaultLowBattery = Common::loadPowerSettings("lowBattery").toInt();
    }
    setDefaultAction(lowBattery, defaultLowBattery);

    int defaultCriticalBattery = CRITICAL_BATTERY;
    if (Common::validPowerSettings("criticalBattery")) {
        defaultCriticalBattery = Common::loadPowerSettings("criticalBattery").toInt();
    }
    setDefaultAction(criticalBattery, defaultCriticalBattery);

    int defaultLidActionBattery = LID_BATTERY_DEFAULT;
    if (Common::validPowerSettings("lidBattery")) {
        defaultLidActionBattery = Common::loadPowerSettings("lidBattery").toInt();
    }
    setDefaultAction(lidActionBattery, defaultLidActionBattery);

    int defaultLidActionAC = LID_AC_DEFAULT;
    if (Common::validPowerSettings("lidAC")) {
        defaultLidActionAC = Common::loadPowerSettings("lidAC").toInt();
    }
    setDefaultAction(lidActionAC, defaultLidActionAC);

    int defaultCriticalAction = CRITICAL_DEFAULT;
    if (Common::validPowerSettings("criticalAction")) {
        defaultCriticalAction = Common::loadPowerSettings("criticalAction").toInt();
    }
    setDefaultAction(criticalActionBattery, defaultCriticalAction);
}

void Dialog::updatePM()
{
    QDBusInterface iface(PM_SERVICE, PM_PATH, PM_SERVICE, QDBusConnection::sessionBus());
    if (!iface.isValid()) {
        qDebug() << iface.lastError().message();
        return;
    }
    iface.call("refresh");
}

void Dialog::setDefaultAction(QComboBox *box, int action)
{
    for (int i=0;i<box->count();i++) {
        if (box->itemData(i).toInt() == action) {
            box->setCurrentIndex(i);
            return;
        }
    }
}

void Dialog::setDefaultAction(QSpinBox *box, int action)
{
    box->setValue(action);
}

void Dialog::handleLidActionBattery(int index)
{
    Common::savePowerSettings("lidBattery", index);
    updatePM();
}

void Dialog::handleLidActionAC(int index)
{
    Common::savePowerSettings("lidAC", index);
    updatePM();
}

void Dialog::handleCriticalAction(int index)
{
    Common::savePowerSettings("criticalAction", index);
    updatePM();
}

void Dialog::handleLowBattery(int value)
{
    Common::savePowerSettings("lowBattery", value);
    updatePM();
}

void Dialog::handleCriticalBattery(int value)
{
    Common::savePowerSettings("criticalBattery", value);
    updatePM();
}
