/*
 *@author: 缪庆瑞
 *@date:   2019.7.10
 *@brief:  写日志文件的工具类,采用单例模式
 * 该类自动以当前日期作为日志文件名，格式形如“yyyy-MM-dd.log”，统一保存在指定的logsDir下。
 * 为了避免随着时间增长日志文件越来越多，可以在每次程序启动时调用rmLogsFile(int retainDays)，
 * 只保留最近几天的日志而删去其他无用的日志。
 */
#include "logs.h"
#include <QFile>
#include <QTextStream>
#include <QMutexLocker>
#include <QDate>
#include <QTime>
#include <QDebug>

#define LOGS_DIR_PATH "./logs/"

Logs::Logs()
{
    setLogsDir(LOGS_DIR_PATH);
}
/*
 *@brief:   设置日志文件目录
 *@author:  缪庆瑞
 *@date:    2019.7.10
 *@param:   dirPath:日志文件目录路径，相对或绝对路径
 */
void Logs::setLogsDir(const QString &dirPath)
{
    QMutexLocker locker(&mutex);//线程互斥，避免多线程同时操作文件
    logsDir.setPath(dirPath);
    //不存在则新建目录,使用mkpath递归创建(mkdir只能往下创建一级)。
    if(!logsDir.exists())
    {
        /* 因为mkpath在创建相对路径时会结合QDir当前对象的路径(无论路径是否存在)，所以这里
         * 将logsDir的路径先设置为"./",避免嵌套创建目录。*/
        logsDir.setPath("./");
        if(!logsDir.mkpath(dirPath))
        {
            qDebug()<<"(setLogsDir)Failed to create dir:"<<dirPath;
            //如果路径创建失败，则默认在程序当前路径下创建Logs目录作为日志目录
            logsDir.mkpath("./logs");
            logsDir.setPath("./logs");
            return;
        }
        logsDir.setPath(dirPath);
    }
}
/*
 *@brief:   删除日志目录(logsDir)下的日志文件
 *@author:  缪庆瑞
 *@date:    2019.7.10
 *@param:   retainDays:保留最近多少天的日志记录
 */
void Logs::rmLogsFile(int retainDays)
{
    QMutexLocker locker(&mutex);//线程互斥，避免多线程同时操作文件
    //获取日志目录下所有的日志文件名，并按名称排序
    QStringList logsFileList =
            logsDir.entryList(QDir::NoDotAndDotDot | QDir::Files,QDir::Name);
    //qDebug()<<"(rmLogsFile)logsFileList:"<<logsFileList;
    int logsFileNum = logsFileList.size();
    if(logsFileNum<=retainDays)
    {
        return;
    }
    else
    {
        for(int i=0;i<logsFileNum-retainDays;i++)
        {
            logsDir.remove(logsFileList.at(i));
        }
    }
}
/*
 *@brief:   写日志文件，filename以当前日期命名
 *@author:  缪庆瑞
 *@date:    2019.7.10
 *@param:   content:日志内容
 *@param:   logsLevel:日志级别，默认为普通info，只存文件不打印信息到终端
 */
void Logs::writeLogs(const QString &content, LogsLevel logsLevel)
{
    QString logsLevelStr;
    switch (logsLevel) {
    case DEBUG_LEVEL:
        qDebug()<<content;//仅打印输出，不写日志
        return;
    case INFO_LEVEL:
        logsLevelStr = "INFO";
        break;
    case WARN_LEVEL:
        logsLevelStr = "WARN";
        qDebug()<<content;//方便调试
        break;
    case ERROR_LEVEL:
        logsLevelStr = "ERROR";
        qDebug()<<content;//方便调试
        break;
    default:
        qDebug()<<"(writeLogs)logsLevel is error.";
        return;
    }
    QMutexLocker locker(&mutex);//线程互斥，避免多线程同时操作文件
    QDate currentDate = QDate::currentDate();
    QString fileName = QString(logsDir.path()+"/"+currentDate.toString("yyyy-MM-dd")+".log");
    //qDebug()<<"(writeLogs)logsFileName:"<<fileName;
    QFile file(fileName);
    if(!file.open(QIODevice::Append))//以追加的方法写文件
    {
        qDebug()<<"(writeLogs)Failed to open file:"<<fileName;
        return;
    }
    QString currentTime = QTime::currentTime().toString("HH:mm:ss ");
    QTextStream out(&file);//创建文本流写文件
    out<<currentTime<<logsLevelStr<<":"<<content<<"\n";//window下的换行即\r\n,linux为\n
    file.close();
}


