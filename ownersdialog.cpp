#include "ownersdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

OwnersDialog::OwnersDialog(QWidget *parent, QSqlRecord *record) : QDialog(parent) {
    setWindowTitle("Редактирование владельца ценных бумаг");
    resize(400, 250);
    
    // Создаем элементы формы
    shareholderComboBox = new QComboBox(this);
    securitiesComboBox = new QComboBox(this);
    
    acquisitionDateEdit = new QDateEdit(QDate::currentDate(), this);
    acquisitionDateEdit->setCalendarPopup(true);
    acquisitionDateEdit->setDisplayFormat("dd.MM.yyyy");
    
    // Загружаем данные для комбобоксов
    shareholdersModel = new QSqlQueryModel(this);
    securitiesModel = new QSqlQueryModel(this);
    
    loadShareholders();
    loadSecurities();
    
    shareholderComboBox->setModel(shareholdersModel);
    shareholderComboBox->setModelColumn(1); // Отображаем ФИО акционера
    
    securitiesComboBox->setModel(securitiesModel);
    securitiesComboBox->setModelColumn(1); // Отображаем название ценной бумаги
    
    // Создаем форму
    QFormLayout *formLayout = new QFormLayout();
    formLayout->addRow("Акционер:", shareholderComboBox);
    formLayout->addRow("Ценная бумага:", securitiesComboBox);
    formLayout->addRow("Дата приобретения:", acquisitionDateEdit);
    
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
        
        // Устанавливаем выбранного акционера
        int shareholderId = record->value("ID_Акционера").toInt();
        for (int i = 0; i < shareholderComboBox->count(); ++i) {
            if (shareholdersModel->record(i).value(0).toInt() == shareholderId) {
                shareholderComboBox->setCurrentIndex(i);
                break;
            }
        }
        
        // Устанавливаем выбранную ценную бумагу
        int securitiesId = record->value("Номер_пакета_акций").toInt();
        for (int i = 0; i < securitiesComboBox->count(); ++i) {
            if (securitiesModel->record(i).value(0).toInt() == securitiesId) {
                securitiesComboBox->setCurrentIndex(i);
                break;
            }
        }
        
        // Устанавливаем дату приобретения, если она есть
        if (!record->isNull("Дата_приобретения")) {
            acquisitionDateEdit->setDate(record->value("Дата_приобретения").toDate());
        }
    }
}

void OwnersDialog::loadShareholders() {
    shareholdersModel->setQuery("SELECT ID_акционера, ФИО_акционера FROM Акционер ORDER BY ФИО_акционера");
    if (shareholdersModel->lastError().isValid()) {
        qDebug() << "Ошибка загрузки акционеров:" << shareholdersModel->lastError().text();
    }
}

void OwnersDialog::loadSecurities() {
    securitiesModel->setQuery("SELECT ID_пакета_акции, Название_ценной_бумаги || ' (' || Тип_ценной_бумаги || ', ' || Количество_в_пакете_акций || ' шт.)' FROM Ценные_бумаги ORDER BY ID_пакета_акции");
    if (securitiesModel->lastError().isValid()) {
        qDebug() << "Ошибка загрузки ценных бумаг:" << securitiesModel->lastError().text();
    }
}

QSqlRecord OwnersDialog::getRecord() const {
    QSqlRecord record = currentRecord;
    
    // Получаем ID выбранного акционера
    int shareholderIndex = shareholderComboBox->currentIndex();
    int shareholderId = shareholdersModel->record(shareholderIndex).value(0).toInt();
    
    // Получаем ID выбранной ценной бумаги
    int securitiesIndex = securitiesComboBox->currentIndex();
    int securitiesId = securitiesModel->record(securitiesIndex).value(0).toInt();
    
    record.setValue("ID_Акционера", shareholderId);
    record.setValue("Номер_пакета_акций", securitiesId);
    record.setValue("Дата_приобретения", acquisitionDateEdit->date());
    
    return record;
} 