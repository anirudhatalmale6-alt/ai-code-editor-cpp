#ifndef AICHATPANEL_H
#define AICHATPANEL_H

#include <QWidget>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QListWidget>
#include <QVBoxLayout>
#include <QTabWidget>

class AIChatPanel : public QWidget
{
    Q_OBJECT

public:
    explicit AIChatPanel(QWidget *parent = nullptr);

    void appendAIResponse(const QString &response);
    void appendOutput(const QString &output, bool isError);
    void showSuggestions(const QStringList &suggestions);
    void clearChat();

signals:
    void messageSubmitted(const QString &message);
    void followUpSubmitted(const QString &followUp);
    void suggestionRequested();

private slots:
    void onSendClicked();
    void onFollowUpClicked();
    void onSuggestionClicked();
    void onSuggestionItemClicked(QListWidgetItem *item);

private:
    void setupUI();
    void appendMessage(const QString &sender, const QString &message, bool isUser);

    // Main components
    QTabWidget *m_tabWidget;

    // Chat tab
    QWidget *m_chatTab;
    QTextEdit *m_chatHistory;
    QLineEdit *m_chatInput;
    QPushButton *m_sendButton;

    // Follow-up tab
    QWidget *m_followUpTab;
    QTextEdit *m_followUpHistory;
    QLineEdit *m_followUpInput;
    QPushButton *m_followUpButton;

    // Suggestions tab
    QWidget *m_suggestionsTab;
    QListWidget *m_suggestionsList;
    QPushButton *m_refreshSuggestionsButton;
    QTextEdit *m_suggestionDetails;

    // Output tab (for compiler output)
    QWidget *m_outputTab;
    QTextEdit *m_outputText;
    QPushButton *m_clearOutputButton;
};

#endif // AICHATPANEL_H
