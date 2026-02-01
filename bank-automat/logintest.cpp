#include <QtTest>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>

class LoginTest : public QObject
{
    Q_OBJECT   
private slots:
    void testLoginReturnsJwt() {
        // get backend IP and port from environment variables
        QNetworkAccessManager manager;
        QString backend_ip = QString::fromUtf8(qgetenv("BACKEND_IP"));
        QString backend_port = QString::fromUtf8(qgetenv("BACKEND_PORT"));
        QString url = "http://" + backend_ip + ":" + backend_port + "/auth/login";
        QVERIFY(QUrl(url).isValid());

        QUrl requestUrl(url);
        QNetworkRequest request(requestUrl);
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
        
        // get test card and pin from environment variables
        QJsonObject data;
        data["idCard"] = QString::fromUtf8(qgetenv("TEST_CARD"));
        data["pin"] = QString::fromUtf8(qgetenv("TEST_PIN"));
        
        //Send POST request and wait for response
        QNetworkReply *reply = manager.post(request, QJsonDocument(data).toJson());
        QSignalSpy spy(reply, &QNetworkReply::finished);
        QVERIFY(spy.wait(5000));

        //check http status
        QCOMPARE(reply->error(), QNetworkReply::NoError);
        // Check that the response contains a JWT token
        QByteArray response_data = reply->readAll();
        QJsonDocument jsonResponse = QJsonDocument::fromJson(response_data);
        QVERIFY(jsonResponse.isObject());
        QJsonObject jsonObj = jsonResponse.object();
        QVERIFY(jsonObj.contains("token"));

        reply->deleteLater();
    }
};

QTEST_MAIN(LoginTest)
#include "logintest.moc"