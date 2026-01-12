#include "aichatpanel.h"
#include <QLabel>
#include <QGroupBox>
#include <QScrollBar>
#include <QDateTime>

AIChatPanel::AIChatPanel(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
}

void AIChatPanel::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    // Create tab widget
    m_tabWidget = new QTabWidget(this);

    // === Chat Tab ===
    m_chatTab = new QWidget();
    QVBoxLayout *chatLayout = new QVBoxLayout(m_chatTab);
    chatLayout->setContentsMargins(8, 8, 8, 8);

    QLabel *chatLabel = new QLabel("AI Assistant", m_chatTab);
    chatLabel->setStyleSheet("font-weight: bold; font-size: 14px; color: #0078d7; margin-bottom: 8px;");
    chatLayout->addWidget(chatLabel);

    m_chatHistory = new QTextEdit(m_chatTab);
    m_chatHistory->setReadOnly(true);
    m_chatHistory->setPlaceholderText("Ask the AI assistant for help with your code...");
    m_chatHistory->setStyleSheet(R"(
        QTextEdit {
            background-color: #1e1e1e;
            border: 1px solid #3d3d3d;
            border-radius: 6px;
            padding: 8px;
            font-family: 'Segoe UI', sans-serif;
            font-size: 13px;
        }
    )");
    chatLayout->addWidget(m_chatHistory, 1);

    QHBoxLayout *chatInputLayout = new QHBoxLayout();
    m_chatInput = new QLineEdit(m_chatTab);
    m_chatInput->setPlaceholderText("Type your question here...");
    m_chatInput->setMinimumHeight(36);
    connect(m_chatInput, &QLineEdit::returnPressed, this, &AIChatPanel::onSendClicked);
    chatInputLayout->addWidget(m_chatInput);

    m_sendButton = new QPushButton("Send", m_chatTab);
    m_sendButton->setMinimumHeight(36);
    m_sendButton->setMinimumWidth(80);
    connect(m_sendButton, &QPushButton::clicked, this, &AIChatPanel::onSendClicked);
    chatInputLayout->addWidget(m_sendButton);

    chatLayout->addLayout(chatInputLayout);
    m_tabWidget->addTab(m_chatTab, "💬 Chat");

    // === Follow-up Tab ===
    m_followUpTab = new QWidget();
    QVBoxLayout *followUpLayout = new QVBoxLayout(m_followUpTab);
    followUpLayout->setContentsMargins(8, 8, 8, 8);

    QLabel *followUpLabel = new QLabel("Follow-up Questions", m_followUpTab);
    followUpLabel->setStyleSheet("font-weight: bold; font-size: 14px; color: #0078d7; margin-bottom: 8px;");
    followUpLayout->addWidget(followUpLabel);

    m_followUpHistory = new QTextEdit(m_followUpTab);
    m_followUpHistory->setReadOnly(true);
    m_followUpHistory->setPlaceholderText("Continue the conversation with follow-up questions...");
    m_followUpHistory->setStyleSheet(R"(
        QTextEdit {
            background-color: #1e1e1e;
            border: 1px solid #3d3d3d;
            border-radius: 6px;
            padding: 8px;
            font-family: 'Segoe UI', sans-serif;
            font-size: 13px;
        }
    )");
    followUpLayout->addWidget(m_followUpHistory, 1);

    QHBoxLayout *followUpInputLayout = new QHBoxLayout();
    m_followUpInput = new QLineEdit(m_followUpTab);
    m_followUpInput->setPlaceholderText("Ask a follow-up question...");
    m_followUpInput->setMinimumHeight(36);
    connect(m_followUpInput, &QLineEdit::returnPressed, this, &AIChatPanel::onFollowUpClicked);
    followUpInputLayout->addWidget(m_followUpInput);

    m_followUpButton = new QPushButton("Ask", m_followUpTab);
    m_followUpButton->setMinimumHeight(36);
    m_followUpButton->setMinimumWidth(80);
    connect(m_followUpButton, &QPushButton::clicked, this, &AIChatPanel::onFollowUpClicked);
    followUpInputLayout->addWidget(m_followUpButton);

    followUpLayout->addLayout(followUpInputLayout);
    m_tabWidget->addTab(m_followUpTab, "🔄 Follow-up");

    // === Ideas & Suggestions Tab ===
    m_suggestionsTab = new QWidget();
    QVBoxLayout *suggestionsLayout = new QVBoxLayout(m_suggestionsTab);
    suggestionsLayout->setContentsMargins(8, 8, 8, 8);

    QLabel *suggestionsLabel = new QLabel("Ideas & Suggestions", m_suggestionsTab);
    suggestionsLabel->setStyleSheet("font-weight: bold; font-size: 14px; color: #0078d7; margin-bottom: 8px;");
    suggestionsLayout->addWidget(suggestionsLabel);

    m_suggestionsList = new QListWidget(m_suggestionsTab);
    m_suggestionsList->setStyleSheet(R"(
        QListWidget {
            background-color: #1e1e1e;
            border: 1px solid #3d3d3d;
            border-radius: 6px;
            padding: 4px;
        }
        QListWidget::item {
            padding: 8px;
            border-radius: 4px;
            margin: 2px;
        }
        QListWidget::item:hover {
            background-color: #2d2d2d;
        }
        QListWidget::item:selected {
            background-color: #0078d7;
        }
    )");
    connect(m_suggestionsList, &QListWidget::itemClicked, this, &AIChatPanel::onSuggestionItemClicked);
    suggestionsLayout->addWidget(m_suggestionsList);

    m_suggestionDetails = new QTextEdit(m_suggestionsTab);
    m_suggestionDetails->setReadOnly(true);
    m_suggestionDetails->setPlaceholderText("Click a suggestion to see details...");
    m_suggestionDetails->setMaximumHeight(120);
    m_suggestionDetails->setStyleSheet(R"(
        QTextEdit {
            background-color: #252525;
            border: 1px solid #3d3d3d;
            border-radius: 6px;
            padding: 8px;
            font-family: 'Segoe UI', sans-serif;
            font-size: 12px;
        }
    )");
    suggestionsLayout->addWidget(m_suggestionDetails);

    m_refreshSuggestionsButton = new QPushButton("🔍 Get Suggestions", m_suggestionsTab);
    m_refreshSuggestionsButton->setMinimumHeight(36);
    connect(m_refreshSuggestionsButton, &QPushButton::clicked, this, &AIChatPanel::onSuggestionClicked);
    suggestionsLayout->addWidget(m_refreshSuggestionsButton);

    m_tabWidget->addTab(m_suggestionsTab, "💡 Ideas");

    // === Output Tab ===
    m_outputTab = new QWidget();
    QVBoxLayout *outputLayout = new QVBoxLayout(m_outputTab);
    outputLayout->setContentsMargins(8, 8, 8, 8);

    QLabel *outputLabel = new QLabel("Compiler Output", m_outputTab);
    outputLabel->setStyleSheet("font-weight: bold; font-size: 14px; color: #0078d7; margin-bottom: 8px;");
    outputLayout->addWidget(outputLabel);

    m_outputText = new QTextEdit(m_outputTab);
    m_outputText->setReadOnly(true);
    m_outputText->setFont(QFont("Consolas", 10));
    m_outputText->setStyleSheet(R"(
        QTextEdit {
            background-color: #1a1a1a;
            border: 1px solid #3d3d3d;
            border-radius: 6px;
            padding: 8px;
            font-family: 'Consolas', 'Courier New', monospace;
            font-size: 12px;
        }
    )");
    outputLayout->addWidget(m_outputText, 1);

    m_clearOutputButton = new QPushButton("Clear Output", m_outputTab);
    m_clearOutputButton->setMinimumHeight(36);
    m_clearOutputButton->setStyleSheet("background-color: #4d4d4d;");
    connect(m_clearOutputButton, &QPushButton::clicked, [this]() {
        m_outputText->clear();
    });
    outputLayout->addWidget(m_clearOutputButton);

    m_tabWidget->addTab(m_outputTab, "📋 Output");

    mainLayout->addWidget(m_tabWidget);
}

