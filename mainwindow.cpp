#include "mainwindow.h"
#include <QMessageBox>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QDebug>
#include <QHeaderView>
#include <QDateTime>

MainWindow::MainWindow(const QString &role, QWidget *parent) : QMainWindow(parent) {
    // Сохраняем роль пользователя
    userRole = role;
    
    // Инициализация главного окна
    tabWidget = new QTabWidget(this);
    setCentralWidget(tabWidget);
    
    // Настройка моделей
    setupModels();
    
    // Создание вкладок
    tabWidget->addTab(createShareholdersTab(), "Акционеры");
    tabWidget->addTab(createSecuritiesTab(), "Ценные бумаги");
    tabWidget->addTab(createOwnersTab(), "Владельцы ценных бумаг");
    tabWidget->addTab(createMeetingsTab(), "Собрания акционеров");
    tabWidget->addTab(createAttendanceTab(), "Присутствие");
    tabWidget->addTab(createOperationsTab(), "Операции с акциями");
    tabWidget->addTab(createReportsTab(), "Отчеты");
    
    // Настройка прав доступа в зависимости от роли
    setupPermissions();
    
    // Подключаем сигналы двойного клика для редактирования записей
    connect(shareholdersView, &QTableView::doubleClicked, this, &MainWindow::editShareholder);
    connect(securitiesView, &QTableView::doubleClicked, this, &MainWindow::editSecurities);
    connect(meetingsView, &QTableView::doubleClicked, this, &MainWindow::editMeeting);
    connect(ownersView, &QTableView::doubleClicked, this, &MainWindow::editOwner);
    connect(operationsView, &QTableView::doubleClicked, this, &MainWindow::editOperation);
}

MainWindow::~MainWindow() {
    // Освобождение ресурсов
}

void MainWindow::setupModels() {
    // Модель для таблицы Акционер
    shareholdersModel = new QSqlTableModel(this);
    shareholdersModel->setTable("Акционер");
    shareholdersModel->setEditStrategy(QSqlTableModel::OnFieldChange);
    shareholdersModel->setHeaderData(0, Qt::Horizontal, "ID");
    shareholdersModel->setHeaderData(1, Qt::Horizontal, "ФИО акционера");
    shareholdersModel->setHeaderData(2, Qt::Horizontal, "Дата рождения");
    shareholdersModel->setHeaderData(3, Qt::Horizontal, "Паспортные данные");
    shareholdersModel->setHeaderData(4, Qt::Horizontal, "Электронная почта");
    shareholdersModel->setHeaderData(5, Qt::Horizontal, "Номер телефона");
    shareholdersModel->select();
    
    // Модель для таблицы Ценные бумаги
    securitiesModel = new QSqlTableModel(this);
    securitiesModel->setTable("Ценные_бумаги");
    securitiesModel->setEditStrategy(QSqlTableModel::OnFieldChange);
    securitiesModel->setHeaderData(0, Qt::Horizontal, "ID пакета");
    securitiesModel->setHeaderData(1, Qt::Horizontal, "Название");
    securitiesModel->setHeaderData(2, Qt::Horizontal, "Тип");
    securitiesModel->setHeaderData(3, Qt::Horizontal, "Номинальная стоимость");
    securitiesModel->setHeaderData(4, Qt::Horizontal, "Количество в пакете");
    securitiesModel->select();
    
    // Модель для таблицы Владельцы ценных бумаг
    ownersModel = new QSqlRelationalTableModel(this);
    ownersModel->setTable("Владельцы_ценных_бумаг");
    ownersModel->setEditStrategy(QSqlTableModel::OnFieldChange);
    ownersModel->setHeaderData(0, Qt::Horizontal, "ID");
    ownersModel->setRelation(1, QSqlRelation("Акционер", "ID_акционера", "ФИО_акционера"));
    ownersModel->setRelation(2, QSqlRelation("Ценные_бумаги", "ID_пакета_акции", "Название_ценной_бумаги"));
    ownersModel->setHeaderData(1, Qt::Horizontal, "Акционер");
    ownersModel->setHeaderData(2, Qt::Horizontal, "Пакет акций");
    ownersModel->select();
    
    // Модель для таблицы Собрание акционеров
    meetingsModel = new QSqlTableModel(this);
    meetingsModel->setTable("Собрание_акционеров");
    meetingsModel->setEditStrategy(QSqlTableModel::OnFieldChange);
    meetingsModel->setHeaderData(0, Qt::Horizontal, "ID");
    meetingsModel->setHeaderData(1, Qt::Horizontal, "Повестка дня");
    meetingsModel->setHeaderData(2, Qt::Horizontal, "Дата");
    meetingsModel->select();
    
    // Модель для таблицы Присутствие
    attendanceModel = new QSqlRelationalTableModel(this);
    attendanceModel->setTable("Присутствие");
    attendanceModel->setEditStrategy(QSqlTableModel::OnFieldChange);
    attendanceModel->setHeaderData(0, Qt::Horizontal, "ID");
    attendanceModel->setRelation(1, QSqlRelation("Акционер", "ID_акционера", "ФИО_акционера"));
    attendanceModel->setHeaderData(1, Qt::Horizontal, "Акционер");
    attendanceModel->setHeaderData(2, Qt::Horizontal, "Присутствие");
    attendanceModel->setRelation(3, QSqlRelation("Собрание_акционеров", "ID_собрания", "Повестка_дня"));
    attendanceModel->setHeaderData(3, Qt::Horizontal, "Собрание");
    attendanceModel->select();
    
    // Модель для таблицы Операция с акцией
    operationsModel = new QSqlTableModel(this);
    operationsModel->setTable("Операция_с_акцией");
    operationsModel->setEditStrategy(QSqlTableModel::OnFieldChange);
    operationsModel->setHeaderData(0, Qt::Horizontal, "ID");
    operationsModel->setHeaderData(1, Qt::Horizontal, "Продавец");
    operationsModel->setHeaderData(2, Qt::Horizontal, "Покупатель");
    operationsModel->setHeaderData(3, Qt::Horizontal, "Время сделки");
    operationsModel->setHeaderData(4, Qt::Horizontal, "Пакет акций");
    operationsModel->select();
}

