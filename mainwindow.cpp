#include "mainwindow.h"
#include <QVBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <fstream>
#include <algorithm>
#include <cctype>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Word Counter");
    setMinimumSize(600, 600);

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

    // Sort alphabetically
    std::vector<std::pair<std::string, int>> sorted(wordCounts.begin(), wordCounts.end());
    std::sort(sorted.begin(), sorted.end(),
              [](const auto &a, const auto &b) { return a.first < b.first; });

    QString result = "=== RESULTS ===\n\n";
    result += QString("Total words: %1\n").arg(totalWords);
    result += QString("Unique words: %1\n\n").arg(wordCounts.size());
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
