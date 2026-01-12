#ifndef AISERVICE_H
#define AISERVICE_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QString>
#include <QStringList>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

class AIService : public QObject
{
    Q_OBJECT

public:
    explicit AIService(QObject *parent = nullptr);

    // Configuration
    void setModel(const QString &model);
    void setEndpoint(const QString &endpoint);
    QString currentModel() const { return m_model; }

    // API methods
    void sendMessage(const QString &message, const QString &codeContext = QString());
    void sendFollowUp(const QString &followUp);
    void requestSuggestions(const QString &code);

signals:
    void responseReceived(const QString &response);
    void suggestionsReceived(const QStringList &suggestions);
    void errorOccurred(const QString &error);

private slots:
    void onReplyFinished(QNetworkReply *reply);

private:
    void sendRequest(const QString &prompt, bool isSuggestion = false);
    QString buildSystemPrompt() const;
    QString buildSuggestionPrompt(const QString &code) const;

    QNetworkAccessManager *m_networkManager;
    QString m_endpoint;
    QString m_model;
    QString m_conversationHistory;
    QString m_lastCodeContext;
    bool m_waitingForSuggestions;
};

#endif // AISERVICE_H
