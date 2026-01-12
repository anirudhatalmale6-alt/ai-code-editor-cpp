#include "aiservice.h"
#include <QNetworkRequest>
#include <QUrl>
#include <QSettings>

AIService::AIService(QObject *parent)
    : QObject(parent)
    , m_waitingForSuggestions(false)
{
    m_networkManager = new QNetworkAccessManager(this);
    connect(m_networkManager, &QNetworkAccessManager::finished,
            this, &AIService::onReplyFinished);

    // Default settings for Ollama (local AI)
    QSettings settings("AICodeEditor", "AICodeEditor");
    m_endpoint = settings.value("ai/endpoint", "http://localhost:11434/api/generate").toString();
    m_model = settings.value("ai/model", "codellama").toString();
}

void AIService::setModel(const QString &model)
{
    m_model = model;
    QSettings settings("AICodeEditor", "AICodeEditor");
    settings.setValue("ai/model", model);
}

void AIService::setEndpoint(const QString &endpoint)
{
    m_endpoint = endpoint;
    QSettings settings("AICodeEditor", "AICodeEditor");
    settings.setValue("ai/endpoint", endpoint);
}

QString AIService::buildSystemPrompt() const
{
    return R"(You are an expert C/C++ programming assistant integrated into a code editor.
Your role is to help users write, debug, and improve their C and C++ code.

Guidelines:
- Provide clear, concise explanations
- When showing code, use proper C/C++ syntax
- Point out potential bugs, memory leaks, or undefined behavior
- Suggest best practices and optimizations
- Be helpful with both C and C++ (all standards from C89 to C23 and C++11 to C++23)
- If asked about compilation, consider common compilers: GCC, Clang, MSVC, MinGW

Keep responses focused and practical. If code is provided, analyze it carefully before responding.)";
}

QString AIService::buildSuggestionPrompt(const QString &code) const
{
    return QString(R"(Analyze the following C/C++ code and provide 3-5 specific suggestions for improvement.
Focus on:
1. Bug fixes or potential issues
2. Performance optimizations
3. Code style and readability
4. Best practices
5. Security concerns

Format each suggestion as a single line starting with an emoji:
🐛 for bugs
⚡ for performance
📝 for style
✨ for best practices
🔒 for security

Code to analyze:
```cpp
%1
```

Provide only the suggestions, one per line, no additional explanation.)").arg(code);
}

void AIService::sendMessage(const QString &message, const QString &codeContext)
{
    m_lastCodeContext = codeContext;
    m_waitingForSuggestions = false;

    QString prompt = buildSystemPrompt() + "\n\n";

    if (!m_conversationHistory.isEmpty()) {
        prompt += "Previous conversation:\n" + m_conversationHistory + "\n\n";
    }

    if (!codeContext.isEmpty()) {
        prompt += "Current code in editor:\n```cpp\n" + codeContext + "\n```\n\n";
    }

    prompt += "User question: " + message;

    // Update conversation history
    m_conversationHistory += "User: " + message + "\n";

    sendRequest(prompt);
}

void AIService::sendFollowUp(const QString &followUp)
{
    m_waitingForSuggestions = false;

    QString prompt = buildSystemPrompt() + "\n\n";
    prompt += "Conversation history:\n" + m_conversationHistory + "\n\n";

    if (!m_lastCodeContext.isEmpty()) {
        prompt += "Current code:\n```cpp\n" + m_lastCodeContext + "\n```\n\n";
    }

    prompt += "Follow-up question: " + followUp;

    m_conversationHistory += "User (follow-up): " + followUp + "\n";

    sendRequest(prompt);
}

void AIService::requestSuggestions(const QString &code)
{
    if (code.trimmed().isEmpty()) {
        emit suggestionsReceived({"📝 Write some code to get suggestions..."});
        return;
    }

    m_waitingForSuggestions = true;
    QString prompt = buildSuggestionPrompt(code);
    sendRequest(prompt, true);
}

void AIService::sendRequest(const QString &prompt, bool isSuggestion)
{
    m_waitingForSuggestions = isSuggestion;

    QUrl url(m_endpoint);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    // Build request body for Ollama API
    QJsonObject body;
    body["model"] = m_model;
    body["prompt"] = prompt;
    body["stream"] = false;

    // Options for better code responses
    QJsonObject options;
    options["temperature"] = 0.7;
    options["top_p"] = 0.9;
    options["num_predict"] = 1024;
    body["options"] = options;

    QJsonDocument doc(body);
    QByteArray data = doc.toJson();

    m_networkManager->post(request, data);
}

void AIService::onReplyFinished(QNetworkReply *reply)
{
    reply->deleteLater();

    if (reply->error() != QNetworkReply::NoError) {
        QString errorMsg = reply->errorString();

        // Check if Ollama is not running
        if (reply->error() == QNetworkReply::ConnectionRefusedError) {
            errorMsg = "Cannot connect to local AI (Ollama). Please make sure Ollama is running.\n\n"
                      "To start Ollama:\n"
                      "1. Install Ollama from https://ollama.ai\n"
                      "2. Run: ollama pull codellama\n"
                      "3. Ollama runs automatically in the background";
        }

        if (m_waitingForSuggestions) {
            emit suggestionsReceived({"❌ " + errorMsg});
        } else {
            emit errorOccurred(errorMsg);
            emit responseReceived("Error: " + errorMsg);
        }
        return;
    }

    QByteArray responseData = reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(responseData);

    if (doc.isNull() || !doc.isObject()) {
        emit errorOccurred("Invalid response from AI service");
        return;
    }

    QJsonObject obj = doc.object();
    QString response = obj["response"].toString();

    if (m_waitingForSuggestions) {
        // Parse suggestions from response
        QStringList suggestions;
        QStringList lines = response.split('\n', Qt::SkipEmptyParts);
        for (const QString &line : lines) {
            QString trimmed = line.trimmed();
            if (!trimmed.isEmpty() &&
                (trimmed.startsWith("🐛") || trimmed.startsWith("⚡") ||
                 trimmed.startsWith("📝") || trimmed.startsWith("✨") ||
                 trimmed.startsWith("🔒") || trimmed.startsWith("-") ||
                 trimmed.startsWith("•") || trimmed.startsWith("*"))) {
                suggestions.append(trimmed);
            }
        }

        if (suggestions.isEmpty() && !response.isEmpty()) {
            // If no formatted suggestions, split response into lines
            suggestions = response.split('\n', Qt::SkipEmptyParts);
            if (suggestions.size() > 5) {
                suggestions = suggestions.mid(0, 5);
            }
        }

        emit suggestionsReceived(suggestions);
    } else {
        // Update conversation history with AI response
        m_conversationHistory += "Assistant: " + response + "\n";

        // Limit history to last 2000 characters to avoid context overflow
        if (m_conversationHistory.length() > 2000) {
            m_conversationHistory = m_conversationHistory.right(2000);
        }

        emit responseReceived(response);
    }
}
