#include <QCoreApplication>
#include "logs.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    Logs *log = Logs::getInstance();
    log->rmLogsFile(7);
    log->writeLogs(LOGS_CODE_POS,"logs debug test");
    log->writeLogs(LOGS_CODE_POS,"logs info test",Logs::LEVEL_INFO);
    log->writeLogs(LOGS_CODE_POS,"logs warn test",Logs::LEVEL_WARN);
    log->closeLogsFile();
    log->writeLogs(LOGS_CODE_POS,"logs error test",Logs::LEVEL_ERROR);

    //return a.exec();
}
