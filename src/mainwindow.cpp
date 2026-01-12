#include "mainwindow.h"
#include <QMenuBar>
#include <QMenu>
#include <QToolBar>
#include <QStatusBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QApplication>
#include <QCloseEvent>
#include <QTextStream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_isModified(false)
{
    setWindowTitle("AI Code Editor");
    setMinimumSize(1200, 800);
    resize(1400, 900);

    // Initialize services
    m_compilerService = new CompilerService(this);
    m_aiService = new AIService(this);

    // Connect service signals
    connect(m_compilerService, &CompilerService::compilationFinished,
            this, &MainWindow::onCompilationFinished);
    connect(m_compilerService, &CompilerService::executionFinished,
            this, &MainWindow::onExecutionFinished);
    connect(m_aiService, &AIService::responseReceived,
            this, &MainWindow::onAIResponseReceived);
    connect(m_aiService, &AIService::suggestionsReceived,
            this, &MainWindow::onAISuggestionsReceived);

    setupUI();
    createMenus();
    createToolBar();
    createStatusBar();
    loadSettings();
}

MainWindow::~MainWindow()
{
    saveSettings();
}

void MainWindow::setupUI()
{
    // Main splitter - code editor on left, AI panel on right
    m_mainSplitter = new QSplitter(Qt::Horizontal, this);
    m_mainSplitter->setHandleWidth(3);

    // Code Editor
    m_codeEditor = new CodeEditor(this);
    connect(m_codeEditor, &QPlainTextEdit::textChanged, [this]() {
        m_isModified = true;
        updateStatusBar();
    });
    connect(m_codeEditor, &QPlainTextEdit::cursorPositionChanged,
            this, &MainWindow::updateStatusBar);

    // AI Chat Panel
    m_aiChatPanel = new AIChatPanel(this);
    connect(m_aiChatPanel, &AIChatPanel::messageSubmitted, [this](const QString &message) {
        QString context = m_codeEditor->toPlainText();
        m_aiService->sendMessage(message, context);
    });
    connect(m_aiChatPanel, &AIChatPanel::followUpSubmitted, [this](const QString &followUp) {
        m_aiService->sendFollowUp(followUp);
    });
    connect(m_aiChatPanel, &AIChatPanel::suggestionRequested, [this]() {
        QString code = m_codeEditor->toPlainText();
        m_aiService->requestSuggestions(code);
    });

    m_mainSplitter->addWidget(m_codeEditor);
    m_mainSplitter->addWidget(m_aiChatPanel);
    m_mainSplitter->setSizes({700, 500});

    setCentralWidget(m_mainSplitter);
}

void MainWindow::createMenus()
{
    // File Menu
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));

    QAction *newAction = fileMenu->addAction(tr("&New"), this, &MainWindow::newFile);
    newAction->setShortcut(QKeySequence::New);

    QAction *openAction = fileMenu->addAction(tr("&Open..."), this, &MainWindow::openFile);
    openAction->setShortcut(QKeySequence::Open);

    QAction *saveAction = fileMenu->addAction(tr("&Save"), this, &MainWindow::saveFile);
    saveAction->setShortcut(QKeySequence::Save);

    QAction *saveAsAction = fileMenu->addAction(tr("Save &As..."), this, &MainWindow::saveFileAs);
    saveAsAction->setShortcut(QKeySequence::SaveAs);

    fileMenu->addSeparator();

    QAction *exitAction = fileMenu->addAction(tr("E&xit"), this, &QMainWindow::close);
    exitAction->setShortcut(QKeySequence::Quit);

    // Build Menu
    QMenu *buildMenu = menuBar()->addMenu(tr("&Build"));

    QAction *compileAction = buildMenu->addAction(tr("&Compile"), this, &MainWindow::compileCode);
    compileAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_B));

    QAction *runAction = buildMenu->addAction(tr("&Run"), this, &MainWindow::runCode);
    runAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_R));

    QAction *compileRunAction = buildMenu->addAction(tr("Compile && Run"), this, &MainWindow::compileAndRun);
    compileRunAction->setShortcut(QKeySequence(Qt::Key_F5));

    // View Menu
    QMenu *viewMenu = menuBar()->addMenu(tr("&View"));

    QAction *toggleAIPanel = viewMenu->addAction(tr("Toggle AI Panel"));
    toggleAIPanel->setCheckable(true);
    toggleAIPanel->setChecked(true);
    toggleAIPanel->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_I));
    connect(toggleAIPanel, &QAction::toggled, [this](bool checked) {
        m_aiChatPanel->setVisible(checked);
    });

    // Help Menu
    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));

    QAction *aboutAction = helpMenu->addAction(tr("&About"), [this]() {
        QMessageBox::about(this, tr("About AI Code Editor"),
            tr("<h3>AI Code Editor v1.0</h3>"
               "<p>An AI-powered C/C++ code editor with local AI integration.</p>"
               "<p>Features:</p>"
               "<ul>"
               "<li>Syntax highlighting for C/C++</li>"
               "<li>AI-powered code assistance</li>"
               "<li>Built-in compiler integration</li>"
               "<li>Code suggestions and ideas</li>"
               "</ul>"));
    });
}

