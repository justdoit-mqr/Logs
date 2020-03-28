# Logs
这是一个写日志文件的工具类，采用单例模式，内部通过加锁实现线程安全
## 功能概述：
该类自动以当前日期作为日志文件名，格式形如“yyyy-MM-dd.log”，统一保存在指定的logsDir下。为了避免随着时间增长日志文件越来越多，可以在每次程序启动时调用rmLogsFile(int retainDays)，只保留最近几天的日志而删去其他无用的日志。  
```
/*接口函数*/
void setLogsDir(const QString &dirPath);//设置日志文件目录
void rmLogsFile(int retainDays=0);//删除日志文件
void writeLogs(const QString &content,LogsLevel logsLevel=INFO_LEVEL);//写日志
```
## 作者联系方式:
**邮箱:justdoit_mqr@163.com**  
**新浪微博:@为-何-而来**  