// Создание вкладки Акционеры
QWidget* MainWindow::createShareholdersTab() {
    QWidget *widget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(widget);
    
    // Поле фильтрации
    QHBoxLayout *filterLayout = new QHBoxLayout();
    QLabel *filterLabel = new QLabel("Фильтр (ФИО):");
    shareholderFilterEdit = new QLineEdit();
    QPushButton *filterButton = new QPushButton("Применить фильтр");
    
    filterLayout->addWidget(filterLabel);
    filterLayout->addWidget(shareholderFilterEdit);
    filterLayout->addWidget(filterButton);
    layout->addLayout(filterLayout);
    
    // Таблица
    shareholdersView = new QTableView();
    shareholdersView->setModel(shareholdersModel);
    shareholdersView->setSelectionBehavior(QAbstractItemView::SelectRows);
    shareholdersView->setSelectionMode(QAbstractItemView::SingleSelection);
    shareholdersView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    layout->addWidget(shareholdersView);
    
    // Кнопки
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QPushButton *addButton = new QPushButton("Добавить");
    QPushButton *deleteButton = new QPushButton("Удалить");
    
    buttonLayout->addWidget(addButton);
    buttonLayout->addWidget(deleteButton);
    layout->addLayout(buttonLayout);
    
    // Соединение сигналов и слотов
    connect(addButton, &QPushButton::clicked, this, &MainWindow::addShareholder);
    connect(deleteButton, &QPushButton::clicked, this, &MainWindow::deleteShareholder);
    connect(filterButton, &QPushButton::clicked, this, &MainWindow::filterShareholders);
    
    return widget;
}

