#ifndef OPERATIONSDIALOG_H
#define OPERATIONSDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QDateTimeEdit>
#include <QPushButton>
#include <QFormLayout>
#include <QSqlRecord>
#include <QSqlQueryModel>

class OperationsDialog : public QDialog {
    Q_OBJECT

public:
    OperationsDialog(QWidget *parent = nullptr, QSqlRecord *record = nullptr);
    QSqlRecord getRecord() const;

private slots:
    void loadSecurities();

private:
    QLineEdit *sellerEdit;
    QLineEdit *buyerEdit;
    QDateTimeEdit *transactionTimeEdit;
    QComboBox *securitiesComboBox;
    QPushButton *saveButton;
    QPushButton *cancelButton;
    
    QSqlQueryModel *securitiesModel;
    QSqlRecord currentRecord;
};

#endif // OPERATIONSDIALOG_H 