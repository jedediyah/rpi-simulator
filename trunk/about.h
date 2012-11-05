#ifndef ABOUT_H
#define ABOUT_H

#include <QtGui>
#include <QDialog>

namespace Ui {
    class about;
}

class about : public QDialog {
    Q_OBJECT
    Q_DISABLE_COPY(about)
public:
    explicit about(QWidget *parent = 0);
    virtual ~about();

private:
    Ui::about *m_ui;
};

#endif // ABOUT_H
