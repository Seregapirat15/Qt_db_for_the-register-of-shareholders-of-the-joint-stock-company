#ifndef OWNERSDIALOG_H
#define OWNERSDIALOG_H

#include <QDialog>
#include <QComboBox>
#include <QDateEdit>
#include <QPushButton>
#include <QFormLayout>
#include <QSqlRecord>
#include <QSqlQueryModel>

class OwnersDialog : public QDialog {
    Q_OBJECT

public:
    OwnersDialog(QWidget *parent = nullptr, QSqlRecord *record = nullptr);
    QSqlRecord getRecord() const;

private slots:
    void loadShareholders();
    void loadSecurities();

private:
    QComboBox *shareholderComboBox;
    QComboBox *securitiesComboBox;
    QDateEdit *acquisitionDateEdit;
    QPushButton *saveButton;
    QPushButton *cancelButton;
    
    QSqlQueryModel *shareholdersModel;
    QSqlQueryModel *securitiesModel;
    QSqlRecord currentRecord;
};

#endif // OWNERSDIALOG_H 