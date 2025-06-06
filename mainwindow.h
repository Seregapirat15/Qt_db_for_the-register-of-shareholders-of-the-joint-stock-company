#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTabWidget>
#include <QTableView>
#include <QSqlTableModel>
#include <QSqlRelationalTableModel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QLabel>
#include <QComboBox>
#include <QDateEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QGroupBox>
#include <QFormLayout>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(const QString &role = "user", QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // Слоты для таблицы Акционер
    void addShareholder();
    void deleteShareholder();
    void filterShareholders();
    
    // Слоты для таблицы Ценные бумаги
    void addSecurities();
    void deleteSecurities();
    
    // Слоты для таблицы Владельцы ценных бумаг
    void addOwner();
    void deleteOwner();
    
    // Слоты для таблицы Собрание акционеров
    void addMeeting();
    void deleteMeeting();
    
    // Слоты для таблицы Присутствие
    void addAttendance();
    void deleteAttendance();
    
    // Слоты для таблицы Операция с акцией
    void addOperation();
    void deleteOperation();
    
    // Слот для управления пользователями (только для администратора)
    void manageUsers();

private:
    // Создание вкладок
    QWidget* createShareholdersTab();
    QWidget* createSecuritiesTab();
    QWidget* createOwnersTab();
    QWidget* createMeetingsTab();
    QWidget* createAttendanceTab();
    QWidget* createOperationsTab();
    QWidget* createReportsTab();
    QWidget* createUsersTab(); // Новая вкладка для управления пользователями
    
    // Модели данных
    QSqlTableModel *shareholdersModel;
    QSqlTableModel *securitiesModel;
    QSqlRelationalTableModel *ownersModel;
    QSqlTableModel *meetingsModel;
    QSqlRelationalTableModel *attendanceModel;
    QSqlTableModel *operationsModel;
    QSqlTableModel *usersModel;     // Модель для управления пользователями
    
    // Представления данных
    QTableView *shareholdersView;
    QTableView *securitiesView;
    QTableView *ownersView;
    QTableView *meetingsView;
    QTableView *attendanceView;
    QTableView *operationsView;
    QTableView *usersView;          // Представление для управления пользователями
    
    // Поля для фильтрации акционеров
    QLineEdit *shareholderFilterEdit;
    
    // Вкладки
    QTabWidget *tabWidget;
    
    // Текущая роль пользователя
    QString userRole;
    
    // Инициализация моделей
    void setupModels();
    
    // Настройка разрешений в зависимости от роли
    void setupPermissions();
};

#endif // MAINWINDOW_H 