#include "httpheader.h"

HTTPHeader::HTTPHeader()
{

}

void HTTPHeader::addValue(QString name,QString value){
    HTTPHeader::Header * p = new HTTPHeader::Header;
    p->name=name;
    p->value=value;
    httpHeader.append(p);
}

void HTTPHeader::addValue(QString name,int value){
    HTTPHeader::Header * p = new HTTPHeader::Header;
    p->name=name;
    p->value=QString::number(value);
    httpHeader.append(p);
}
