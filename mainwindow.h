#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QTextEdit>
#include <QLabel>
#include <map>
#include <string>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void selectFile();
    void runAnalysis();

private:
    // UI Components
    QPushButton *uploadButton;
    QPushButton *runButton;
    QTextEdit *outputDisplay;
    QLabel *fileLabel;

    // Data
    std::map<std::string, int> wordCounts;
    QString selectedFile;

    // Helper functions
    void processFile();
    std::string cleanWord(const std::string &word);
};

#endif // MAINWINDOW_H
