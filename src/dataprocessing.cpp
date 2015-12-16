 #include "dataprocessing.h"
#include <QDataStream>
#include <QDebug>
#include <math.h>

DataProcessing::DataProcessing(QObject *parent) : QObject(parent)
{

}

DataProcessing::~DataProcessing()
{

}

void DataProcessing::dataParsing(int command, QByteArray data)
{
    switch (command){
    case 1: // Voltage 3
        {
            double doubleTmp = 0;
            QByteArray tmpArr,t;
            t.append(data[2]);
            t.append(data[1]);
            t.append(data[0]);
            tmpArr=t.toHex();
            bool ok;
            uint VALERA = tmpArr.toUInt(&ok, 16);
            //int VASA = (ushort) VALERA;
            doubleTmp = (double)((int)VALERA) / 1000;                     //BOLT
            qDebug() << "Voltage: " << doubleTmp<< endl;
            break;
        }
    case 2: // Current 3
        {
            double doubleTmp = 0;
            QByteArray tmpArr,t;
            t.append(data[2]);
            t.append(data[1]);
            t.append(data[0]);
            tmpArr=t.toHex();
            bool ok;
            uint VALERA = tmpArr.toUInt(&ok, 16);
            int VASA = (short) VALERA;                                //MELEEEMPIRE
            doubleTmp = (double)(VASA) / 1000;                //EMPIRE

            qDebug() << "Current: " << doubleTmp << endl;
            break;
        }
    case 3: // Active power 4
    case 77:// Reactive power 4
    case 78: //Power 4
        {
            double doubleTmp = 0;
            QByteArray tmpArr,t;
            t.append(data[3]);
            t.append(data[2]);
            t.append(data[1]);
            t.append(data[0]);
            tmpArr=t.toHex();
            bool ok;
            uint VALERA = tmpArr.toUInt(&ok, 16);
            int VASA = (int) VALERA;                                //MELEEBOLT
            doubleTmp = (double)(VASA) / 1000;                      //BOLT
            qDebug() << "Active power: " << doubleTmp << endl;
            break;
        }
    case 4: // Active energy 0 4
    case 5: // Active energy 1 4
    case 6: // Active energy 2 4
    case 7: // Active energy 3 4
    case 8: // Active energy 4 4
    case 72: //Reactive energy 0 4
    case 73: //Reactive energy 1 4
    case 74: //Reactive energy 2 4
    case 75: //Reactive energy 3 4
    case 76: //Reactive energy 4 4
           {
            double doubleTmp = 0;
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
                doubleTmp +=  tmpInt2 * pow(10, (j << 1) - 2) + tmpInt3 * pow(10, (j << 1) - 1);
            }

            qDebug() << "Energy: " << doubleTmp << endl;
            break;
        }
    case 10: //Current tariff 1
        {
            double doubleTmp = 0;
            /////////////////////
            // HERE COMES THE PARSING (QBYTEARRAY -> VALUE), IF NEEDED
            QByteArray tmpArr = data.toHex();
            bool ok;
            uint VASA = tmpArr.toUInt(&ok, 16);
            doubleTmp = (double)(VASA);
            /////////////////////
            qDebug() << "Current tariff: " << doubleTmp << endl;
            break;
        }
    case 57: //CDB: Reserve battery 2
        {
            double doubleTmp = 0;
            QByteArray tmpArr = data.toHex();
            bool ok;
            uint VASA = tmpArr.toUInt(&ok, 16);                     //MELEEBOLT
            doubleTmp = (double)(VASA) / 1000;                      //BOLT
            qDebug() << "Reserve battery: " << doubleTmp << endl;
            break;
        }
    case 80: //cos fi
        {
            double doubleTmp = 0;
            QByteArray tmpArr = data.toHex();
            bool ok;
            uint VASA = tmpArr.toUInt(&ok, 16);
            doubleTmp = (double)((short)VASA) / 1000;
            qDebug() << "cos fi: " << doubleTmp << endl;
            break;
        }
    case 84: //relay
        {
            qDebug()<<"RELEEE ON OR OF?!";
            break;
        }
    case 100: //Voltage A
    case 101: //Voltage B
    case 102: //Voltage C
        {
            qDebug()<<"RELEEE ON OR OF?!";
            break;
        }
    case 106://Active power A
    case 107://Active power B
    case 108://Active power C
    case 109://Active power
    case 110://Reactive power A
    case 111://Reactive power B
    case 112://Reactive power C
    case 113://Reactive power
    case 114://Power A
    case 115://Power B
    case 116://Power C
    case 117://Power
        {
            double doubleTmp = 0;
            QByteArray tmpArr = data.toHex();
            bool ok;
            uint VALERA = tmpArr.toUInt(&ok, 16);
            int VASA = (int) VALERA;                                //MELEEBOLT
            doubleTmp = (double)(VASA) / 1000;                      //BOLT
            qDebug() << "Active power: " << doubleTmp << endl;
            break;
        }

    }
}