void MainWindow::createToolBar()
{
    QToolBar *mainToolBar = addToolBar(tr("Main Toolbar"));
    mainToolBar->setMovable(false);
    mainToolBar->setIconSize(QSize(20, 20));

    mainToolBar->addAction(tr("New"), this, &MainWindow::newFile);
    mainToolBar->addAction(tr("Open"), this, &MainWindow::openFile);
    mainToolBar->addAction(tr("Save"), this, &MainWindow::saveFile);
    mainToolBar->addSeparator();

    // Compiler selector
    QLabel *compilerLabel = new QLabel(tr(" Compiler: "), this);
    mainToolBar->addWidget(compilerLabel);

    m_compilerSelector = new QComboBox(this);
    m_compilerSelector->addItem("GCC (g++)", "g++");
    m_compilerSelector->addItem("Clang (clang++)", "clang++");
    m_compilerSelector->addItem("MSVC (cl)", "cl");
    m_compilerSelector->addItem("MinGW (mingw-g++)", "mingw-g++");
    m_compilerSelector->setMinimumWidth(150);
    connect(m_compilerSelector, QOverload<int>::of(&QComboBox::currentIndexChanged), [this](int index) {
        QString compiler = m_compilerSelector->currentData().toString();
        m_compilerService->setCompiler(compiler);
    });
    mainToolBar->addWidget(m_compilerSelector);

    mainToolBar->addSeparator();
    mainToolBar->addAction(tr("Compile"), this, &MainWindow::compileCode);
    mainToolBar->addAction(tr("Run"), this, &MainWindow::runCode);
    mainToolBar->addAction(tr("▶ Build & Run"), this, &MainWindow::compileAndRun);
}

void MainWindow::createStatusBar()
{
    m_statusLabel = new QLabel(tr("Ready"), this);
    m_cursorPositionLabel = new QLabel(tr("Line: 1, Col: 1"), this);

    statusBar()->addWidget(m_statusLabel, 1);
    statusBar()->addPermanentWidget(m_cursorPositionLabel);
}

void MainWindow::updateStatusBar()
{
    QString title = "AI Code Editor";
    if (!m_currentFilePath.isEmpty()) {
        QFileInfo fi(m_currentFilePath);
        title = fi.fileName() + " - AI Code Editor";
    }
    if (m_isModified) {
        title = "• " + title;
    }
    setWindowTitle(title);

    QTextCursor cursor = m_codeEditor->textCursor();
    int line = cursor.blockNumber() + 1;
    int col = cursor.columnNumber() + 1;
    m_cursorPositionLabel->setText(tr("Line: %1, Col: %2").arg(line).arg(col));
}

