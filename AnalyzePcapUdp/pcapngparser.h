#ifndef PCAPNGPARSER_H
#define PCAPNGPARSER_H

#include "parserfactory.h"

class PrivPCapNg;

class PCapNGParser : public Parser
{
public:
    PCapNGParser();
    ~PCapNGParser();

    bool open(const QString &fileName);
    int next_packet(Pkt &pkt);
    void close();
    float position();
    bool canOpen(const QString &fileName);
    virtual QString getFileName() const;

private:
    QSharedPointer<PrivPCapNg> mD;
};

#endif // PCAPNGPARSER_H
