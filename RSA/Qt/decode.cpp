#include "decode.h"
#include "ui_decode.h"

decode::decode(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::decode)
{
    ui->setupUi(this);

    connect(ui->btn_decode, SIGNAL(click()), this, SLOT(decode_func()));
}

decode::~decode()
{
    delete ui;
}

void decode::decode_func(void)
{
    QString cipher = ui->cipherText->toPlainText();
    QString message;
    int N = ui->text_N->toPlainText().toInt();
    int d = ui->text_d->toPlainText().toInt();

    //TODO: 根据密文和密钥解密明文

    ui->messageText->setPlainText(message);
}