// Создание вкладки Ценные бумаги
QWidget* MainWindow::createSecuritiesTab() {
    QWidget *widget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(widget);
    
    // Таблица
    securitiesView = new QTableView();
    securitiesView->setModel(securitiesModel);
    securitiesView->setSelectionBehavior(QAbstractItemView::SelectRows);
    securitiesView->setSelectionMode(QAbstractItemView::SingleSelection);
    securitiesView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    layout->addWidget(securitiesView);
    
    // Кнопки
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QPushButton *addButton = new QPushButton("Добавить");
    QPushButton *deleteButton = new QPushButton("Удалить");
    
    buttonLayout->addWidget(addButton);
    buttonLayout->addWidget(deleteButton);
    layout->addLayout(buttonLayout);
    
    // Соединение сигналов и слотов
    connect(addButton, &QPushButton::clicked, this, &MainWindow::addSecurities);
    connect(deleteButton, &QPushButton::clicked, this, &MainWindow::deleteSecurities);
    
    return widget;
}

// Создание вкладки Владельцы ценных бумаг
QWidget* MainWindow::createOwnersTab() {
    QWidget *widget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(widget);
    
    // Таблица
    ownersView = new QTableView();
    ownersView->setModel(ownersModel);
    ownersView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ownersView->setSelectionMode(QAbstractItemView::SingleSelection);
    ownersView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    layout->addWidget(ownersView);
    
    // Кнопки
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QPushButton *addButton = new QPushButton("Добавить");
    QPushButton *deleteButton = new QPushButton("Удалить");
    
    buttonLayout->addWidget(addButton);
    buttonLayout->addWidget(deleteButton);
    layout->addLayout(buttonLayout);
    
    // Соединение сигналов и слотов
    connect(addButton, &QPushButton::clicked, this, &MainWindow::addOwner);
    connect(deleteButton, &QPushButton::clicked, this, &MainWindow::deleteOwner);
    
    return widget;
}

// Создание вкладки Собрания акционеров
QWidget* MainWindow::createMeetingsTab() {
    QWidget *widget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(widget);
    
    // Таблица
    meetingsView = new QTableView();
    meetingsView->setModel(meetingsModel);
    meetingsView->setSelectionBehavior(QAbstractItemView::SelectRows);
    meetingsView->setSelectionMode(QAbstractItemView::SingleSelection);
    meetingsView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    layout->addWidget(meetingsView);
    
    // Кнопки
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QPushButton *addButton = new QPushButton("Добавить");
    QPushButton *deleteButton = new QPushButton("Удалить");
    
    buttonLayout->addWidget(addButton);
    buttonLayout->addWidget(deleteButton);
    layout->addLayout(buttonLayout);
    
    // Соединение сигналов и слотов
    connect(addButton, &QPushButton::clicked, this, &MainWindow::addMeeting);
    connect(deleteButton, &QPushButton::clicked, this, &MainWindow::deleteMeeting);
    
    return widget;
}

// Создание вкладки Присутствие
QWidget* MainWindow::createAttendanceTab() {
    QWidget *widget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(widget);
    
    // Таблица
    attendanceView = new QTableView();
    attendanceView->setModel(attendanceModel);
    attendanceView->setSelectionBehavior(QAbstractItemView::SelectRows);
    attendanceView->setSelectionMode(QAbstractItemView::SingleSelection);
    attendanceView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    layout->addWidget(attendanceView);
    
    // Кнопки
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QPushButton *addButton = new QPushButton("Добавить");
    QPushButton *deleteButton = new QPushButton("Удалить");
    
    buttonLayout->addWidget(addButton);
    buttonLayout->addWidget(deleteButton);
    layout->addLayout(buttonLayout);
    
    // Соединение сигналов и слотов
    connect(addButton, &QPushButton::clicked, this, &MainWindow::addAttendance);
    connect(deleteButton, &QPushButton::clicked, this, &MainWindow::deleteAttendance);
    
    return widget;
}

// Создание вкладки Операции с акциями
QWidget* MainWindow::createOperationsTab() {
    QWidget *widget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(widget);
    
    // Таблица
    operationsView = new QTableView();
    operationsView->setModel(operationsModel);
    operationsView->setSelectionBehavior(QAbstractItemView::SelectRows);
    operationsView->setSelectionMode(QAbstractItemView::SingleSelection);
    operationsView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    layout->addWidget(operationsView);
    
    // Кнопки
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QPushButton *addButton = new QPushButton("Добавить");
    QPushButton *deleteButton = new QPushButton("Удалить");
    
    buttonLayout->addWidget(addButton);
    buttonLayout->addWidget(deleteButton);
    layout->addLayout(buttonLayout);
    
    // Соединение сигналов и слотов
    connect(addButton, &QPushButton::clicked, this, &MainWindow::addOperation);
    connect(deleteButton, &QPushButton::clicked, this, &MainWindow::deleteOperation);
    
    return widget;
}

