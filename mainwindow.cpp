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
    
    // Если пользователь администратор, добавляем вкладку управления пользователями
    if (userRole == "admin") {
        tabWidget->addTab(createUsersTab(), "Пользователи");
    }
    
    // Настройка прав доступа в зависимости от роли
    setupPermissions();
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
    
    QSqlQueryModel *attendanceReportModel = new QSqlQueryModel(this);
    attendanceReportModel->setQuery("SELECT * FROM Посещаемость_собраний");
    
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
    // Добавление новой записи
    int row = shareholdersModel->rowCount();
    shareholdersModel->insertRow(row);
    shareholdersModel->setData(shareholdersModel->index(row, 1), "Новый акционер");
    shareholdersModel->setData(shareholdersModel->index(row, 2), QDate::currentDate());
    shareholdersModel->setData(shareholdersModel->index(row, 3), "Паспорт");
    shareholdersModel->setData(shareholdersModel->index(row, 4), "email@example.com");
    shareholdersModel->setData(shareholdersModel->index(row, 5), "+7 (XXX) XXX-XX-XX");
    
    // Фокус на новой строке для редактирования
    shareholdersView->selectRow(row);
    shareholdersView->scrollToBottom();
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
    int row = securitiesModel->rowCount();
    securitiesModel->insertRow(row);
    securitiesModel->setData(securitiesModel->index(row, 1), "Акция АО \"Компания\"");
    securitiesModel->setData(securitiesModel->index(row, 2), "Обычная");
    securitiesModel->setData(securitiesModel->index(row, 3), 1000.00);
    securitiesModel->setData(securitiesModel->index(row, 4), 100);
    
    securitiesView->selectRow(row);
    securitiesView->scrollToBottom();
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
    int row = ownersModel->rowCount();
    ownersModel->insertRow(row);
    
    ownersView->selectRow(row);
    ownersView->scrollToBottom();
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
    int row = meetingsModel->rowCount();
    meetingsModel->insertRow(row);
    meetingsModel->setData(meetingsModel->index(row, 1), "Новое собрание");
    meetingsModel->setData(meetingsModel->index(row, 2), QDate::currentDate());
    
    meetingsView->selectRow(row);
    meetingsView->scrollToBottom();
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
    int row = operationsModel->rowCount();
    operationsModel->insertRow(row);
    operationsModel->setData(operationsModel->index(row, 1), "Продавец");
    operationsModel->setData(operationsModel->index(row, 2), "Покупатель");
    operationsModel->setData(operationsModel->index(row, 3), QDateTime::currentDateTime());
    
    operationsView->selectRow(row);
    operationsView->scrollToBottom();
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

// Создание вкладки управления пользователями
QWidget* MainWindow::createUsersTab() {
    QWidget *widget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(widget);
    
    // Таблица пользователей
    usersModel = new QSqlTableModel(this);
    usersModel->setTable("Пользователи");
    usersModel->setEditStrategy(QSqlTableModel::OnFieldChange);
    usersModel->setHeaderData(0, Qt::Horizontal, "ID");
    usersModel->setHeaderData(1, Qt::Horizontal, "Имя пользователя");
    usersModel->setHeaderData(2, Qt::Horizontal, "Пароль");
    usersModel->setHeaderData(3, Qt::Horizontal, "Роль");
    usersModel->setHeaderData(4, Qt::Horizontal, "ФИО");
    usersModel->setHeaderData(5, Qt::Horizontal, "Email");
    usersModel->setHeaderData(6, Qt::Horizontal, "Дата создания");
    usersModel->setHeaderData(7, Qt::Horizontal, "Активен");
    usersModel->select();
    
    usersView = new QTableView();
    usersView->setModel(usersModel);
    usersView->setSelectionBehavior(QAbstractItemView::SelectRows);
    usersView->setSelectionMode(QAbstractItemView::SingleSelection);
    usersView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    layout->addWidget(usersView);
    
    // Кнопки
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QPushButton *addButton = new QPushButton("Добавить");
    QPushButton *deleteButton = new QPushButton("Удалить");
    
    buttonLayout->addWidget(addButton);
    buttonLayout->addWidget(deleteButton);
    layout->addLayout(buttonLayout);
    
    // Соединение сигналов и слотов
    connect(addButton, &QPushButton::clicked, this, &MainWindow::manageUsers);
    connect(deleteButton, &QPushButton::clicked, this, [this]() {
        QModelIndex currentIndex = usersView->currentIndex();
        if (!currentIndex.isValid()) {
            QMessageBox::warning(this, "Предупреждение", "Выберите пользователя для удаления");
            return;
        }
        
        int reply = QMessageBox::question(this, "Подтверждение", 
                                         "Вы уверены, что хотите удалить этого пользователя?",
                                         QMessageBox::Yes | QMessageBox::No);
        
        if (reply == QMessageBox::Yes) {
            usersModel->removeRow(currentIndex.row());
            usersModel->select();
        }
    });
    
    return widget;
}

// Реализация слота для управления пользователями
void MainWindow::manageUsers() {
    int row = usersModel->rowCount();
    usersModel->insertRow(row);
    usersModel->setData(usersModel->index(row, 1), "новый_пользователь");
    usersModel->setData(usersModel->index(row, 2), "пароль");
    usersModel->setData(usersModel->index(row, 3), "user");
    usersModel->setData(usersModel->index(row, 4), "Новый Пользователь");
    usersModel->setData(usersModel->index(row, 5), "new_user@example.com");
    usersModel->setData(usersModel->index(row, 7), true);
    
    usersView->selectRow(row);
    usersView->scrollToBottom();
} 