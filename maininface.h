#ifndef MAININFACE_H
#define MAININFACE_H

#include <QWidget>
#include "logmanager.h"

namespace Ui {
class MainInFace;
}

class MainInFace : public QWidget
{
    Q_OBJECT

public:
    explicit MainInFace(QWidget *parent = nullptr);
    ~MainInFace();

     LogManager *log;


private:
    Ui::MainInFace *ui;
};

#endif // MAININFACE_H
