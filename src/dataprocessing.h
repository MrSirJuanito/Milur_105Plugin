#ifndef DATAPROCESSING_H
#define DATAPROCESSING_H

#include <QObject>

class DataProcessing : public QObject
{
    Q_OBJECT
public:
    explicit DataProcessing(QObject *parent = 0);
    ~DataProcessing();

signals:

public slots:
    void dataParsing(int command,QByteArray data);
};

#endif // DATAPROCESSING_H
