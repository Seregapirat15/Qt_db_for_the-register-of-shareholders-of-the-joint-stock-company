#include "operationsdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

OperationsDialog::OperationsDialog(QWidget *parent, QSqlRecord *record) : QDialog(parent) {
    setWindowTitle("Редактирование операции с акциями");
    resize(450, 300);
    
    // Создаем элементы формы
    sellerEdit = new QLineEdit(this);
    buyerEdit = new QLineEdit(this);
    
    transactionTimeEdit = new QDateTimeEdit(QDateTime::currentDateTime(), this);
    transactionTimeEdit->setCalendarPopup(true);
    transactionTimeEdit->setDisplayFormat("dd.MM.yyyy hh:mm:ss");
    
    securitiesComboBox = new QComboBox(this);
    
    // Загружаем данные для комбобокса
    securitiesModel = new QSqlQueryModel(this);
    loadSecurities();
    
    securitiesComboBox->setModel(securitiesModel);
    securitiesComboBox->setModelColumn(1); // Отображаем информацию о ценной бумаге
    
    // Создаем форму
    QFormLayout *formLayout = new QFormLayout();
    formLayout->addRow("Продавец:", sellerEdit);
    formLayout->addRow("Покупатель:", buyerEdit);
    formLayout->addRow("Время сделки:", transactionTimeEdit);
    formLayout->addRow("Пакет акций:", securitiesComboBox);
    
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
        
        sellerEdit->setText(record->value("ФИО_Продавца").toString());
        buyerEdit->setText(record->value("ФИО_Покупателя").toString());
        
        if (!record->isNull("Время_сделки")) {
            transactionTimeEdit->setDateTime(record->value("Время_сделки").toDateTime());
        }
        
        // Устанавливаем выбранную ценную бумагу
        int securitiesId = record->value("Номер_пакета_акций").toInt();
        for (int i = 0; i < securitiesComboBox->count(); ++i) {
            if (securitiesModel->record(i).value(0).toInt() == securitiesId) {
                securitiesComboBox->setCurrentIndex(i);
                break;
            }
        }
    }
}

void OperationsDialog::loadSecurities() {
    securitiesModel->setQuery("SELECT ID_пакета_акции, Название_ценной_бумаги || ' (' || Тип_ценной_бумаги || ', ' || Количество_в_пакете_акций || ' шт.)' FROM Ценные_бумаги ORDER BY ID_пакета_акции");
    if (securitiesModel->lastError().isValid()) {
        qDebug() << "Ошибка загрузки ценных бумаг:" << securitiesModel->lastError().text();
    }
}

QSqlRecord OperationsDialog::getRecord() const {
    QSqlRecord record = currentRecord;
    
    record.setValue("ФИО_Продавца", sellerEdit->text());
    record.setValue("ФИО_Покупателя", buyerEdit->text());
    record.setValue("Время_сделки", transactionTimeEdit->dateTime());
    
    // Получаем ID выбранной ценной бумаги
    int securitiesIndex = securitiesComboBox->currentIndex();
    int securitiesId = securitiesModel->record(securitiesIndex).value(0).toInt();
    record.setValue("Номер_пакета_акций", securitiesId);
    
    return record;
} 