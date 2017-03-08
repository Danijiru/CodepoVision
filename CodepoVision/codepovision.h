#ifndef CODEPOVISION_H
#define CODEPOVISION_H

#include <QMainWindow>

namespace Ui {
class CodepoVision;
}

class CodepoVision : public QMainWindow
{
    Q_OBJECT

public:
    explicit CodepoVision(QWidget *parent = 0);
    ~CodepoVision();

private:
    Ui::CodepoVision *ui;
};

#endif // CODEPOVISION_H
