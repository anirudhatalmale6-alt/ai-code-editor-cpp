#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSplitter>
#include <QComboBox>
#include <QLabel>
#include <QAction>
#include <QToolBar>
#include "codeeditor.h"
#include "aichatpanel.h"
#include "compilerservice.h"
#include "aiservice.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void newFile();
    void openFile();
    void saveFile();
    void saveFileAs();
    void compileCode();
    void runCode();
    void compileAndRun();
    void onCompilationFinished(bool success, const QString &output);
    void onExecutionFinished(const QString &output);
    void onAIResponseReceived(const QString &response);
    void onAISuggestionsReceived(const QStringList &suggestions);
    void updateStatusBar();

private:
    void createMenus();
    void createToolBar();
    void createStatusBar();
    void setupUI();
    void loadSettings();
    void saveSettings();

    // UI Components
    QSplitter *m_mainSplitter;
    CodeEditor *m_codeEditor;
    AIChatPanel *m_aiChatPanel;
    QComboBox *m_compilerSelector;
    QLabel *m_statusLabel;
    QLabel *m_cursorPositionLabel;

    // Services
    CompilerService *m_compilerService;
    AIService *m_aiService;

    // State
    QString m_currentFilePath;
    bool m_isModified;
};

#endif // MAINWINDOW_H