void AIChatPanel::appendMessage(const QString &sender, const QString &message, bool isUser)
{
    QString timestamp = QDateTime::currentDateTime().toString("hh:mm");
    QString color = isUser ? "#569cd6" : "#4ec9b0";
    QString bgColor = isUser ? "#1e3a5f" : "#1e3d35";

    QString html = QString(R"(
        <div style="margin: 8px 0; padding: 10px; background-color: %4; border-radius: 8px; border-left: 3px solid %3;">
            <div style="color: %3; font-weight: bold; margin-bottom: 4px;">%1 <span style="color: #666; font-weight: normal; font-size: 11px;">%5</span></div>
            <div style="color: #e0e0e0; line-height: 1.5;">%2</div>
        </div>
    )").arg(sender, message.toHtmlEscaped().replace("\n", "<br>"), color, bgColor, timestamp);

    m_chatHistory->append(html);
    m_followUpHistory->append(html);

    // Auto-scroll to bottom
    QScrollBar *sb = m_chatHistory->verticalScrollBar();
    sb->setValue(sb->maximum());
    sb = m_followUpHistory->verticalScrollBar();
    sb->setValue(sb->maximum());
}

void AIChatPanel::onSendClicked()
{
    QString message = m_chatInput->text().trimmed();
    if (message.isEmpty()) return;

    appendMessage("You", message, true);
    m_chatInput->clear();
    emit messageSubmitted(message);
}