// Создание вкладки Отчеты
QWidget* MainWindow::createReportsTab() {
    QWidget *widget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(widget);
    
    // Создаем представления для отображения отчетов
    QTableView *shareholdersReportView = new QTableView();
    QTableView *attendanceReportView = new QTableView();
    
    // Создаем модели для отчетов
    QSqlQueryModel *shareholdersReportModel = new QSqlQueryModel(this);
    shareholdersReportModel->setQuery("SELECT * FROM Акционеры_и_Акции");
    
    // Проверка ошибок для первой модели
    if (shareholdersReportModel->lastError().isValid()) {
        qDebug() << "Ошибка запроса к Акционеры_и_Акции:" << shareholdersReportModel->lastError().text();
    }
    
    QSqlQueryModel *attendanceReportModel = new QSqlQueryModel(this);
    
    // Используем прямой SQL-запрос вместо представления
    QString directQuery = "SELECT "
                          "s.ID_собрания, "
                          "s.Повестка_дня, "
                          "s.Дата, "
                          "COUNT(CASE WHEN p.Присутствие = 'Да' THEN 1 END) AS Количество_присутствующих, "
                          "COUNT(p.ID_записи) AS Общее_количество_акционеров, "
                          "ROUND((COUNT(CASE WHEN p.Присутствие = 'Да' THEN 1 END)::numeric / NULLIF(COUNT(p.ID_записи), 0)) * 100, 2) AS Процент_посещаемости "
                          "FROM Собрание_акционеров s "
                          "LEFT JOIN Присутствие p ON s.ID_собрания = p.причина_собрания "
                          "GROUP BY s.ID_собрания, s.Повестка_дня, s.Дата";
    
    attendanceReportModel->setQuery(directQuery);
    
    // Проверка ошибок для второй модели
    if (attendanceReportModel->lastError().isValid()) {
        qDebug() << "Ошибка запроса к отчету посещаемости:" << attendanceReportModel->lastError().text();
    }
    
    // Настройка представлений
    shareholdersReportView->setModel(shareholdersReportModel);
    shareholdersReportView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    
    attendanceReportView->setModel(attendanceReportModel);
    attendanceReportView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    
    // Создаем вкладки для отчетов
    QTabWidget *reportsTabWidget = new QTabWidget();
    reportsTabWidget->addTab(shareholdersReportView, "Акционеры и акции");
    reportsTabWidget->addTab(attendanceReportView, "Посещаемость собраний");
    
    layout->addWidget(reportsTabWidget);
    
    return widget;
}

// Реализация слотов для Акционеров
void MainWindow::addShareholder() {
    // Создаем диалог для добавления нового акционера
    ShareholderDialog dialog(this);
    
    if (dialog.exec() == QDialog::Accepted) {
        // Получаем данные из диалога
        QSqlRecord record = dialog.getRecord();
        
        // Добавление новой записи
        int row = shareholdersModel->rowCount();
        shareholdersModel->insertRow(row);
        
        // Заполняем данные
        shareholdersModel->setData(shareholdersModel->index(row, 1), record.value("ФИО_акционера"));
        shareholdersModel->setData(shareholdersModel->index(row, 2), record.value("Дата_рождения"));
        shareholdersModel->setData(shareholdersModel->index(row, 3), record.value("Паспортные_данные"));
        shareholdersModel->setData(shareholdersModel->index(row, 4), record.value("Электронная_почта"));
        shareholdersModel->setData(shareholdersModel->index(row, 5), record.value("Номер_телефона"));
        
        // Фокус на новой строке
        shareholdersView->selectRow(row);
        shareholdersView->scrollToBottom();
    }
}

