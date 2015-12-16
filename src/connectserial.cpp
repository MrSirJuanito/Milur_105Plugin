#include "connectserial.h"
#include <QSettings>
#include <QDebug>
#include <QThread>

ConnectSerial::ConnectSerial(QObject *parent)
{
    Q_UNUSED(parent);
    mSerialCounter = new QSerialPort(this);
}

ConnectSerial::~ConnectSerial()
{
    mSerialCounter->close();
    mSerialCounter->deleteLater();
}

void ConnectSerial::setConfiguration(QString type)
{
    mConfigurationPath.clear();
    mConfigurationPath = type + ".cfg";

    QSettings *settings = new QSettings(mConfigurationPath, QSettings::IniFormat);

    settings->beginGroup("Common");
    mPortName = settings->value("port").toString();
    mDataBits = static_cast<QSerialPort::DataBits>(settings->value("dataBits").toInt());
    mBaudRate = settings->value("baudRate").toInt();
    mParity = static_cast<QSerialPort::Parity>(settings->value("parity").toInt());
    mStopBits = static_cast<QSerialPort::StopBits>(settings->value("stopBits").toInt());
    mFlowControl = static_cast<QSerialPort::FlowControl>(settings->value("flowControl").toInt());
    settings->endGroup();

    settings->beginGroup("Serial");
    mDeathDoors = settings->value("deathDoors").toInt();
    mWait = settings->value("wait").toInt();
    mWaitForRead = settings->value("waitForRead").toInt();
    mWaitForWritten = settings->value("waitForWritten").toInt();
    settings->endGroup();

    qDebug() << "Milur_105Plugin(SerialPort): settings serial."
             << "mwait =" << mWait
             << "mdeathdoors =" << mDeathDoors
             << "mwaitforread =" << mWaitForRead
             << "mwaitforwritten =" << mWaitForWritten;

    delete settings;
}

void ConnectSerial::setAnswerSize(int size)
{
    mAnswerPacketSize = size;
}

bool ConnectSerial::connectPort()
{
    QThread::sleep(mWait);
    mSerialCounter->setPortName(mPortName);
    mSerialCounter->setDataBits(mDataBits);
    mSerialCounter->setBaudRate(mBaudRate);
    mSerialCounter->setParity(mParity);
    mSerialCounter->setFlowControl(mFlowControl);
    if (mSerialCounter->open(QIODevice::ReadWrite))
        {
            qDebug() << "Milur_105Plugin(SerialPort): connection."
                     << mPortName << " port is opened.";
            mStatus = true;
            return true;
        }
    else
        {
            qDebug() << "Milur_105Plugin(SerialPort): connection."
                     << mPortName << " port doesn't exist or busy.";
            mStatus = false;
            return false;
    }
}

void ConnectSerial::disconnectPort()
{
    QThread::sleep(mWait);
    mSerialCounter->close();
    mStatus = false;
}

QByteArray ConnectSerial::writeData(QByteArray data)
{
    QThread::sleep(mWait);
    QByteArray temp;
    temp.clear();
    temp = data;
    if (mStatus)
        {
            mSerialCounter->write(temp);
            mSerialCounter->waitForBytesWritten(mWaitForWritten);
            qDebug() << "Milur_105Plugin(SerialPort): writing data to port"
                     << mPortName;
            return readData();
        }
    qDebug() << "Milur_105Plugin(SerialPort): writing."
             << mPortName << " port doesn't open or exist.";
    return QByteArray();
}

bool ConnectSerial::getStatus()
{
   return mStatus;
}

QByteArray ConnectSerial::readData()
{
    int i = 0;
    QByteArray temp;
    do
        {
            //qDebug() << "Notification::waiting for data...";
            mSerialCounter->waitForReadyRead(mWaitForRead);
            temp.append(mSerialCounter->readAll());
            //qDebug() << "Notification::data to HEX - " << QString(temp.toHex());

                if (i == mAnswerPacketSize + mDeathDoors)
                    {
                     break;
                    }
            i++;
        }
    while(temp.size() != mAnswerPacketSize);
    qDebug() << "Milur_105Plugin(SerialPort): data read" << QString(temp.toHex());
    return temp;
}