void MainWindow::newFile()
{
    if (m_isModified) {
        QMessageBox::StandardButton reply = QMessageBox::question(this,
            tr("Save Changes?"),
            tr("The document has been modified. Do you want to save changes?"),
            QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

        if (reply == QMessageBox::Save) {
            saveFile();
        } else if (reply == QMessageBox::Cancel) {
            return;
        }
    }

    m_codeEditor->clear();
    m_currentFilePath.clear();
    m_isModified = false;
    updateStatusBar();
}

void MainWindow::openFile()
{
    QString filePath = QFileDialog::getOpenFileName(this,
        tr("Open File"),
        QString(),
        tr("C/C++ Files (*.c *.cpp *.cc *.cxx *.h *.hpp);;All Files (*)"));

    if (filePath.isEmpty()) {
        return;
    }

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, tr("Error"),
            tr("Cannot open file: %1").arg(file.errorString()));
        return;
    }

    QTextStream in(&file);
    m_codeEditor->setPlainText(in.readAll());
    file.close();

    m_currentFilePath = filePath;
    m_isModified = false;
    updateStatusBar();

    m_statusLabel->setText(tr("Opened: %1").arg(filePath));
}

void MainWindow::saveFile()
{
    if (m_currentFilePath.isEmpty()) {
        saveFileAs();
        return;
    }

    QFile file(m_currentFilePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, tr("Error"),
            tr("Cannot save file: %1").arg(file.errorString()));
        return;
    }

    QTextStream out(&file);
    out << m_codeEditor->toPlainText();
    file.close();

    m_isModified = false;
    updateStatusBar();

    m_statusLabel->setText(tr("Saved: %1").arg(m_currentFilePath));
}

void MainWindow::saveFileAs()
{
    QString filePath = QFileDialog::getSaveFileName(this,
        tr("Save File As"),
        QString(),
        tr("C Files (*.c);;C++ Files (*.cpp);;Header Files (*.h *.hpp);;All Files (*)"));

    if (filePath.isEmpty()) {
        return;
    }

    m_currentFilePath = filePath;
    saveFile();
}

void MainWindow::compileCode()
{
    // Save file first if needed
    if (m_currentFilePath.isEmpty()) {
        saveFileAs();
        if (m_currentFilePath.isEmpty()) {
            return;
        }
    } else if (m_isModified) {
        saveFile();
    }

    m_statusLabel->setText(tr("Compiling..."));
    m_compilerService->compile(m_currentFilePath);
}

void MainWindow::runCode()
{
    m_statusLabel->setText(tr("Running..."));
    m_compilerService->run();
}

void MainWindow::compileAndRun()
{
    // Save file first if needed
    if (m_currentFilePath.isEmpty()) {
        saveFileAs();
        if (m_currentFilePath.isEmpty()) {
            return;
        }
    } else if (m_isModified) {
        saveFile();
    }

    m_statusLabel->setText(tr("Building..."));
    m_compilerService->compileAndRun(m_currentFilePath);
}

void MainWindow::onCompilationFinished(bool success, const QString &output)
{
    if (success) {
        m_statusLabel->setText(tr("Compilation successful"));
        m_aiChatPanel->appendOutput("✓ Compilation successful\n" + output, false);
    } else {
        m_statusLabel->setText(tr("Compilation failed"));
        m_aiChatPanel->appendOutput("✗ Compilation failed:\n" + output, true);
    }
}

void MainWindow::onExecutionFinished(const QString &output)
{
    m_statusLabel->setText(tr("Execution finished"));
    m_aiChatPanel->appendOutput("Program output:\n" + output, false);
}

void MainWindow::onAIResponseReceived(const QString &response)
{
    m_aiChatPanel->appendAIResponse(response);
    m_statusLabel->setText(tr("AI response received"));
}

void MainWindow::onAISuggestionsReceived(const QStringList &suggestions)
{
    m_aiChatPanel->showSuggestions(suggestions);
    m_statusLabel->setText(tr("Suggestions ready"));
}

void MainWindow::loadSettings()
{
    QSettings settings("AICodeEditor", "AICodeEditor");
    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("windowState").toByteArray());

    int compilerIndex = settings.value("compilerIndex", 0).toInt();
    m_compilerSelector->setCurrentIndex(compilerIndex);

    m_mainSplitter->restoreState(settings.value("splitterState").toByteArray());
}

void MainWindow::saveSettings()
{
    QSettings settings("AICodeEditor", "AICodeEditor");
    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());
    settings.setValue("compilerIndex", m_compilerSelector->currentIndex());
    settings.setValue("splitterState", m_mainSplitter->saveState());
}
