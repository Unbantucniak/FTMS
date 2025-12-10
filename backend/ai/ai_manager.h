#ifndef AI_MANAGER_H
#define AI_MANAGER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QProcessEnvironment>

class AIManager : public QObject
{
    Q_OBJECT
public:
    explicit AIManager(QObject *parent = nullptr);
    void sendMessage(const QString& message, const QString& context = "");

signals:
    void responseReceived(const QString& response);
    void errorOccurred(const QString& error);

private slots:
    void onReplyFinished(QNetworkReply *reply);

private:
    QNetworkAccessManager *m_networkManager;
    QString m_apiKey;
    QString m_apiUrl;
    QString m_model;
    int m_maxTokens;
};

#endif // AI_MANAGER_H
