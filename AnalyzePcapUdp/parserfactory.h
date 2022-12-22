#ifndef PARSERFACTORY_H
#define PARSERFACTORY_H

#include <QObject>
#include <QScopedPointer>
#include <QSharedPointer>

#ifdef _MSC_VER
#include <WinSock2.h>
#else
#include <sys/time.h>
#endif

struct Pkt{
    struct timeval ts;	/* time stamp */
    uint32_t caplen;	/* length of portion present */
    uint32_t len;	/* length this packet (off wire) */
    std::vector<uint8_t> data;
};

class Parser{
public:
    virtual bool open(const QString& fileName) = 0;
    virtual int next_packet(Pkt& pkt) = 0;
    virtual void close() = 0;
    virtual float position() = 0;
    virtual bool canOpen(const QString& fileName) = 0;
    virtual QString getFileName() const = 0;

};

class ParserFactory
{
public:
    ParserFactory();

    static QSharedPointer<Parser> getParser(const QString& fileName);
};

#endif // PARSERFACTORY_H
