#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "encode.h"
#include "decode.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    encode* encode_tab = new encode();
    decode* decode_tab = new decode();
    ui->tabWidget->addTab(encode_tab, "加密");
    ui->tabWidget->addTab(decode_tab, "解密");
}

MainWindow::~MainWindow()
{
    delete ui;
}

