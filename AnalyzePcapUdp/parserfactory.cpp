#include "parserfactory.h"

#include "pcapparser.h"
#include "pcapngparser.h"

ParserFactory::ParserFactory()
{

}

QSharedPointer<Parser> ParserFactory::getParser(const QString &fileName)
{
    QSharedPointer<Parser> fact[] = {
        QSharedPointer<PCapParser>(new PCapParser()),
        QSharedPointer<PCapNGParser>(new PCapNGParser()),
    };

    for(auto it: fact){
        if(it->canOpen(fileName))
            return it;
    }
    return nullptr;
}
