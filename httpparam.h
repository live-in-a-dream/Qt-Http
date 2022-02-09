#ifndef GETPARAM_H
#define GETPARAM_H
#include "QString"
#include "QStringList"

class HTTPParam
{
public:
    HTTPParam();

    class param
    {
    public:
        param() {}
        //参数名
        QString name;
        //参数值
        QString value;
    };

    QList<param*> httpparams;

    //添加键值
    void addValue(QString name,QString value);
    void addValue(QString name,int value);

    //转换成字符串
    QString paramToString();

};

#endif // GETPARAM_H
