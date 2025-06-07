#include "meetingdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>

MeetingDialog::MeetingDialog(QWidget *parent, QSqlRecord *record) : QDialog(parent) {
    setWindowTitle("Редактирование собрания акционеров");
    resize(500, 350);
    
    // Создаем элементы формы
    agendaEdit = new QTextEdit(this);
    agendaEdit->setPlaceholderText("Введите повестку дня собрания");
    
    dateEdit = new QDateEdit(QDate::currentDate(), this);
    dateEdit->setCalendarPopup(true);
    dateEdit->setDisplayFormat("dd.MM.yyyy");
    
    startTimeEdit = new QTimeEdit(QTime(10, 0), this);
    startTimeEdit->setDisplayFormat("hh:mm");
    
    endTimeEdit = new QTimeEdit(QTime(12, 0), this);
    endTimeEdit->setDisplayFormat("hh:mm");
    
    // Создаем форму
    QFormLayout *formLayout = new QFormLayout();
    formLayout->addRow("Повестка дня:", agendaEdit);
    formLayout->addRow("Дата:", dateEdit);
    formLayout->addRow("Время начала:", startTimeEdit);
    formLayout->addRow("Время окончания:", endTimeEdit);
    
    // Создаем кнопки
    saveButton = new QPushButton("Сохранить", this);
    cancelButton = new QPushButton("Отмена", this);
    
    // Размещаем кнопки
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    buttonLayout->addWidget(saveButton);
    buttonLayout->addWidget(cancelButton);
    
    // Создаем основной layout
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(formLayout);
    mainLayout->addLayout(buttonLayout);
    
    // Соединяем сигналы и слоты
    connect(saveButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    
    // Если передана запись, заполняем поля
    if (record) {
        currentRecord = *record;
        agendaEdit->setText(record->value("Повестка_дня").toString());
        dateEdit->setDate(record->value("Дата").toDate());
        
        // Если есть время начала и окончания
        if (!record->isNull("Время_начала")) {
            startTimeEdit->setTime(record->value("Время_начала").toTime());
        }
        
        if (!record->isNull("Время_окончания")) {
            endTimeEdit->setTime(record->value("Время_окончания").toTime());
        }
    }
}

QSqlRecord MeetingDialog::getRecord() const {
    QSqlRecord record = currentRecord;
    record.setValue("Повестка_дня", agendaEdit->toPlainText());
    record.setValue("Дата", dateEdit->date());
    record.setValue("Время_начала", startTimeEdit->time());
    record.setValue("Время_окончания", endTimeEdit->time());
    return record;
} 