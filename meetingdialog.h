#ifndef MEETINGDIALOG_H
#define MEETINGDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QTextEdit>
#include <QDateEdit>
#include <QTimeEdit>
#include <QPushButton>
#include <QFormLayout>
#include <QSqlRecord>

class MeetingDialog : public QDialog {
    Q_OBJECT

public:
    MeetingDialog(QWidget *parent = nullptr, QSqlRecord *record = nullptr);
    QSqlRecord getRecord() const;

private:
    QTextEdit *agendaEdit;
    QDateEdit *dateEdit;
    QTimeEdit *startTimeEdit;
    QTimeEdit *endTimeEdit;
    QPushButton *saveButton;
    QPushButton *cancelButton;
    
    QSqlRecord currentRecord;
};

#endif // MEETINGDIALOG_H 