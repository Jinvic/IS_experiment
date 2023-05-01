#include "mainwindow.h"
#include "ui_mainwindow.h"
#include"Tiger.cpp"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , bg(new QButtonGroup)
{
    ui->setupUi(this);

    ui->RB_Tiger2->setChecked(true);
    TH::tiger_mode = TH::Tiger2;

    bg->setExclusive(true);
    bg->addButton(ui->RB_Tiger, 0);
    bg->addButton(ui->RB_Tiger2, 1);

    connect(ui->RB_Tiger, SIGNAL(clicked()), this, SLOT(mode_change()));
    connect(ui->RB_Tiger2, SIGNAL(clicked()), this, SLOT(mode_change()));

    connect(ui->PB_start, SIGNAL(clicked()), this, SLOT(start()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::mode_change()
{
    if(bg->checkedId() == 0)
        TH::tiger_mode = TH::Tiger;
    else
        TH::tiger_mode = TH::Tiger2;
}

void MainWindow::start()
{
    const int buffer_size = 8192;
    char input_buf[buffer_size] = {0};
    char output_buf[48] = {0};
    char* input;

    input = ui->msgText->toPlainText().toLatin1().data();
    int len = ui->msgText->toPlainText().length();
    memcpy(input_buf, input, len);

    TH::multi512_func(input_buf, &len);
    TH::main_func(input_buf, output_buf, len);

    ui->hashText->setPlainText(QString(output_buf));
}
