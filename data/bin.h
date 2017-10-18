#ifndef BIN_H
#define BIN_H

#include <QString>

class Bin
{
public:
    Bin();

public:
    QString name;
    Bin *parent;
};

#endif // BIN_H
