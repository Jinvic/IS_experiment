#ifndef DECODE_H
#define DECODE_H

#include <QWidget>

namespace Ui
{
class decode;
}

class decode : public QWidget
{
    Q_OBJECT

public:
    explicit decode(QWidget *parent = nullptr);
    ~decode();

private slots:
    void decode_func(void);

private:
    Ui::decode *ui;
};

#endif // DECODE_H
