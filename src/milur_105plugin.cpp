#include "milur_105plugin.h"
#include <QSettings>
#include <qplugin.h>

Milur_105Plugin::~Milur_105Plugin()
{
    deleteLater();
}

void Milur_105Plugin::initialize(GlobalStructs::deviceInterviews handler)
{
    this->handler = handler;
    serial = new ConnectSerial();
    serial->setConfiguration(handler.type);
    mReadCommandList();
    status = false;
}

bool Milur_105Plugin::connect()
{
    if( status ) return true;
    if(serial->connectPort())
        {
            mStatusPacket=1;
            status=true;
            mComposeAuthorization();
            return true;
        }
    else
        {
            status=false;
            mStatusPacket=-1;
            return false;
        }
}

double Milur_105Plugin::disconnect()
{
    if(!status)
        {
            mStatusPacket=-1;
            serial->disconnectPort();
            mComposeAnswerPacket(mAnswerData,2,mErrors);
            return 0;
        }
   mStatusPacket=5;
   mComposeCloseSession();
   //mDataParsing(mComposeCloseSession());
   serial->disconnectPort();
   status = false;
   return 0;
}

double Milur_105Plugin::getData()
{
    qDebug() << "Milur_105Plugin: getData executed. NOT IMPLEMENTED!";
    return 0;
}

double Milur_105Plugin::sendData(int command)
{
    if (status)
        {
            mStatusPacket=3;
            serial->setAnswerSize(mCommandList.value(command-1));
            return mGetData(mComposePacket(command));
            qDebug() << "Milur_105Plugin: data sending. device =" <<
                        index << "address =" <<
                        handler.address << "command =" << command;

        }

     mStatusPacket=-1;
     qDebug() << "Milur_105Plugin: data sending. device ="
              << index << "address ="
              << handler.address << "session doesn't open";
     return 0;
}

double Milur_105Plugin::mGetData(QByteArray data)
{
    QByteArray temp;
    temp.clear();
    temp.append(data);

    /*
    if( !handler.counter )
    {
        qDebug() << "Device::getDataFromDevice, sensor data:" << QString(data);
        emit sensorDataReady(this, data.toDouble());
        return;
    }
    */

    switch (mStatusPacket) {
    case 1:
    {
        if (mCheckCRC(temp))
            {
                qDebug() << "Milur_105Plugin: opening. address =" <<
                            handler.address << " is opened.";
            }
        else
            {
                qDebug() << "Milur_105Plugin: opening error. address =" <<
                            handler.address << " doesn't exist or bad crc.";
                status = false;
            }
       break;
    }
    case 3:
    {
        if (mCheckCRC(temp))
            {
                mAnswerData.clear();
                mAnswerData.append(temp);
                qDebug() << "Milur_105Plugin: data received. device ="
                         << index << "address ="
                         << handler.address << "response ="
                         << QString(mAnswerData.toHex());
                mComposePacketForProcessor(mAnswerData);
                return mDataParsing(mAnswerData);
            }
        else
            {
                qDebug() << "Milur_105Plugin: data receiving error. device ="
                         << index << "address ="
                         << handler.address
                         << "doesn't exist or bad crc.";
                status = false;
            }
       break;
    }
    case 5:
    {
        if (mCheckCRC(temp))
            {
                 qDebug() << "Milur_105Plugin: closing. device ="
                          << index << "address ="
                          << handler.address << "session closed.";
                 //mComposeAnswerPacket(mAnswerData, 1, mErrors);
                 //mComposePacketForProcessor(mAnswerData);
                 //emit deviceDataReady(mCurCommand, mAnswerData);
            }
        else
            {
                 qDebug() << "Milur_105Plugin: closing error. device ="
                          << index << "address ="
                          << handler.address << "session closed with problem.";
                 //mComposeAnswerPacket(mAnswerData, 1, mErrors);
                 //mComposePacketForProcessor(mAnswerData);
                 //emit deviceDataReady(mCurCommand, mAnswerData);
            }
        break;
    }
    }

    return 0;
    //error code return
}

