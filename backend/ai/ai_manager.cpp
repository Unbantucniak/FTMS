#include "ai_manager.h"
#include <QDebug>
#include <QRegularExpression>

AIManager::AIManager(QObject *parent) : QObject(parent)
{
    m_networkManager = new QNetworkAccessManager(this);
    m_apiKey = "local";
    m_apiUrl = "http://localhost:11434/v1/chat/completions"; 
    m_model = "qwen3:4b"; 
    m_maxTokens = 0; 

    // 允许通过环境变量覆盖配置，便于切换小模型/不同服务
    auto env = QProcessEnvironment::systemEnvironment();
    const QString envModel = env.value("FTMS_AI_MODEL").trimmed();
    if (!envModel.isEmpty()) m_model = envModel;
    const QString envUrl = env.value("FTMS_AI_URL").trimmed();
    if (!envUrl.isEmpty()) m_apiUrl = envUrl;
    const QString envKey = env.value("FTMS_AI_KEY");
    if (!envKey.isEmpty()) m_apiKey = envKey;
    const QString envMax = env.value("FTMS_AI_MAX_TOKENS").trimmed();
    bool ok = false; int maxTok = envMax.toInt(&ok);
    if (ok && maxTok > 0) m_maxTokens = maxTok;
}

void AIManager::sendMessage(const QString& message, const QString& context)
{
    QUrl url(m_apiUrl);
    QNetworkRequest request(url);
    
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", QString("Bearer %1").arg(m_apiKey).toUtf8());
    
    QJsonObject systemMessage;
    systemMessage["role"] = "system";
    QString systemPrompt = "你是扶摇航空的智能旅行助手，热情、专业、博学。😊\n\n"
                           "🎯 核心职责：\n"
                           "• 旅行咨询服务：提供目的地文化、风景名胜、地方美食、交通接驳、行程规划建议\n"
                           "• 实用出行贴士：天气提醒、最佳游玩季节、当地习俗注意事项等\n"
                           "• 旅行体验分享：推荐特色活动、住宿建议、当地交通攻略\n\n"
                           "💬 回答风格：\n"
                           "• 简洁友好，适度使用表情符号增强亲和力\n"
                           "• 实用且贴近用户需求，多提供可行的建议\n\n"
                           "⚠️ 重要边界：\n"
                           "• 你不提供航班、订单等数据库查询服务\n"
                           "• 若用户询问具体航班、票价、余票、订单信息，请礼貌告知：\"请通过航班查询页面或我的订单页面查看实时信息哦～我更擅长帮您规划行程和介绍目的地！😊\"\n"
                           "• 拒绝与旅行无关的话题，礼貌引导回核心服务。";
    
    if (!context.isEmpty()) {
        systemPrompt += "\n\n" + context;
    }
    
    systemMessage["content"] = systemPrompt;

    QJsonObject userMessage;
    userMessage["role"] = "user";
    userMessage["content"] = message;
    
    QJsonArray messages;
    messages.append(systemMessage);
    messages.append(userMessage);
    
    QJsonObject json;
    json["model"] = m_model;
    json["messages"] = messages;
    json["temperature"] = 0.7;
    if (m_maxTokens == 0) {
        json["max_tokens"] = m_maxTokens;
    } else {
        json["max_tokens"] = 1024;
    }
    
    QByteArray data = QJsonDocument(json).toJson();
    
    QNetworkReply *reply = m_networkManager->post(request, data);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        onReplyFinished(reply);
    });
}

void AIManager::onReplyFinished(QNetworkReply *reply)
{
    const int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    const QByteArray responseData = reply->readAll();

    if (reply->error() == QNetworkReply::NoError && statusCode >= 200 && statusCode < 300) {
        QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
        QJsonObject jsonObj = jsonDoc.object();
        
        if (jsonObj.contains("choices") && jsonObj["choices"].isArray()) {
            QJsonArray choices = jsonObj["choices"].toArray();
            if (!choices.isEmpty()) {
                QJsonObject firstChoice = choices[0].toObject();
                if (firstChoice.contains("message")) {
                    QJsonObject message = firstChoice["message"].toObject();
                    QString content = message["content"].toString();
                    
                    // 移除 <think>...</think> 标签及其内容，净化输出
                    static QRegularExpression thinkRegex("<think>.*?</think>", QRegularExpression::DotMatchesEverythingOption);
                    content.remove(thinkRegex);
                    
                    emit responseReceived(content.trimmed());
                }
            }
        } else {
            emit errorOccurred("无法解析服务器响应");
        }
    } else {
        QString serverMsg;
        if (!responseData.isEmpty()) {
            serverMsg = QString::fromUtf8(responseData).left(200);
        }
        QString friendly = QString("网络请求错误 (%1): %2").arg(statusCode > 0 ? statusCode : -1).arg(reply->errorString());
        if (!serverMsg.isEmpty()) {
            friendly += QString(" | 服务端返回: %1").arg(serverMsg.trimmed());
        }
        emit errorOccurred(friendly);
    }
    
    reply->deleteLater();
}
