#ifndef COMPILERSERVICE_H
#define COMPILERSERVICE_H

#include <QObject>
#include <QProcess>
#include <QString>

class CompilerService : public QObject
{
    Q_OBJECT

public:
    explicit CompilerService(QObject *parent = nullptr);

    void setCompiler(const QString &compiler);
    QString currentCompiler() const { return m_compiler; }

    void compile(const QString &sourceFile);
    void run();
    void compileAndRun(const QString &sourceFile);

signals:
    void compilationFinished(bool success, const QString &output);
    void executionFinished(const QString &output);

private slots:
    void onCompileProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onRunProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);

private:
    QString getCompilerCommand() const;
    QString getOutputPath(const QString &sourceFile) const;
    QStringList getCompilerFlags() const;

    QProcess *m_compileProcess;
    QProcess *m_runProcess;
    QString m_compiler;
    QString m_currentOutputPath;
    QString m_currentSourceFile;
    bool m_runAfterCompile;
};

#endif // COMPILERSERVICE_H
