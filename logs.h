/*
 *@file:   logs.h
 *@author: 缪庆瑞
 *@date:   2019.7.10
 *@brief:  写日志文件的工具类,采用单例模式
 * 该类自动以当前日期作为日志文件名，格式形如“yyyy-MM-dd.log”，统一保存在指定的logsDir下。
 * 为了避免随着时间增长日志文件越来越多，可以在每次程序启动时调用rmLogsFile(int retainDays)，
 * 只保留最近几天的日志而删去其他无用的日志。
 */
#ifndef LOGS_H
#define LOGS_H

#include <QDir>

class Logs
{
public:
    //单例模式
    static Logs* getInstance()
    {
        static Logs logs;
        return &logs;
    }

    void setLogsDir(const QString &dirPath);//设置日志文件目录
    void rmLogsFile(int retainDays=0);//删除日志文件
    void writeLogs(const QString &title,const QString &content);//写日志

private:
    Logs();//私有构造函数

    QDir logsDir;//日志文件目录
};

#endif // LOGS_H
