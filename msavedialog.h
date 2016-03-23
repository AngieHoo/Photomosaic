#ifndef MSAVEDIALOG_H
#define MSAVEDIALOG_H
#include <QDialog>
#include <QPushButton>
#include <QMouseEvent>
class MsaveDialog : public QDialog
{
    Q_OBJECT
public:
    MsaveDialog(QWidget *parent = 0);
    ~MsaveDialog();
protected:
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
private:
    QPoint last;
signals:
    saveMosaic();
private slots:
    void toSaveMosaic();

};

#endif // MSAVEDIALOG_H

