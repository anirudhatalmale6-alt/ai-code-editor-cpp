#include "compilerservice.h"
#include <QFileInfo>
#include <QDir>
#include <QSettings>

CompilerService::CompilerService(QObject *parent)
    : QObject(parent)
    , m_runAfterCompile(false)
{
    m_compileProcess = new QProcess(this);
    m_runProcess = new QProcess(this);

    connect(m_compileProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &CompilerService::onCompileProcessFinished);
    connect(m_runProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &CompilerService::onRunProcessFinished);

    // Load saved compiler or default to g++
    QSettings settings("AICodeEditor", "AICodeEditor");
    m_compiler = settings.value("compiler", "g++").toString();
}

void CompilerService::setCompiler(const QString &compiler)
{
    m_compiler = compiler;
    QSettings settings("AICodeEditor", "AICodeEditor");
    settings.setValue("compiler", compiler);
}

QString CompilerService::getCompilerCommand() const
{
    if (m_compiler == "g++" || m_compiler == "mingw-g++") {
        return "g++";
    } else if (m_compiler == "clang++") {
        return "clang++";
    } else if (m_compiler == "cl") {
        return "cl";
    }
    return "g++";
}

QStringList CompilerService::getCompilerFlags() const
{
    QStringList flags;

    if (m_compiler == "cl") {
        // MSVC flags
        flags << "/EHsc" << "/W4" << "/std:c++17";
    } else {
        // GCC/Clang flags
        flags << "-std=c++17" << "-Wall" << "-Wextra" << "-g";
    }

    return flags;
}

QString CompilerService::getOutputPath(const QString &sourceFile) const
{
    QFileInfo fi(sourceFile);
    QString outputDir = fi.absolutePath();
    QString baseName = fi.completeBaseName();

#ifdef Q_OS_WIN
    return outputDir + QDir::separator() + baseName + ".exe";
#else
    return outputDir + QDir::separator() + baseName;
#endif
}

void CompilerService::compile(const QString &sourceFile)
{
    m_runAfterCompile = false;
    m_currentSourceFile = sourceFile;
    m_currentOutputPath = getOutputPath(sourceFile);

    QString compiler = getCompilerCommand();
    QStringList args = getCompilerFlags();

    if (m_compiler == "cl") {
        // MSVC uses /Fe for output
        args << "/Fe:" + m_currentOutputPath;
        args << sourceFile;
    } else {
        // GCC/Clang use -o for output
        args << "-o" << m_currentOutputPath;
        args << sourceFile;
    }

    m_compileProcess->setWorkingDirectory(QFileInfo(sourceFile).absolutePath());
    m_compileProcess->start(compiler, args);
}

void CompilerService::run()
{
    if (m_currentOutputPath.isEmpty() || !QFileInfo::exists(m_currentOutputPath)) {
        emit executionFinished("Error: No compiled executable found. Please compile first.");
        return;
    }

    m_runProcess->setWorkingDirectory(QFileInfo(m_currentOutputPath).absolutePath());
    m_runProcess->start(m_currentOutputPath, QStringList());
}

void CompilerService::compileAndRun(const QString &sourceFile)
{
    m_runAfterCompile = true;
    m_currentSourceFile = sourceFile;
    m_currentOutputPath = getOutputPath(sourceFile);

    compile(sourceFile);
}

void CompilerService::onCompileProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    Q_UNUSED(exitStatus);

    QString output;
    QString stdOut = m_compileProcess->readAllStandardOutput();
    QString stdErr = m_compileProcess->readAllStandardError();

    if (!stdOut.isEmpty()) {
        output += stdOut;
    }
    if (!stdErr.isEmpty()) {
        if (!output.isEmpty()) output += "\n";
        output += stdErr;
    }

    if (output.isEmpty() && exitCode == 0) {
        output = "Build completed successfully.";
    }

    bool success = (exitCode == 0);
    emit compilationFinished(success, output);

    if (success && m_runAfterCompile) {
        run();
    }
}

void CompilerService::onRunProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    Q_UNUSED(exitStatus);

    QString output;
    QString stdOut = m_runProcess->readAllStandardOutput();
    QString stdErr = m_runProcess->readAllStandardError();

    if (!stdOut.isEmpty()) {
        output += stdOut;
    }
    if (!stdErr.isEmpty()) {
        if (!output.isEmpty()) output += "\n";
        output += "Errors:\n" + stdErr;
    }

    output += QString("\n\n[Process exited with code %1]").arg(exitCode);

    emit executionFinished(output);
}
