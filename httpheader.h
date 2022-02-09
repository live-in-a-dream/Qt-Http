#ifndef HTTPHEADER_H
#define HTTPHEADER_H
#include "QString"
#include "QList"
class HTTPHeader
{
public:
    HTTPHeader();


    class Header
    {
    public:
        Header() {}
        //参数名
        QString name;
        //参数值
        QString value;
    };

    QList<Header*> httpHeader;

    //添加键值
    void addValue(QString name,QString value);
    void addValue(QString name,int value);
};

#endif // HTTPHEADER_H
