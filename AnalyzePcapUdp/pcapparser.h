#ifndef PCAPPARSER_H
#define PCAPPARSER_H

#include "parserfactory.h"

class PrivPCapParser;

class PCapParser : public Parser
{
public:
    PCapParser();
    ~PCapParser();

    bool open(const QString &fileName);
    int next_packet(Pkt& pkt);
    void close();
    float position();
    bool canOpen(const QString &fileName);
    virtual QString getFileName() const;

private:
    QSharedPointer<PrivPCapParser> mD;

};

#endif // PCAPPARSER_H
