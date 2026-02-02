#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <fstream>
#include <algorithm>
#include <cctype>

using namespace std;

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

    uploadButton = new QPushButton("Upload File", this);
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
    showTopWordsCheck->setStyleSheet("font-size: 14px;");
    layout->addWidget(showTopWordsCheck);

    QHBoxLayout *sliderLayout = new QHBoxLayout();

    QLabel *minLabel = new QLabel("5", this);
    sliderLayout->addWidget(minLabel);

    topNSlider = new QSlider(Qt::Horizontal, this);
    topNSlider->setMinimum(5);
    topNSlider->setMaximum(50);
    topNSlider->setValue(10);
    topNSlider->setTickPosition(QSlider::TicksBelow);
    topNSlider->setTickInterval(5);
    sliderLayout->addWidget(topNSlider);

    QLabel *maxLabel = new QLabel("50", this);
    sliderLayout->addWidget(maxLabel);

    sliderValueLabel = new QLabel("10 words", this);
    sliderValueLabel->setStyleSheet("font-weight: bold; color: #2196F3;");
    sliderLayout->addWidget(sliderValueLabel);

    layout->addLayout(sliderLayout);

    connect(topNSlider, &QSlider::valueChanged, this, &MainWindow::updateSliderLabel);

    runButton = new QPushButton("RUN", this);
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
    outputDisplay->setFont(QFont("Courier", 11));
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
        fileLabel->setText("Selected: " + file);
        runButton->setEnabled(true);
        outputDisplay->setText("File ready. Press RUN to analyze.");
    }
}

void MainWindow::runAnalysis()
{
    if (selectedFile.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please select a file first!");
        return;
    }

    outputDisplay->setText("Processing...");
    processFile();
}

void MainWindow::processFile()
{
    wordCounts.clear();

    ifstream file(selectedFile.toStdString());
    if (!file.is_open()) {
        outputDisplay->setText("ERROR: Could not open file!");
        return;
    }

    string word;
    int totalWords = 0;

    // Read and count words
    while (file >> word) {
        string cleaned = cleanWord(word);
        if (!cleaned.empty()) {
            wordCounts[cleaned]++;
            totalWords++;
        }
    }
    file.close();

    // Put into vector for sorting
    vector<pair<string, int>> wordList;
    for (auto& pair : wordCounts) {
        wordList.push_back(pair);
    }

    // Sort based on checkbox
    if (showTopWordsCheck->isChecked()) {
        // Sort by frequency, then alphabetically
        sort(wordList.begin(), wordList.end(),
             [](const auto &a, const auto &b) {
                 if (a.second != b.second) {
                     return a.second > b.second;  // higher count first
                 }
                 return a.first < b.first;  // alphabetical for ties
             });

        // Only keep top N
        int topN = topNSlider->value();
        if (wordList.size() > topN) {
            wordList.resize(topN);
        }
    } else {
        // Alphabetical sort
        sort(wordList.begin(), wordList.end(),
             [](const auto &a, const auto &b) {
                 return a.first < b.first;
             });
    }

    // Display results
    QString output = "=== RESULTS ===\n\n";
    output += "Total words: " + QString::number(totalWords) + "\n";
    output += "Unique words: " + QString::number(wordCounts.size()) + "\n\n";

    if (showTopWordsCheck->isChecked()) {
        output += "TOP " + QString::number(topNSlider->value()) + " MOST COMMON WORDS\n";
    } else {
        output += "ALL WORDS (A-Z)\n";
    }

    output += "--------------------------------\n";

    for (size_t i = 0; i < wordList.size(); i++) {
        output += QString::fromStdString(wordList[i].first);
        output += " - " + QString::number(wordList[i].second) + " times\n";
    }

    outputDisplay->setText(output);
}

string MainWindow::cleanWord(const string &word)
{
    string result;
    for (char c : word) {
        if (isalpha(c)) {
            result += tolower(c);
        }
    }
    return result;
}
