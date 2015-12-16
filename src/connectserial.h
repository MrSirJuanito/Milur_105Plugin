#ifndef CONNECTSERIAL_H
#define CONNECTSERIAL_H

#include <QObject>
#include <QtSerialPort/QSerialPort>

class ConnectSerial : public QObject
{
Q_OBJECT
public:
    explicit ConnectSerial(QObject *parent = 0);
    ~ConnectSerial();

    void setConfiguration(QString path);
    void setAnswerSize(int size);
    bool connectPort();
    void disconnectPort();
    QByteArray writeData(QByteArray data);
    QByteArray readData();
    bool getStatus();

private:
    QSerialPort*                mSerialCounter;
    bool                        mStatus;
    QString                     mConfigurationPath;
    QString                     mPortName;
    int                         mAnswerPacketSize;
    qint32                      mBaudRate;
    QSerialPort::DataBits       mDataBits;
    QSerialPort::Parity         mParity;
    QSerialPort::StopBits       mStopBits;
    QSerialPort::FlowControl    mFlowControl;

    int                         mDeathDoors,
                                mWait,
                                mWaitForRead,
                                mWaitForWritten;
};

#endif // CONNECTSERIAL_H
