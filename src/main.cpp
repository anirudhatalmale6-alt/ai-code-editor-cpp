#include <QApplication>
#include <QStyleFactory>
#include <QPalette>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Set application info
    app.setApplicationName("AI Code Editor");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("AICodeEditor");

    // Apply modern dark theme (Windows 10+ style)
    app.setStyle(QStyleFactory::create("Fusion"));

    QPalette darkPalette;
    darkPalette.setColor(QPalette::Window, QColor(32, 32, 32));
    darkPalette.setColor(QPalette::WindowText, QColor(240, 240, 240));
    darkPalette.setColor(QPalette::Base, QColor(25, 25, 25));
    darkPalette.setColor(QPalette::AlternateBase, QColor(45, 45, 45));
    darkPalette.setColor(QPalette::ToolTipBase, QColor(50, 50, 50));
    darkPalette.setColor(QPalette::ToolTipText, QColor(240, 240, 240));
    darkPalette.setColor(QPalette::Text, QColor(240, 240, 240));
    darkPalette.setColor(QPalette::Button, QColor(45, 45, 45));
    darkPalette.setColor(QPalette::ButtonText, QColor(240, 240, 240));
    darkPalette.setColor(QPalette::BrightText, Qt::red);
    darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::Highlight, QColor(0, 120, 215));
    darkPalette.setColor(QPalette::HighlightedText, Qt::white);
    darkPalette.setColor(QPalette::Disabled, QPalette::Text, QColor(127, 127, 127));
    darkPalette.setColor(QPalette::Disabled, QPalette::ButtonText, QColor(127, 127, 127));

    app.setPalette(darkPalette);

    // Apply stylesheet for modern look
    app.setStyleSheet(R"(
        QMainWindow {
            background-color: #202020;
        }
        QToolTip {
            color: #f0f0f0;
            background-color: #323232;
            border: 1px solid #505050;
            border-radius: 4px;
            padding: 4px;
        }
        QMenuBar {
            background-color: #2d2d2d;
            border-bottom: 1px solid #3d3d3d;
        }
        QMenuBar::item {
            padding: 6px 12px;
        }
        QMenuBar::item:selected {
            background-color: #3d3d3d;
        }
        QMenu {
            background-color: #2d2d2d;
            border: 1px solid #3d3d3d;
        }
        QMenu::item {
            padding: 6px 24px;
        }
        QMenu::item:selected {
            background-color: #0078d7;
        }
        QPushButton {
            background-color: #0078d7;
            color: white;
            border: none;
            padding: 8px 16px;
            border-radius: 4px;
            font-weight: bold;
        }
        QPushButton:hover {
            background-color: #1084d9;
        }
        QPushButton:pressed {
            background-color: #006cc1;
        }
        QPushButton:disabled {
            background-color: #4d4d4d;
            color: #808080;
        }
        QLineEdit, QTextEdit, QPlainTextEdit {
            background-color: #1e1e1e;
            border: 1px solid #3d3d3d;
            border-radius: 4px;
            padding: 6px;
            selection-background-color: #0078d7;
        }
        QLineEdit:focus, QTextEdit:focus, QPlainTextEdit:focus {
            border: 1px solid #0078d7;
        }
        QScrollBar:vertical {
            background-color: #2d2d2d;
            width: 12px;
            margin: 0;
        }
        QScrollBar::handle:vertical {
            background-color: #5a5a5a;
            min-height: 30px;
            border-radius: 6px;
            margin: 2px;
        }
        QScrollBar::handle:vertical:hover {
            background-color: #6a6a6a;
        }
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
            height: 0;
        }
        QScrollBar:horizontal {
            background-color: #2d2d2d;
            height: 12px;
            margin: 0;
        }
        QScrollBar::handle:horizontal {
            background-color: #5a5a5a;
            min-width: 30px;
            border-radius: 6px;
            margin: 2px;
        }
        QScrollBar::handle:horizontal:hover {
            background-color: #6a6a6a;
        }
        QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal {
            width: 0;
        }
        QSplitter::handle {
            background-color: #3d3d3d;
        }
        QTabWidget::pane {
            border: 1px solid #3d3d3d;
            background-color: #202020;
        }
        QTabBar::tab {
            background-color: #2d2d2d;
            padding: 8px 16px;
            border: 1px solid #3d3d3d;
            border-bottom: none;
        }
        QTabBar::tab:selected {
            background-color: #202020;
            border-top: 2px solid #0078d7;
        }
        QComboBox {
            background-color: #2d2d2d;
            border: 1px solid #3d3d3d;
            border-radius: 4px;
            padding: 6px 12px;
        }
        QComboBox:hover {
            border-color: #0078d7;
        }
        QComboBox::drop-down {
            border: none;
            width: 24px;
        }
        QComboBox QAbstractItemView {
            background-color: #2d2d2d;
            border: 1px solid #3d3d3d;
            selection-background-color: #0078d7;
        }
        QGroupBox {
            border: 1px solid #3d3d3d;
            border-radius: 6px;
            margin-top: 12px;
            padding-top: 12px;
            font-weight: bold;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 12px;
            padding: 0 6px;
        }
        QStatusBar {
            background-color: #007acc;
            color: white;
        }
    )");

    MainWindow mainWindow;
    mainWindow.show();

    return app.exec();
}
