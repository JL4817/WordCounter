#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QTextEdit>
#include <QLabel>
#include <QCheckBox>
#include <QSlider>
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
    void updateSliderLabel(int value);

private:
    QPushButton *uploadButton;
    QPushButton *runButton;
    QTextEdit *outputDisplay;
    QLabel *fileLabel;
    QCheckBox *showTopWordsCheck;
    QSlider *topNSlider;
    QLabel *sliderValueLabel;

    std::map<std::string, int> wordCounts;
    QString selectedFile;

    void processFile();
    std::string cleanWord(const std::string &word);
};

#endif
