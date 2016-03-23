#include "msavedialog.h"
#include <QLabel>
#include <QBoxLayout>
MsaveDialog::MsaveDialog(QWidget *parent)
    :QDialog(parent)
{
    setMinimumSize(350,150);
    setStyleSheet("border-radius:12px;"
                  "background: rgb(235, 251, 255);"
                 );
    setWindowFlags(Qt::FramelessWindowHint);
    QBoxLayout *mainLayout = new  QBoxLayout(QBoxLayout::TopToBottom,this);;
    mainLayout->setMargin(10);
    mainLayout->setSpacing(10);

    QLabel *AdressLabel = new QLabel("Success!!!\nSave your \"mosaics\"?",this);
    AdressLabel->setStyleSheet("color:black;"
                               "backround:transparent;"
                               "font-family:System;"
                               "font-size:24px");
    AdressLabel->setAlignment(Qt::AlignCenter);
    AdressLabel->show();
    QBoxLayout *adressLayout = new QBoxLayout(QBoxLayout::LeftToRight,this);
    adressLayout->setSpacing(10);
    adressLayout->setContentsMargins(0,0,0,0);
    adressLayout->addWidget(AdressLabel);
    mainLayout->addLayout(adressLayout);

    QPushButton *okButton = new QPushButton(tr("OK"),this);
    okButton->setStyleSheet("QPushButton"
                            "{"
                            "border-radius:5px;"
                            "font-size:26px;"
                            "color:white;"
                            "font-family:System;"
                            " background-color: rgb(0, 170, 255);"
                            "}"
                            "QPushButton:pressed{"
                            "background:rgb(86, 128, 136);"
                            "}");
    okButton->setFixedHeight(40);
    QPushButton *cancelButton = new QPushButton(tr("NO"),this);
    cancelButton->setStyleSheet("QPushButton"
                                "{"
                                "border-radius:5px;"
                                "font-size:26px;"
                                "color:white;"
                                "font-family:System;"
                                " background-color: rgb(0, 170, 255);"
                                "}"
                                "QPushButton:pressed{"
                                "background:rgb(86, 128, 136);"
                                "}");
    cancelButton->setFixedHeight(40);
    QBoxLayout *btnLayout = new QBoxLayout(QBoxLayout::LeftToRight,this);;
    btnLayout->setSpacing(60);
    btnLayout->addWidget(okButton);
    btnLayout->addWidget(cancelButton);
    mainLayout->addLayout(btnLayout);

    setLayout(mainLayout);
    connect(okButton,SIGNAL(clicked()),this,SLOT(toSaveMosaic()));
    connect(cancelButton,SIGNAL(clicked()),this,SLOT(reject()));

}
void MsaveDialog::mousePressEvent(QMouseEvent *e)
{
    last = e->globalPos();
}
void MsaveDialog::mouseMoveEvent(QMouseEvent *e)
{
    int dx = e->globalX() - last.x();
    int dy = e->globalY() - last.y();
    last = e->globalPos();
    move(x()+dx,y()+dy);
}
void MsaveDialog::mouseReleaseEvent(QMouseEvent *e)
{
    int dx = e->globalX() - last.x();
    int dy = e->globalY() - last.y();
    move(x()+dx, y()+dy);
}
void MsaveDialog::toSaveMosaic()
{
    emit  saveMosaic();
    accept();
}

MsaveDialog::~MsaveDialog()
{

}