void Milur_105Plugin::mComposeAuthorization()
{
    if(handler.address <= ADDRESS_COUNTER)
        {
            serial->setAnswerSize(ADDRESS_ONE-1);
            mDataCounter.clear();
            mDataCounter[0] = handler.address;
            mDataCounter[1] = A_OPEN;
            mDataCounter[2] = handler.access;
            mDataCounter.append(handler.pass);
            mMakeCRC(mDataCounter);
            qDebug() << "Milur_105Plugin: sending connection packet"
                     << mDataCounter.toHex();
            serial->writeData(mDataCounter);
        }
    else
        {
            unsigned short FOUR_;
            FOUR_ = handler.address;
            serial->setAnswerSize(ADDRESS_FOUR-1);
            mDataCounter.clear();
            mDataCounter[0] = (int) FOUR_;
            mDataCounter[1] = (int) FOUR_ >> 8;
            mDataCounter[2] = 0x00;
            mDataCounter[3] = 0x00;
            mDataCounter[4] = A_OPEN;
            mDataCounter[5] = handler.access;
            mDataCounter.append(handler.pass);
            mMakeCRC(mDataCounter);
            qDebug() << "Milur_105Plugin: sending connection packet"
                     << mDataCounter.toHex();
            serial->writeData(mDataCounter);
    }
}

QByteArray Milur_105Plugin::mComposePacket(int command)
{
    curCommand = command;
    if(handler.address <= ADDRESS_COUNTER)
        {
            serial->setAnswerSize(mCommandList.value(curCommand-1)+ADDRESS_ONE);
            mDataCounter.clear();
            mDataCounter[0] = handler.address;
            mDataCounter[1] = handler.access;
            mDataCounter[2] = curCommand;
            mMakeCRC(mDataCounter);
            qDebug() << "Milur_105Plugin: sending command" << curCommand << "packet ="
                     << mDataCounter.toHex();
            return serial->writeData(mDataCounter);
        }
    else
        {
            unsigned short FOUR_;
            FOUR_ = handler.address;
            serial->setAnswerSize(mCommandList.value(curCommand-1)+ADDRESS_FOUR);
            mDataCounter.clear();
            mDataCounter[0] = (int) FOUR_;
            mDataCounter[1] = (int) FOUR_ >> 8;
            mDataCounter[2] = 0x00;
            mDataCounter[3] = 0x00;
            mDataCounter[4] = handler.access;
            mDataCounter[5] = curCommand;
            mMakeCRC(mDataCounter);
            qDebug() << "Milur_105Plugin: sending command" << curCommand << "packet ="
                     << mDataCounter.toHex();
            return serial->writeData(mDataCounter);
        }
}

void Milur_105Plugin::mComposeAnswerPacket(QByteArray &data, int stat, int check)
{
    Q_UNUSED(check);

    QByteArray temp,tempData;
    tempData.clear();
    int DATA_SIZE,CRC_16,SIZE_BUFF;
    CRC_16=2;
    DATA_SIZE = mCommandList.value(curCommand-1);
    temp.clear();
    switch (stat) {
    case 1:
    {
        temp[0] = handler.address; // в будущем 4 байта
        temp[1] = 0x0e;//длинна сервисного поля
        temp[2] = 0x00; //код ошибки
        mSetDateTime(tempData);
        temp.append(tempData);
        temp[16]=DATA_SIZE;//длинна данных
        temp.append(data.right(DATA_SIZE+CRC_16));
        SIZE_BUFF=temp.length();
        data.clear();
        data.append(temp.left(SIZE_BUFF-CRC_16));
        qDebug() << "Milur_105Plugin: composing answer packet" << data.toHex();
        break;
    }
    case 2:
    {
        temp[0] = handler.address;
        temp[1] = 0x0e;//длинна сервисного поля
        temp[2] = 0x00; //код ошибки
        mSetDateTime(tempData);
        temp.append(tempData);
        temp[16] = DATA_SIZE;//длинна данных
        for(int i=0; i < DATA_SIZE; i++)
            {
                temp.append(QByteArray("\xff",1));
            }
        data.clear();
        data.append(temp);
        qDebug() << "Milur_105Plugin: composing answer packet" << data.toHex();
        break;
    }
    }
}

void Milur_105Plugin::mComposePacketForProcessor(QByteArray &data)
{
    QByteArray temp;
    temp.clear();
    int DATA_SIZE,CRC_16;
    CRC_16=2;
    DATA_SIZE = mCommandList.value(curCommand-1);
    temp.append(data.right(DATA_SIZE+CRC_16));
    data.clear();
    data.append(temp.left(DATA_SIZE));
    qDebug() << "Device::mComposePacketForProcessor" << data.toHex();
}

