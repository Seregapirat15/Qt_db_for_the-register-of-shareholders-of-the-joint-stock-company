#include "securitiesdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>

SecuritiesDialog::SecuritiesDialog(QWidget *parent, QSqlRecord *record) : QDialog(parent) {
    setWindowTitle("Редактирование ценных бумаг");
    resize(400, 250);
    
    // Создаем элементы формы
    nameEdit = new QLineEdit(this);
    
    typeComboBox = new QComboBox(this);
    typeComboBox->addItem("Обычная");
    typeComboBox->addItem("Привилегированная");
    
    nominalValueSpinBox = new QDoubleSpinBox(this);
    nominalValueSpinBox->setRange(0.01, 1000000.00);
    nominalValueSpinBox->setDecimals(2);
    nominalValueSpinBox->setSingleStep(100.00);
    nominalValueSpinBox->setValue(1000.00);
    nominalValueSpinBox->setSuffix(" руб.");
    
    quantitySpinBox = new QSpinBox(this);
    quantitySpinBox->setRange(1, 1000000);
    quantitySpinBox->setValue(100);
    quantitySpinBox->setSuffix(" шт.");
    
    // Создаем форму
    QFormLayout *formLayout = new QFormLayout();
    formLayout->addRow("Название ценной бумаги:", nameEdit);
    formLayout->addRow("Тип ценной бумаги:", typeComboBox);
    formLayout->addRow("Номинальная стоимость:", nominalValueSpinBox);
    formLayout->addRow("Количество в пакете:", quantitySpinBox);
    
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
        nameEdit->setText(record->value("Название_ценной_бумаги").toString());
        
        // Устанавливаем тип ценной бумаги
        QString type = record->value("Тип_ценной_бумаги").toString();
        int index = typeComboBox->findText(type);
        if (index >= 0) {
            typeComboBox->setCurrentIndex(index);
        }
        
        nominalValueSpinBox->setValue(record->value("Номинальная_стоимость").toDouble());
        quantitySpinBox->setValue(record->value("Количество_в_пакете_акций").toInt());
    }
}

QSqlRecord SecuritiesDialog::getRecord() const {
    QSqlRecord record = currentRecord;
    record.setValue("Название_ценной_бумаги", nameEdit->text());
    record.setValue("Тип_ценной_бумаги", typeComboBox->currentText());
    record.setValue("Номинальная_стоимость", nominalValueSpinBox->value());
    record.setValue("Количество_в_пакете_акций", quantitySpinBox->value());
    return record;
} 