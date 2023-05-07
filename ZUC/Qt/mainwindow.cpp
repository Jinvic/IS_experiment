#include "mainwindow.h"
#include "ui_mainwindow.h"
#include"ZUC.hpp"
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
    std::string k_str = ui->text_k->toPlainText().toStdString();
    std::string iv_str = ui->text_vi->toPlainText().toStdString();
    bit128 k = 0, iv = 0;
    k = PT::hex_to_bit128(k_str);
    iv = PT::hex_to_bit128(iv_str);

    std::string input_str = ui->messageText->toPlainText().toStdString();
    std::string input_bit = PT::str_to_bit(input_str);
    std::string output_bit = ZUC::ZUCEncrypt(k, iv, input_bit.length(), input_bit);
    std::string output_str = PT::bit_to_str(output_bit);
    output_str = PT::str_to_hex(output_str);
    ui->cipherText->setPlainText(QString::fromStdString(output_str));
}

void MainWindow::decode_func(void)
{
    qDebug() << "decode";
    std::string k_str = ui->text_k->toPlainText().toStdString();
    std::string iv_str = ui->text_vi->toPlainText().toStdString();
    bit128 k, iv;
    k = PT::hex_to_bit128(k_str);
    iv = PT::hex_to_bit128(iv_str);

    std::string input_str = ui->cipherText->toPlainText().toStdString();
    input_str = PT::hex_to_str(input_str);
    std::string input_bit = PT::str_to_bit(input_str);
    std::string output_bit = ZUC::ZUCEncrypt(k, iv, input_bit.length(), input_bit);
    std::string output_str = PT::bit_to_str(output_bit);
    ui->messageText->setPlainText(QString::fromStdString(output_str));
}