void Milur_105Plugin::mComposeCloseSession()
{
    if(handler.address <= ADDRESS_COUNTER)
        {
            serial->setAnswerSize(ADDRESS_ONE-1);
            mDataCounter.clear();
            mDataCounter[0] = handler.address;
            mDataCounter[1] = A_RELEASE;
            mDataCounter[2] = handler.access;
            mMakeCRC(mDataCounter);
            qDebug() << "Notification:: Send disconnect packet: "
                     << mDataCounter.toHex();
            serial->writeData(mDataCounter);
        }
    else
        {
            unsigned short FOUR_;
            FOUR_ = handler.address;
            serial->setAnswerSize(ADDRESS_FOUR-1);
            mDataCounter.clear();
            mDataCounter[0] = (int) FOUR_;
            mDataCounter[1] = (int) FOUR_ >> 8;
            mDataCounter[2] = 0x00;
            mDataCounter[3] = 0x00;
            mDataCounter[4] = A_RELEASE;
            mDataCounter[5] = handler.access;
            mMakeCRC(mDataCounter);
            qDebug() << "Notification:: Send disconnect packet: "
                     << mDataCounter.toHex();
            serial->writeData(mDataCounter);
        }

}

void Milur_105Plugin::mReadCommandList()
{
    mCommandList.clear();
    QString path = handler.type + ".cfg";
    QSettings *settings = new QSettings(path, QSettings::IniFormat);

    int size = settings->beginReadArray("Commands");
    for (int i = 0; i < size; ++i)
        {
            settings->setArrayIndex(i);
            mCommandList << settings->value("length").toInt();
        }
    settings->endArray();

    delete settings;
}

bool Milur_105Plugin::mCheckCRC(QByteArray &temp)
{
    QByteArray tempWihtOutCrc;
    int lenght=0;
    tempWihtOutCrc.clear();
    lenght=temp.length();
    tempWihtOutCrc=temp.left(lenght-2);
    mMakeCRC(tempWihtOutCrc);
    if(tempWihtOutCrc!=temp)
        {
            return false;
        }
    else
        {
            return true;
        }
}

void Milur_105Plugin::mMakeCRC(QByteArray &data)
{
    int lenght = data.length();
    QByteArray crcCalc;
    QByteArray dataWihtOutCrc = data.left(lenght);
    unsigned short crc_16Modbus;
    initCrc16Tab();
    crc_16Modbus  = 0xffff;

    for ( int i = 0; i < dataWihtOutCrc.length(); ++i )
        {
            crc_16Modbus = updateCrc_16( crc_16Modbus, dataWihtOutCrc[i] );
        }
    crcCalc[0] = (int) crc_16Modbus;
    crcCalc[1] = (int) crc_16Modbus >> 8;
    data.append(crcCalc[0]);
    data.append(crcCalc[1]);
}

void Milur_105Plugin::mSetDateTime(QByteArray &data)
{
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_5_4);
    stream<<QDateTime::currentDateTime();
    stream.unsetDevice();
}

double Milur_105Plugin::mDataParsing(QByteArray data)
{
    //Reading configure
    QString path = handler.type + ".cfg";
    QSettings *settings = new QSettings(path, QSettings::IniFormat);

    bool energy, bytesInvert, typeConvert;
    int div;
    QString name;
    settings->beginGroup(QString::number(curCommand));
    energy = settings->value("energy", false).toBool();
    bytesInvert = settings->value("bytesInvert", false).toBool();
    typeConvert = settings->value("typeConvert", false).toBool();
    div = settings->value("div", 1).toInt();
    name = settings->value("name", "Unnamed parameter").toString();
    settings->endGroup();

    double dparam = 0;
    if( energy )
    {
        uint tmpInt1;
        QDataStream stream(data);
        stream >> tmpInt1;

        quint8 tmpIntArr[4];
        quint8 tmpInt2, tmpInt3;
        for (int j = 4 - 1; j >= 0 ; j--)
        {

            tmpIntArr[j] = tmpInt1 % 0x0100;
            tmpInt1 = tmpInt1 >> 8;
            tmpInt2 = tmpIntArr[j] >> 4;
            tmpInt3 = tmpIntArr[j] % 16;
            dparam +=  tmpInt2 * pow(10, (j << 1) - 2) + tmpInt3 * pow(10, (j << 1) - 1);
        }
    }
    else
    {
        QByteArray tmp1;
        if( bytesInvert )
        {
            QByteArray tmp2;
            if( data.length() > 0 )
            for( int i = data.length()-1; i >= 0; i-- )
                tmp2.append(data[i]);
            tmp1 = tmp2.toHex();
        }
        else
            tmp1 = data.toHex();

        bool ok;
        uint udata = tmp1.toUInt(&ok, 16);

        int idata;
        if( typeConvert )
            idata = (short)udata;
        else
            idata = (int)udata;

        dparam = (double)(idata) / div;
    }

    qDebug() << name + ":" << dparam;
    return dparam;
}
