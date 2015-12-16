#ifndef MILUR_105PLUGIN_H
#define MILUR_105PLUGIN_H

#include "D:/Work/Coding/uspd_ssd/src/idevice.h"
#include "connectserial.h"
#include "crc.h"

#define ADDRESS_ONE 6
#define ADDRESS_FOUR 9
#define A_OPEN 8
#define A_RELEASE 9
#define ADDRESS_COUNTER 255

class Milur_105Plugin : public QObject, public IDevice
{

Q_OBJECT
Q_PLUGIN_METADATA(IID "com.pro-prof.Milur_105Plugin")
Q_INTERFACES(IDevice)

//IDevice
public:
    void initialize(GlobalStructs::deviceInterviews handler);
    bool connect();
    double disconnect();
    double sendData(int command);

public slots:
    double getData(QByteArray data);

//own
private:
    bool mStatus;
    int mErrors, mStatusPacket;
    ConnectSerial *serial;
    QList<int> mCommandList;
    QByteArray mDataCounter, mAnswerData;

    double mDataParsing(QByteArray data);
    void mComposeAuthorization();
    QByteArray mComposePacket(int command);
    void mComposeAnswerPacket(QByteArray &data, int stat, int check);
    void mComposePacketForProcessor(QByteArray &data);
    QByteArray mComposeCloseSession();
    void mReadCommandList();
    void mSetDateTime(QByteArray&);
    bool mCheckCRC(QByteArray &data);
    void mMakeCRC(QByteArray &data);
    double mGetData(QByteArray data);
};

#endif // MILUR_105PLUGIN_H
