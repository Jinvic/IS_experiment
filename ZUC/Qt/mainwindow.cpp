#include "mainwindow.h"
#include "ui_mainwindow.h"
#include"ZUC.cpp"
#include"pretreat.cpp"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->PB_encode, SIGNAL(clicked()), this, SLOT(encode_func()));
    connect(ui->PB_decode, SIGNAL(clicked()), this, SLOT(decode_func()));
    qDebug() << "init";
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::encode_func(void)
{
    qDebug() << "encode";
    QString k_str = ui->text_k->toPlainText();
    QString vi_str = ui->text_vi->toPlainText();
    uint8_t k[32] = {0}, vi[32] = {0};
    PT::hex_to_dec(k_str.toStdString(), k);
    PT::hex_to_dec(vi_str.toStdString(), vi);

    std::string input_str = ui->messageText->toPlainText().toStdString();
    int len = input_str.length();
    len += (4 - len % 4); // 补齐为四字节倍数

    char* input = new char[len + 1];
    memset(input, 0, len + 1);
    char* output = new char[len * 2 + 1];
    memset(output, 0, len * 2 + 1);
    for(unsigned long long i = 0; i < input_str.length(); i++)
        input[i] = input_str[i];

    ZUC::stream_work(input, output, len, k, vi);
    PT::encode_visable_func(output, &len);
    ui->cipherText->setPlainText(QString(output));

    delete[] input;
    delete[] output;
}

void MainWindow::decode_func(void)
{
    qDebug() << "decode";
    QString k_str = ui->text_k->toPlainText();
    QString vi_str = ui->text_vi->toPlainText();
    uint8_t k[32] = {0}, vi[32] = {0};
    PT::hex_to_dec(k_str.toStdString(), k);
    PT::hex_to_dec(vi_str.toStdString(), vi);

    std::string input_str = ui->cipherText->toPlainText().toStdString();
    int len = input_str.length();

    char* input = new char[len + 1];
    memset(input, 0, len + 1);
    char* output = new char[len / 2 + 1];
    memset(output, 0, len / 2 + 1);
    for(unsigned long long i = 0; i < input_str.length(); i++)
        input[i] = input_str[i];
    PT::decode_visable_rev_func(input, &len);

    ZUC::stream_work(input, output, len, k, vi);
    ui->messageText->setPlainText(QString::fromUtf8(output));

    delete[] input;
    delete[] output;
}


