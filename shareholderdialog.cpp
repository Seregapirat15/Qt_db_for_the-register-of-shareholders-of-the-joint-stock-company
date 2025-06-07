#include "shareholderdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>

ShareholderDialog::ShareholderDialog(QWidget *parent, QSqlRecord *record) : QDialog(parent) {
    setWindowTitle("Редактирование акционера");
    resize(400, 300);
    
    // Создаем элементы формы
    nameEdit = new QLineEdit(this);
    birthDateEdit = new QDateEdit(this);
    birthDateEdit->setCalendarPopup(true);
    birthDateEdit->setDisplayFormat("dd.MM.yyyy");
    passportEdit = new QLineEdit(this);
    emailEdit = new QLineEdit(this);
    phoneEdit = new QLineEdit(this);
    
    // Создаем форму
    QFormLayout *formLayout = new QFormLayout();
    formLayout->addRow("ФИО акционера:", nameEdit);
    formLayout->addRow("Дата рождения:", birthDateEdit);
    formLayout->addRow("Паспортные данные:", passportEdit);
    formLayout->addRow("Электронная почта:", emailEdit);
    formLayout->addRow("Номер телефона:", phoneEdit);
    
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
        nameEdit->setText(record->value("ФИО_акционера").toString());
        birthDateEdit->setDate(record->value("Дата_рождения").toDate());
        passportEdit->setText(record->value("Паспортные_данные").toString());
        emailEdit->setText(record->value("Электронная_почта").toString());
        phoneEdit->setText(record->value("Номер_телефона").toString());
    }
}

QSqlRecord ShareholderDialog::getRecord() const {
    QSqlRecord record = currentRecord;
    record.setValue("ФИО_акционера", nameEdit->text());
    record.setValue("Дата_рождения", birthDateEdit->date());
    record.setValue("Паспортные_данные", passportEdit->text());
    record.setValue("Электронная_почта", emailEdit->text());
    record.setValue("Номер_телефона", phoneEdit->text());
    return record;
} 