void MainWindow::deleteShareholder() {
    // Получение выбранной строки
    QModelIndex currentIndex = shareholdersView->currentIndex();
    if (!currentIndex.isValid()) {
        QMessageBox::warning(this, "Предупреждение", "Выберите акционера для удаления");
        return;
    }
    
    // Подтверждение удаления
    int reply = QMessageBox::question(this, "Подтверждение", 
                                     "Вы уверены, что хотите удалить этого акционера?",
                                     QMessageBox::Yes | QMessageBox::No);
    
    if (reply == QMessageBox::Yes) {
        // Удаление строки
        shareholdersModel->removeRow(currentIndex.row());
        shareholdersModel->select();
    }
}

void MainWindow::filterShareholders() {
    QString filter = shareholderFilterEdit->text();
    if (filter.isEmpty()) {
        shareholdersModel->setFilter("");
    } else {
        shareholdersModel->setFilter("ФИО_акционера LIKE '%" + filter + "%'");
    }
    shareholdersModel->select();
}

// Реализация слотов для Ценных бумаг
void MainWindow::addSecurities() {
    // Создаем диалог для добавления новой ценной бумаги
    SecuritiesDialog dialog(this);
    
    if (dialog.exec() == QDialog::Accepted) {
        // Получаем данные из диалога
        QSqlRecord record = dialog.getRecord();
        
        // Добавление новой записи
        int row = securitiesModel->rowCount();
        securitiesModel->insertRow(row);
        
        // Заполняем данные
        securitiesModel->setData(securitiesModel->index(row, 1), record.value("Название_ценной_бумаги"));
        securitiesModel->setData(securitiesModel->index(row, 2), record.value("Тип_ценной_бумаги"));
        securitiesModel->setData(securitiesModel->index(row, 3), record.value("Номинальная_стоимость"));
        securitiesModel->setData(securitiesModel->index(row, 4), record.value("Количество_в_пакете_акций"));
        
        // Фокус на новой строке
        securitiesView->selectRow(row);
        securitiesView->scrollToBottom();
    }
}

void MainWindow::deleteSecurities() {
    QModelIndex currentIndex = securitiesView->currentIndex();
    if (!currentIndex.isValid()) {
        QMessageBox::warning(this, "Предупреждение", "Выберите пакет акций для удаления");
        return;
    }
    
    int reply = QMessageBox::question(this, "Подтверждение", 
                                     "Вы уверены, что хотите удалить этот пакет акций?",
                                     QMessageBox::Yes | QMessageBox::No);
    
    if (reply == QMessageBox::Yes) {
        securitiesModel->removeRow(currentIndex.row());
        securitiesModel->select();
    }
}

// Реализация слотов для Владельцев ценных бумаг
void MainWindow::addOwner() {
    // Создаем диалог для добавления нового владельца ценных бумаг
    OwnersDialog dialog(this);
    
    if (dialog.exec() == QDialog::Accepted) {
        // Получаем данные из диалога
        QSqlRecord record = dialog.getRecord();
        
        // Добавление новой записи
        int row = ownersModel->rowCount();
        ownersModel->insertRow(row);
        
        // Заполняем данные
        ownersModel->setData(ownersModel->index(row, 1), record.value("ID_Акционера"));
        ownersModel->setData(ownersModel->index(row, 2), record.value("Номер_пакета_акций"));
        ownersModel->setData(ownersModel->index(row, 3), record.value("Дата_приобретения"));
        
        // Фокус на новой строке
        ownersView->selectRow(row);
        ownersView->scrollToBottom();
    }
}

void MainWindow::deleteOwner() {
    QModelIndex currentIndex = ownersView->currentIndex();
    if (!currentIndex.isValid()) {
        QMessageBox::warning(this, "Предупреждение", "Выберите запись для удаления");
        return;
    }
    
    int reply = QMessageBox::question(this, "Подтверждение", 
                                     "Вы уверены, что хотите удалить эту запись?",
                                     QMessageBox::Yes | QMessageBox::No);
    
    if (reply == QMessageBox::Yes) {
        ownersModel->removeRow(currentIndex.row());
        ownersModel->select();
    }
}

