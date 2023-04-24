#include "encode.h"
#include "ui_encode.h"

encode::encode(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::encode)
{
    ui->setupUi(this);

    connect(ui->btn_pq_generate, SIGNAL(clicked()), this, SLOT(pq_generate()));
    connect(ui->btn_encode, SIGNAL(clicked()), this, SLOT(encode_func()));
}

encode::~encode()
{
    delete ui;
}

void encode::pq_generate(void)
{
    ui->btn_pq_generate->setEnabled(false);
    ui->btn_encode->setEnabled(true);

    QString message = ui->messageText->toPlainText();

    //TODO: 由message求pq
    int p = 0, q = 0;

    ui->text_p->setPlainText(QString::number(p));
    ui->text_q->setPlainText(QString::number(q));
}


void encode::encode_func(void)
{
    ui->btn_pq_generate->setEnabled(true);
    ui->btn_encode->setEnabled(false);

    QString message = ui->messageText->toPlainText();
    QString cipher = "";
    int p = ui->text_p->toPlainText().toInt();
    int q = ui->text_q->toPlainText().toInt();
    int N = ui->text_N->toPlainText().toInt();
    int r = ui->text_r->toPlainText().toInt();
    int e = ui->text_e->toPlainText().toInt();
    int d = ui->text_d->toPlainText().toInt();

    //TODO: 加密处理得到密文

    ui->cipherText->setPlainText(cipher);
}
