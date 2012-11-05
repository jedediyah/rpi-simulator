

#include "about.h"
#include "ui_about.h"

about::about(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::about)
{
    m_ui->setupUi(this);
}

about::~about()
{
    delete m_ui;
}