// Реализация слотов для Собраний акционеров
void MainWindow::addMeeting() {
    // Создаем диалог для добавления нового собрания
    MeetingDialog dialog(this);
    
    if (dialog.exec() == QDialog::Accepted) {
        // Получаем данные из диалога
        QSqlRecord record = dialog.getRecord();
        
        // Добавление новой записи
        int row = meetingsModel->rowCount();
        meetingsModel->insertRow(row);
        
        // Заполняем данные
        meetingsModel->setData(meetingsModel->index(row, 1), record.value("Повестка_дня"));
        meetingsModel->setData(meetingsModel->index(row, 2), record.value("Дата"));
        meetingsModel->setData(meetingsModel->index(row, 3), record.value("Время_начала"));
        meetingsModel->setData(meetingsModel->index(row, 4), record.value("Время_окончания"));
        
        // Фокус на новой строке
        meetingsView->selectRow(row);
        meetingsView->scrollToBottom();
    }
}

void MainWindow::deleteMeeting() {
    QModelIndex currentIndex = meetingsView->currentIndex();
    if (!currentIndex.isValid()) {
        QMessageBox::warning(this, "Предупреждение", "Выберите собрание для удаления");
        return;
    }
    
    int reply = QMessageBox::question(this, "Подтверждение", 
                                     "Вы уверены, что хотите удалить это собрание?",
                                     QMessageBox::Yes | QMessageBox::No);
    
    if (reply == QMessageBox::Yes) {
        meetingsModel->removeRow(currentIndex.row());
        meetingsModel->select();
    }
}

// Реализация слотов для Присутствия
void MainWindow::addAttendance() {
    int row = attendanceModel->rowCount();
    attendanceModel->insertRow(row);
    attendanceModel->setData(attendanceModel->index(row, 2), "Да");
    
    attendanceView->selectRow(row);
    attendanceView->scrollToBottom();
}

void MainWindow::deleteAttendance() {
    QModelIndex currentIndex = attendanceView->currentIndex();
    if (!currentIndex.isValid()) {
        QMessageBox::warning(this, "Предупреждение", "Выберите запись для удаления");
        return;
    }
    
    int reply = QMessageBox::question(this, "Подтверждение", 
                                     "Вы уверены, что хотите удалить эту запись?",
                                     QMessageBox::Yes | QMessageBox::No);
    
    if (reply == QMessageBox::Yes) {
        attendanceModel->removeRow(currentIndex.row());
        attendanceModel->select();
    }
}

// Реализация слотов для Операций с акциями
void MainWindow::addOperation() {
    // Создаем диалог для добавления новой операции с акциями
    OperationsDialog dialog(this);
    
    if (dialog.exec() == QDialog::Accepted) {
        // Получаем данные из диалога
        QSqlRecord record = dialog.getRecord();
        
        // Добавление новой записи
        int row = operationsModel->rowCount();
        operationsModel->insertRow(row);
        
        // Заполняем данные
        operationsModel->setData(operationsModel->index(row, 1), record.value("ФИО_Продавца"));
        operationsModel->setData(operationsModel->index(row, 2), record.value("ФИО_Покупателя"));
        operationsModel->setData(operationsModel->index(row, 3), record.value("Время_сделки"));
        operationsModel->setData(operationsModel->index(row, 4), record.value("Номер_пакета_акций"));
        
        // Фокус на новой строке
        operationsView->selectRow(row);
        operationsView->scrollToBottom();
    }
}

void MainWindow::deleteOperation() {
    QModelIndex currentIndex = operationsView->currentIndex();
    if (!currentIndex.isValid()) {
        QMessageBox::warning(this, "Предупреждение", "Выберите операцию для удаления");
        return;
    }
    
    int reply = QMessageBox::question(this, "Подтверждение", 
                                     "Вы уверены, что хотите удалить эту операцию?",
                                     QMessageBox::Yes | QMessageBox::No);
    
    if (reply == QMessageBox::Yes) {
        operationsModel->removeRow(currentIndex.row());
        operationsModel->select();
    }
}

