#include "httpparam.h"

HTTPParam::HTTPParam()
{

}

QString HTTPParam::paramToString(){
    QString pa;
    for(int i=0;i<httpparams.size();i++){
        if(i!=0){
            pa.append("&");
        }
        pa.append(httpparams.at(i)->name)
                .append("=")
                .append(httpparams.at(i)->value);
    }
    return pa;
}


void HTTPParam::addValue(QString name,QString value){
    HTTPParam::param * p = new HTTPParam::param;
    p->name=name;
    p->value=value.toUtf8().toPercentEncoding();
    httpparams.append(p);
}

void HTTPParam::addValue(QString name,int value){
    HTTPParam::param * p = new HTTPParam::param;
    p->name=name;
    p->value=QString::number(value);
    httpparams.append(p);
}
