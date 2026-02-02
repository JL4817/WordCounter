#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <fstream>
#include <algorithm>
#include <cctype>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Word Counter");
    setMinimumSize(600, 650);

    QWidget *central = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(central);

    fileLabel = new QLabel("No file selected", this);
    fileLabel->setStyleSheet("font-size: 14px; color: #666;");
    layout->addWidget(fileLabel);

    uploadButton = new QPushButton("📁 Upload File", this);
    uploadButton->setMinimumHeight(50);
    uploadButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #4CAF50;"
        "   color: white;"
        "   font-size: 16px;"
        "   border-radius: 5px;"
        "}"
        "QPushButton:hover { background-color: #45a049; }"
        );
    connect(uploadButton, &QPushButton::clicked, this, &MainWindow::selectFile);
    layout->addWidget(uploadButton);

    showTopWordsCheck = new QCheckBox("Show only top words", this);
    showTopWordsCheck->setStyleSheet("font-size: 14px; font-weight: bold;");
    layout->addWidget(showTopWordsCheck);

    QHBoxLayout *sliderLayout = new QHBoxLayout();

    QLabel *minLabel = new QLabel("5", this);
    minLabel->setStyleSheet("font-size: 12px; color: #666;");
    sliderLayout->addWidget(minLabel);

    topNSlider = new QSlider(Qt::Horizontal, this);
    topNSlider->setMinimum(5);
    topNSlider->setMaximum(50);
    topNSlider->setValue(10);
    topNSlider->setTickPosition(QSlider::TicksBelow);
    topNSlider->setTickInterval(5);
    topNSlider->setStyleSheet(
        "QSlider::groove:horizontal {"
        "   height: 8px;"
        "   background: #ddd;"
        "   border-radius: 4px;"
        "}"
        "QSlider::handle:horizontal {"
        "   background: #2196F3;"
        "   border: 2px solid #1976D2;"
        "   width: 18px;"
        "   height: 18px;"
        "   margin: -6px 0;"
        "   border-radius: 9px;"
        "}"
        "QSlider::handle:horizontal:hover {"
        "   background: #1976D2;"
        "}"
        );
    sliderLayout->addWidget(topNSlider);

    QLabel *maxLabel = new QLabel("50", this);
    maxLabel->setStyleSheet("font-size: 12px; color: #666;");
    sliderLayout->addWidget(maxLabel);

    sliderValueLabel = new QLabel("10 words", this);
    sliderValueLabel->setStyleSheet(
        "font-size: 16px; font-weight: bold; color: #2196F3; min-width: 80px;"
        );
    sliderValueLabel->setAlignment(Qt::AlignCenter);
    sliderLayout->addWidget(sliderValueLabel);

    layout->addLayout(sliderLayout);

    connect(topNSlider, &QSlider::valueChanged, this, &MainWindow::updateSliderLabel);

    runButton = new QPushButton("▶️ RUN!", this);
    runButton->setMinimumHeight(50);
    runButton->setEnabled(false);
    runButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #2196F3;"
        "   color: white;"
        "   font-size: 16px;"
        "   border-radius: 5px;"
        "}"
        "QPushButton:hover { background-color: #0b7dda; }"
        "QPushButton:disabled { background-color: #ccc; }"
        );
    connect(runButton, &QPushButton::clicked, this, &MainWindow::runAnalysis);
    layout->addWidget(runButton);

    outputDisplay = new QTextEdit(this);
    outputDisplay->setReadOnly(true);
    outputDisplay->setFont(QFont("Courier", 12));
    outputDisplay->setStyleSheet("background-color: #f5f5f5; padding: 10px;");
    layout->addWidget(outputDisplay);

    setCentralWidget(central);
}

MainWindow::~MainWindow()
{
}

void MainWindow::updateSliderLabel(int value)
{
    sliderValueLabel->setText(QString::number(value) + " words");
}

void MainWindow::selectFile()
{
    QString file = QFileDialog::getOpenFileName(
        this,
        "Select Text File",
        QDir::homePath(),
        "Text Files (*.txt)"
        );

    if (!file.isEmpty()) {
        selectedFile = file;
        fileLabel->setText("File: " + file);
        runButton->setEnabled(true);
        outputDisplay->setText("File loaded! Click RUN to analyze.");
    }
}

void MainWindow::runAnalysis()
{
    if (selectedFile.isEmpty()) {
        QMessageBox::warning(this, "Error", "No file selected!");
        return;
    }

    outputDisplay->setText("Processing...");
    processFile();
}

void MainWindow::processFile()
{
    wordCounts.clear();

    std::ifstream file(selectedFile.toStdString());
    if (!file.is_open()) {
        outputDisplay->setText("ERROR: Could not open file!");
        return;
    }

    // Count words
    std::string word;
    int totalWords = 0;
    while (file >> word) {
        std::string cleaned = cleanWord(word);
        if (!cleaned.empty()) {
            wordCounts[cleaned]++;
            totalWords++;
        }
    }
    file.close();

    // Sort words
    std::vector<std::pair<std::string, int>> sorted(wordCounts.begin(), wordCounts.end());

    // Check if we should show top words by frequency
    if (showTopWordsCheck->isChecked()) {
        // Sort by count, then by alphabetically for words with same count
        std::sort(sorted.begin(), sorted.end(),
                  [](const auto &a, const auto &b) {
                      if (a.second == b.second) {
                          // If counts are equal, sort alphabetically
                          return a.first < b.first;
                      }
                      // Otherwise sort by count (from descending order)
                      return a.second > b.second;
                  });

        // Keep only top N words
        int topN = topNSlider->value();
        if (sorted.size() > static_cast<size_t>(topN)) {
            sorted.resize(topN);
        }
    } else {
        // Sort alphabetically
        std::sort(sorted.begin(), sorted.end(),
                  [](const auto &a, const auto &b) {
                      return a.first < b.first;
                  });
    }

    QString result = "=== RESULTS ===\n\n";
    result += QString("Total words: %1\n").arg(totalWords);
    result += QString("Unique words: %1\n\n").arg(wordCounts.size());

    if (showTopWordsCheck->isChecked()) {
        result += QString("TOP %1 MOST USED WORDS\n").arg(topNSlider->value());
    } else {
        result += "ALL WORDS (ALPHABETICALLY)\n";
    }

    result += "WORD                    COUNT\n";
    result += "--------------------------------\n";

    for (const auto &p : sorted) {
        result += QString::fromStdString(p.first) + " - " + QString::number(p.second) + " times\n";
    }

    outputDisplay->setText(result);
}

std::string MainWindow::cleanWord(const std::string &word)
{
    std::string cleaned;
    for (char c : word) {
        if (std::isalpha(c)) {
            cleaned += std::tolower(c);
        }
    }
    return cleaned;
}
