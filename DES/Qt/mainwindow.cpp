#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "DES.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , bg(new QButtonGroup)
{
    ui->setupUi(this);

    ui->encode_RB->setChecked(true);

    bg->setExclusive(true);
    bg->addButton(ui->encode_RB, 0);
    bg->addButton(ui->decode_RB, 1);

    connect(ui->encode_RB, SIGNAL(clicked()), this, SLOT(code_mode_Changed()));
    connect(ui->decode_RB, SIGNAL(clicked()), this, SLOT(code_mode_Changed()));

    connect(ui->run_button, SIGNAL(clicked()), this, SLOT(run()));
}

MainWindow::~MainWindow()
{
    delete ui;
    delete bg;
}

void MainWindow::code_mode_Changed()
{
    code_mode = bg->checkedId();
}

void MainWindow::run()
{
    Code_Mode cm = (code_mode) ? decode : encode;
    QString plaintext = ui->input_text->toPlainText();
    QString key = ui->key_text->toPlainText();

    if(plaintext[plaintext.length() - 1] == '\n')
        plaintext.resize(plaintext.length() - 1);
    if(key[key.length() - 1] == '\n')
        key.resize(key.length() - 1);


    char* plaintext_buf = (char*) calloc(plaintext.length(), sizeof(char));
    int len = plaintext.length();
    for(int i = 0; i < len; i++)
        plaintext_buf[i] = plaintext[i].toLatin1();
    if (cm == encode)
        // 对明文预处理，补齐为64位的倍数并返回长度
        encode_multi64_func(plaintext_buf, &len);
    else
        // 加密时最最终密文进行了可视化处理，所以解密时需要还原为原始密文
        decode_visable_rev_func(plaintext_buf, &len);


    char* key_buf = (char*) calloc(8, sizeof(char));
    int klen = (key.length() < 8) ? key.length() : 8;
    for(int i = 0; i < klen; i++)
        key_buf[i] = key[i].toLatin1();
    for(int i = 0; i + klen < 8; i++)
        memset(key_buf, 0, sizeof(char));

    char* cipher_buf = (char*) calloc(2 * len + 1, sizeof(char));

    main_func(plaintext_buf, cipher_buf, len, key_buf, cm);

    if (cm == encode)
        // 预处理为可显示字符，每次取4位,即每一字节拆成两个字符
        encode_visable_func(cipher_buf, &len);
    cipher_buf[len] = '\0';

    QString cipher = QString(QLatin1String(cipher_buf));
    ui->output_text->setText(cipher);
}

