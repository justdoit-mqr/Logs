#include <QCoreApplication>
#include "logs.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    Logs *log = Logs::getInstance();
    log->rmLogsFile();
    log->writeLogs("test","log test;");

    //return a.exec();
}
