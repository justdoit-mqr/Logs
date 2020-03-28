/*
 *@author: 缪庆瑞
 *@date:   2019.7.10
 *@update:  2020.3.28
 *@brief:  写日志文件的工具类
 */
#include "logs.h"
#include <QFile>
#include <QTextStream>
#include <QMutexLocker>
#include <QDate>
#include <QTime>

#define LOGS_DIR_PATH "./logs/"

//初始化互斥锁(允许同一线程多次加锁)
QMutex Logs::mutex(QMutex::Recursive);

/*
 *@brief:   私有构造，内部会设置默认的日志目录
 *@author:  缪庆瑞
 *@date:    2019.7.10
 */
Logs::Logs()
{
    setLogsDir(LOGS_DIR_PATH);
}
/*
 *@brief:   单例模式，获取实例对象(懒汉式,静态局部对象,自动完成资源释放)
 *@author:  缪庆瑞
 *@date:    2020.3.28
 */
Logs *Logs::getInstance()
{
    /* C++ 11标准规定了编译器要保证静态局部变量的线程安全性，即在一个线程开始静态局部对象的初始化后
     * 到完成初始化前，其他线程执行到这个静态局部对象的初始化语句就会等待，直到该对象初始化完成。但
     * 对于C++ 11之前的标准(比如我们这里的arm-fsl-linux-gnueabi-g++编译器用的是c99)还是需要手动加锁来
     * 保证线程安全*/
    QMutexLocker locker(&mutex);//线程互斥，避免多线程同时实例化单例对象
    static Logs instance;//静态局部对象第一次调用该方法时才初始化且只实例化一次
    return &instance;
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
            qDebug()<<"setLogsDir():Failed to create dir "<<dirPath;
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
            logsDir.entryList(QDir::Files,QDir::Name);
    //qDebug()<<"rmLogsFile():logsFileList is"<<logsFileList;
    //先删除大于当前日期(不符合常理)的日志文件
    QString currentDateLog = QDate::currentDate().toString("yyyy-MM-dd'.log'");
    for(int i=0;i<logsFileList.size();i++)
    {
        if(currentDateLog.compare(logsFileList.at(i))<0)
        {
            logsDir.remove(logsFileList.at(i));
            logsFileList.removeAt(i);
            i--;
        }
    }
    //删除指定保留以外的日志文件
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
        qDebug()<<"writeLogs():logsLevel is error.";
        return;
    }
    QMutexLocker locker(&mutex);//线程互斥，避免多线程同时操作文件
    QDate currentDate = QDate::currentDate();
    QString fileName = QString(logsDir.path()+"/"+currentDate.toString("yyyy-MM-dd")+".log");
    //qDebug()<<"writeLogs():logsFileName is"<<fileName;
    QFile file(fileName);
    if(!file.open(QIODevice::Append))//以追加的方法写文件
    {
        qDebug()<<"writeLogs():Failed to open file "<<fileName;
        return;
    }
    QString currentTime = QTime::currentTime().toString("HH:mm:ss ");
    QTextStream out(&file);//创建文本流写文件
    out<<currentTime<<logsLevelStr<<":"<<content<<"\n";//window下的换行即\r\n,linux为\n
    file.close();
}
/*
 *@brief:   打印当前时间，主要为了测试某个函数的执行时间，不写文件
 *@author:  缪庆瑞
 *@date:    2019.10.17
 *@param:   title:时间标题
 */
void Logs::printCurrentTime(const QString &title)
{
    qDebug()<<title<<QTime::currentTime().toString("mm:ss:zzz");
}

