#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets>

QT_BEGIN_NAMESPACE
namespace Ui
{
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void code_mode_Changed(void);
    void run(void);

private:
    Ui::MainWindow *ui;
    QButtonGroup* bg;
    int code_mode = 0;
};
#endif // MAINWINDOW_H