// Настройка прав доступа в зависимости от роли
void MainWindow::setupPermissions() {
    if (userRole == "user") {
        // Для обычного пользователя ограничиваем права
        // Запрещаем редактирование для некоторых таблиц
        securitiesView->setEditTriggers(QAbstractItemView::NoEditTriggers);
        meetingsView->setEditTriggers(QAbstractItemView::NoEditTriggers);
        ownersView->setEditTriggers(QAbstractItemView::NoEditTriggers);
        operationsView->setEditTriggers(QAbstractItemView::NoEditTriggers);
        
        // Скрываем кнопки добавления/удаления для некоторых таблиц
        QPushButton *addSecButton = securitiesView->parentWidget()->findChild<QPushButton*>();
        QPushButton *delSecButton = securitiesView->parentWidget()->findChildren<QPushButton*>().at(1);
        if (addSecButton) addSecButton->setVisible(false);
        if (delSecButton) delSecButton->setVisible(false);
        
        QPushButton *addMeetButton = meetingsView->parentWidget()->findChild<QPushButton*>();
        QPushButton *delMeetButton = meetingsView->parentWidget()->findChildren<QPushButton*>().at(1);
        if (addMeetButton) addMeetButton->setVisible(false);
        if (delMeetButton) delMeetButton->setVisible(false);
        
        QPushButton *addOwnerButton = ownersView->parentWidget()->findChild<QPushButton*>();
        QPushButton *delOwnerButton = ownersView->parentWidget()->findChildren<QPushButton*>().at(1);
        if (addOwnerButton) addOwnerButton->setVisible(false);
        if (delOwnerButton) delOwnerButton->setVisible(false);
        
        QPushButton *addOpButton = operationsView->parentWidget()->findChild<QPushButton*>();
        QPushButton *delOpButton = operationsView->parentWidget()->findChildren<QPushButton*>().at(1);
        if (addOpButton) addOpButton->setVisible(false);
        if (delOpButton) delOpButton->setVisible(false);
    }
}

// Добавляем новые слоты для редактирования записей по двойному клику
void MainWindow::editShareholder(const QModelIndex &index) {
    if (!index.isValid()) return;
    
    // Получаем запись для редактирования
    QSqlRecord record = shareholdersModel->record(index.row());
    
    // Создаем диалог с текущими данными
    ShareholderDialog dialog(this, &record);
    
    if (dialog.exec() == QDialog::Accepted) {
        // Получаем обновленные данные
        QSqlRecord updatedRecord = dialog.getRecord();
        
        // Обновляем запись в модели
        shareholdersModel->setData(shareholdersModel->index(index.row(), 1), updatedRecord.value("ФИО_акционера"));
        shareholdersModel->setData(shareholdersModel->index(index.row(), 2), updatedRecord.value("Дата_рождения"));
        shareholdersModel->setData(shareholdersModel->index(index.row(), 3), updatedRecord.value("Паспортные_данные"));
        shareholdersModel->setData(shareholdersModel->index(index.row(), 4), updatedRecord.value("Электронная_почта"));
        shareholdersModel->setData(shareholdersModel->index(index.row(), 5), updatedRecord.value("Номер_телефона"));
        
        // Применяем изменения
        shareholdersModel->submitAll();
    }
}

void MainWindow::editSecurities(const QModelIndex &index) {
    // Проверяем роль пользователя, если не admin - показываем сообщение и выходим
    if (userRole != "admin") {
        QMessageBox::warning(this, "Ограниченный доступ", 
                           "У вас нет прав на редактирование этой таблицы");
        return;
    }
    
    if (!index.isValid()) return;
    
    // Получаем запись для редактирования
    QSqlRecord record = securitiesModel->record(index.row());
    
    // Создаем диалог с текущими данными
    SecuritiesDialog dialog(this, &record);
    
    if (dialog.exec() == QDialog::Accepted) {
        // Получаем обновленные данные
        QSqlRecord updatedRecord = dialog.getRecord();
        
        // Обновляем запись в модели
        securitiesModel->setData(securitiesModel->index(index.row(), 1), updatedRecord.value("Название_ценной_бумаги"));
        securitiesModel->setData(securitiesModel->index(index.row(), 2), updatedRecord.value("Тип_ценной_бумаги"));
        securitiesModel->setData(securitiesModel->index(index.row(), 3), updatedRecord.value("Номинальная_стоимость"));
        securitiesModel->setData(securitiesModel->index(index.row(), 4), updatedRecord.value("Количество_в_пакете_акций"));
        
        // Применяем изменения
        securitiesModel->submitAll();
    }
}

