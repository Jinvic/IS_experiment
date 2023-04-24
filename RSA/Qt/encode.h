#ifndef ENCODE_H
#define ENCODE_H

#include <QWidget>

namespace Ui
{
class encode;
}

class encode : public QWidget
{
    Q_OBJECT

public:
    explicit encode(QWidget *parent = nullptr);
    ~encode();

private slots:
    void pq_generate(void);
    void encode_func(void);

private:
    Ui::encode *ui;
};

#endif // ENCODE_H