void AIChatPanel::onFollowUpClicked()
{
    QString followUp = m_followUpInput->text().trimmed();
    if (followUp.isEmpty()) return;

    appendMessage("You (Follow-up)", followUp, true);
    m_followUpInput->clear();
    emit followUpSubmitted(followUp);
}

void AIChatPanel::onSuggestionClicked()
{
    m_suggestionsList->clear();
    m_suggestionDetails->clear();
    m_suggestionsList->addItem("🔄 Analyzing your code...");
    emit suggestionRequested();
}

void AIChatPanel::onSuggestionItemClicked(QListWidgetItem *item)
{
    if (item) {
        QString text = item->text();
        QString details = item->data(Qt::UserRole).toString();
        if (!details.isEmpty()) {
            m_suggestionDetails->setText(details);
        } else {
            m_suggestionDetails->setText(text);
        }
    }
}

void AIChatPanel::appendAIResponse(const QString &response)
{
    appendMessage("AI Assistant", response, false);
}

void AIChatPanel::appendOutput(const QString &output, bool isError)
{
    QString color = isError ? "#ff6b6b" : "#98c379";
    QString html = QString("<pre style='color: %1; margin: 4px 0;'>%2</pre>")
                   .arg(color, output.toHtmlEscaped());
    m_outputText->append(html);

    // Switch to output tab
    m_tabWidget->setCurrentWidget(m_outputTab);

    // Auto-scroll to bottom
    QScrollBar *sb = m_outputText->verticalScrollBar();
    sb->setValue(sb->maximum());
}

void AIChatPanel::showSuggestions(const QStringList &suggestions)
{
    m_suggestionsList->clear();
    for (const QString &suggestion : suggestions) {
        QListWidgetItem *item = new QListWidgetItem(suggestion);
        // Add detailed explanation as tooltip and data
        QString details = "Click to apply this suggestion to your code.";
        item->setData(Qt::UserRole, details);
        item->setToolTip(suggestion);
        m_suggestionsList->addItem(item);
    }

    if (suggestions.isEmpty()) {
        m_suggestionsList->addItem("✓ No suggestions - your code looks good!");
    }
}

void AIChatPanel::clearChat()
{
    m_chatHistory->clear();
    m_followUpHistory->clear();
}
