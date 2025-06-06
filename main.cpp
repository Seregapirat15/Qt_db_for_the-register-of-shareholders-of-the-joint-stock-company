#include <QApplication>
#include <QMessageBox>
#include <QtSql>
#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QComboBox>
#include "mainwindow.h"
#include <QDebug>

// Константы для подключения к базе данных
const QString DB_SERVER = "localhost";
const QString DB_PORT = "5432";
const QString DB_NAME = "stocks";

// Глобальная переменная для хранения текущей роли пользователя
QString currentRole;

class LoginDialog : public QDialog {
public:
    LoginDialog(QWidget *parent = nullptr) : QDialog(parent) {
        setWindowTitle("Вход в систему");
        
        QGridLayout *layout = new QGridLayout(this);
        
        // Поля для ввода данных
        QLabel *usernameLabel = new QLabel("Имя пользователя:");
        QLabel *passwordLabel = new QLabel("Пароль:");
        
        usernameEdit = new QLineEdit();
        passwordEdit = new QLineEdit();
        passwordEdit->setEchoMode(QLineEdit::Password);
        
        // Добавление полей в сетку
        layout->addWidget(usernameLabel, 0, 0);
        layout->addWidget(usernameEdit, 0, 1);
        
        layout->addWidget(passwordLabel, 1, 0);
        layout->addWidget(passwordEdit, 1, 1);
        
        // Кнопки
        QPushButton *loginButton = new QPushButton("Войти");
        QPushButton *cancelButton = new QPushButton("Отмена");
        
        QHBoxLayout *buttonLayout = new QHBoxLayout();
        buttonLayout->addWidget(loginButton);
        buttonLayout->addWidget(cancelButton);
        
        layout->addLayout(buttonLayout, 2, 0, 1, 2);
        
        // Соединение сигналов и слотов
        connect(loginButton, &QPushButton::clicked, this, &LoginDialog::accept);
        connect(cancelButton, &QPushButton::clicked, this, &LoginDialog::reject);
        
        setLayout(layout);
        resize(350, 150);
    }
    
    QString getUsername() const { return usernameEdit->text(); }
    QString getPassword() const { return passwordEdit->text(); }
    
private:
    QLineEdit *usernameEdit;
    QLineEdit *passwordEdit;
};

bool createConnection(const QString &username, const QString &password) {
    // Вывод доступных драйверов
    qDebug() << "Доступные драйверы:" << QSqlDatabase::drivers();
    
    QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL");
    db.setHostName(DB_SERVER);
    db.setPort(DB_PORT.toInt());
    db.setDatabaseName(DB_NAME);
    db.setUserName(username);
    db.setPassword(password);
    
    // Отладочная информация о подключении
    qDebug() << "Попытка подключения к БД:";
    qDebug() << "  Сервер:" << DB_SERVER;
    qDebug() << "  Порт:" << DB_PORT;
    qDebug() << "  База данных:" << DB_NAME;
    qDebug() << "  Пользователь:" << username;
    
    if (!db.open()) {
        QString errorMsg = db.lastError().text();
        QString fullErrorMsg = QString("Не удалось подключиться к базе данных: %1\n"
                                     "Сервер: %2\n"
                                     "Порт: %3\n"
                                     "База данных: %4\n"
                                     "Пользователь: %5\n"
                                     "Проверьте настройки PostgreSQL и запущен ли сервер.")
                                    .arg(errorMsg)
                                    .arg(DB_SERVER)
                                    .arg(DB_PORT)
                                    .arg(DB_NAME)
                                    .arg(username);
        QMessageBox::critical(nullptr, "Ошибка соединения", fullErrorMsg);
        qDebug() << "Ошибка подключения:" << errorMsg;
        return false;
    }
    
    qDebug() << "Подключение успешно!";
    
    // Определяем роль пользователя
    if (username.toLower() == "postgres") {
        // Пользователь postgres - администратор
        currentRole = "admin";
    } else {
        // Все остальные пользователи - обычные пользователи
        currentRole = "user";
    }
    
    qDebug() << "Установлена роль:" << currentRole;
    
    return true;
}

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    // Отображение диалога входа
    LoginDialog loginDialog;
    if (loginDialog.exec() != QDialog::Accepted) {
        return 0;
    }
    
    // Получение данных из диалога
    QString username = loginDialog.getUsername();
    QString password = loginDialog.getPassword();
    
    // Подключение к базе данных
    if (!createConnection(username, password)) {
        return 1;
    }
    
    // Создание и отображение главного окна
    MainWindow mainWindow(currentRole);
    QString windowTitle = "Реестр акционеров";
    if (currentRole == "admin") {
        windowTitle += " - Администратор";
    } else if (currentRole == "user") {
        windowTitle += " - Пользователь";
    }
    mainWindow.setWindowTitle(windowTitle);
    mainWindow.resize(1024, 768);
    mainWindow.show();
    
    return app.exec();
}
