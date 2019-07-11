#include <QCoreApplication>
#include "logs.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    Logs *log = Logs::getInstance();
    log->rmLogsFile(7);
    log->writeLogs("logs info test");
    log->writeLogs("logs debug test",Logs::DEBUG_LEVEL);
    log->writeLogs("logs warn test",Logs::WARN_LEVEL);

    //return a.exec();
}
