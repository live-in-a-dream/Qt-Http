#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H

#include <QObject>
#include <QMap>
#include <QSslConfiguration>
/*
@
本套HTTP请求采用组赛模式实现
使用步骤
1、构建对象
2、设置请求参数
3、执行请求 post get Header put 等
4、获取请求结果、cookie、header 等
@
*/
class QNetworkAccessManager;
class QNetworkCookieJar;
class QNetworkReply;
class QNetworkRequest;
#include "QFile"
#include "log.h"
#include "QString"
#include "QTextCodec"
#include "QEventLoop"
#include "QMutex"
//http RequestType 参考 https://www.cnblogs.com/acles/p/5300824.html
// 目前只使用如下三种方式
enum RequestType{
    GET,
    POST,
    HEAD
};

enum ContentType{
    NONE,
    XWwwFormUrlencoded,
    JSON,
};

class HttpRequest : public QObject
{
    Q_OBJECT
public:
    explicit HttpRequest(QObject *parent = nullptr);
    ~HttpRequest();
signals:

public slots:

public:
    //设置请求header key => value
    void setRequestHeader(const QString& key,const QString& value);
    // map 形式
    void setRequestHeader(const QMap<QString,QString>& headers);
    //获取请求后得到的cookie
    QString getCookie();
    //根据url 获取指定url 的all cookie
    QString getCookie(const QUrl& url);
    //判断返回的协议头是否存在
    bool hasRawHeader(const QByteArray& headerName);
    //返回远程协议头的value
    QString rawHeasssder(const QByteArray& headerName);
    //发送请求
    QByteArray send(RequestType type,const QString& url,const QByteArray &data = nullptr);
    //
    QByteArray sendAsy(RequestType type, const QString &url, const QByteArray &data = nullptr,
                                 std::function< void (qint64 bytesReceived, qint64 bytesTotal)> downloadProgress = nullptr,
                                 std::function< void (QByteArray)> readDataAll = nullptr);
    //获取请求的body内容
    QByteArray getBodyContent();
    //获取本次请求http的状态码
    int getStatusCode();
    //开启/关闭HTTP重定向 默认是关闭的 true 开启 false 关闭
    void setRedirects(bool status);
    //设置http请求超时时间 默认60秒
    void setTimeOut(int nSec);
    //获取全部请求返回的协议头
    QString getHeaders();
    //停止代理
    void stopProxy();
    //设置代理启用代理
    void startProxy(QString proxyIp,int proxyPort,QString proxyUser = "",QString proxyPassword = "");
private:
    // 访问管理
    QNetworkAccessManager* mManger;
    // 结果信息
    QNetworkReply* mReply;
    //cookie manger
    QNetworkCookieJar* mCookieJar;
    //管理请求需要的协议头来自外部设置
    QMap<QString,QString> mHeaders;
    //https 请求的支持
    QSslConfiguration mConfig;
    //记录本次请求后的url用于获取请求后的cookie
    QString mUrl;
    //保存接收的结果
    QByteArray mData;
    // 是否启用重定向
    bool mIsRedirects;
    //超时时间
    int mTimeOut;
    //文件对象
    QFile* file;
    //代理IP
    QString proxyIp = "";
    //代理端口
    int proxyPort = 8080;
    //代理用户名
    QString proxyUser = "";
    //代理密码
    QString proxyPassword = "";
    //是否启用代理
    bool isProxy = false;
    //
    QEventLoop *loop;


    QMutex mutex;
};

#endif // HTTPREQUEST_H