void MainWindow::editMeeting(const QModelIndex &index) {
    // Проверяем роль пользователя, если не admin - показываем сообщение и выходим
    if (userRole != "admin") {
        QMessageBox::warning(this, "Ограниченный доступ", 
                           "У вас нет прав на редактирование этой таблицы");
        return;
    }
    
    if (!index.isValid()) return;
    
    // Получаем запись для редактирования
    QSqlRecord record = meetingsModel->record(index.row());
    
    // Создаем диалог с текущими данными
    MeetingDialog dialog(this, &record);
    
    if (dialog.exec() == QDialog::Accepted) {
        // Получаем обновленные данные
        QSqlRecord updatedRecord = dialog.getRecord();
        
        // Обновляем запись в модели
        meetingsModel->setData(meetingsModel->index(index.row(), 1), updatedRecord.value("Повестка_дня"));
        meetingsModel->setData(meetingsModel->index(index.row(), 2), updatedRecord.value("Дата"));
        meetingsModel->setData(meetingsModel->index(index.row(), 3), updatedRecord.value("Время_начала"));
        meetingsModel->setData(meetingsModel->index(index.row(), 4), updatedRecord.value("Время_окончания"));
        
        // Применяем изменения
        meetingsModel->submitAll();
    }
}

// Добавляем реализацию для редактирования владельцев ценных бумаг
void MainWindow::editOwner(const QModelIndex &index) {
    // Проверяем роль пользователя, если не admin - показываем сообщение и выходим
    if (userRole != "admin") {
        QMessageBox::warning(this, "Ограниченный доступ", 
                           "У вас нет прав на редактирование этой таблицы");
        return;
    }
    
    if (!index.isValid()) return;
    
    // Получаем запись для редактирования
    QSqlRecord record = ownersModel->record(index.row());
    
    // Создаем диалог с текущими данными
    OwnersDialog dialog(this, &record);
    
    if (dialog.exec() == QDialog::Accepted) {
        // Получаем обновленные данные
        QSqlRecord updatedRecord = dialog.getRecord();
        
        // Обновляем запись в модели
        ownersModel->setData(ownersModel->index(index.row(), 1), updatedRecord.value("ID_Акционера"));
        ownersModel->setData(ownersModel->index(index.row(), 2), updatedRecord.value("Номер_пакета_акций"));
        ownersModel->setData(ownersModel->index(index.row(), 3), updatedRecord.value("Дата_приобретения"));
        
        // Применяем изменения
        ownersModel->submitAll();
    }
}

// Добавляем реализацию для редактирования операций с акциями
void MainWindow::editOperation(const QModelIndex &index) {
    // Проверяем роль пользователя, если не admin - показываем сообщение и выходим
    if (userRole != "admin") {
        QMessageBox::warning(this, "Ограниченный доступ", 
                           "У вас нет прав на редактирование этой таблицы");
        return;
    }
    
    if (!index.isValid()) return;
    
    // Получаем запись для редактирования
    QSqlRecord record = operationsModel->record(index.row());
    
    // Создаем диалог с текущими данными
    OperationsDialog dialog(this, &record);
    
    if (dialog.exec() == QDialog::Accepted) {
        // Получаем обновленные данные
        QSqlRecord updatedRecord = dialog.getRecord();
        
        // Обновляем запись в модели
        operationsModel->setData(operationsModel->index(index.row(), 1), updatedRecord.value("ФИО_Продавца"));
        operationsModel->setData(operationsModel->index(index.row(), 2), updatedRecord.value("ФИО_Покупателя"));
        operationsModel->setData(operationsModel->index(index.row(), 3), updatedRecord.value("Время_сделки"));
        operationsModel->setData(operationsModel->index(index.row(), 4), updatedRecord.value("Номер_пакета_акций"));
        
        // Применяем изменения
        operationsModel->submitAll();
    }
} 