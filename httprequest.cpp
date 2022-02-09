#include "httprequest.h"
#include <QList>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QNetworkCookieJar>
#include <QNetworkCookie>
#include <QNetworkProxy>

#include <QByteArray>
#include <QEventLoop>
#include <QTimer>
HttpRequest::HttpRequest(QObject *parent) : QObject(parent)
  ,mReply(nullptr)
  ,mCookieJar(nullptr)
  ,mManger(nullptr)
  ,mUrl("")
  ,mIsRedirects(false)
  ,mTimeOut(10*60*1000)
{
    mManger = new QNetworkAccessManager(this);
    mCookieJar = new QNetworkCookieJar(this);

    mManger->setCookieJar(mCookieJar);
    mConfig.setPeerVerifyMode(QSslSocket::VerifyNone);
    mConfig.setProtocol(QSsl::TlsV1SslV3);
}

HttpRequest::~HttpRequest()
{
    if(mReply){
        mReply->deleteLater();
        mReply = nullptr;
    }

}

void HttpRequest::setRequestHeader(const QString &key, const QString &value)
{
    if(key.isEmpty()){
        return;
    }
    mHeaders[key] = value;
}

void HttpRequest::setRequestHeader(const QMap<QString,QString> &headers)
{
    if(headers.isEmpty()){
        return;
    }
    auto it = headers.begin();
    for(;it!= headers.end();++it){
        mHeaders[it.key()] = it.value();
    }
}

QString HttpRequest::getCookie()
{
    return getCookie(QUrl(mUrl));
}

QString HttpRequest::getCookie(const QUrl &url)
{
    QString strResult;
    if(!url.isValid()){
        return strResult;
    }
    if(!mCookieJar){
        return strResult;
    }
    for(auto& v : mCookieJar->cookiesForUrl(url)){
        strResult+= v.name() + "=" + v.value() + ";";
    }
    return strResult;
}

bool HttpRequest::hasRawHeader(const QByteArray &headerName)
{
    bool bResult = false;
    if(mReply){
        bResult = mReply->hasRawHeader(headerName);
    }
    return bResult;
}


QString HttpRequest::rawHeasssder(const QByteArray &headerName)
{
    QString strResult;
    if(mReply){
       strResult = mReply->rawHeader(headerName);
    }
    return strResult;
}

QByteArray HttpRequest::send(RequestType type, const QString &url, const QByteArray &data)
{

    if(url.isEmpty()){
        return "";
    }
    QUrl urls(url);
    if(urls.isValid()){
        mUrl = url;
    }
    if(mReply){
        mReply->deleteLater();
        mReply = nullptr;
    }
    if(!mData.isEmpty()){
        mData.clear();
    }
    QNetworkRequest Requst(urls);
    // 支持HTTPS协议
    Requst.setSslConfiguration(mConfig);
    //协议头设置
    auto it = mHeaders.begin();
    for(;it != mHeaders.end();++it){
        Requst.setRawHeader(it.key().toLatin1(),it.value().toLatin1());
    }
    //重定向设置
    Requst.setAttribute(QNetworkRequest::FollowRedirectsAttribute,QVariant(mIsRedirects));
    loop = new QEventLoop(this);
    switch (type) {
    case GET:
       mReply = mManger->get(Requst);
    break;
    case POST:
        mReply = mManger->post(Requst,data);
    break;
    case HEAD:
       mReply = mManger->head(Requst);
    break;
    }
    //信号--获取完成
    connect(mReply,&QNetworkReply::finished,loop,&QEventLoop::quit);

    QTimer::singleShot(mTimeOut,loop,&QEventLoop::quit);
    loop->exec();

    //没有错则赋值
    if(QNetworkReply::NoError == mReply->error()){
        mData =  mReply->readAll();
    }
    //清空原来的协议头
    mHeaders.clear();
    return mData;
}


QByteArray HttpRequest::sendAsy(RequestType type, const QString &url, const QByteArray &data,
                             std::function< void (qint64 bytesReceived, qint64 bytesTotal)> downloadProgress,
                             std::function< void (QByteArray)> readDataAll)
{
    QNetworkReply* mReply = nullptr;
    if(url.isEmpty()){
        return "";
    }
    QUrl urls(url);
    if(urls.isValid()){
        mUrl = url;
    }
    if(mReply){
        mReply->deleteLater();
        mReply = nullptr;
    }
    if(!mData.isEmpty()){
        mData.clear();
    }
    QNetworkRequest Requst(urls);
    // 支持HTTPS协议
    Requst.setSslConfiguration(mConfig);
    //协议头设置
    auto it = mHeaders.begin();
    for(;it != mHeaders.end();++it){
        Requst.setRawHeader(it.key().toLatin1(),it.value().toLatin1());
    }
    //重定向设置
    Requst.setAttribute(QNetworkRequest::FollowRedirectsAttribute,QVariant(mIsRedirects));
    switch (type) {
    case GET:
       mReply = mManger->get(Requst);
    break;
    case POST:
        mReply = mManger->post(Requst,data);
    break;
    case HEAD:
       mReply = mManger->head(Requst);
    break;
    }


    connect(mReply,&QNetworkReply::downloadProgress,[=](qint64 bytesReceived, qint64 bytesTotal){
        if(downloadProgress != nullptr){
            downloadProgress(bytesReceived,bytesTotal);
        }
        if(readDataAll != nullptr){
            if(bytesReceived == bytesTotal)
            {
                readDataAll(mReply->readAll());
            }
        }
    });
    return mData;
}

QByteArray HttpRequest::getBodyContent()
{
    return mData;
}

int HttpRequest::getStatusCode()
{
    int nStatus = 0;
    if(mReply){
        nStatus = mReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    }
    return nStatus;
}

void HttpRequest::setRedirects(bool status)
{
    mIsRedirects = status;
}

QString HttpRequest::getHeaders()
{
    QString strResult;
    if(mReply){
        auto HeaderList = mReply->rawHeaderList();
        for(auto it = HeaderList.begin();it!=HeaderList.end();++it){
            strResult.append(it->data());
            strResult.append(":");
            strResult+= mReply->rawHeader(it->data());
            strResult+="\r\n";
        }
    }
    return strResult;
}

//停止代理
void HttpRequest::stopProxy(){
    isProxy = false;
    QNetworkProxy proxy;
    proxy.setType(QNetworkProxy::NoProxy);
    QNetworkProxy::setApplicationProxy(proxy);
//    mManger->setProxy(proxy);
}
//设置代理启用代理
void HttpRequest::startProxy(QString proxyIp,int proxyPort,QString proxyUser,QString proxyPassword){
    this->proxyIp=proxyIp;
    this->proxyPort=proxyPort;
    this->proxyUser=proxyUser;
    this->proxyPassword=proxyPassword;

    QNetworkProxy proxy;
    proxy.setHostName(proxyIp);
    proxy.setPort(proxyPort);
    if(proxyUser != "" && proxyPassword != ""){
        proxy.setUser(proxyUser);
        proxy.setPassword(proxyPassword);
    }
    QNetworkProxy::setApplicationProxy(proxy);
//    mManger->setProxy(proxy);
}
