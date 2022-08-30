/*
 *@author: 缪庆瑞
 *@date:   2019.7.10
 *@update:  2022.8.30
 *@brief:  写日志文件的工具类，采用单例模式，内部通过加锁实现线程安全
 * 1.该类自动以当前日期作为日志文件名，格式形如“yyyy-MM-dd.log”，统一保存在指定的logsDir下。
 * 2.为了避免随着时间增长日志文件越来越多，可以在每次程序启动时调用rmLogsFile(int retainDays)，
 * 只保留最近几天的日志而删去其他无用的日志。
 * 3.该类根据日志级别决定写文件与否，同时也会根据日志级别选择将信息输出到终端方便调试。
 * 4.该类功能比较简单，写文件处理未做优化，仅适用于极小体量日志的记录，其他场景请使用成熟的log库
 */
#ifndef LOGS_H
#define LOGS_H

#include <QDir>
#include <QMutex>

//记录log输出的代码位置，在调用writeLogs()接口时作为参数传递即可
#define LOGS_CODE_POS QString("[%1::%2().%3]").arg(__FILE__).arg(__FUNCTION__).arg(__LINE__)

class Logs
{
public:
    //日志级别
    enum LogsLevel
    {
        LEVEL_DEBUG=0,//调试输出，不写文件
        LEVEL_INFO,//普通日志，只写文件
        LEVEL_WARN,//警告信息，写文件同时输出到终端
        LEVEL_ERROR//错误信息，写文件同时输出到终端
    };
    //单例模式
    static Logs* getInstance();

    void setLogsDir(const QString &dirPath);//设置日志文件目录
    void rmLogsFile(int retainDays=0);//删除日志文件
    void writeLogs(const QString &logsCodePos,const QString &logsContent,
                   LogsLevel logsLevel=LEVEL_DEBUG);//写日志

private:
    Logs();//构造函数私有化
    Logs(const Logs &);//拷贝构造私有化,不定义
    Logs & operator=(const Logs &);//拷贝赋值运算符私有化，不定义

    static QMutex mutex;//互斥锁
    QDir logsDir;//日志文件目录

};

#endif // LOGS_H
