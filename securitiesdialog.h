#ifndef SECURITIESDIALOG_H
#define SECURITIESDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QPushButton>
#include <QFormLayout>
#include <QSqlRecord>

class SecuritiesDialog : public QDialog {
    Q_OBJECT

public:
    SecuritiesDialog(QWidget *parent = nullptr, QSqlRecord *record = nullptr);
    QSqlRecord getRecord() const;

private:
    QLineEdit *nameEdit;
    QComboBox *typeComboBox;
    QDoubleSpinBox *nominalValueSpinBox;
    QSpinBox *quantitySpinBox;
    QPushButton *saveButton;
    QPushButton *cancelButton;
    
    QSqlRecord currentRecord;
};

#endif // SECURITIESDIALOG_H 