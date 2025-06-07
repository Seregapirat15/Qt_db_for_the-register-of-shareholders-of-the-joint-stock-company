#ifndef SHAREHOLDERDIALOG_H
#define SHAREHOLDERDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QDateEdit>
#include <QPushButton>
#include <QFormLayout>
#include <QSqlRecord>

class ShareholderDialog : public QDialog {
    Q_OBJECT

public:
    ShareholderDialog(QWidget *parent = nullptr, QSqlRecord *record = nullptr);
    QSqlRecord getRecord() const;

private:
    QLineEdit *nameEdit;
    QDateEdit *birthDateEdit;
    QLineEdit *passportEdit;
    QLineEdit *emailEdit;
    QLineEdit *phoneEdit;
    QPushButton *saveButton;
    QPushButton *cancelButton;
    
    QSqlRecord currentRecord;
};

#endif // SHAREHOLDERDIALOG_H 